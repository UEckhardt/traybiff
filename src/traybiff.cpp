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
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

void logMsg(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();
  const char *file = context.file ? context.file : "";
  const char *function = context.function ? context.function : "";
  FILE *fp = fopen("/tmp/traybiff.log", "a");
  if (fp == nullptr)
  {
    fprintf(stderr, "Can not open /tmp/traybiff.log\n");
    exit(-1);
  }
  time_t timer = time(NULL);
  tm* tm_info = localtime(&timer);
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%m-%d %H:%M:%S > ", tm_info);
  fputs(buffer, fp);
  switch (type)
  {
  case QtDebugMsg:
    fprintf(fp, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    break;
  case QtInfoMsg:
    fprintf(fp, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    break;
  case QtWarningMsg:
    fprintf(fp, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    break;
  case QtCriticalMsg:
    fprintf(fp, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    break;
  case QtFatalMsg:
    fprintf(fp, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    break;
  }
  fclose(fp);
}

int main(int argc, char **argv)
{
  qInstallMessageHandler(logMsg);
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

  if (!parser.isSet(dbg))
  {
    pid_t pid;
    // Fork the parent process
    if ((pid = fork()) < 0)
    {
      qCritical("Fork failed");
      exit(-1); // Fork failed
    }
    else if (pid != 0)
    {
      exit(0); // Parent process exits
    }

    // Create a new session
    if (setsid() < 0)
    {
      qCritical("Failed to create a new session");
      exit(-2);
    }
  }
  qInfo("Start tray menu");

  QApplication qAppli(argc, argv);
  qAppli.setQuitOnLastWindowClosed(false);
  qAppli.setApplicationName("TrayBiff");
  qAppli.setApplicationVersion(VER_STR);
  qAppli.setOrganizationName("uli-eckhardt");
  qAppli.setOrganizationDomain("uli-eckhardt.de");
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
