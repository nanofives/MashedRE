@echo off
REM B5b acceptance harness — vendored qhull-2002.1 + ported RwpQHullWrapper bridge.
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set QDEP=%~dp0
set QSRC=%QDEP%src
set COLL=%QDEP%..\..\src\mashed_re\Collision
set OUT=%QDEP%b5b_selftest
if not exist "%OUT%" mkdir "%OUT%"

call %VCVARS% >nul
if not exist "%QDEP%qhull_2002_1.lib" call "%QDEP%build_qhull.bat"

REM x87 to match the qhull lib / original FP mode.
cl /nologo /EHsc /W3 /arch:IA32 /fp:precise /Ox /D_CRT_SECURE_NO_WARNINGS ^
   /I "%QSRC%" /Fo"%OUT%\\" /Fe"%OUT%\b5b_qhull_selftest.exe" ^
   "%COLL%\b5b_qhull_selftest.cpp" "%COLL%\QhullBridge.cpp" ^
   /link "%QDEP%qhull_2002_1.lib"
if errorlevel 1 (echo [ERROR] selftest build failed & exit /b 1)
echo === b5b_qhull_selftest.exe OK ===
endlocal
