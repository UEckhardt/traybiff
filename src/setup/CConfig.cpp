/*
 * CConfig.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class for save/load the configuration.
 */

#include "CConfig.h"

CConfig::CConfig() : m_KeyChain(this)
{
  QSettings settings;

  connect(&m_KeyChain, &CKeyChain::keyRestored, this, &CConfig::keyRestored);
  connect(&m_KeyChain, &CKeyChain::keyStored, this, &CConfig::keyStored);
  connect(&m_KeyChain, &CKeyChain::error, this, &CConfig::keyError);

  qInfo() << "Config file " << settings.fileName();

  settings.beginGroup(GROUP_MAIN);
  m_PollTime = settings.value(KEY_POLL, 360).toInt();
  m_DockInPanel = settings.value(KEY_DOCK, false).toBool();
  m_UseSessionManangement = settings.value(KEY_USE_SESSION, false).toBool();

  for (int i = 0; i <= static_cast<int>(IconType::icLast); ++i)
  {
    LoadIcon(settings, static_cast<IconType>(i));
  }

  settings.endGroup();

  settings.beginGroup(GROUP_MAILBOX);
  int servers = settings.beginReadArray(ARRAY_MAILBOX);
  if (servers < 1)
  {
    qWarning("Config Server Array empty");
  }
  else
  {
    m_isConfigured = true;
  }
  for (int j = 0; j < servers; j++)
  {
    settings.setArrayIndex(j);
    const QString &mailboxName = settings.value(KEY_MAILBOX_NAME, QString("")).toString();
    int protocol = settings.value(KEY_PROTOCOL, QVariant((int)PROTO_POP3)).toInt();
    const QString &user = settings.value(KEY_USER_NAME, QString("")).toString();
    const QString &server = settings.value(KEY_SERVER, QString("")).toString();
    const QString &imap_mailbox = settings.value(KEY_IMAP_MAILBOX, QString("")).toString();
    int port = settings.value(KEY_PORT, 0).toInt();
    qInfo() << "Reading Mailbox" << mailboxName << " " << user;
    addConfig(mailboxName, (PROTOCOLS)protocol, user, server,
              port, imap_mailbox);
    getPassword(mailboxName);
  }
  settings.endArray();
  settings.endGroup();
}

void CConfig::keyError(const QString &errorText)
{
  qWarning() << "Error getting password " << errorText;
}

void CConfig::keyStored(const QString &key)
{
  qDebug() << "Key " << key << " stored";
}

void CConfig::keyRestored(const QString &key, const QString &value)
{
  qDebug() << "Restore Passwd " << key;
  int idx = findMailbox(key);
  if (idx == -1)
  {
    qInfo() << "keyRestored " << key << "not found";
    return;
  }
  m_CurrentConfig.m_MailboxConfig[idx].m_Password = value;
  updatePassword(key, value);
}

QIcon CConfig::getIconInfo(QSettings &settings, const QString &key,
                           const QString &name, const QIcon &fallback,
                           QString &iconname)
{
  iconname.clear();
  QString file = settings.value(key, QString("")).toString();
  qDebug() << "getIconInfo " << key << " " << file;
  if (!file.isEmpty())
  {
    QPixmap p;
    if (p.load(file))
    {
      iconname = file;
      return QIcon(p);
    }
  }

  auto ic = QIcon::fromTheme(name);
  if (ic.isNull())
  {
    qWarning() << "Icon " << name << " not found in " << QIcon::themeName()
               << QIcon::themeSearchPaths();
    ic = fallback;
  }

  return ic;
}

void CConfig::LoadIcon(QSettings &settings, const IconType &type)
{
  const QIcon fbicon = QIcon(FALLBACK_ICON);
  assert(!fbicon.isNull());
  QString iconname;
  int tp = static_cast<int>(type);
  auto ic = getIconInfo(settings, IC_KEYS[tp], DEFAULT_THEME_ICONS[tp],
                        fbicon, iconname);
  setIcon(type, ic);
  setIconName(type, iconname);
}

QIcon CConfig::ResetIcon(const IconType &type)
{
  const QIcon fbicon = QIcon(FALLBACK_ICON);
  assert(!fbicon.isNull());
  const auto ic = QIcon::fromTheme(
      DEFAULT_THEME_ICONS[static_cast<int>(type)], fbicon);
  setIcon(type, ic);
  setIconName(type, "");
  return m_CurrentConfig.m_IcType[static_cast<int>(type)];
}

int CConfig::findMailbox(const QString &mailboxname) const
{
  for (int i = 0; i < m_CurrentConfig.m_MailboxConfig.size(); ++i)
  {
    if (m_CurrentConfig.m_MailboxConfig.at(i).m_MailboxName == mailboxname)
    {
      return i;
    }
  }
  return -1;
}

void CConfig::addConfig(const QString &mailboxname, PROTOCOLS protocol,
                        const QString &user, const QString &server, uint16_t port,
                        const QString &imap_mailbox)
{
  MAILBOX_CONFIG_T config;

  if (mailboxname.size() == 0)
  {
    qDebug() << "addConfig empty mailbox name";
    return;
  }
  qDebug() << "addConfig" << mailboxname << user;
  config.m_MailboxName = mailboxname;
  config.m_Protocol = protocol;
  config.m_User = user;
  config.m_Server = server;
  config.m_Port = port;
  config.m_ImapMailBox = imap_mailbox;

  int idx = findMailbox(mailboxname);
  if (idx != -1)
  {
    m_CurrentConfig.m_MailboxConfig[idx] = config;
  }
  else
  {
    m_CurrentConfig.m_MailboxConfig.append(config);
  }
  m_isConfigured = true;
}

void CConfig::deleteConfig(const QString &mailboxname)
{
  int idx = findMailbox(mailboxname);
  if (idx != -1)
  {
    m_KeyChain.deleteKey(mailboxname);
    m_CurrentConfig.m_MailboxConfig.remove(idx);
  }
}

void CConfig::getMailboxes(QVector<QString> &mailboxes) const
{
  mailboxes.clear();
  for (int i = 0; i < m_CurrentConfig.m_MailboxConfig.size(); ++i)
  {
    mailboxes.append(m_CurrentConfig.m_MailboxConfig[i].m_MailboxName);
  }
}

void CConfig::getConfig(const QString &mailboxname, PROTOCOLS &protocol,
                        QString &user, QString &password, QString &server,
                        uint16_t &port, QString &imap_mailbox) const
{
  int idx = findMailbox(mailboxname);
  if (idx == -1)
  {
    qInfo("getConfig %s not found", qUtf8Printable(mailboxname));
    return;
  }
  const MAILBOX_CONFIG_T &cfg = m_CurrentConfig.m_MailboxConfig.at(idx);
  protocol = cfg.m_Protocol;
  user = cfg.m_User;
  password = cfg.m_Password;
  server = cfg.m_Server;
  port = cfg.m_Port;
  imap_mailbox = cfg.m_ImapMailBox;
}

void CConfig::beginUpdate()
{
  m_OldConfig = m_CurrentConfig;
}

void CConfig::abortUpdate()
{
  m_CurrentConfig = m_OldConfig;
}

void CConfig::saveIconName(QSettings &settings, const IconType &type,
                           const QString &key)
{
  QString iconname = getIconName(type);
  if (iconname.isEmpty())
  {
    settings.remove(key);
  }
  else
  {
    settings.setValue(key, iconname);
  }
}

void CConfig::save()
{
  QSettings settings;

  settings.beginGroup(GROUP_MAIN);
  settings.setValue(KEY_POLL, m_PollTime);
  settings.setValue(KEY_DOCK, m_DockInPanel);
  settings.setValue(KEY_USE_SESSION, m_UseSessionManangement);

  saveIconName(settings, IconType::icNewMail, KEY_ICNEWMAIL);
  saveIconName(settings, IconType::icNoMail, KEY_ICNOMAIL);
  saveIconName(settings, IconType::icNoMailbox, KEY_ICNOMAILBOX);
  saveIconName(settings, IconType::icOldMail, KEY_ICOLDMAIL);
  saveIconName(settings, IconType::icStopped, KEY_ICSTOPPED);

  settings.endGroup();

  settings.beginGroup(GROUP_MAILBOX);
  settings.beginWriteArray(ARRAY_MAILBOX);
  for (int i = 0; i < m_CurrentConfig.m_MailboxConfig.size(); ++i)
  {
    const QString &mbName = m_CurrentConfig.m_MailboxConfig.at(i).m_MailboxName;
    settings.setArrayIndex(i);
    settings.setValue(KEY_MAILBOX_NAME, mbName);
    settings.setValue(KEY_PROTOCOL,
                      m_CurrentConfig.m_MailboxConfig.at(i).m_Protocol);
    settings.setValue(KEY_USER_NAME,
                      m_CurrentConfig.m_MailboxConfig.at(i).m_User);
    settings.setValue(KEY_SERVER,
                      m_CurrentConfig.m_MailboxConfig.at(i).m_Server);
    settings.setValue(KEY_PORT,
                      m_CurrentConfig.m_MailboxConfig.at(i).m_Port);
    settings.setValue(KEY_IMAP_MAILBOX,
                      m_CurrentConfig.m_MailboxConfig.at(i).m_ImapMailBox);
    m_KeyChain.writeKey(mbName, m_CurrentConfig.m_MailboxConfig.at(i).m_Password);
  }
  settings.endArray();
  settings.endGroup();
}

void CConfig::saveIconDir(const QString &dir)
{
  QSettings settings;
  settings.beginGroup(GROUP_SESSION);
  settings.setValue(KEY_ICON_DIR, dir);
  settings.endGroup();
}

QString CConfig::getIconDir() const
{
  QSettings settings;
  settings.beginGroup(GROUP_SESSION);
  QString dir = settings.value(KEY_ICON_DIR, QDir::homePath()).toString();
  settings.endGroup();
  return dir;
}

void CConfig::saveIcon(QIcon &icon, QString &savename, const QString &name)
{
  QPixmap p;
  if (p.load(name))
  {
    savename = name;
    icon = QIcon(p);
  }
}
