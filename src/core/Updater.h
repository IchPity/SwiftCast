#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

namespace SwiftCast {

class Updater : public QObject {
    Q_OBJECT

public:
    explicit Updater(QObject* parent = nullptr);

    void checkForUpdates();

    static QString currentVersion();
    static bool isNewer(const QString& remoteVersion);

signals:
    void updateAvailable(const QString& version, const QString& downloadUrl,
                         const QString& releaseNotes);
    void upToDate();
    void checkFailed(const QString& reason);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager* m_nam;
    static constexpr const char* k_releasesUrl =
        "https://api.github.com/repos/IchPity/SwiftCast/releases/latest";
};

} // namespace SwiftCast
