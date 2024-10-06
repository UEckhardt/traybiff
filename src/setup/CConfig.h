/*
 * CConfig.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Class for save/load the configuration.
 */

#ifndef SRC_SETUP_CCONFIG_H_
#define SRC_SETUP_CCONFIG_H_

#include "../traybiff.h"
#include <QDir>
#include <QIcon>
#include <QSettings>
#include <QString>
#include <QVector>

enum class IconType
{
  icNewMail,
  icNoMail,
  icNoMailbox,
  icOldMail,
  icStopped,
  icFirst = icNewMail,
  icLast = icStopped
};

typedef struct
{
  QString m_MailboxName;
  PROTOCOLS m_Protocol;
  QString m_Server;
  uint16_t m_Port;
  QString m_User;
  QString m_Password;
  QString m_ImapMailBox;
} MAILBOX_CONFIG_T;

typedef struct
{
  QVector<MAILBOX_CONFIG_T> m_MailboxConfig;
  QString m_IcTypeFileName[static_cast<int>(IconType::icLast) + 1];
  QIcon m_IcType[static_cast<int>(IconType::icLast) + 1];
} CONFIG_DATA_T;

class CConfig
{
public:
  static CConfig &instance()
  {
    static CConfig instance;
    return instance;
  }
  virtual ~CConfig() {}
  bool isConfigured()
  {
    return m_isConfigured;
  }
  void getMailboxes(QVector<QString> &mailboxes);
  void addConfig(const QString &mailboxname, PROTOCOLS protocol,
                 const QString &user, const QString &password,
                 const QString &server, uint16_t port,
                 const QString &imap_mailbox);
  void deleteConfig(const QString &mailboxname);
  void getConfig(const QString &mailboxname, PROTOCOLS &protocol,
                 QString &user, QString &password, QString &server,
                 uint16_t &port, QString &imap_mailbox);
  void save();
  void beginUpdate();
  void abortUpdate();

  void saveIconDir(const QString &dir);
  QString getIconDir();
  QIcon getIcon(const IconType &type)
  {
    return (m_CurrentConfig.m_IcType[static_cast<int>(type)]);
  }
  void setIcon(const IconType &type, const QIcon &newIcon)
  {
    m_CurrentConfig.m_IcType[static_cast<int>(type)] = newIcon;
  }
  QString getIconName(const IconType &type)
  {
    return m_CurrentConfig.m_IcTypeFileName[static_cast<int>(type)];
  }
  void setIconName(const IconType &type, const QString &newName)
  {
    m_CurrentConfig.m_IcTypeFileName[static_cast<int>(type)] = newName;
  }
  QIcon ResetIcon(const IconType &type);

  int m_PollTime = 0;
  bool m_DockInPanel = false;
  bool m_UseSessionManangement = false;

private:
  CConfig();
  CConfig(const CConfig &);
  CConfig &operator=(const CConfig &);
  int findMailbox(const QString &mailboxname);
  void saveIconName(QSettings &settings, const IconType &type,
                    const QString &key);
  void saveIcon(QIcon &icon, QString &savename, const QString &name);
  QIcon getIconInfo(QSettings &settings, const QString &key,
                    const QString &name, const QIcon &fallback,
                    QString &iconname);
  void LoadIcon(QSettings &settings, const IconType &type);
  bool m_isConfigured = false;

  CONFIG_DATA_T m_CurrentConfig;
  CONFIG_DATA_T m_OldConfig;

  static inline const QString KEY_ICNEWMAIL = "icon_newmail";
  static inline const QString KEY_ICNOMAIL = "icon_nomail";
  static inline const QString KEY_ICNOMAILBOX = "icon_nomailbox";
  static inline const QString KEY_ICOLDMAIL = "icon_oldmail";
  static inline const QString KEY_ICSTOPPED = "icon_stopped";

  // Static data
  static inline const QString IC_KEYS[static_cast<int>(IconType::icLast) + 1] = {KEY_ICNEWMAIL, KEY_ICNOMAIL, KEY_ICNOMAILBOX, KEY_ICOLDMAIL,
                                                                                 KEY_ICSTOPPED};

  static inline const char
      *DEFAULT_THEME_ICONS[static_cast<int>(IconType::icLast) + 1] = {"mail-read", "mail-unread", "dialog-warning", "mail-replied",
                                                                      "network-offline"};

  static inline const QString FALLBACK_ICON = ":/icons/32x32/hi-app-kbiff.png";
  static inline const QString GROUP_MAIN = "main";
  static inline const QString GROUP_MAILBOX = "mailboxes";
  static inline const QString GROUP_SESSION = "session";

  // Mailbox config keys
  static inline const QString ARRAY_MAILBOX = "mailbox";
  static inline const QString KEY_MAILBOX_NAME = "mailboxname";
  static inline const QString KEY_PROTOCOL = "protocol";
  static inline const QString KEY_USER_NAME = "user_name";
  static inline const QString KEY_PASSWORD = "password";
  static inline const QString KEY_SERVER = "server";
  static inline const QString KEY_PORT = "port";
  static inline const QString KEY_IMAP_MAILBOX = "imap_mailbox";

  // Global config keys
  static inline const QString KEY_POLL = "poll";
  static inline const QString KEY_DOCK = "dock";
  static inline const QString KEY_USE_SESSION = "sessionmanagement";

  // Session keys
  static inline const QString KEY_ICON_DIR = "icondir";
};

#endif /* SRC_SETUP_CCONFIG_H_ */
