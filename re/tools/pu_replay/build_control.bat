@echo off
REM CONTROL build (non-degeneracy): same harness + same PowerupSystem.cpp, but the
REM per-type effects from a given git revision (default 9dbac058, the pre-D3-powerups
REM port). If pu_diff reports CLEAN for this build too, the diff cannot see the
REM effect-level fixes and the CLEAN verdict on the current build means nothing.
setlocal
set REV=%1
if "%REV%"=="" set REV=9dbac058
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
set HERE=%~dp0
set SRC=%HERE%..\..\..\mashedmod\src\mashed_re
set OUT=%HERE%out_control
if not exist "%OUT%" mkdir "%OUT%"
git -C "%HERE%..\..\.." show %REV%:mashedmod/src/mashed_re/Powerup/PowerupEffects.cpp > "%OUT%\PowerupEffects_%REV%.cpp" || exit /b 1
cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /I "%SRC%" /I "%SRC%\Powerup" /Fo"%OUT%\\" /Fe"%OUT%\pu_replay_control.exe" ^
   "%HERE%pu_replay.cpp" "%SRC%\Powerup\PowerupSystem.cpp" "%OUT%\PowerupEffects_%REV%.cpp"
exit /b %ERRORLEVEL%
