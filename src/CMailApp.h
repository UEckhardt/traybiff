/*
 * CMailApp.h
 *
 * Copyright (C) 2021-2022 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef SRC_CMAILAPP_H_
#define SRC_CMAILAPP_H_

#include "traybiff.h"

class CMailApp : public QObject
{
  Q_OBJECT
public:
  CMailApp(CTrayMenu &menu, bool debug_protocol);
  ~CMailApp() {}

  void
  halt(void)
  {
    m_Monitor.halt();
    m_Monitor.quit();
    m_Monitor.wait(2000);
  }

private:
  CMailMonitor m_Monitor;
  CTrayMenu &m_Traymenu;
  bool m_DebugProtocol;

  void loadConfig();
  QString getMailboxName(int configidx);

private slots:
  void updateResult();
  void mailError(IMailProtocol *server, const QString &errtxt);
  void saveStateRequest(QSessionManager &manager);
  void aboutToQuit();

public slots:
  void reloadConfig();
};
#endif /* SRC_CMAILAPP_H_ */
