/*
 * CAbout.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * About dialog.
 */

#ifndef SRC_CABOUT_H_
#define SRC_CABOUT_H_

#include "traybiff.h"
#include "ui_IAbout.h"
#include <QDialog>
#include <QtWidgets>

class CAbout : public QDialog, private Ui::IAbout
{
  Q_DECLARE_TR_FUNCTIONS(CAbout)
public:
  CAbout(QWidget *parent);
  virtual ~CAbout() {}
};

#endif /* SRC_CABOUT_H_ */
