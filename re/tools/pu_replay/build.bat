@echo off
REM Build pu_replay.exe from the SAME Powerup TUs the shipping exe links, with the
REM exe's flags for them (build.bat: /EHa /W3 /O2 /DMASHED_STANDALONE; neither TU is
REM in x87_tus.txt, so default /arch:SSE2 as in the exe).
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
set HERE=%~dp0
set SRC=%HERE%..\..\..\mashedmod\src\mashed_re
set OUT=%HERE%out
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /I "%SRC%" /Fo"%OUT%\\" /Fe"%OUT%\pu_replay.exe" ^
   "%HERE%pu_replay.cpp" "%SRC%\Powerup\PowerupSystem.cpp" "%SRC%\Powerup\PowerupEffects.cpp" "%SRC%\Powerup\PowerupContact.cpp"
exit /b %ERRORLEVEL%
