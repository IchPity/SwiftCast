#include "HotkeyEditor.h"
#include "../core/HotkeyManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

namespace SwiftCast {

// ── KeyCapture ───────────────────────────────────────────────────────────────

KeyCapture::KeyCapture(QWidget* parent)
    : QPushButton(parent)
{
    setObjectName("KeyBadge");
    setFocusPolicy(Qt::StrongFocus);
    setText("Click to set…");
    setCheckable(true);
    setMinimumWidth(160);
    setFixedHeight(34);
    setFlat(true);
}

void KeyCapture::setConfig(const HotkeyConfig& cfg)
{
    m_config = cfg;
    setText(cfg.label.isEmpty() ? "Not set" : cfg.label);
    setChecked(false);
    m_capturing = false;
}

HotkeyConfig KeyCapture::currentConfig() const
{
    return m_config;
}

void KeyCapture::reset()
{
    m_config = {};
    setText("Click to set…");
    setChecked(false);
    m_capturing = false;
}

void KeyCapture::focusInEvent(QFocusEvent* e)
{
    QPushButton::focusInEvent(e);
    m_capturing = true;
    setChecked(true);
    setText("Press keys…");
}

void KeyCapture::focusOutEvent(QFocusEvent* e)
{
    QPushButton::focusOutEvent(e);
    m_capturing = false;
    setChecked(false);
    if (m_config.label.isEmpty())
        setText("Click to set…");
    else
        setText(m_config.label);
}

void KeyCapture::keyPressEvent(QKeyEvent* e)
{
    if (!m_capturing) { QPushButton::keyPressEvent(e); return; }

    // Ignore standalone modifiers
    int key = e->key();
    if (key == Qt::Key_Control || key == Qt::Key_Alt ||
        key == Qt::Key_Shift   || key == Qt::Key_Meta) {
        return;
    }
    if (key == Qt::Key_Escape) {
        reset();
        clearFocus();
        return;
    }

    Qt::KeyboardModifiers mods = e->modifiers();

    QStringList parts;
    if (mods & Qt::ControlModifier) parts << "Ctrl";
    if (mods & Qt::AltModifier)     parts << "Alt";
    if (mods & Qt::ShiftModifier)   parts << "Shift";
    if (mods & Qt::MetaModifier)    parts << "Win";

    QString keyName = QKeySequence(key).toString();
    if (!keyName.isEmpty()) parts << keyName;

    m_config.modifiers = static_cast<int>(mods);
    m_config.key       = key;
    m_config.label     = parts.join("+");

    setText(m_config.label);
    m_capturing = false;
    setChecked(false);
    clearFocus();
    emit captureChanged();
}

// ── HotkeyEditor ─────────────────────────────────────────────────────────────

HotkeyEditor::HotkeyEditor(Settings* settings, HotkeyManager* hotkeys, QWidget* parent)
    : QDialog(parent)
    , m_settings(settings)
    , m_hotkeys(hotkeys)
{
    setWindowTitle("SwiftCast · Keyboard Shortcuts");
    setObjectName("HotkeyEditor");
    setModal(true);
    setFixedSize(440, 300);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    buildUi();

    // Load stylesheet
    QFile qss(":/styles/dark.qss");
    if (qss.open(QIODevice::ReadOnly))
        setStyleSheet(qss.readAll());
}

void HotkeyEditor::buildUi()
{
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 20, 20, 20);
    outer->setSpacing(16);

    auto* title = new QLabel("Keyboard Shortcuts", this);
    title->setObjectName("TitleLabel");
    outer->addWidget(title);

    auto* sub = new QLabel("Click a field and press your desired key combination.", this);
    sub->setStyleSheet("color: #6D5FA0; font-size: 12px;");
    sub->setWordWrap(true);
    outer->addWidget(sub);

    // ── Rows ──
    auto* grid = new QGridLayout();
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 0);
    grid->setSpacing(12);

    auto addRow = [&](int row, const QString& label,
                      KeyCapture*& capture, const HotkeyConfig& cfg) {
        auto* lbl = new QLabel(label, this);
        lbl->setStyleSheet("color: #C4B8E0; font-size: 13px;");
        capture = new KeyCapture(this);
        capture->setConfig(cfg);
        grid->addWidget(lbl,     row, 0, Qt::AlignVCenter);
        grid->addWidget(capture, row, 1, Qt::AlignRight);
    };

    addRow(0, "Open / Close SwiftCast",  m_launchCapture,   m_settings->launchHotkey());
    addRow(1, "Open Shortcut Settings",  m_settingsCapture, m_settings->settingsHotkey());

    outer->addLayout(grid);
    outer->addStretch();

    // ── Buttons ──
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    auto* resetBtn = new QPushButton("Reset to Defaults", this);
    resetBtn->setObjectName("ResetButton");
    connect(resetBtn, &QPushButton::clicked, this, [this]{
        HotkeyConfig lh; lh.modifiers = Qt::ControlModifier;
                         lh.key = Qt::Key_Return; lh.label = "Ctrl+Enter";
        HotkeyConfig sh; sh.modifiers = 0;
                         sh.key = Qt::Key_F12;    sh.label = "F12";
        m_launchCapture->setConfig(lh);
        m_settingsCapture->setConfig(sh);
    });

    auto* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto* saveBtn = new QPushButton("Save", this);
    saveBtn->setObjectName("SaveButton");
    connect(saveBtn, &QPushButton::clicked, this, &HotkeyEditor::save);

    btnRow->addWidget(resetBtn);
    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(saveBtn);
    outer->addLayout(btnRow);
}

void HotkeyEditor::save()
{
    m_settings->setLaunchHotkey(m_launchCapture->currentConfig());
    m_settings->setSettingsHotkey(m_settingsCapture->currentConfig());
    accept();
}

} // namespace SwiftCast
