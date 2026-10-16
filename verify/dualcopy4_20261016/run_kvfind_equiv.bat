@echo off
rem dualcopy4: build + run the KvLookup old-vs-new equivalence test. Outputs to %TEMP%\dc4kv, exit code = test result.
setlocal
set HERE=%~dp0
set SRC=%HERE%..\..\mashedmod\src\mashed_re\Frontend\KvFind.cpp
set OUT=%TEMP%\dc4kv
if not exist "%OUT%" mkdir "%OUT%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /std:c++17 "%HERE%kvfind_equiv_test.cpp" "%SRC%" /Fo"%OUT%\\" /Fe"%OUT%\kvfind_equiv_test.exe"
if errorlevel 1 (echo BUILD FAILED & exit /b 2)
"%OUT%\kvfind_equiv_test.exe"
exit /b %errorlevel%
