/*
 * CPop3.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of the POP3 and POP3S protocol.
 */

#ifndef CPOP3_H_
#define CPOP3_H_

#include <QAbstractSocket>
#include <QMessageLogger>
#include <QSslSocket>
#include <QString>
#include <QStringList>
#include <iostream>

#include "CMailSocket.h"

class CPop3 : public CMailSocket
{
  Q_OBJECT
public:
  CPop3(const QString &server, const QString &user, const QString &password,
        uint16_t port, bool useSSL = false, bool allowSelfSigned = false);
  virtual ~CPop3();

private:
  CPop3() : m_User(""), m_Password(""), m_MailboxName("") {}

  typedef enum
  {
    POP3_OK,
    POP3_ERR
  } Pop3Return;

  QString m_User;
  QString m_Password;
  uint16_t m_Port = 0;
  QString m_MailboxName;

  bool login(void);
  Pop3Return readCapa(void);
  Pop3Return readChall(QString &result);
  bool startProtocol(void);
  bool readResponse(QStringList &result);
  void end();
  bool getMail(int &unread, int &read);
  void createConnection(void);

  bool m_AuthCramMd5 = false;
  bool m_AuthApop = false;
  bool m_StartTLS = false;
  bool m_AllowSelfSigned = false;
  QString m_ChallApop;

public slots:
  void doWork(void) override;
private slots:
  void socketError(QAbstractSocket::SocketError socketError);
  void sslErrors(const QList<QSslError> &errors);
};

#endif /* CPOP3_H_ */
