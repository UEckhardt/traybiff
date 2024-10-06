/*
 * CAbout.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * About dialog.
 */

#include "CAbout.h"

CAbout::CAbout (QWidget *parent) : QDialog (parent)
{
  setupUi (this);
  labelVersion->setText (VER_STR);
  QSettings settings;
  QString info = tr ("Config file : ") + settings.fileName ();
  labelInfo->setText (info);
}
