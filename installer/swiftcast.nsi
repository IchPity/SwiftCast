; SwiftCast NSIS Installer Script
; Requires NSIS 3.x: https://nsis.sourceforge.io

!define APP_NAME        "SwiftCast"
!define APP_VERSION     "0.1.0"
!define APP_PUBLISHER   "Peter Scheikl"
!define APP_URL         "https://github.com/IchPity/SwiftCast"
!define APP_EXE         "SwiftCast.exe"
!define INSTALL_DIR     "$PROGRAMFILES64\SwiftCast"
!define UNINSTALL_KEY   "Software\Microsoft\Windows\CurrentVersion\Uninstall\SwiftCast"

; ── General ──────────────────────────────────────────────────────────────────
Name          "${APP_NAME} ${APP_VERSION}"
OutFile       "SwiftCast-${APP_VERSION}-Setup.exe"
InstallDir    "${INSTALL_DIR}"
InstallDirRegKey HKLM "${UNINSTALL_KEY}" "InstallLocation"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

; Modern UI
!include "MUI2.nsh"
!include "x64.nsh"

; ── UI Settings ──────────────────────────────────────────────────────────────
!define MUI_ABORTWARNING
!define MUI_ICON   "..\resources\icons\logo.ico"
!define MUI_UNICON "..\resources\icons\logo.ico"

!define MUI_HEADERIMAGE
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"

!define MUI_FINISHPAGE_RUN          "$INSTDIR\${APP_EXE}"
!define MUI_FINISHPAGE_RUN_TEXT     "Launch SwiftCast"

; ── Pages ────────────────────────────────────────────────────────────────────
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"

; ── Installer ────────────────────────────────────────────────────────────────
Section "SwiftCast (required)" SecMain
    SectionIn RO

    ; Stop running instance
    ExecWait 'taskkill /F /IM "${APP_EXE}"'

    SetOutPath "$INSTDIR"
    File "..\build\Release\${APP_EXE}"

    ; Qt runtime DLLs (deploy with windeployqt)
    File /r "..\build\Release\*.dll"
    File /r "..\build\Release\platforms"
    File /r "..\build\Release\styles"
    File /r "..\build\Release\imageformats"

    ; Resources
    SetOutPath "$INSTDIR\resources"
    File /r "..\resources\styles\*"

    ; Extensions directory
    CreateDirectory "$APPDATA\SwiftCast\extensions"

    ; Start Menu shortcut
    CreateDirectory "$SMPROGRAMS\SwiftCast"
    CreateShortcut "$SMPROGRAMS\SwiftCast\SwiftCast.lnk" \
                   "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0

    ; Desktop shortcut
    CreateShortcut "$DESKTOP\SwiftCast.lnk" \
                   "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0

    ; Auto-start (can be toggled in settings)
    WriteRegStr HKCU \
        "Software\Microsoft\Windows\CurrentVersion\Run" \
        "SwiftCast" "$INSTDIR\${APP_EXE}"

    ; Uninstall registry entries
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "DisplayName"      "${APP_NAME}"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "DisplayVersion"   "${APP_VERSION}"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "Publisher"        "${APP_PUBLISHER}"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "URLInfoAbout"     "${APP_URL}"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "InstallLocation"  "$INSTDIR"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "UninstallString"  "$INSTDIR\Uninstall.exe"
    WriteRegStr   HKLM "${UNINSTALL_KEY}" "DisplayIcon"      "$INSTDIR\${APP_EXE}"
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify"         1
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair"         1

    ; Estimate install size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "EstimatedSize" "$0"

    WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

; ── Uninstaller ───────────────────────────────────────────────────────────────
Section "Uninstall"
    ExecWait 'taskkill /F /IM "${APP_EXE}"'

    RMDir /r "$INSTDIR"
    RMDir /r "$SMPROGRAMS\SwiftCast"
    Delete "$DESKTOP\SwiftCast.lnk"

    DeleteRegKey HKLM "${UNINSTALL_KEY}"
    DeleteRegValue HKCU \
        "Software\Microsoft\Windows\CurrentVersion\Run" "SwiftCast"

    ; Leave user data in %APPDATA%\SwiftCast intact
SectionEnd
