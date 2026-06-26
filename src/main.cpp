#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "core/Application.h"

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
            "System tray is not available on this system.\n"
            "SwiftCast requires a system tray to run.");
        return 1;
    }

    SwiftCast::Application swiftCastApp;
    if (!swiftCastApp.initialize()) {
        return 1;
    }

    return app.exec();
}
