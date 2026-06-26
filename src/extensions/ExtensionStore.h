#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QList>
#include <QString>

namespace SwiftCast {

struct ExtensionInfo {
    QString id;
    QString name;
    QString description;
    QString version;
    QString author;
    QString downloadUrl;
    bool    installed = false;
};

class ExtensionStore : public QObject {
    Q_OBJECT

public:
    explicit ExtensionStore(const QString& installDir, QObject* parent = nullptr);

    void fetchCatalog();
    void install(const ExtensionInfo& info);
    void uninstall(const QString& extensionId);

signals:
    void catalogReady(const QList<ExtensionInfo>& extensions);
    void installProgress(const QString& id, int percent);
    void installFinished(const QString& id, bool success, const QString& error);
    void fetchFailed(const QString& reason);

private slots:
    void onCatalogReply();
    void onDownloadProgress(qint64 received, qint64 total);
    void onDownloadFinished();

private:
    QNetworkAccessManager* m_nam;
    QString                m_installDir;
    QString                m_pendingId;

    static constexpr const char* k_catalogUrl =
        "https://raw.githubusercontent.com/IchPity/SwiftCast/main/extensions/catalog.json";
};

} // namespace SwiftCast
