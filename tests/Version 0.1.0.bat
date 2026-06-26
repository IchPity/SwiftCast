@echo off
:: SwiftCast Version 0.1.0 — Startskript
:: Doppelklick, um SwiftCast zu starten.

setlocal

set "ROOT=%~dp0.."

for %%P in (
    "%ROOT%\build\Release\SwiftCast.exe"
    "%ROOT%\build\Debug\SwiftCast.exe"
    "%ROOT%\build\SwiftCast.exe"
    "%ROOT%\out\Release\SwiftCast.exe"
    "%ROOT%\out\build\x64-Release\SwiftCast.exe"
    "%ROOT%\out\build\x64-Debug\SwiftCast.exe"
) do (
    if exist %%P (
        start "" %%P
        exit /b 0
    )
)

echo SwiftCast.exe wurde nicht gefunden.
echo Bitte zuerst das Projekt mit CMake bauen.
pause
exit /b 1
