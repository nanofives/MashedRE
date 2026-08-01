@echo off
REM Double-click launcher — desktop profile (menus 60 fps, races 165 fps,
REM everything unlocked, no savegame writes). Extra args pass through to
REM scripts\mashed_launch.ps1 (e.g. "Play Mashed.bat" -EnableSave).
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\mashed_launch.ps1" %*
if errorlevel 1 pause
