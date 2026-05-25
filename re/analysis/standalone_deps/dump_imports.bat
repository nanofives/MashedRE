@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
dumpbin /imports "%~dp0\..\..\..\original\MASHED.exe"
