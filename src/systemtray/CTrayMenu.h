/*
 * CTrayMenu.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class for display of the tray menu.
 */

#ifndef SRC_SYSTEMTRAY_CTRAYMENU_H_
#define SRC_SYSTEMTRAY_CTRAYMENU_H_

#include <QObject>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QDebug>

class CMailApp;

class CTrayMenu : public QObject
{
public:
  CTrayMenu(QApplication &app);
  virtual ~CTrayMenu() {}

  void show(QIcon icon, const QString &msg);

  void setApp(CMailApp *ca)
  {
    m_CMailApp = ca;
  }

private:
  QMenu m_TrayMenu;
  QSystemTrayIcon *m_TrayIcon;
  QApplication &m_App;
  CMailApp *m_CMailApp;

  QAction aboutAct;
  QAction setupAct;
  QAction quitAct;

private slots:
  void about();
  void setup();
  void quit();
};

#endif /* SRC_SYSTEMTRAY_CTRAYMENU_H_ */
