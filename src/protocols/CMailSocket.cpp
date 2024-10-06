/*
 * CMailSocket.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Socket class for IMAP and POP3.
 */

#include "CMailSocket.h"

bool CMailSocket::isConnected()
{
  if (m_Socket == nullptr)
  {
    return false;
  }
  if (m_UseSSL)
  {
    return (m_Socket->isEncrypted());
  }
  return (m_Socket->state() == QTcpSocket::ConnectedState);
}

bool CMailSocket::readLine(QString &result)
{
  if (m_Socket->bytesAvailable() == 0)
  {
    if (!m_Socket->waitForReadyRead(TIMEOUT))
    {
      const QString err = "readLine: Connection timed out";
      if (m_Debug)
      {
        qCritical() << err;
      }
      setError(err);
      return false;
    }
  }
  result = m_Socket->readLine();
  result.chop(2);

  if (m_Debug)
  {
    qDebug() << "readLine " << result;
  }
  return true;
}

bool CMailSocket::readLine(QStringList &result)
{
  QString line;
  if (!readLine(line))
  {
    return false;
  }

  result = line.split(QChar(' '));
  if (result.isEmpty())
  {
    const QString err = "Protocol Error, empty line";
    qCritical() << err;
    setError(err);
    return false;
  }
  return true;
}

bool CMailSocket::writeLine(const QString &str)
{
  if (m_Debug)
  {
    if (str.contains("PASS") || str.contains("LOGIN"))
    {
      qDebug() << "writeLine xxxxx";
    }
    else
    {
      qDebug() << "writeLine " << str;
    }
  }
  QByteArray arr = str.toLocal8Bit();
  arr = arr + "\r\n";
  int size = m_Socket->write(arr);
  if (size != arr.size())
  {
    QString err = QString("writeLine can not write all data %1 of %2")
                      .arg(size)
                      .arg(arr.size());
    qCritical() << err;
    setError(err);
    return (false);
  }
  return (true);
}
