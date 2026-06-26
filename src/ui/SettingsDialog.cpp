#include "SettingsDialog.h"
#include "HotkeyEditor.h"
#include "../core/Settings.h"
#include "../core/HotkeyManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QFrame>
#include <QFile>
#include <QApplication>

namespace SwiftCast {

SettingsDialog::SettingsDialog(Settings* settings, HotkeyManager* hotkeys, QWidget* parent)
    : QDialog(parent)
    , m_settings(settings)
    , m_hotkeys(hotkeys)
{
    setWindowTitle("SwiftCast Settings");
    setObjectName("SettingsDialog");
    setModal(true);
    setFixedSize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    buildUi();

    QFile qss(":/styles/dark.qss");
    if (qss.open(QIODevice::ReadOnly))
        setStyleSheet(qss.readAll());
}

void SettingsDialog::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Title bar ──
    auto* titleBar = new QWidget(this);
    titleBar->setFixedHeight(52);
    titleBar->setStyleSheet("background: #0D0B14; border-bottom: 1px solid #1E1B2E;");

    auto* tbLayout = new QHBoxLayout(titleBar);
    tbLayout->setContentsMargins(20, 0, 16, 0);

    auto* titleLbl = new QLabel("Settings", titleBar);
    titleLbl->setStyleSheet("color: #F0EAFF; font-size: 15px; font-weight: 600;");

    auto* closeBtn = new QPushButton("✕", titleBar);
    closeBtn->setFixedSize(28, 28);
    closeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #6D5FA0; border: none; font-size: 14px; }"
        "QPushButton:hover { color: #F0EAFF; background: #2D2640; border-radius: 6px; }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    tbLayout->addWidget(titleLbl);
    tbLayout->addStretch();
    tbLayout->addWidget(closeBtn);
    root->addWidget(titleBar);

    // ── Tabs ──
    auto* tabs = new QTabWidget(this);
    tabs->setStyleSheet(
        "QTabWidget::pane { border: none; background: #12101A; }"
        "QTabBar::tab { background: #1E1B2E; color: #6D5FA0; padding: 10px 20px;"
        "  border: none; font-size: 13px; }"
        "QTabBar::tab:selected { background: #12101A; color: #F0EAFF; "
        "  border-bottom: 2px solid #7C3AED; }"
        "QTabBar::tab:hover { color: #C4B8E0; }");
    root->addWidget(tabs);

    // ─ General tab ─
    auto* genTab = new QWidget();
    auto* genLayout = new QVBoxLayout(genTab);
    genLayout->setContentsMargins(24, 20, 24, 20);
    genLayout->setSpacing(16);

    auto* startupCheck = new QCheckBox("Launch SwiftCast on Windows startup", genTab);
    startupCheck->setChecked(m_settings->launchOnStartup());
    startupCheck->setStyleSheet("QCheckBox { color: #C4B8E0; font-size: 13px; }");

    auto* updateCheck = new QCheckBox("Check for updates on startup", genTab);
    updateCheck->setChecked(m_settings->checkUpdatesOnStartup());
    updateCheck->setStyleSheet("QCheckBox { color: #C4B8E0; font-size: 13px; }");

    auto* versionLbl = new QLabel(
        "SwiftCast v" + QApplication::applicationVersion(), genTab);
    versionLbl->setStyleSheet("color: #4A3F6B; font-size: 12px;");

    genLayout->addWidget(startupCheck);
    genLayout->addWidget(updateCheck);
    genLayout->addStretch();
    genLayout->addWidget(versionLbl);

    connect(startupCheck, &QCheckBox::toggled,
            m_settings, &Settings::setLaunchOnStartup);
    connect(updateCheck, &QCheckBox::toggled,
            m_settings, &Settings::setCheckUpdatesOnStartup);

    tabs->addTab(genTab, "General");

    // ─ Shortcuts tab ─
    auto* scTab = new QWidget();
    auto* scLayout = new QVBoxLayout(scTab);
    scLayout->setContentsMargins(24, 20, 24, 20);
    scLayout->setSpacing(12);

    auto* scInfo = new QLabel(
        "Configure global keyboard shortcuts. Changes take effect immediately.", scTab);
    scInfo->setWordWrap(true);
    scInfo->setStyleSheet("color: #6D5FA0; font-size: 12px;");

    auto* editShortcutsBtn = new QPushButton("Edit Keyboard Shortcuts…", scTab);
    editShortcutsBtn->setFixedHeight(36);
    editShortcutsBtn->setStyleSheet(
        "QPushButton { background: #1E1B2E; color: #C4B8E0; border: 1px solid #2D2640;"
        " border-radius: 8px; font-size: 13px; padding: 0 16px; }"
        "QPushButton:hover { background: #2D2640; color: #F0EAFF; }");
    connect(editShortcutsBtn, &QPushButton::clicked, this, [this]{
        auto* ed = new HotkeyEditor(m_settings, m_hotkeys, this);
        if (ed->exec() == QDialog::Accepted)
            m_settings->save();
        ed->deleteLater();
    });

    scLayout->addWidget(scInfo);
    scLayout->addWidget(editShortcutsBtn);
    scLayout->addStretch();
    tabs->addTab(scTab, "Shortcuts");

    // ── Bottom bar ──
    auto* botBar = new QWidget(this);
    botBar->setFixedHeight(52);
    botBar->setStyleSheet("background: #0D0B14; border-top: 1px solid #1E1B2E;");

    auto* botLayout = new QHBoxLayout(botBar);
    botLayout->setContentsMargins(20, 0, 20, 0);

    auto* cancelBtn = new QPushButton("Cancel", botBar);
    auto* saveBtn   = new QPushButton("Save",   botBar);
    saveBtn->setStyleSheet(
        "QPushButton { background: #6D28D9; color: white; border-radius: 8px;"
        " padding: 8px 20px; font-size: 13px; border: none; }"
        "QPushButton:hover { background: #7C3AED; }");

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(saveBtn,   &QPushButton::clicked, this, [this]{
        m_settings->save();
        accept();
    });

    botLayout->addStretch();
    botLayout->addWidget(cancelBtn);
    botLayout->addWidget(saveBtn);
    root->addWidget(botBar);
}

} // namespace SwiftCast
