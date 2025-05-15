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
#include "setup/CConfig.h"

CMailMonitor::CMailMonitor() : m_Running(false)
{
  CConfig &inst = CConfig::instance();
  m_Polltime = inst.m_PollTime;
  connect(&inst, &CConfig::updatePassword, this, &CMailMonitor::updatePassword);
}

void CMailMonitor::updatePassword(const QString &mailbox, const QString &password)
{
  for (auto data : m_Data)
  {
    if (data->m_MailboxName == mailbox)
    {
      data->m_Server->updatePassword(password);
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

void CMailMonitor::run()
{
  int i;
  m_Running = true;
  qDebug() << "Start Mail Monitor " << m_Polltime;

  while (m_Running)
  {
    emit doWork();

    i = 0;
    while (m_Running && i < m_Polltime)
    {
      sleep(1);
      i++;
    }
  }
  for (i = 0; i < m_Data.size(); i++)
  {
    m_Data[i]->m_Thread->quit();
  }

  for (i = 0; i < m_Data.size(); i++)
  {
    m_Data[i]->m_Thread->wait(1000);
    delete (m_Data[i]->m_Thread);
    delete (m_Data[i]->m_Server);
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
  QString mboxName = getMailboxName(configurationidx);
  if ((m_Data[configurationidx]->m_Unread != numUnread) || (m_Data[configurationidx]->m_Read != numRead))
  {
    m_Data[configurationidx]->m_MailboxName = mboxName;
    m_Data[configurationidx]->m_Unread = numUnread;
    m_Data[configurationidx]->m_Read = numRead;
    emit updateResult();
  }
}
