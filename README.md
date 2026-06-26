# SwiftCast

**A fast, extensible productivity launcher for Windows — inspired by RayCast.**

> Press `Ctrl+Enter` anywhere. Find anything. Launch instantly.

---

## Features

- **Global Hotkey** — Open SwiftCast from anywhere with `Ctrl+Enter`
- **Instant Search** — Search apps, commands, and extensions in real time
- **Extension System** — Download and install extensions from the built-in store
- **Keyboard-first** — Navigate everything with arrow keys and Enter
- **Custom Shortcuts** — Press `F12` inside SwiftCast to rebind any hotkey
- **Auto-Update** — Checks for new versions automatically on startup
- **System Tray** — Runs silently in the background, always ready
- **Dark Theme** — Sleek dark UI with purple accent, optimized for Windows 11

## Quick Start

1. Download `SwiftCast-Setup.exe` from [Releases](https://github.com/IchPity/SwiftCast/releases)
2. Run the installer
3. SwiftCast starts automatically
4. Press **`Ctrl+Enter`** to open the launcher

## Usage

| Key | Action |
|-----|--------|
| `Ctrl+Enter` | Open / close SwiftCast |
| `↑` / `↓` | Navigate results |
| `Enter` | Execute selected item |
| `Esc` | Close launcher |
| `F12` | Open shortcut / settings editor |

## Extension Store

SwiftCast supports downloadable extensions that add new capabilities:

- Tray the extension store via **Settings → Extensions**
- Browse, install, and manage extensions
- Extensions are DLL plugins loaded from `%APPDATA%\SwiftCast\extensions\`

**Writing your own extension:** See [docs/BUILDING.md](docs/BUILDING.md)

## Building from Source

See [docs/BUILDING.md](docs/BUILDING.md) for full build instructions.

**Requirements:** Visual Studio 2022, CMake 3.15+, Qt 6.5+

```bat
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"
cmake --build build --config Release
```

## Auto-Update

SwiftCast checks [GitHub Releases](https://github.com/IchPity/SwiftCast/releases) on startup.  
When an update is available, a tray notification appears — click it to download the new installer.

## Project Structure

```
SwiftCast/
├── src/core/        Core logic: hotkeys, settings, updater
├── src/ui/          Launcher window, hotkey editor, settings dialog
├── src/extensions/  Extension API and store
├── src/utils/       File system utilities
├── resources/       Logo, dark theme stylesheet
├── installer/       NSIS installer script
└── docs/            Build and developer documentation
```

## License

To be defined.

## Author

**Peter Scheikl** — [@IchPity](https://github.com/IchPity)

---

*SwiftCast — Built with C++ and Qt6*
