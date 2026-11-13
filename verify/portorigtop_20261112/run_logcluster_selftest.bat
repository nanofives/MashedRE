@echo off
rem BIND-CRT-STDIO-CALLERS self-test: build logcluster_selftest.cpp plus the four knob-path log wrappers with VS2022 x86
rem (the exe toolchain, /MT like mashed_re.exe) and run it in a fresh scratch directory (argument 1, default
rem %TEMP%\logcluster_selftest).
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
call %VCVARS% >nul
set M=%~dp0..\..\mashedmod\src\mashed_re\Match
set OUT=%~1
if "%OUT%"=="" set OUT=%TEMP%\logcluster_selftest
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"
pushd "%OUT%"
cl /nologo /std:c++17 /EHsc /MT /O2 /DMASHED_STANDALONE=1 "%~dp0logcluster_selftest.cpp" "%M%\PortTop_00496490.cpp" "%M%\PortTop_004963e0.cpp" "%M%\PortTop_00496400.cpp" "%M%\PortTop_00496470.cpp" /Fe:logcluster_selftest.exe /link /BASE:0x00400000 /FIXED /DYNAMICBASE:NO >build.log 2>&1 || (type build.log & popd & exit /b 1)
.\logcluster_selftest.exe
set RC=%ERRORLEVEL%
popd
exit /b %RC%
