/*
 * traybiff.cpp
 *
 * Copyright (C) 2021-2022 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Main function.
 */

#include <iostream>

#include "CMailApp.h"
#include "traybiff.h"
#include "setup/CSetupDialog.h"

using namespace std;

int main(int argc, char **argv)
{
  QApplication qAppli(argc, argv);
  qAppli.setQuitOnLastWindowClosed(false);
  qAppli.setApplicationName("TrayBiff");
  qAppli.setApplicationVersion(VER_STR);
  qAppli.setOrganizationName("uli-eckhardt");
  qAppli.setOrganizationDomain("uli-eckhardt.de");

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  const QCommandLineOption altconfig(
      QStringList() << "c" << "alt-config",
      QCoreApplication::translate("main", "Use an alternate config file."),
      QCoreApplication::translate("main", "directory"));
  parser.addOption(altconfig);
  const QCommandLineOption dbg(
      QStringList() << "d" << "debug",
      QCoreApplication::translate("main", "Debug protocol."));
  parser.addOption(dbg);
  parser.addVersionOption();
  parser.setApplicationDescription(QObject::tr("TrayBiff mail monitor"));

  parser.process(qAppli);
  QString altConfigStr = parser.value(altconfig);
  if (!altConfigStr.isEmpty())
  {
    qAppli.setApplicationName(altConfigStr);
  }

  if (!CConfig::instance().isConfigured())
  {
    CSetupDialog setup(nullptr);
    setup.exec();
  }
  qInfo("Start tray menu");

  CTrayMenu menu(qAppli);
  menu.show(QIcon::fromTheme("dialog-warning",
                             QIcon(":/icons/32x32/hi-app-kbiff.png")),
            QString(QObject::tr("TrayBiff not configured")));

  CMailApp mailappl(menu, parser.isSet(dbg));
  menu.setApp(&mailappl);
  int ret = qAppli.exec();

  qDebug() << "quit traybiff " << ret;

  return 0;
}
