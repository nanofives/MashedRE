@echo off
REM D4-AUDIO-VOLTODS: build the offline sweep (x86, the same cl as mashedmod\build.bat). Output dir = %1 (default .)
setlocal
set OUTD=%~1
if "%OUTD%"=="" set OUTD=.
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /O2 /W3 /DMASHED_STANDALONE "%~dp0sweep.cpp" /Fo"%OUTD%\sweep.obj" /Fe"%OUTD%\sweep.exe" /link /DYNAMICBASE:NO /BASE:0x400000
