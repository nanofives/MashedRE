@echo off
REM Double-click launcher — TV profile (menus 60 fps, races 120 fps,
REM everything unlocked, no savegame writes). Extra args pass through to
REM scripts\mashed_launch.ps1.
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\mashed_launch.ps1" -Profile tv %*
if errorlevel 1 pause
