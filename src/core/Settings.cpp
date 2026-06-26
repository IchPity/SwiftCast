#include "Settings.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <Qt>

namespace SwiftCast {

Settings* Settings::s_instance = nullptr;

Settings::Settings(QObject* parent)
    : QObject(parent)
{
    s_instance = this;

    // Defaults: Ctrl+Enter to launch
    m_launchHotkey.modifiers = Qt::ControlModifier;
    m_launchHotkey.key       = Qt::Key_Return;
    m_launchHotkey.label     = "Ctrl+Enter";

    // Defaults: F12 to open settings/hotkey editor
    m_settingsHotkey.modifiers = 0;
    m_settingsHotkey.key       = Qt::Key_F12;
    m_settingsHotkey.label     = "F12";
}

Settings& Settings::instance()
{
    Q_ASSERT(s_instance);
    return *s_instance;
}

QString Settings::settingsFilePath() const
{
    return dataDirectory() + "/settings.json";
}

QString Settings::dataDirectory() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path;
}

QString Settings::extensionsDirectory() const
{
    QString path = dataDirectory() + "/extensions";
    QDir().mkpath(path);
    return path;
}

static QJsonObject hotkeyToJson(const HotkeyConfig& cfg)
{
    QJsonObject obj;
    obj["modifiers"] = cfg.modifiers;
    obj["key"]       = cfg.key;
    obj["label"]     = cfg.label;
    return obj;
}

static HotkeyConfig hotkeyFromJson(const QJsonObject& obj, const HotkeyConfig& fallback)
{
    if (obj.isEmpty()) return fallback;
    HotkeyConfig cfg;
    cfg.modifiers = obj["modifiers"].toInt(fallback.modifiers);
    cfg.key       = obj["key"].toInt(fallback.key);
    cfg.label     = obj["label"].toString(fallback.label);
    return cfg;
}

void Settings::load()
{
    QFile file(settingsFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();

    m_launchHotkey   = hotkeyFromJson(root["launchHotkey"].toObject(),   m_launchHotkey);
    m_settingsHotkey = hotkeyFromJson(root["settingsHotkey"].toObject(), m_settingsHotkey);
    m_launchOnStartup = root["launchOnStartup"].toBool(true);
    m_checkUpdates    = root["checkUpdatesOnStartup"].toBool(true);
}

void Settings::save()
{
    QJsonObject root;
    root["launchHotkey"]           = hotkeyToJson(m_launchHotkey);
    root["settingsHotkey"]         = hotkeyToJson(m_settingsHotkey);
    root["launchOnStartup"]        = m_launchOnStartup;
    root["checkUpdatesOnStartup"]  = m_checkUpdates;

    QFile file(settingsFilePath());
    if (!file.open(QIODevice::WriteOnly)) return;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));

    emit settingsChanged();
}

HotkeyConfig Settings::launchHotkey() const   { return m_launchHotkey; }
HotkeyConfig Settings::settingsHotkey() const { return m_settingsHotkey; }

void Settings::setLaunchHotkey(const HotkeyConfig& cfg)
{
    m_launchHotkey = cfg;
}

void Settings::setSettingsHotkey(const HotkeyConfig& cfg)
{
    m_settingsHotkey = cfg;
}

bool Settings::launchOnStartup() const      { return m_launchOnStartup; }
void Settings::setLaunchOnStartup(bool v)   { m_launchOnStartup = v; }

bool Settings::checkUpdatesOnStartup() const    { return m_checkUpdates; }
void Settings::setCheckUpdatesOnStartup(bool v) { m_checkUpdates = v; }

} // namespace SwiftCast
