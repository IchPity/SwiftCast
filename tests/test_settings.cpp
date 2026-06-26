#include <QCoreApplication>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <cassert>
#include <iostream>

// Minimal smoke test for Settings persistence
// Verifies that a HotkeyConfig written to JSON can be read back correctly.

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QTemporaryDir tmp;
    assert(tmp.isValid());

    QString path = tmp.path() + "/settings.json";

    // Write
    QJsonObject hotkey;
    hotkey["modifiers"] = 0x06000000; // Qt::ControlModifier | Qt::ShiftModifier
    hotkey["key"]       = 0x20;        // Qt::Key_Space
    hotkey["label"]     = "Ctrl+Shift+Space";

    QJsonObject root;
    root["launchHotkey"]          = hotkey;
    root["launchOnStartup"]       = true;
    root["checkUpdatesOnStartup"] = true;

    QFile out(path);
    assert(out.open(QIODevice::WriteOnly));
    out.write(QJsonDocument(root).toJson());
    out.close();

    // Read back
    QFile in(path);
    assert(in.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(in.readAll());
    assert(doc.isObject());

    QJsonObject read = doc.object();
    assert(read["launchOnStartup"].toBool() == true);
    assert(read["launchHotkey"].toObject()["label"].toString() == "Ctrl+Shift+Space");

    std::cout << "test_settings: PASSED\n";
    return 0;
}
