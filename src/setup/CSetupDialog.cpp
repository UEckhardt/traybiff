/*
 * CSetupDialog.cpp
 *
 * Copyright (C) 2021-2022 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#include "CSetupDialog.h"

static uint32_t default_ports[PROTO_LAST] = {
    110, // PROTO_POP3
    995, // PROTO_POP3S
    143, // PROTO_IMAP
    220, // PROTO_IMAP3
    993, // PROTO_IMAPS
};

CSetupDialog::CSetupDialog(QWidget *parent) : QDialog(parent)
{
  CConfig &cfg = CConfig::instance();
  setupUi(this);
  labelVersion->setText(VER_STR);
  toolButtonStopped->setIcon(cfg.getIcon(IconType::icStopped));
  AddResetMenu(toolButtonStopped, IconType::icStopped);
  toolButtonNoMailbox->setIcon(cfg.getIcon(IconType::icNoMailbox));
  AddResetMenu(toolButtonNoMailbox, IconType::icNoMailbox);
  toolButtonNoMail->setIcon(cfg.getIcon(IconType::icNoMail));
  AddResetMenu(toolButtonNoMail, IconType::icNoMail);
  toolButtonOldMail->setIcon(cfg.getIcon(IconType::icOldMail));
  AddResetMenu(toolButtonOldMail, IconType::icOldMail);
  toolButtonNewMail->setIcon(cfg.getIcon(IconType::icNewMail));
  AddResetMenu(toolButtonNewMail, IconType::icNewMail);
  comboBoxProtocol->addItem("pop3", QVariant(PROTO_POP3));
  comboBoxProtocol->addItem("pop3s", QVariant(PROTO_POP3S));
  comboBoxProtocol->addItem("imap4", QVariant(PROTO_IMAP4));
  comboBoxProtocol->addItem("imap3", QVariant(PROTO_IMAP3));
  comboBoxProtocol->addItem("imaps", QVariant(PROTO_IMAPS));
  spinBoxPoll->setValue(cfg.m_PollTime);
  checkBoxDockInPanel->setChecked(cfg.m_DockInPanel);
  checkBoxUseSessionManagement->setChecked(cfg.m_UseSessionManangement);

  auto con_line_edit = [this](QLineEdit *li)
  {
    connect(li, &QLineEdit::textChanged, this,
            &CSetupDialog::on_InputChanged);
  };
  con_line_edit(lineEditName);
  con_line_edit(lineEditUser);
  con_line_edit(lineEditPassword);
  con_line_edit(lineEditServer);
  con_line_edit(lineEditPort);
  con_line_edit(lineEditIMAPMailbox);

  QVector<QString> mailboxes;
  cfg.getMailboxes(mailboxes);
  for (int i = 0; i < mailboxes.size(); ++i)
  {
    listWidgetServers->addItem(mailboxes[i]);
  }
  if (cfg.isConfigured())
  {
    listWidgetServers->sortItems();
    listWidgetServers->setCurrentRow(0);
    displayMailBox(listWidgetServers->currentItem()->text());
  }
  cfg.beginUpdate();
}

void CSetupDialog::displayMailBox(const QString mailboxname)
{
  QSignalBlocker b1(lineEditName);
  QSignalBlocker b2(lineEditUser);
  QSignalBlocker b3(lineEditPassword);
  QSignalBlocker b4(lineEditServer);
  QSignalBlocker b5(lineEditPort);
  QSignalBlocker b6(lineEditIMAPMailbox);
  QSignalBlocker b7(comboBoxProtocol);
  CConfig &cfg = CConfig::instance();
  PROTOCOLS protocol;
  QString user;
  QString password;
  QString server;
  QString imap_mailbox;
  uint16_t port;

  cfg.getConfig(mailboxname, protocol, user, password, server, port,
                imap_mailbox);

  qInfo("Mailbox %s %d %s", qUtf8Printable(mailboxname), protocol,
        qUtf8Printable(user));
  int idx = comboBoxProtocol->findData(QVariant(protocol));
  if (idx != -1)
  { // -1 for not found
    comboBoxProtocol->setCurrentIndex(idx);
  }
  lineEditName->setText(mailboxname);
  lineEditUser->setText(user);
  lineEditPassword->setText(password);
  lineEditServer->setText(server);
  lineEditPort->setText(QString::number(port));
  lineEditIMAPMailbox->setText(imap_mailbox);
}

void CSetupDialog::done(int result)
{
  CConfig &cfg = CConfig::instance();
  if (result == Accepted)
  {
    qInfo("CSetupDialog::OK");
    save();
    cfg.m_PollTime = spinBoxPoll->value();
    cfg.m_DockInPanel = checkBoxDockInPanel->isChecked();
    cfg.m_UseSessionManangement = checkBoxUseSessionManagement->isChecked();
    cfg.save();
  }
  else
  {
    qInfo("Cancel");
    cfg.abortUpdate();
  }

  QDialog::done(result);
}

bool CSetupDialog::inputOk()
{
  bool ok = true;
  if (lineEditName->text().isEmpty() || lineEditUser->text().isEmpty() || lineEditPassword->text().isEmpty() || lineEditServer->text().isEmpty() || lineEditPort->text().isEmpty())
  {
    return false;
  }
  int proto = comboBoxProtocol->currentData().toInt();
  if (proto < 0)
  {
    return false;
  }
  if ((proto >= PROTO_IMAP4) && (proto <= PROTO_IMAPS))
  {
    if (lineEditIMAPMailbox->text().isEmpty())
    {
      return false;
    }
  }
  lineEditPort->text().toInt(&ok);
  return ok;
}

void CSetupDialog::clear()
{
  QSignalBlocker b1(lineEditName);
  QSignalBlocker b2(lineEditUser);
  QSignalBlocker b3(lineEditPassword);
  QSignalBlocker b4(lineEditServer);
  QSignalBlocker b5(lineEditPort);
  QSignalBlocker b6(lineEditIMAPMailbox);
  QSignalBlocker b7(comboBoxProtocol);

  lineEditName->setText("");
  lineEditUser->setText("");
  lineEditPassword->setText("");
  lineEditServer->setText("");
  lineEditPort->setText("");
  lineEditIMAPMailbox->setText("");

  comboBoxProtocol->setCurrentIndex(-1);
}

void CSetupDialog::save()
{
  qDebug("Save");
  bool ok = true;
  CConfig &cfg = CConfig::instance();
  int proto = comboBoxProtocol->currentData().toInt();
  const QString &mailboxname = lineEditName->text();
  const QString &user = lineEditUser->text();
  const QString &password = lineEditPassword->text();
  const QString &server = lineEditServer->text();
  const QString &imap_mailbox = lineEditIMAPMailbox->text();
  uint16_t port = lineEditPort->text().toInt(&ok);

  if (inputOk())
  {
    cfg.addConfig(mailboxname, (PROTOCOLS)proto, user, server,
                  port, imap_mailbox);
    cfg.setPassword(mailboxname, password);
    QList<QListWidgetItem *> items = listWidgetServers->findItems(mailboxname, Qt::MatchExactly);
    if (items.size() == 0)
    {
      listWidgetServers->addItem(mailboxname);
      listWidgetServers->sortItems();
    }
  }
}

void CSetupDialog::on_toolButtonServerAdd_released()
{
  qDebug("AddServer");
  save();
  clear();
}
void CSetupDialog::on_toolButtonServerDelete_released()
{
  qDebug("DeleteServer");
  CConfig &cfg = CConfig::instance();
  cfg.deleteConfig(lineEditName->text());
  QList<QListWidgetItem *> items = listWidgetServers->findItems(lineEditName->text(), Qt::MatchExactly);
  clear();
  qDeleteAll(items);
}

void CSetupDialog::on_listWidgetServers_itemSelectionChanged()
{
  const QListWidgetItem *curr = listWidgetServers->currentItem();
  if (curr == nullptr)
  {
    return;
  }
  if (curr->isSelected())
  {
    save();
    displayMailBox(listWidgetServers->currentItem()->text());
    on_InputChanged("");
  }
}

void CSetupDialog::on_comboBoxProtocol_currentIndexChanged(int idx)
{
  if (idx >= 0)
  {
    lineEditPort->setText(QString::number(default_ports[idx]));
    on_InputChanged("");
  }
}

void CSetupDialog::on_InputChanged(const QString &text)
{
  Q_UNUSED(text);
  int proto = comboBoxProtocol->currentData().toInt();
  if ((proto >= PROTO_IMAP4) && (proto <= PROTO_IMAPS))
  {
    lineEditIMAPMailbox->setEnabled(true);
  }
  else
  {
    lineEditIMAPMailbox->setText("");
    lineEditIMAPMailbox->setEnabled(false);
  }
  bool ok = inputOk();
  toolButtonServerAdd->setEnabled(ok);
  toolButtonServerDelete->setEnabled(ok);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

bool CSetupDialog::getIcon(QIcon &icon, const IconType &icontype)
{
  bool loaded = false;
  CConfig &cfg = CConfig::instance();
  QFileDialog dialog(this, tr("Select Icon"), cfg.getIconDir(),
                     tr("Image Files (*.png *.jpg *.bmp)"));

  dialog.setFileMode(QFileDialog::ExistingFile);

  if (dialog.exec())
  {
    QStringList fileNames = dialog.selectedFiles();
    QDir dir = dialog.directory();
    cfg.saveIconDir(dir.path());
    QString name = fileNames[0];
    QPixmap p;
    if (p.load(name))
    {
      cfg.setIconName(icontype, name);
      icon = QIcon(p);
      loaded = true;
    }
  }
  return loaded;
}

void CSetupDialog::updateIcon(QToolButton *button, const IconType &icontype)
{
  QIcon i;
  CConfig &cfg = CConfig::instance();

  if (getIcon(i, icontype))
  {
    cfg.setIcon(icontype, i);
    button->setIcon(i);
  }
}

void CSetupDialog::on_toolButtonNewMail_released()
{
  updateIcon(toolButtonNewMail, IconType::icNewMail);
}

void CSetupDialog::on_toolButtonNoMail_released()
{
  updateIcon(toolButtonNoMail, IconType::icNoMail);
}

void CSetupDialog::on_toolButtonNoMailbox_released()
{
  updateIcon(toolButtonNoMailbox, IconType::icNoMailbox);
}

void CSetupDialog::on_toolButtonOldMail_released()
{
  updateIcon(toolButtonOldMail, IconType::icOldMail);
}

void CSetupDialog::on_toolButtonStopped_released()
{
  updateIcon(toolButtonStopped, IconType::icStopped);
}

void CSetupDialog::AddResetMenu(QToolButton *toolButton, const IconType &type)
{
  QVariant v(static_cast<int>(type));
  QAction *a = new QAction(tr("Reset"));
  a->setData(v);
  m_Actions.append(a); // Save action for later free.
  QMenu *m = new QMenu(toolButton);
  m->addAction(a);
  toolButton->setMenu(m);
  connect(m, SIGNAL(triggered(QAction *)), this,
          SLOT(on_Reset(QAction *)));
}

void CSetupDialog::on_Reset(QAction *action)
{
  bool ok;
  QVariant v = action->data();
  IconType type = static_cast<IconType>(v.toInt(&ok));
  qDebug() << "on_Reset" << static_cast<int>(type);

  CConfig &cfg = CConfig::instance();
  QIcon i = cfg.ResetIcon(type);
  switch (type)
  {
  case IconType::icNewMail:
    toolButtonNewMail->setIcon(i);
    break;
  case IconType::icNoMail:
    toolButtonNoMail->setIcon(i);
    break;
  case IconType::icNoMailbox:
    toolButtonNoMailbox->setIcon(i);
    break;
  case IconType::icOldMail:
    toolButtonOldMail->setIcon(i);
    break;
  case IconType::icStopped:
    toolButtonStopped->setIcon(i);
    break;
  }
}

CSetupDialog::~CSetupDialog()
{
  for (const auto &i : m_Actions)
  {
    delete i;
  }
}
