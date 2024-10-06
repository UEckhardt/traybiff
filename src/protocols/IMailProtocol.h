/*
 * IMailProtocol.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Interface Class for the implemented mail protocols.
 */

#ifndef IMAILPROTOCOL_H_
#define IMAILPROTOCOL_H_

#include <QObject>
#include <QString>

class IMailProtocol : public QObject
{
  Q_OBJECT
public:
  IMailProtocol() {}
  virtual ~IMailProtocol() {}

  QString getError(void)
  {
    return m_Error;
  }

  QString getServer(void)
  {
    return m_Server;
  }

  void setServer(const QString &server)
  {
    m_Server = server;
  }

  void setConfigurationIndex(int idx)
  {
    m_ConfigurationIdx = idx;
  }

  int getConfigurationIndex(void)
  {
    return m_ConfigurationIdx;
  }

public slots:
  virtual void doWork(void) = 0;

signals:
  void mailError(IMailProtocol *srv, const QString &errtxt);
  void resultReady(int configurationidx, int numUnread, int numRead);

protected:
  QString m_Error;
  QString m_Server;
  int m_ConfigurationIdx = 0;

  void setError(const QString &err)
  {
    m_Error = err;
    emit mailError(this, err);
  }

  void clearError()
  {
    m_Error.clear();
  }
};

#endif /* IMAILPROTOCOL_H_ */
