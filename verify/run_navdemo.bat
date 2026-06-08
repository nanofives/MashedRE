@echo off
cd /d "%~dp0.."
set MASHED_NAV_DEMO=1
echo launching with MASHED_NAV_DEMO=%MASHED_NAV_DEMO% cwd=%CD% > verify\navdemo_launch.txt
mashedmod\build\mashed_re.exe
echo exit=%ERRORLEVEL% >> verify\navdemo_launch.txt
