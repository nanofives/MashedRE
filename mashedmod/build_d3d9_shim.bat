@echo off
REM Mashed RE — build + deploy the d3d9 proxy that forces
REM IDirect3D9::CreateDevice into windowed 800x600.
REM
REM Outputs:
REM   build\d3d9.dll        proxy DLL
REM Deployed (to original\):
REM   d3d9.dll              proxy (Windows loads this for d3d9 imports)
REM   d3d9_real.dll         copy of SysWOW64\d3d9.dll (loader-dedup workaround)
REM
REM Run scripts/setup_mashed_compat.ps1 AFTER this build — the proxy
REM requires the shim layer WITHOUT DISABLEDXMAXIMIZEDWINDOWEDMODE (which
REM hooks d3d9 and deadlocks against our proxy).
setlocal

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\d3d9_shim
set OUT=%ROOT%build
set ORIG=%ROOT%..\original

if not exist "%OUT%" mkdir "%OUT%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

echo === Building d3d9 shim (windowed-force proxy) ===
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\\" /Fe"%OUT%\d3d9.dll" ^
    "%SRC%\d3d9_shim.cpp" ^
    user32.lib ^
    /link /DLL /DEF:"%SRC%\d3d9_shim.def"
if errorlevel 1 (echo [ERROR] d3d9 shim build failed & exit /b 1)

echo === Deploying to %ORIG% ===
copy /Y "%OUT%\d3d9.dll" "%ORIG%\d3d9.dll" >nul
if errorlevel 1 (echo [ERROR] deploy d3d9.dll failed & exit /b 1)

if not exist "%ORIG%\d3d9_real.dll" (
    echo Copying SysWOW64\d3d9.dll -^> d3d9_real.dll  one-time setup
    copy /Y "%SystemRoot%\SysWOW64\d3d9.dll" "%ORIG%\d3d9_real.dll" >nul
    if errorlevel 1 (echo [ERROR] copy d3d9_real.dll failed & exit /b 1)
)

echo === d3d9 shim build + deploy OK ===
dir /b "%ORIG%\d3d9.dll" "%ORIG%\d3d9_real.dll"
endlocal
