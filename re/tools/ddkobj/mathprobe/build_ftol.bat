@echo off
REM DDKOBJ-CRTMATH __ftol probe: ftol_ddk.exe (XP DDK cl + libc __ftol), ftol_vs_ia32.exe (VS2022 /arch:IA32),
REM ftol_vs_sse2.exe (VS2022 default). usage: build_ftol.bat <outdir> <repo-main-checkout>
setlocal
set OUTD=%~1
set MAIN=%~2
set HERE=%~dp0
if not exist "%OUTD%" mkdir "%OUTD%"
set DDK=%MAIN%\tools\vc7ddk
setlocal
set PATH=%DDK%\bin;%SystemRoot%\system32
set INCLUDE=%DDK%\inc\crt;%DDK%\inc\wxp
set LIB=%DDK%\lib\wxp\i386
cl /nologo /O2 /Fo"%OUTD%\\" /Fe"%OUTD%\ftol_ddk.exe" "%HERE%ftolprobe.c" /link /NODEFAULTLIB libc.lib kernel32.lib
if errorlevel 1 (echo [ERROR] DDK ftol probe build failed & exit /b 1)
endlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
if not exist "%OUTD%\vs" mkdir "%OUTD%\vs"
cl /nologo /O2 /arch:IA32 /Fo"%OUTD%\vs\\" /Fe"%OUTD%\ftol_vs_ia32.exe" "%HERE%ftolprobe.c"
if errorlevel 1 (echo [ERROR] VS IA32 ftol probe build failed & exit /b 1)
if not exist "%OUTD%\vs2" mkdir "%OUTD%\vs2"
cl /nologo /O2 /Fo"%OUTD%\vs2\\" /Fe"%OUTD%\ftol_vs_sse2.exe" "%HERE%ftolprobe.c"
if errorlevel 1 (echo [ERROR] VS SSE2 ftol probe build failed & exit /b 1)
echo built
