/*
 * CCrypt.cpp
 *
 * Copyright (C) 2021-2024 Ulrich Eckhardt <uli@uli-eckhardt.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Implementation of HMAC MD5 and CRAM MD5
 */
#include "CCrypt.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

QByteArray CCrypt::hmac_md5(const QString &k, const QString &text)
{
  return QMessageAuthenticationCode::hash(
      text.toLocal8Bit(), k.toLocal8Bit(), QCryptographicHash::Md5);
}

QString CCrypt::cram_md5(const QString &user, const QString &password,
                         const QString &digest)
{
  QString result = QByteArray::fromBase64(digest.toLatin1(),
                                          QByteArray::Base64Encoding);
  QByteArray response = user.toLocal8Bit() + " " + hmac_md5(password, result).toHex();
  response = response.toBase64();
  return response;
}
