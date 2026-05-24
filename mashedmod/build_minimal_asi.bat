@echo off
REM Diagnostic: minimal .asi (dll_main + HookSystem only, MSVC CRT, no reimpls).
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\mashed_re
set OUT=%ROOT%build

if not exist "%OUT%\min" mkdir "%OUT%\min"
call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\min\\" /Fe"%OUT%\mashed_re_dev.asi" ^
    "dll_main.cpp" ^
    "Core\HookSystem.cpp" ^
    /link /DLL
popd
if errorlevel 1 (echo [ERROR] minimal asi build failed & exit /b 1)
echo === Minimal .asi build OK ===
dir /b "%OUT%\mashed_re_dev.asi"
endlocal
