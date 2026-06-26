#pragma once
#include <QObject>
#include <QJsonObject>
#include <QString>

namespace SwiftCast {

struct HotkeyConfig {
    int modifiers = 0; // Qt::KeyboardModifiers as int
    int key = 0;       // Qt::Key as int
    QString label;
};

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject* parent = nullptr);

    static Settings& instance();

    void load();
    void save();

    // Hotkeys
    HotkeyConfig launchHotkey() const;
    void setLaunchHotkey(const HotkeyConfig& cfg);

    HotkeyConfig settingsHotkey() const;
    void setSettingsHotkey(const HotkeyConfig& cfg);

    // General
    bool launchOnStartup() const;
    void setLaunchOnStartup(bool enabled);

    bool checkUpdatesOnStartup() const;
    void setCheckUpdatesOnStartup(bool enabled);

    QString dataDirectory() const;
    QString extensionsDirectory() const;

signals:
    void settingsChanged();

private:
    QString settingsFilePath() const;

    HotkeyConfig m_launchHotkey;
    HotkeyConfig m_settingsHotkey;
    bool m_launchOnStartup = true;
    bool m_checkUpdates = true;

    static Settings* s_instance;
};

} // namespace SwiftCast
