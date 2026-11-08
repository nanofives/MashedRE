@echo off
REM usage: cxxmany.bat <file.c> [...]  -- C++ check of each file with VS2022 (one vcvars call)
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul 2>nul
:loop
if "%~1"=="" goto :eof
cl /nologo /c /TP /W1 /O2 /EHa /DMATCH_PORT /Fo"%TEMP%\cxxm_%~n1.obj" "%~1" >"%TEMP%\cxxm_%~n1.txt" 2>&1
if errorlevel 1 (echo CXXFAIL %~n1 & findstr /C:"error C" "%TEMP%\cxxm_%~n1.txt") else (echo CXXOK %~n1)
shift
goto loop
