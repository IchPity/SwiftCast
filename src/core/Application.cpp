#include "Application.h"
#include "Settings.h"
#include "HotkeyManager.h"
#include "Updater.h"
#include "../ui/LauncherWindow.h"
#include "../ui/SettingsDialog.h"
#include "../extensions/ExtensionLoader.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>

#ifdef Q_OS_WIN
#  include <windows.h>
#  include <shlobj.h>
#endif

namespace SwiftCast {

Application::Application(QObject* parent)
    : QObject(parent)
{}

Application::~Application() = default;

bool Application::initialize()
{
    // Settings first
    m_settings = std::make_unique<Settings>(this);
    m_settings->load();

    // Extension loader
    m_extensionLoader = std::make_unique<ExtensionLoader>(this);
    m_extensionLoader->loadAll(m_settings->extensionsDirectory());

    // Main launcher window
    m_launcherWindow = std::make_unique<LauncherWindow>(m_extensionLoader.get());

    // Hotkeys
    m_hotkeyManager = std::make_unique<HotkeyManager>(this);
    const HotkeyConfig& lh = m_settings->launchHotkey();
    const HotkeyConfig& sh = m_settings->settingsHotkey();

    if (!m_hotkeyManager->registerLaunchHotkey(lh.modifiers, lh.key)) {
        QMessageBox::warning(nullptr, "SwiftCast",
            "Could not register launch hotkey (" + lh.label + ").\n"
            "Another application may be using it. You can change it via F12.");
    }
    m_hotkeyManager->registerSettingsHotkey(sh.modifiers, sh.key);

    connect(m_hotkeyManager.get(), &HotkeyManager::launchTriggered,
            this, &Application::onLaunchHotkey);
    connect(m_hotkeyManager.get(), &HotkeyManager::settingsTriggered,
            this, &Application::onSettingsHotkey);

    // Also F12 while launcher is visible opens hotkey editor
    connect(m_launcherWindow.get(), &LauncherWindow::settingsRequested,
            this, &Application::onSettingsHotkey);

    // System tray
    setupTrayIcon();

    // Startup registry entry
    if (m_settings->launchOnStartup())
        applyStartupRegistry();

    // Auto-update check (delayed 5s to let UI settle)
    if (m_settings->checkUpdatesOnStartup()) {
        m_updater = std::make_unique<Updater>(this);
        connect(m_updater.get(), &Updater::updateAvailable,
                this, &Application::onUpdateAvailable);
        QTimer::singleShot(5000, this, [this]{ m_updater->checkForUpdates(); });
    }

    return true;
}

void Application::onLaunchHotkey()
{
    if (m_launcherWindow->isVisible()) {
        m_launcherWindow->hide();
    } else {
        m_launcherWindow->popup();
    }
}

void Application::onSettingsHotkey()
{
    bool wasVisible = m_launcherWindow->isVisible();
    if (wasVisible) m_launcherWindow->hide();

    auto* dlg = new SettingsDialog(m_settings.get(), m_hotkeyManager.get());
    connect(dlg, &SettingsDialog::accepted, this, [this]{
        m_settings->save();
        // Re-register hotkeys after change
        const HotkeyConfig& lh = m_settings->launchHotkey();
        const HotkeyConfig& sh = m_settings->settingsHotkey();
        m_hotkeyManager->registerLaunchHotkey(lh.modifiers, lh.key);
        m_hotkeyManager->registerSettingsHotkey(sh.modifiers, sh.key);
        applyStartupRegistry();
    });
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void Application::showSettingsDialog()
{
    onSettingsHotkey();
}

void Application::onUpdateAvailable(const QString& version,
                                     const QString& url,
                                     const QString& notes)
{
    Q_UNUSED(notes)
    m_trayIcon->showMessage(
        "SwiftCast Update Available",
        QString("Version %1 is available. Click to download.").arg(version),
        QSystemTrayIcon::Information,
        8000
    );

    if (!url.isEmpty()) {
        connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this,
                [url]{ QDesktopServices::openUrl(QUrl(url)); },
                Qt::SingleShotConnection);
    }
}

void Application::setupTrayIcon()
{
    m_trayMenu = new QMenu();
    m_trayMenu->addAction("Open SwiftCast", this, &Application::onLaunchHotkey);
    m_trayMenu->addAction("Settings (F12)", this, &Application::showSettingsDialog);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("Quit", this, &Application::quit);

    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/logo.svg"), this);
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->setToolTip("SwiftCast");
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &Application::onTrayActivated);
}

void Application::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
        onLaunchHotkey();
}

void Application::applyStartupRegistry()
{
#ifdef Q_OS_WIN
    const QString appPath = QApplication::applicationFilePath().replace('/', '\\');
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (m_settings->launchOnStartup()) {
            std::wstring val = appPath.toStdWString();
            RegSetValueExW(hKey, L"SwiftCast", 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(val.c_str()),
                           static_cast<DWORD>((val.size() + 1) * sizeof(wchar_t)));
        } else {
            RegDeleteValueW(hKey, L"SwiftCast");
        }
        RegCloseKey(hKey);
    }
#endif
}

void Application::quit()
{
    m_trayIcon->hide();
    QApplication::quit();
}

} // namespace SwiftCast
