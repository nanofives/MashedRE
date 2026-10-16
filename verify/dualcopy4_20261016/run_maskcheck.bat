@echo off
rem dualcopy4 G-MASK for 0x0042d3e0: build + run maskcheck_42d3e0.cpp (host). Outputs to %TEMP%\dc4mask.
setlocal
set HERE=%~dp0
set OUT=%TEMP%\dc4mask
if not exist "%OUT%" mkdir "%OUT%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /std:c++17 "%HERE%maskcheck_42d3e0.cpp" /Fo"%OUT%\\" /Fe"%OUT%\maskcheck.exe"
if errorlevel 1 (echo BUILD FAILED & exit /b 2)
"%OUT%\maskcheck.exe"
exit /b %errorlevel%
