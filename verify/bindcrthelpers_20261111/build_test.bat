@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d %~dp0
cl /nologo /O2 /EHsc /DMASHED_STANDALONE helper_test.cpp /Fe:helper_test.exe > build_test.log 2>&1
type build_test.log | findstr /i "error warning"
if exist helper_test.exe echo BUILD-OK
