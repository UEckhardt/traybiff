/*
 * CCrypt.h
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of HMAC MD5 and CRAM MD5
 */

#ifndef CCRYPT_H_
#define CCRYPT_H_

#include <QObject>
#include <QString>

class CCrypt : public QObject
{
  Q_OBJECT
public:
  static QByteArray hmac_md5(const QString &k, const QString &text);
  static QString cram_md5(const QString &user, const QString &password,
                          const QString &challenge);

private:
  CCrypt() {}
  virtual ~CCrypt() {}
};

#endif /* CCRYPT_H_ */
