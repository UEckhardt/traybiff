/*
 * CMailApp.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#include "CMailApp.h"
#include "protocols/CImap.h"
#include "protocols/CPop3.h"
#include "setup/CConfig.h"
#include <QSessionManager>

void CMailApp::loadConfig(void)
{
  CConfig &cfg = CConfig::instance();
  QVector<QString> mailboxes;
  cfg.getMailboxes(mailboxes);
  for (int i = 0; i < mailboxes.size(); ++i)
  {
    IMailProtocol *mp = nullptr;
    PROTOCOLS protocol;
    QString user;
    QString password;
    QString server;
    uint16_t port;
    QString imap_mailbox;
    cfg.getConfig(mailboxes[i], protocol, user, password, server, port,
                  imap_mailbox);
    switch (protocol)
    {
    case PROTO_POP3:
      mp = new CPop3(server, user, password, port, false, false);
      break;
    case PROTO_POP3S:
      mp = new CPop3(server, user, password, port, true, true);
      break;
    case PROTO_IMAP4:
      mp = new CImap(server, user, password, port, imap_mailbox, m_DebugProtocol, false,
                     false);
      break;
    case PROTO_IMAP3:
      mp = new CImap(server, user, password, port, imap_mailbox, m_DebugProtocol, false,
                     false);
      break;
    case PROTO_IMAPS:
      mp = new CImap(server, user, password, port, imap_mailbox, m_DebugProtocol, true,
                     true);
      break;
    default:
      qCritical() << "Invalid protocol " << protocol;
      exit(-1);
    }

    m_Monitor.addServer(mailboxes[i], mp);
  }

  qDebug() << "connect monitor";

  m_Monitor.updatePollTime(cfg.m_PollTime);
  m_Monitor.start();
  qDebug() << "monitor running";
}

CMailApp::CMailApp(CTrayMenu &menu, bool debug_protocol) : m_Monitor(), m_Traymenu(menu),
                                                           m_DebugProtocol(debug_protocol)
{
  if (!connect(&m_Monitor, &CMailMonitor::updateResult, this,
               &CMailApp::updateResult))
  {
    qFatal() << "connect(&m_Monitor, &CMailMonitor::updateResult failed";
  }
  if (!connect(&m_Monitor, &CMailMonitor::mailError, this, &CMailApp::mailError))
  {
    qFatal() << "connect(&m_Monitor, &CMailMonitor::mailError failed";
  }
  if (!connect(qApp, &QGuiApplication::saveStateRequest, this,
               &CMailApp::saveStateRequest, Qt::DirectConnection))
  {
    qFatal() << "connect(qApp, &QGuiApplication::saveStateRequest failed";
  }
  if (!connect(qApp, &QGuiApplication::aboutToQuit, this,
               &CMailApp::aboutToQuit, Qt::DirectConnection))
  {
    qFatal() << "connect(qApp, &QGuiApplication::aboutToQuit failed";
  }
  loadConfig();
}

void CMailApp::aboutToQuit()
{
  halt();
}

void CMailApp::saveStateRequest(QSessionManager &manager)
{
  qDebug() << "saveStateRequest " << manager.restartCommand();
  CConfig &cfg = CConfig::instance();
  if (cfg.m_UseSessionManangement)
  {
    manager.setRestartHint(QSessionManager::RestartAnyway);
  }
  else
  {
    manager.setRestartHint(QSessionManager::RestartNever);
  }
}

void CMailApp::updateResult()
{
  IconType itype = IconType::icNoMail;
  qDebug() << "Update Result";
  CConfig &cfg = CConfig::instance();
  QVector<SMailData *> data = m_Monitor.getData();
  QString out;
  QString line;
  for (int i = 0; i < data.size(); i++)
  {
    line = QString("%1 %2/%3\n")
               .arg(data[i]->m_MailboxName, 6)
               .arg(data[i]->m_Unread, 2)
               .arg(data[i]->m_Read, 2);
    out.append(line);
    if (data[i]->m_Read > 0)
    {
      if (itype == IconType::icNoMail)
      {
        itype = IconType::icOldMail;
      }
    }
    if (data[i]->m_Unread > 0)
    {
      itype = IconType::icNewMail;
    }
  }

  m_Traymenu.show(cfg.getIcon(itype), out);
}

QString CMailApp::getMailboxName(int configidx)
{
  return m_Monitor.getMailboxName(configidx);
}

void CMailApp::mailError(IMailProtocol *server, const QString &errtxt)
{
  qDebug() << "Error " << server->getServer() << " "
           << getMailboxName(server->getConfigurationIndex()) << ":"
           << errtxt;

  CConfig &cfg = CConfig::instance();
  m_Traymenu.show(cfg.getIcon(IconType::icStopped), tr("Error\n") + errtxt);
}

void CMailApp::reloadConfig()
{
  m_Monitor.halt();
  m_Monitor.quit();
  m_Monitor.wait(10000);
  loadConfig();
}
