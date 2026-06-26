# SwiftCast - RayCast Alternative

A high-performance productivity launcher built primarily in C++, inspired by RayCast.

## Project Overview

SwiftCast is a productivity tool providing quick access to applications, scripts, system commands, and extensions through an intuitive interface.

## Architecture & Tech Stack

- **Primary Language**: C++
- **Target**: Cross-platform productivity launcher
- **Focus**: Performance, extensibility, and user experience

## Development Guidelines

### Git Workflow

- **Commits**: I can commit freely whenever logical progress is made
- **Commit Messages**: Must be descriptive and clearly state what was done
  - ✅ Good: "Add core launcher UI framework"
  - ❌ Bad: "fix stuff" or "updates"
- **Pushing**: Only on your explicit command (I will ask for permission)
- **Feature Publishing**: Features are only published/merged when you approve

### CI / Testing Pipeline

- **Tests run only on push to `main`** — never on local-only commits
- The CI pipeline is defined in `.github/workflows/ci.yml`
- It runs automatically on GitHub whenever a push reaches `main`
- You can also trigger it manually via GitHub → Actions → "CI" → "Run workflow"
- Pipeline steps: Install Qt6 → CMake configure → Build (Release) → CTest
- New tests go in `tests/` and must be registered with `add_test()` in CMakeLists.txt
- **Rule**: A locally committed but unpushed change is never in CI — only what is on GitHub counts

### Project Structure

Use a logical folder hierarchy:
```
SwiftCast/
├── src/              # C++ source code
│   ├── core/         # Core launcher logic
│   ├── ui/           # User interface
│   ├── extensions/   # Plugin/extension system
│   └── utils/        # Utility functions
├── include/          # Header files
├── tests/            # Unit and integration tests
├── docs/             # Documentation
├── cmake/            # Build configuration
├── README.md         # Project overview
├── CLAUDE.md         # This file
└── .gitignore
```

### Documentation

- Create a comprehensive **README.md** with:
  - Project description
  - Installation instructions
  - Usage guide
  - Feature list
  - Build instructions
  - Contributing guidelines

### Build & Deployment

- Use CMake for cross-platform builds
- Support Windows, macOS, and Linux
- [Additional build details TBD]

### Features & Releases

- Features are developed in feature branches
- Only publish/release features when explicitly approved
- Document all new features in README

### Code Standards

- Follow C++ best practices
- Clear naming conventions
- Modular architecture
- Proper error handling

## Important Notes

- All work is tracked in commits with logical, descriptive messages
- Project structure must remain organized and maintainable
- README is the single source of truth for users
- No feature goes public without explicit approval
