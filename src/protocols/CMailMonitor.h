/*
 * CMailMonitor.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class to register and monitor E-Mail servers.
 */

#ifndef CMAILMONITOR_H_
#define CMAILMONITOR_H_

#include <QDebug>
#include <QObject>
#include <QSharedPointer>
#include <QThread>
#include <QTimer>
#include <QVector>
#include "IMailProtocol.h"

struct SMailData
{
  IMailProtocol *m_Server;
  QThread *m_Thread;
  QString m_MailboxName;
  int m_Read;
  int m_Unread;
};

class CMailMonitor : public QObject
{
  Q_OBJECT
public:
  CMailMonitor();

  virtual ~CMailMonitor()
  {
    halt();
  }
  void addServer(const QString &mailboxname, IMailProtocol *server);

  void start();
  void halt();

  QString getMailboxName(int configidx) const
  {
    if ((configidx < 0) || (configidx >= m_Data.size()))
    {
      return QString();
    }
    return m_Data[configidx]->m_MailboxName;
  }

  const QVector<SMailData *> &getData(void) const
  {
    return (m_Data);
  }

  void updatePollTime(int tm)
  {
    m_Polltime = tm;
    if (m_Timer.isActive())
    {
      m_Timer.setInterval(m_Polltime * 1000);
    }
  }

signals:
  void updateResult(void);
  void mailError(IMailProtocol *server, const QString &errtxt);
  void doWork(void);

private:
  int m_Polltime;
  QTimer m_Timer;
  QVector<SMailData *> m_Data;

private slots:
  void handleMailError(IMailProtocol *server, const QString &errtxt);
  void handleResultReady(int configurationidx, int numUnread, int numRead);
  void updatePassword(const QString &mailbox, const QString &password);
};

#endif /* CMAILMONITOR_H_ */
