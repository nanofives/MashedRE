@echo off
REM Mashed RE — build + deploy mashed_qol.asi (player-facing QoL runtime patches).
REM Env-gated features: MASHED_NO_SAVE, MASHED_UNLOCK (see src\qol_asi\mashed_qol.cpp).
REM Loaded by the dinput8 proxy; inert when no MASHED_QOL/feature env vars are set.
setlocal

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\qol_asi
set OUT=%ROOT%build
set ORIG=%ROOT%..\original

if not exist "%OUT%" mkdir "%OUT%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

echo === Building mashed_qol.asi ===
cl /nologo /EHsc /W3 /O2 /MT /LD /Fo"%OUT%\\" /Fe"%OUT%\mashed_qol.asi" ^
    "%SRC%\mashed_qol.cpp" ^
    /link /DLL kernel32.lib
if errorlevel 1 (echo [ERROR] mashed_qol.asi build failed & exit /b 1)

echo === Deploying to %ORIG% ===
copy /Y "%OUT%\mashed_qol.asi" "%ORIG%\mashed_qol.asi" >nul
if errorlevel 1 (echo [ERROR] deploy mashed_qol.asi failed ^(game running?^) & exit /b 1)

echo === mashed_qol.asi build + deploy OK ===
dir /b "%ORIG%\mashed_qol.asi"
endlocal
