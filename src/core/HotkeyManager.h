#pragma once
#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QMap>

namespace SwiftCast {

enum class HotkeyId {
    Launch   = 1,
    Settings = 2
};

class HotkeyManager : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit HotkeyManager(QObject* parent = nullptr);
    ~HotkeyManager();

    bool registerLaunchHotkey(int qtModifiers, int qtKey);
    bool registerSettingsHotkey(int qtModifiers, int qtKey);
    void unregisterAll();

    bool nativeEventFilter(const QByteArray& eventType,
                           void* message,
                           qintptr* result) override;

signals:
    void launchTriggered();
    void settingsTriggered();

private:
    bool registerNative(int id, int qtModifiers, int qtKey);
    void unregisterNative(int id);

    static unsigned int toWin32Modifiers(int qtModifiers);
    static unsigned int toVirtualKey(int qtKey);

    QMap<int, bool> m_registered;
};

} // namespace SwiftCast
