#include "HotkeyManager.h"
#include <QApplication>
#include <Qt>

#ifdef Q_OS_WIN
#  include <windows.h>
#endif

namespace SwiftCast {

HotkeyManager::HotkeyManager(QObject* parent)
    : QObject(parent)
{
    qApp->installNativeEventFilter(this);
}

HotkeyManager::~HotkeyManager()
{
    unregisterAll();
    qApp->removeNativeEventFilter(this);
}

bool HotkeyManager::registerLaunchHotkey(int qtModifiers, int qtKey)
{
    unregisterNative(static_cast<int>(HotkeyId::Launch));
    return registerNative(static_cast<int>(HotkeyId::Launch), qtModifiers, qtKey);
}

bool HotkeyManager::registerSettingsHotkey(int qtModifiers, int qtKey)
{
    unregisterNative(static_cast<int>(HotkeyId::Settings));
    return registerNative(static_cast<int>(HotkeyId::Settings), qtModifiers, qtKey);
}

void HotkeyManager::unregisterAll()
{
    for (int id : m_registered.keys()) {
        unregisterNative(id);
    }
}

bool HotkeyManager::registerNative(int id, int qtModifiers, int qtKey)
{
#ifdef Q_OS_WIN
    unsigned int mods = toWin32Modifiers(qtModifiers);
    unsigned int vk   = toVirtualKey(qtKey);
    if (vk == 0) return false;
    bool ok = ::RegisterHotKey(nullptr, id, mods, vk);
    if (ok) m_registered[id] = true;
    return ok;
#else
    Q_UNUSED(id) Q_UNUSED(qtModifiers) Q_UNUSED(qtKey)
    return false;
#endif
}

void HotkeyManager::unregisterNative(int id)
{
#ifdef Q_OS_WIN
    if (m_registered.contains(id)) {
        ::UnregisterHotKey(nullptr, id);
        m_registered.remove(id);
    }
#else
    Q_UNUSED(id)
#endif
}

bool HotkeyManager::nativeEventFilter(const QByteArray& eventType,
                                       void* message,
                                       qintptr* result)
{
    Q_UNUSED(result)
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            int id = static_cast<int>(msg->wParam);
            if (id == static_cast<int>(HotkeyId::Launch)) {
                emit launchTriggered();
                return true;
            }
            if (id == static_cast<int>(HotkeyId::Settings)) {
                emit settingsTriggered();
                return true;
            }
        }
    }
#else
    Q_UNUSED(eventType) Q_UNUSED(message)
#endif
    return false;
}

unsigned int HotkeyManager::toWin32Modifiers(int qtModifiers)
{
    unsigned int mods = MOD_NOREPEAT;
    if (qtModifiers & Qt::ControlModifier) mods |= MOD_CONTROL;
    if (qtModifiers & Qt::AltModifier)     mods |= MOD_ALT;
    if (qtModifiers & Qt::ShiftModifier)   mods |= MOD_SHIFT;
    if (qtModifiers & Qt::MetaModifier)    mods |= MOD_WIN;
    return mods;
}

unsigned int HotkeyManager::toVirtualKey(int qtKey)
{
    // Function keys
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F24)
        return static_cast<unsigned int>(VK_F1 + (qtKey - Qt::Key_F1));

    // Letters
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z)
        return static_cast<unsigned int>('A' + (qtKey - Qt::Key_A));

    // Digits
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9)
        return static_cast<unsigned int>('0' + (qtKey - Qt::Key_0));

    switch (qtKey) {
        case Qt::Key_Return:    return VK_RETURN;
        case Qt::Key_Enter:     return VK_RETURN;
        case Qt::Key_Space:     return VK_SPACE;
        case Qt::Key_Escape:    return VK_ESCAPE;
        case Qt::Key_Tab:       return VK_TAB;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Delete:    return VK_DELETE;
        case Qt::Key_Insert:    return VK_INSERT;
        case Qt::Key_Home:      return VK_HOME;
        case Qt::Key_End:       return VK_END;
        case Qt::Key_PageUp:    return VK_PRIOR;
        case Qt::Key_PageDown:  return VK_NEXT;
        case Qt::Key_Left:      return VK_LEFT;
        case Qt::Key_Right:     return VK_RIGHT;
        case Qt::Key_Up:        return VK_UP;
        case Qt::Key_Down:      return VK_DOWN;
        case Qt::Key_Print:     return VK_PRINT;
        case Qt::Key_Pause:     return VK_PAUSE;
        case Qt::Key_CapsLock:  return VK_CAPITAL;
        case Qt::Key_NumLock:   return VK_NUMLOCK;
        case Qt::Key_ScrollLock:return VK_SCROLL;
        default:                return 0;
    }
}

} // namespace SwiftCast
