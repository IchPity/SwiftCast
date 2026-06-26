#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QTimer>
#include "core/Application.h"

// Manual UI test: starts SwiftCast and immediately opens the launcher window.
// Run this executable to test the UI without waiting for the global hotkey.

int main(int argc, char* argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName("SwiftCast");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("SwiftCast");
    app.setOrganizationDomain("swiftcast.app");
    app.setQuitOnLastWindowClosed(false);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "SwiftCast",
            "System tray is not available on this system.");
        return 1;
    }

    SwiftCast::Application swiftCastApp;
    if (!swiftCastApp.initialize()) {
        return 1;
    }

    QTimer::singleShot(0, &swiftCastApp, &SwiftCast::Application::showLauncher);

    return app.exec();
}
