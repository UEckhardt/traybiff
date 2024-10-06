/*
 * CImap.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of the IMAP protocol.
 */

#include "CImap.h"

CImap::CImap(const QString &server, const QString &user,
             const QString &password, uint16_t port, const QString &mailbox,
             bool debug_protocol, bool useSSL, bool allowSelfSigned)
    : m_User(user), m_Password(password), m_Mailbox(mailbox), m_Port(port),
      m_CmdSeq(0), m_StartTLS(false), m_AllowSelfSigned(allowSelfSigned),
      m_DebugProtocol(debug_protocol)
{
  m_UseSSL = useSSL;
  setServer(server);

  clearError();
}

void CImap::createConnection(void)
{
  qRegisterMetaType<QAbstractSocket::SocketError>(
      "QAbstractSocket::SocketError");
  m_Socket = new QSslSocket(this);

  connect(m_Socket,
          QOverload<QAbstractSocket::SocketError>::of(
              &QAbstractSocket::errorOccurred),
          this, &CImap::socketError);
  connect(m_Socket,
          QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
          this, &CImap::sslErrors);
}

void CImap::end()
{
  if (m_Socket->state() == QTcpSocket::ConnectedState)
  {
    QStringList list;
    bool last;
    writeCmd(QString("LOGOUT"));
    while (readResponse(list, last))
    {
      if (last)
      {
        break;
      }
    }
  }
  m_Socket->close();
}

bool CImap::writeCmd(const QString &str)
{
  m_CmdSeq++;
  if (m_CmdSeq > 999)
  {
    m_CmdSeq = 0;
  }
  QString cmd = QString("A%1 %2").arg(m_CmdSeq, 3, 10, QLatin1Char('0')).arg(str);
  if (m_DebugProtocol)
  {
    qDebug() << "writeCmd " << cmd;
  }
  return writeLine(cmd);
}

bool CImap::readResponse(QStringList &list, bool &last)
{
  last = false;
  if (!readLine(list))
  {
    const QString err = "Read error";
    qCritical() << err;
    setError(err);
    return false;
  }
  QString resp = list.takeFirst();
  if (resp == '*')
  {
    last = false;
  }
  else
  {
    last = true;
    QString cmd = QString("A%1").arg(m_CmdSeq, 3, 10, QLatin1Char('0'));
    if (m_DebugProtocol)
    {
      qDebug() << "readResponse " << list;
    }
    if (resp != cmd)
    {
      const QString err = "Unexpected response";
      qCritical() << err;
      setError(err);
      return false;
    }
  }
  return true;
}

bool CImap::getMail(int &unread, int &read)
{
  QStringList list;
  bool last;
  bool ok;
  int nummails = -1;

  clearError();
  if (!isConnected())
  {
    const QString err = "no connection to host";
    qCritical() << err;
    setError(err);
    return false;
  }

  QString cmd = "SELECT " + m_Mailbox;
  if (!writeCmd(cmd))
  {
    return false;
  }
  while (readResponse(list, last))
  {
    if (last)
    {
      break;
    }
    if (list.at(1) == "EXISTS")
    {
      nummails = list.at(0).toInt(&ok);
      if (!ok)
      {
        const QString err = "invalid number of e-mails";
        qCritical() << err;
        setError(err);
        return false;
      }
    }
  }
  if (!last)
  {
    const QString err = "protocol error on SELECT";
    qCritical() << err;
    setError(err);
    return false;
  }
  cmd = "SEARCH UNSEEN";
  if (!writeCmd(cmd))
  {
    return false;
  }
  while (readResponse(list, last))
  {
    if (last)
    {
      break;
    }
    if (list.at(0) == "SEARCH")
    {
      if (list.size() == 1)
      {
        unread = 0;
      }
      else
      {
        unread = list.at(1).toInt(&ok);
        if (!ok)
        {
          const QString err = "invalid number of read e-mails";
          qCritical() << err;
          setError(err);
          return false;
        }
      }
    }
  }
  if (!last)
  {
    const QString err = "protocol error on SEARCH";
    qCritical() << err;
    setError(err);
    return false;
  }

  read = nummails - unread;
  return true;
}

void CImap::doWork(void)
{
  if (m_Socket == NULL)
  {
    createConnection();
  }
  clearError();
  if (m_UseSSL)
  {
    m_Socket->setPeerVerifyMode(QSslSocket::VerifyNone);
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

bool CImap::startProtocol()
{
  QStringList list;
  bool success = false;
  bool imap = false;

  if (!readLine(list))
  {
    const QString err = "Error on connection";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }

  QString s = list.takeFirst();
  if (s != "*")
  {
    const QString err = "Protocol error";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }
  s = list.takeFirst();
  if (s != "OK")
  {
    const QString err = "Error on connection";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }
  QStringListIterator li(list);
  while (li.hasNext())
  {
    QString s = li.next();

    if (s.contains("IMAP", Qt::CaseInsensitive))
    {
      imap = true;
    }
    else if (s == "STARTTLS")
    {
      m_StartTLS = true;
    }
  }
  if (!imap)
  {
    const QString err = "Protocol error IMAP not found";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }
  success = login();
  return success;
}

bool CImap::login()
{
  QString str;
  QStringList list;

  if (m_StartTLS)
  {
    str = "STARTTLS";
    writeCmd(str);
    if (!readLine(list))
    {
      qWarning("Error on starttls");
    }
    else
    {
      qInfo("Starting TLS");
      if (m_AllowSelfSigned)
      {
        m_Socket->setPeerVerifyMode(QSslSocket::VerifyNone);
      }
      m_Socket->startClientEncryption();
    }
  }
  // Plaintext authentication
  str = "LOGIN " + m_User + " " + m_Password;
  writeCmd(str);
  if (!readLine(list))
  {
    const QString err = "Read error during authentication";
    qCritical() << err;
    setError(err);
    end();
    return false;
  }

  return true;
}

// Slots

void CImap::socketError(QAbstractSocket::SocketError error)
{
  qCritical() << "Socket error " << error;
}

void CImap::sslErrors(const QList<QSslError> &errors)
{
  qCritical() << "sslErrors" << errors;
}
