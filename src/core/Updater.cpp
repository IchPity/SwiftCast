#include "Updater.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

namespace SwiftCast {

Updater::Updater(QObject* parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{}

QString Updater::currentVersion()
{
    return QApplication::applicationVersion();
}

bool Updater::isNewer(const QString& remote)
{
    auto split = [](const QString& v) {
        QList<int> parts;
        for (const QString& p : v.split('.'))
            parts.append(p.toInt());
        while (parts.size() < 3) parts.append(0);
        return parts;
    };

    QList<int> cur = split(currentVersion());
    QList<int> rem = split(remote);
    for (int i = 0; i < 3; ++i) {
        if (rem[i] > cur[i]) return true;
        if (rem[i] < cur[i]) return false;
    }
    return false;
}

void Updater::checkForUpdates()
{
    QNetworkRequest req{QUrl(k_releasesUrl)};
    req.setRawHeader("Accept", "application/vnd.github.v3+json");
    req.setRawHeader("User-Agent", "SwiftCast/" + currentVersion().toUtf8());

    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, &Updater::onReplyFinished);
}

void Updater::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit checkFailed(reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (!doc.isObject()) {
        emit checkFailed("Invalid response from server");
        return;
    }

    QJsonObject root = doc.object();
    QString tag = root["tag_name"].toString().remove('v');
    QString notes = root["body"].toString();
    QString downloadUrl;

    QJsonArray assets = root["assets"].toArray();
    for (const auto& a : assets) {
        QJsonObject asset = a.toObject();
        QString name = asset["name"].toString();
        if (name.endsWith(".exe") || name.endsWith(".msi")) {
            downloadUrl = asset["browser_download_url"].toString();
            break;
        }
    }

    if (isNewer(tag)) {
        emit updateAvailable(tag, downloadUrl, notes);
    } else {
        emit upToDate();
    }
}

} // namespace SwiftCast
