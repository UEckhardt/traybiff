#ifndef KEYCHAINCLASS_H
#define KEYCHAINCLASS_H

#include <QObject>

#include <keychain.h>

class CKeyChain : public QObject
{
    Q_OBJECT
public:
    CKeyChain(QObject *parent = nullptr);

    void readKey(const QString &key);
    void writeKey(const QString &key, const QString &value);
    void deleteKey(const QString &key);

signals:
    void keyStored(const QString &key);
    void keyRestored(const QString &key, const QString &value);
    void keyDeleted(const QString &key);
    void error(const QString &errorText);

private:
    QKeychain::ReadPasswordJob m_readCredentialJob;
    QKeychain::WritePasswordJob m_writeCredentialJob;
    QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif // KEYCHAINCLASS_H
