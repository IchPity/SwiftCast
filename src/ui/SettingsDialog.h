#pragma once
#include <QDialog>

namespace SwiftCast {

class Settings;
class HotkeyManager;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(Settings* settings,
                            HotkeyManager* hotkeys,
                            QWidget* parent = nullptr);

private:
    void buildUi();

    Settings*      m_settings;
    HotkeyManager* m_hotkeys;
};

} // namespace SwiftCast
