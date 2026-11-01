#include <QDebug>

#include "CKeyChain.h"

CKeyChain::CKeyChain(QObject *parent) : QObject(parent)
{
}

void CKeyChain::readKey(const QString &key)
{
    auto *job = new QKeychain::ReadPasswordJob(QLatin1String("TrayBiff"), this);
    job->setAutoDelete(true);
    job->setKey(key);

    QObject::connect(job, &QKeychain::ReadPasswordJob::finished, this, [this, job, key]()
                     {
        if (job->error()) {
            if (job->error() == QKeychain::EntryNotFound) {
                emit keyRestored(key, QString());
                return;
            }
            emit error(tr("Read key failed: %1").arg(qPrintable(job->errorString())));
            return;
        }
        emit keyRestored(key, job->textData()); });

    job->start();
}

void CKeyChain::writeKey(const QString &key, const QString &value)
{
    auto *job = new QKeychain::WritePasswordJob(QLatin1String("TrayBiff"), this);
    job->setAutoDelete(true);
    job->setKey(key);

    QObject::connect(job, &QKeychain::WritePasswordJob::finished, this, [this, job, key]()
                     {
        if (job->error()) {
            emit error(tr("Write key failed: %1").arg(qPrintable(job->errorString())));
            return;
        }

        emit keyStored(key); });

    job->setTextData(value);
    job->start();
}

void CKeyChain::deleteKey(const QString &key)
{
    auto *job = new QKeychain::DeletePasswordJob(QLatin1String("TrayBiff"), this);
    job->setAutoDelete(true);
    job->setKey(key);

    QObject::connect(job, &QKeychain::DeletePasswordJob::finished, this, [this, job, key]()
                     {
        if (job->error()) {
            emit error(tr("Delete key failed: %1").arg(qPrintable(job->errorString())));
            return;
        }
        emit keyDeleted(key); });

    job->start();
}
