#include "ExtensionStore.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>

namespace SwiftCast {

ExtensionStore::ExtensionStore(const QString& installDir, QObject* parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_installDir(installDir)
{}

void ExtensionStore::fetchCatalog()
{
    QNetworkRequest req{QUrl(k_catalogUrl)};
    req.setRawHeader("User-Agent", "SwiftCast");
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, &ExtensionStore::onCatalogReply);
}

void ExtensionStore::onCatalogReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit fetchFailed(reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (!doc.isArray()) {
        emit fetchFailed("Invalid catalog format");
        return;
    }

    QList<ExtensionInfo> infos;
    for (const auto& val : doc.array()) {
        QJsonObject obj = val.toObject();
        ExtensionInfo info;
        info.id          = obj["id"].toString();
        info.name        = obj["name"].toString();
        info.description = obj["description"].toString();
        info.version     = obj["version"].toString();
        info.author      = obj["author"].toString();
        info.downloadUrl = obj["downloadUrl"].toString();
        info.installed   = QFile::exists(m_installDir + "/" + info.id + ".dll");
        infos.append(info);
    }

    emit catalogReady(infos);
}

void ExtensionStore::install(const ExtensionInfo& info)
{
    QDir().mkpath(m_installDir);
    m_pendingId = info.id;

    QNetworkRequest req{QUrl(info.downloadUrl)};
    req.setRawHeader("User-Agent", "SwiftCast");
    QNetworkReply* reply = m_nam->get(req);

    connect(reply, &QNetworkReply::downloadProgress,
            this, &ExtensionStore::onDownloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &ExtensionStore::onDownloadFinished);
}

void ExtensionStore::onDownloadProgress(qint64 received, qint64 total)
{
    if (total > 0)
        emit installProgress(m_pendingId,
                             static_cast<int>(received * 100 / total));
}

void ExtensionStore::onDownloadFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit installFinished(m_pendingId, false, reply->errorString());
        return;
    }

    QString dest = m_installDir + "/" + m_pendingId + ".dll";
    QFile file(dest);
    if (!file.open(QIODevice::WriteOnly)) {
        emit installFinished(m_pendingId, false, "Cannot write to " + dest);
        return;
    }
    file.write(reply->readAll());
    file.close();

    emit installFinished(m_pendingId, true, {});
}

void ExtensionStore::uninstall(const QString& extensionId)
{
    QString path = m_installDir + "/" + extensionId + ".dll";
    if (QFile::remove(path))
        emit installFinished(extensionId, true, {});
    else
        emit installFinished(extensionId, false, "Could not remove file");
}

} // namespace SwiftCast
