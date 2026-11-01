/*
 * CPop3.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of the POP3 and POP3S protocol.
 */

#include "CPop3.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "CCrypt.h"

using namespace std;

CPop3::CPop3(const QString &server, const QString &user,
             const QString &password, uint16_t port, bool debug_protocol,
             bool useSSL, bool allowSelfSigned)
    : m_User(user), m_Password(password), m_Port(port), m_AuthCramMd5(false),
      m_AuthApop(false), m_AllowSelfSigned(allowSelfSigned)
{
  m_UseSSL = useSSL;
  enableDebug(debug_protocol);
  setServer(server);
  clearError();
}

CPop3::~CPop3() { end(); }

void CPop3::createConnection(void)
{
  qRegisterMetaType<QAbstractSocket::SocketError>(
      "QAbstractSocket::SocketError");
  m_Socket = new QSslSocket(this);

  connect(m_Socket,
          QOverload<QAbstractSocket::SocketError>::of(
              &QAbstractSocket::errorOccurred),
          this, &CPop3::socketError);
  connect(m_Socket,
          QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
          this, &CPop3::sslErrors);
}

bool CPop3::readResponse(QStringList &result)
{
  if (!readLine(result))
  {
    return false;
  }
  QString first = result.takeFirst();
  if (first == "+OK")
  {
    return true;
  }
  if (first != "-ERR")
  {
    QString err = "Protocol Error, unexpected response " + first;
    qCritical() << err;
    setError(err);
  }
  return false;
}

void CPop3::doWork(void)
{
  if (!m_PasswordReady)
  {
    qDebug() << "Skipping check until password is loaded from keychain for" << m_Server;
    return;
  }
  if (m_Socket == NULL)
  {
    createConnection();
  }
  clearError();
  if (m_UseSSL)
  {
    m_Socket->setPeerVerifyMode(
        m_AllowSelfSigned ? QSslSocket::VerifyNone : QSslSocket::VerifyPeer);
    m_Socket->connectToHostEncrypted(m_Server, m_Port);
    if (!m_Socket->waitForEncrypted(TIMEOUT))
    {
      QString err = "E: Can not connect to host " + m_Server + " " + QString::number(m_Port) + ": " + m_Socket->errorString();
      qCritical() << err;
      setError(err);
      return;
    }
  }
  else
  {
    m_Socket->connectToHost(m_Server, m_Port);

    if (!m_Socket->waitForConnected(TIMEOUT))
    {
      QString err = "P: Can not connect to host " + m_Server + " " + QString::number(m_Port) + ": " + m_Socket->errorString();
      setError(err);
      qCritical() << err;
      return;
    }
  }
  if (!startProtocol())
  {
    return;
  }
  int unread;
  int read;
  if (getMail(unread, read))
  {
    emit resultReady(getConfigurationIndex(), unread, read);
  }
  end();
  return;
}

CPop3::Pop3Return CPop3::readChall(QString &result)
{
  QStringList list;
  if (!readLine(list))
  {
    return POP3_ERR;
  }
  if (list.empty())
  {
    const QString err = "Protocol Error, empty line";
    qCritical() << err;
    setError(err);
    return POP3_ERR;
  }
  QString first = list.at(0);
  if (first == "+")
  {
    if (list.size() < 2)
    {
      const QString err = "Protocol Error, missing challenge";
      qCritical() << err;
      setError(err);
      return POP3_ERR;
    }
    result = list.at(1);
    qDebug() << "readChall " << result;
    return POP3_OK;
  }
  const QString err = "Protocol Error missing +";
  qCritical() << err;
  setError(err);
  return POP3_ERR;
}

void CPop3::end()
{
  if (m_Socket->state() == QTcpSocket::ConnectedState)
  {
    writeLine(QString("QUIT"));
  }
  m_Socket->close();
}

CPop3::Pop3Return CPop3::readCapa()
{
  QString response;
  if (!readLine(response))
  {
    return POP3_ERR;
  }
  while (response != ".")
  {
    if (m_Debug)
    {
      qDebug() << "CAPA: " << response;
    }
    if (response.left(4) == "SASL")
    {
      m_AuthCramMd5 = response.contains("CRAM-MD5");
    }
    else if (response.left(4) == "STLS")
    {
      m_StartTLS = true;
    }
    if (!readLine(response))
    {
      return POP3_ERR;
    }
  }
  return (POP3_OK);
}

bool CPop3::login()
{
  QString str;
  QStringList list;

  // Get Capabilities
  if (!m_UseSSL)
  {
    str = "CAPA";
    writeLine(str);
    if (!readResponse(list))
    {
      qWarning("CAPA not supported");
    }
    else
    {
      readCapa();
    }

    if (m_StartTLS)
    {
      str = "STLS";
      writeLine(str);
      if (!readResponse(list))
      {
        qWarning("Error on STLS");
      }
      else
      {
        qInfo("Starting TLS");
        m_Socket->setPeerVerifyMode(
            m_AllowSelfSigned ? QSslSocket::VerifyNone : QSslSocket::VerifyPeer);
        m_Socket->startClientEncryption();
        if (!m_Socket->waitForEncrypted(TIMEOUT))
        {
          const QString err = "TLS handshake failed: " + m_Socket->errorString();
          qCritical() << err;
          setError(err);
          end();
          return false;
        }
      }
    }
  }

  // First try CRAM-MD5
  if (m_AuthCramMd5)
  {
    if (!writeLine("AUTH CRAM-MD5"))
    {
      const QString err = "Can not send AUTH CRAM-MD5";
      qCritical() << err;
      setError(err);
      end();
      return (false);
    }
    QString chall_cram_md5;
    if (readChall(chall_cram_md5) == POP3_OK)
    {
      QString response = CCrypt::cram_md5(m_User, m_Password, chall_cram_md5);
      if (!writeLine(response))
      {
        const QString err = "Can not send response";
        qCritical() << err;
        setError(err);
        end();
        return false;
      }
      if (readResponse(list))
      {
        return true;
      }
      qInfo("CRAM-MD5 failed");
    }
  }

  QString cleanUser = m_User;
  cleanUser.remove('\r').remove('\n');
  QString cleanPass = m_Password;
  cleanPass.remove('\r').remove('\n');

  // APOP is not as secure as CRAM-MD5 but it's still better
  // than sending the password in the clear
  if ((m_AuthApop) && (!m_UseSSL))
  {
    QString digest;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(m_ChallApop.toLatin1());
    md5.addData(cleanPass.toLatin1());

    digest = md5.result().toHex();

    str = QString("APOP %1 %2").arg(cleanUser, digest);
    if (writeLine(str))
    {
      // APOP successful
      if (readResponse(list))
      {
        return true;
      }
    }
    qInfo("APOP failed");
  }
  // Plaintext authentication
  str = "USER " + cleanUser;
  writeLine(str);
  if (!readResponse(list))
  {
    const QString err = "Authentication failed";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }
  str = "PASS " + cleanPass;
  writeLine(str);
  if (!readResponse(list))
  {
    const QString err = "Can not send password";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }
  return true;
}

bool CPop3::getMail(int &unread, int &read)
{
  QStringList list;

  unread = -1;
  read = 0;
  clearError();
  if (!isConnected())
  {
    const QString err = "no connection to host";
    qCritical() << err;
    setError(err);
    return false;
  }
  writeLine(QString("STAT"));
  if (!readResponse(list))
  {
    return false;
  }
  if (list.isEmpty())
  {
    const QString err = "invalid number of e-mails";
    qCritical() << err;
    setError(err);
    return false;
  }
  bool ok = false;
  unread = list.at(0).toInt(&ok);
  if (!ok)
  {
    const QString err = "invalid number of e-mails";
    qCritical() << err;
    setError(err);
    return false;
  }
  return true;
}

bool CPop3::startProtocol()
{
  QStringList list;
  bool success = false;

  if (readResponse(list))
  {
    if (!list.isEmpty())
    {
      QString last = list.last();
      QRegularExpression rx("<[a-zA-Z0-9_+.-=]+@[a-zA-Z0-9_+.-]+>");

      if (last.contains(rx))
      {
        m_ChallApop = last;
        m_AuthApop = true;
        if (m_Debug)
        {
          qDebug() << "APOP " << m_ChallApop;
        }
      }
    }

    success = login();
  }
  return success;
}
// Slots

void CPop3::socketError(QAbstractSocket::SocketError socketError)
{
  qCritical() << "Socket error " << socketError;
}

void CPop3::sslErrors(const QList<QSslError> &errors)
{
  qCritical() << "sslErrors" << errors;
}
