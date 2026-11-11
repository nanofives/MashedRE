@echo off
REM DDKOBJ-SPIKE math probe: probe_ddk.exe (XP DDK cl + link + libc.lib) and probe_vs.exe (VS2022 + UCRT, /arch:IA32).
REM usage: build_probe.bat <outdir> <repo-main-checkout>
setlocal
set OUTD=%~1
set MAIN=%~2
set HERE=%~dp0
if not exist "%OUTD%" mkdir "%OUTD%"
set DDK=%MAIN%\tools\vc7ddk
REM --- DDK: clean env, its own headers and libs
setlocal
set PATH=%DDK%\bin;%SystemRoot%\system32
set INCLUDE=%DDK%\inc\crt;%DDK%\inc\wxp
set LIB=%DDK%\lib\wxp\i386
cl /nologo /O2 /Fo"%OUTD%\\" /Fe"%OUTD%\probe_ddk.exe" "%HERE%probe.c" "%HERE%fn.c" /link /NODEFAULTLIB libc.lib kernel32.lib
if errorlevel 1 (echo [ERROR] DDK probe build failed & exit /b 1)
endlocal
REM --- VS2022
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
if not exist "%OUTD%\vs" mkdir "%OUTD%\vs"
cl /nologo /O2 /arch:IA32 /Fo"%OUTD%\vs\\" /Fe"%OUTD%\probe_vs.exe" "%HERE%probe.c" "%HERE%fn.c"
if errorlevel 1 (echo [ERROR] VS probe build failed & exit /b 1)
REM --- VS2022 default float model (SSE2), the way most exe TUs are compiled
if not exist "%OUTD%\vs2" mkdir "%OUTD%\vs2"
cl /nologo /O2 /Fo"%OUTD%\vs2\\" /Fe"%OUTD%\probe_vs_sse2.exe" "%HERE%probe.c" "%HERE%fn.c"
if errorlevel 1 (echo [ERROR] VS SSE2 probe build failed & exit /b 1)
echo built
