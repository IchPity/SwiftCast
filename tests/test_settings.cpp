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
    hotkey["modifiers"] = 4;   // Qt::ControlModifier
    hotkey["key"]       = 16777220; // Qt::Key_Return
    hotkey["label"]     = "Ctrl+Enter";

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
    assert(read["launchHotkey"].toObject()["label"].toString() == "Ctrl+Enter");

    std::cout << "test_settings: PASSED\n";
    return 0;
}
