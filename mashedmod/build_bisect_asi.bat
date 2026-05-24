@echo off
REM Diagnostic bisect: build .asi with a subset of cpp files.
REM Usage: build_bisect_asi.bat <txt-file-with-cpp-paths-one-per-line>
setlocal EnableDelayedExpansion
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\mashed_re
set OUT=%ROOT%build
set FILELIST=%~1

if "%FILELIST%"=="" (echo Usage: %~nx0 file-list.txt & exit /b 1)
if not exist "%FILELIST%" (echo File list not found: %FILELIST% & exit /b 1)

if not exist "%OUT%\bisect" mkdir "%OUT%\bisect"
call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

set CPPS=
for /F "usebackq tokens=*" %%a in ("%FILELIST%") do (
    set CPPS=!CPPS! "%%a"
)

pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\bisect\\" /Fe"%OUT%\mashed_re_dev.asi" ^
    !CPPS! ^
    /link /DLL
popd
if errorlevel 1 (echo [ERROR] bisect build failed & exit /b 1)
echo === Bisect build OK ===
dir /b "%OUT%\mashed_re_dev.asi"
endlocal
