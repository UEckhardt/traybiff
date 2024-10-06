/*
 * CImap.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of the IMAP protocol.
 */

#ifndef CIMAP_H_
#define CIMAP_H_

#include <QAbstractSocket>
#include <QMessageLogger>
#include <QSslSocket>
#include <QString>
#include <QStringList>
#include <iostream>

#include "CMailSocket.h"

class CImap : public CMailSocket
{
  Q_OBJECT
public:
  CImap(const QString &server, const QString &user, const QString &password,
        uint16_t port, const QString &mailbox, bool debug_protocol, bool useSSL = false,
        bool allowSelfSigned = false);
  virtual ~CImap() { end(); }

private:
  CImap() {}
  bool writeCmd(const QString &str);
  bool startProtocol(void);
  bool login(void);
  bool readResponse(QStringList &list, bool &last);
  void end(void);
  bool getMail(int &unread, int &read);
  void createConnection(void);

  QString m_User = "";
  QString m_Password = "";
  QString m_Mailbox = "";
  uint16_t m_Port = 0;
  uint16_t m_CmdSeq = 0;
  bool m_StartTLS = false;
  bool m_AllowSelfSigned = false;
  bool m_DebugProtocol;

public slots:
  void doWork(void) override;
private slots:
  void socketError(QAbstractSocket::SocketError error);
  void sslErrors(const QList<QSslError> &errors);
};

#endif /* CIMAP_H_ */
