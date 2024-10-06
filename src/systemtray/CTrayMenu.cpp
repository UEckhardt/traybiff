/*
 * CTrayMenu.cpp
 *
 * Copyright (C) 2021-2022 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class for display of the tray menu.
 */

#include "systemtray/CTrayMenu.h"
#include "CAbout.h"
#include "CMailApp.h"
#include "setup/CSetupDialog.h"

CTrayMenu::CTrayMenu(QApplication &app)
    : m_TrayIcon(nullptr), m_App(app), m_CMailApp(nullptr)
{
  aboutAct.setText(tr("About"));
  aboutAct.setShortcuts(QKeySequence::WhatsThis);
  aboutAct.setStatusTip(tr("About TrayBiff"));
  connect(&aboutAct, &QAction::triggered, this, &CTrayMenu::about);
  m_TrayMenu.addAction(&aboutAct);

  setupAct.setText(tr("Setup"));
  setupAct.setShortcuts(QKeySequence::Preferences);
  setupAct.setStatusTip(tr("Setup"));
  connect(&setupAct, &QAction::triggered, this, &CTrayMenu::setup);
  m_TrayMenu.addAction(&setupAct);
  m_TrayMenu.addSeparator();

  quitAct.setText(tr("Quit"));
  quitAct.setShortcuts(QKeySequence::Quit);
  quitAct.setStatusTip(tr("Quit"));
  connect(&quitAct, &QAction::triggered, this, &CTrayMenu::quit);
  m_TrayMenu.addAction(&quitAct);
}

// mail-unread mail-read network-offline
void CTrayMenu::show(QIcon icon, const QString &msg)
{
  Q_ASSERT(!icon.isNull());
  if (m_TrayIcon == nullptr)
  {
    m_TrayIcon = new QSystemTrayIcon(icon);
    m_TrayIcon->setContextMenu(&m_TrayMenu);
  }
  else
  {
    m_TrayIcon->setIcon(icon);
  }

  m_TrayIcon->setToolTip(msg);
  m_TrayIcon->show();
}

void CTrayMenu::about()
{
  CAbout dlg(nullptr);
  dlg.exec();
}

void CTrayMenu::setup()
{
  CSetupDialog setup(nullptr);
  setup.exec();
  emit m_CMailApp->reloadConfig();
}

void CTrayMenu::quit()
{
  qDebug() << "Quit";
  m_App.exit();
}
