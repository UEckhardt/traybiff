/*
 * CSetupDialog.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#ifndef SETUP_CSETUPDIALOG_H_
#define SETUP_CSETUPDIALOG_H_

#include <QDialog>
#include <QtWidgets>
#include "ui_ISetupDialog.h"
#include "traybiff.h"
#include "CConfig.h"

class CSetupDialog : public QDialog, private Ui::ISetupDialog
{
  Q_OBJECT
  // Q_DECLARE_TR_FUNCTIONS(CSetupDialog)
public:
  CSetupDialog(QWidget *parent);
  virtual ~CSetupDialog();

protected:
  void done(int result);

private:
  QList<QAction *> m_Actions;

private:
  void save();
  void clear();
  void displayMailBox(const QString mailboxname);
  bool inputOk();
  bool getIcon(QIcon &icon, const IconType &icontype);
  void updateIcon(QToolButton *button, const IconType &icontype);
  void AddResetMenu(QToolButton *toolButton, const IconType &type);

private slots:
  void on_toolButtonNewMail_released();
  void on_toolButtonNoMail_released();
  void on_toolButtonNoMailbox_released();
  void on_toolButtonOldMail_released();
  void on_toolButtonStopped_released();
  void on_toolButtonServerAdd_released();
  void on_toolButtonServerDelete_released();
  void on_listWidgetServers_itemSelectionChanged();
  void on_comboBoxProtocol_currentIndexChanged(int idx);
  void on_InputChanged(const QString &text);
  void on_Reset(QAction *action);
};

#endif /* SETUP_CSETUPDIALOG_H_ */
