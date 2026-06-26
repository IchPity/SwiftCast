#pragma once
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QKeySequence>
#include "../core/Settings.h"

namespace SwiftCast {

class HotkeyManager;

// Widget that captures a key combination when clicked
class KeyCapture : public QPushButton {
    Q_OBJECT
public:
    explicit KeyCapture(QWidget* parent = nullptr);

    HotkeyConfig currentConfig() const;
    void setConfig(const HotkeyConfig& cfg);
    void reset();

signals:
    void captureChanged();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;

private:
    HotkeyConfig m_config;
    bool         m_capturing = false;
};

class HotkeyEditor : public QDialog {
    Q_OBJECT

public:
    explicit HotkeyEditor(Settings* settings,
                          HotkeyManager* hotkeys,
                          QWidget* parent = nullptr);

private slots:
    void save();

private:
    void buildUi();

    Settings*      m_settings;
    HotkeyManager* m_hotkeys;
    KeyCapture*    m_launchCapture   = nullptr;
    KeyCapture*    m_settingsCapture = nullptr;
};

} // namespace SwiftCast
