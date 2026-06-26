# Changelog — SwiftCast

Alle nennenswerten Änderungen werden in dieser Datei dokumentiert.  
Format basiert auf [Keep a Changelog](https://keepachangelog.com/de/1.0.0/).  
Versionierung folgt dem Schema `0.MINOR.PATCH` (siehe [VERSIONING.md](VERSIONING.md)).

---

## [Unreleased]

> Änderungen die committed, aber noch nicht als Version getaggt sind.

---

## [0.1.0] — 2026-06-26

### Hinzugefügt

#### Kern
- Globaler Hotkey `Ctrl+Enter` zum Öffnen/Schließen des Launchers
- `HotkeyManager` — Registrierung und Verwaltung systemweiter Tastenkürzel
- `Settings` — Persistente Konfiguration via `QSettings` (INI-Format)
- `Application` — Zentraler App-Controller mit System-Tray-Integration
- `Updater` — Automatische Update-Prüfung gegen GitHub Releases beim Start

#### UI
- `LauncherWindow` — Hauptfenster mit Suchfeld und Ergebnisliste, frameless Dark-Theme
- `HotkeyEditor` — Dialog zum Neubinden aller Hotkeys (`F12`)
- `SettingsDialog` — Einstellungsdialog für Konfigurationsoptionen

#### Erweiterungssystem
- `ExtensionBase` — Abstrakte Plugin-Basisklasse für externe Extensions
- `ExtensionLoader` — DLL-basiertes Ladesystem aus `%APPDATA%\SwiftCast\extensions\`
- `ExtensionStore` — Eingebauter Store zum Durchsuchen und Installieren von Extensions

#### Build & Infrastruktur
- CMake-Buildsystem (Qt 6.8, MSVC 2022, C++17)
- Windows RC-Ressource mit Versions-Metadaten und App-Icon
- Qt-Ressourcendatei (`resources.qrc`) mit Dark-Theme-Stylesheet (`dark.qss`)
- NSIS-Installer-Skript (`installer/`) für `SwiftCast-Setup.exe`
- GitHub Actions CI-Pipeline (Build + CTest bei Push auf `main`)
- `test_settings` — Automatisierter CTest für Settings-Persistenz
- `test_launch` — Manuelles UI-Testprogramm (Entwicklungs-Launch)
- Startskript `tests/Version 0.1.0.bat` — findet und startet SwiftCast ohne IDE

#### Dokumentation
- `README.md` — Projektübersicht, Quickstart, Tastenkürzel, Build-Anleitung
- `docs/BUILDING.md` — Detaillierte Build-Dokumentation
- `VERSIONING.md` — Versionierungsstrategie (`0.x.x` während Entwicklung)
- `CHANGELOG.md` — Diese Datei

### Technische Details
- Zielplattform: Windows 10/11 (64-bit)
- Qt-Version: 6.8.3 (msvc2022_64)
- Compiler: MSVC 19.44 (VS Build Tools 2022)

---

## Versionsformat

```
[0.MINOR.PATCH] — YYYY-MM-DD

### Hinzugefügt    — neue Features
### Geändert       — Änderungen an bestehenden Features
### Behoben        — Bugfixes
### Entfernt       — entfernte Features
### Sicherheit     — Sicherheitsrelevante Korrekturen
```

[Unreleased]: https://github.com/IchPity/SwiftCast/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/IchPity/SwiftCast/releases/tag/v0.1.0
