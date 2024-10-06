/*
 * CMailSocket.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Socket class for IMAP and POP3.
 */

#ifndef CMAILSOCKET_H_
#define CMAILSOCKET_H_

#include <QMessageLogger>
#include <QSslSocket>
#include <QString>
#include <QStringList>
#include <iostream>

#include "IMailProtocol.h"

class CMailSocket : public IMailProtocol
{
  Q_OBJECT
public:
  CMailSocket() {}
  virtual ~CMailSocket() {}

protected:
  bool readLine(QStringList &result);
  bool readLine(QString &result);
  bool writeLine(const QString &str);

  bool isConnected(void);
  void enableDebug(bool enable)
  {
    m_Debug = enable;
  }

  inline const static int TIMEOUT = 10 * 1000;

protected:
  QSslSocket *m_Socket = nullptr;
  bool m_UseSSL = false;
  bool m_Debug = false;
};

#endif /* CMAILSOCKET_H_ */
