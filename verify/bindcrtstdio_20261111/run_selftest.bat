@echo off
rem BIND-CRT-STDIO self-test: build stdio_selftest.cpp with VS2022 x86 (the exe toolchain, /MT like mashed_re.exe) and
rem run it in a fresh scratch directory (argument 1, default %TEMP%\bindcrtstdio_selftest).
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
call %VCVARS% >nul
set SRC=%~dp0stdio_selftest.cpp
set OUT=%~1
if "%OUT%"=="" set OUT=%TEMP%\bindcrtstdio_selftest
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"
pushd "%OUT%"
cl /nologo /std:c++17 /EHsc /MT /O2 "%SRC%" /Fe:stdio_selftest.exe /link /BASE:0x10000000 /DYNAMICBASE:NO >build.log 2>&1 || (type build.log & popd & exit /b 1)
.\stdio_selftest.exe
set RC=%ERRORLEVEL%
popd
exit /b %RC%
