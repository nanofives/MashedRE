@echo off
REM Mashed RE — build + deploy the dinput8 proxy that autoloads .asi.
REM Companion to build_d3d9_shim.bat. See dinput8_shim.cpp header comment.
setlocal

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\dinput8_shim
set OUT=%ROOT%build
set ORIG=%ROOT%..\original

if not exist "%OUT%" mkdir "%OUT%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

echo === Building dinput8 shim (asi autoload) ===
cl /nologo /EHsc /W3 /O2 /MT /LD /Fo"%OUT%\\" /Fe"%OUT%\dinput8.dll" ^
    "%SRC%\dinput8_shim.cpp" ^
    /link /DLL kernel32.lib
if errorlevel 1 (echo [ERROR] dinput8 shim build failed & exit /b 1)

echo === Deploying to %ORIG% ===
copy /Y "%OUT%\dinput8.dll" "%ORIG%\dinput8.dll" >nul
if errorlevel 1 (echo [ERROR] deploy dinput8.dll failed & exit /b 1)

if not exist "%ORIG%\dinput8_real.dll" (
    echo Copying SysWOW64\dinput8.dll -^> dinput8_real.dll  one-time setup
    copy /Y "%SystemRoot%\SysWOW64\dinput8.dll" "%ORIG%\dinput8_real.dll" >nul
    if errorlevel 1 (echo [ERROR] copy dinput8_real.dll failed & exit /b 1)
)

echo === dinput8 shim build + deploy OK ===
dir /b "%ORIG%\dinput8.dll" "%ORIG%\dinput8_real.dll"
endlocal
