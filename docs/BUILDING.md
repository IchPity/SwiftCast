# Building SwiftCast

## Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| Visual Studio | 2022+ (with C++ workload) | https://visualstudio.microsoft.com |
| CMake | 3.15+ | https://cmake.org |
| Qt6 | 6.5+ | https://www.qt.io/download |
| vcpkg | latest | https://github.com/microsoft/vcpkg |
| NSIS | 3.x (installer only) | https://nsis.sourceforge.io |

## Quick Start

```bat
git clone https://github.com/IchPity/SwiftCast.git
cd SwiftCast

:: Configure (adjust Qt path)
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64" -DCMAKE_BUILD_TYPE=Release

:: Build
cmake --build build --config Release

:: Run
build\Release\SwiftCast.exe
```

## With vcpkg

```bat
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
cmake -B build -DCMAKE_TOOLCHAIN_FILE=".\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Release
```

## Building the Installer

1. Install NSIS from https://nsis.sourceforge.io
2. Run:

```bat
cd installer
build_installer.bat
```

This produces `installer\SwiftCast-0.1.0-Setup.exe`.

## Project Structure

```
SwiftCast/
├── src/
│   ├── main.cpp                  Entry point
│   ├── core/
│   │   ├── Application.h/.cpp    App lifecycle, tray icon, startup
│   │   ├── HotkeyManager.h/.cpp  Win32 global hotkey registration
│   │   ├── Settings.h/.cpp       JSON settings in %APPDATA%\SwiftCast
│   │   └── Updater.h/.cpp        GitHub releases auto-update
│   ├── ui/
│   │   ├── LauncherWindow.h/.cpp Main search window
│   │   ├── HotkeyEditor.h/.cpp   F12 shortcut editor
│   │   └── SettingsDialog.h/.cpp Full settings panel
│   ├── extensions/
│   │   ├── ExtensionBase.h       Extension interface
│   │   ├── ExtensionLoader.h/.cpp Load built-in + DLL extensions
│   │   └── ExtensionStore.h/.cpp Download from extension catalog
│   └── utils/
│       └── FileSystem.h/.cpp     File utilities
├── resources/
│   ├── icons/logo.svg            App logo (SVG)
│   ├── styles/dark.qss           Qt dark theme stylesheet
│   └── resources.qrc             Qt resource manifest
├── installer/
│   ├── swiftcast.nsi             NSIS installer script
│   └── build_installer.bat       One-click installer builder
└── docs/
    └── BUILDING.md               This file
```

## Writing an Extension

Create a DLL that exports:

```cpp
extern "C" __declspec(dllexport)
SwiftCast::ExtensionBase* swiftcast_create_extension();
```

Implement `ExtensionBase` (see `src/extensions/ExtensionBase.h`).

Drop the DLL into `%APPDATA%\SwiftCast\extensions\` — SwiftCast loads it on next start.
