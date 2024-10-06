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
#include <QDebug>

QByteArray CCrypt::hmac_md5(const QString &k, const QString &text)
{
  char k_ipad[65]; /* inner padding key XORd with ipad */
  char k_opad[65]; /* outer padding key XORd with opad */
  // If the original key is too long, the new key will be a hash of
  // the original key.  Then the new key might have NULL bytes in it,
  // so we can't use QCString
  QByteArray key = k.toLocal8Bit();

  QCryptographicHash context(
      QCryptographicHash::Md5); // for calculating MD5 sums
  // KMD5::Digest digest;  // intermediate storage for MD5 sums
  QByteArray digest;

  // if key is longer than 64 bytes reset it to key=MD5(key)
  if (key.size() > 64)
  {
    key = QCryptographicHash::hash(key, QCryptographicHash::Md5);
  }

  /* the HMAC-MD5 transform looks like this:
   *
   * MD5(K XOR opad, MD5(K XOR ipad, text))
   *
   * where K is an n byte key
   * ipad is the byte 0x36 repeated 64 times
   * opad is the byte 0x5c repeated 64 times
   * text is the data being protected
   */

  // XOR key with ipad and opad values, copying
  // the pad values after the key's end

  int key_len = key.size();

  bzero(k_ipad, sizeof k_ipad);
  bzero(k_opad, sizeof k_opad);
  bcopy(key.data(), k_ipad, key_len);
  bcopy(key.data(), k_opad, key_len);
  for (int i = 0; i < 64; i++)
  {
    k_ipad[i] ^= 0x36;
    k_opad[i] ^= 0x5c;
  }

  // perform inner MD5
  QByteArray data(k_ipad, 64);     // start with inner pad
  data.append(text.toLocal8Bit()); // then text of datagram
  context.reset();                 // init context for 1st pass
  context.addData(data);           // Add data
  // context.rawDigest(digest);           // finish up 1st pass
  digest = context.result();

  // perform outer MD5
  context.reset(); // init context for 2nd pass
  data.clear();
  data.append(k_opad, 64); // start with outer pad
  data.append(digest, 16); // then results of 1st hash
  context.addData(data);   // start with outer pad

  return context.result(); // finish up 2nd pass and return
}

QString CCrypt::cram_md5(const QString &user, const QString &password,
                         const QString &digest)
{
  QString result = QByteArray::fromBase64(digest.toLatin1(),
                                          QByteArray::Base64Encoding);
  qDebug() << "digest_md5: " << result;
  QByteArray response = user.toLocal8Bit() + " " + hmac_md5(password, result).toHex();
  qDebug() << "response " << response;
  response = response.toBase64();
  return response;
}
