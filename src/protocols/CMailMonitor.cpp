/*
 * CMailMonitor.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class to register and monitor E-Mail servers.
 */

#include "CMailMonitor.h"
#include "CMailSocket.h"
#include "setup/CConfig.h"
#include <QMetaObject>

CMailMonitor::CMailMonitor() : m_Timer(this)
{
  auto &inst = CConfig::instance();
  m_Polltime = inst.m_PollTime;
  connect(&inst, &CConfig::updatePassword, this, &CMailMonitor::updatePassword);
  connect(&m_Timer, &QTimer::timeout, this, &CMailMonitor::doWork);
}

void CMailMonitor::updatePassword(const QString &mailbox, const QString &password)
{
  for (auto data : m_Data)
  {
    if (data->m_MailboxName == mailbox)
    {
      IMailProtocol *srv = data->m_Server;
      QMetaObject::invokeMethod(
          srv,
          [srv, password]() {
            srv->updatePassword(password);
            srv->doWork();
          },
          Qt::QueuedConnection);
      break;
    }
  }
}

void CMailMonitor::addServer(const QString &mailboxname, IMailProtocol *server)
{
  auto *data = new (SMailData);
  auto *thread = new QThread(this);
  server->moveToThread(thread);
  server->setConfigurationIndex(m_Data.size());
  data->m_Server = server;
  data->m_Thread = thread;
  data->m_MailboxName = mailboxname;
  data->m_Read = -1;
  data->m_Unread = -1;

  m_Data.append(data);

  connect(server, &IMailProtocol::mailError, this,
          &CMailMonitor::handleMailError);
  connect(server, &IMailProtocol::resultReady, this,
          &CMailMonitor::handleResultReady);
  connect(this, &CMailMonitor::doWork, server,
          &IMailProtocol::doWork);

  thread->start();
}

void CMailMonitor::start()
{
  qDebug() << "Start Mail Monitor " << m_Polltime;
  emit doWork();
  m_Timer.start(m_Polltime * 1000);
}

void CMailMonitor::halt()
{
  m_Timer.stop();
  for (int i = 0; i < m_Data.size(); i++)
  {
    m_Data[i]->m_Thread->quit();
  }

  for (int i = 0; i < m_Data.size(); i++)
  {
    m_Data[i]->m_Thread->wait();
    delete (m_Data[i]->m_Server);
    delete (m_Data[i]->m_Thread);
    delete (m_Data[i]);
  }

  m_Data.clear();
  qDebug("Stop Mail Monitor");
}

void CMailMonitor::handleMailError(IMailProtocol *server, const QString &errtxt)
{
  qDebug() << "CMailMonitor::handleMailError Error  " << errtxt;
  emit mailError(server, errtxt);
}

void CMailMonitor::handleResultReady(int configurationidx, int numUnread, int numRead)
{
  if ((configurationidx < 0) || (configurationidx >= m_Data.size()))
  {
    return;
  }
  QString mboxName = getMailboxName(configurationidx);
  if ((m_Data[configurationidx]->m_Unread != numUnread) || (m_Data[configurationidx]->m_Read != numRead))
  {
    m_Data[configurationidx]->m_MailboxName = mboxName;
    m_Data[configurationidx]->m_Unread = numUnread;
    m_Data[configurationidx]->m_Read = numRead;
    emit updateResult();
  }
}
