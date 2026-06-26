#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <memory>

namespace SwiftCast {

class HotkeyManager;
class LauncherWindow;
class Settings;
class Updater;
class ExtensionLoader;

class Application : public QObject {
    Q_OBJECT

public:
    explicit Application(QObject* parent = nullptr);
    ~Application();

    bool initialize();

private slots:
    void onLaunchHotkey();
    void onSettingsHotkey();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onUpdateAvailable(const QString& version, const QString& url,
                           const QString& notes);
    void showSettingsDialog();
    void quit();

private:
    void setupTrayIcon();
    void applyStartupRegistry();

    std::unique_ptr<Settings>         m_settings;
    std::unique_ptr<HotkeyManager>    m_hotkeyManager;
    std::unique_ptr<LauncherWindow>   m_launcherWindow;
    std::unique_ptr<Updater>          m_updater;
    std::unique_ptr<ExtensionLoader>  m_extensionLoader;

    QSystemTrayIcon* m_trayIcon = nullptr;
    QMenu*           m_trayMenu = nullptr;
};

} // namespace SwiftCast
