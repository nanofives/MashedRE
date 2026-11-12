@echo off
rem BIND-CRT-STRING self-test: build string_selftest.cpp with VS2022 x86 (the exe toolchain, /MT like mashed_re.exe) and
rem run it in a fresh scratch directory (argument 1, default %TEMP%\bindcrtstring_selftest).
setlocal
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
call %VCVARS% >nul
set SRC=%~dp0string_selftest.cpp
set OUT=%~1
if "%OUT%"=="" set OUT=%TEMP%\bindcrtstring_selftest
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"
pushd "%OUT%"
cl /nologo /std:c++17 /EHsc /MT /O2 /D_CRT_SECURE_NO_WARNINGS "%SRC%" /Fe:string_selftest.exe >build.log 2>&1 || (type build.log & popd & exit /b 1)
.\string_selftest.exe C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched
set RC=%ERRORLEVEL%
popd
exit /b %RC%
