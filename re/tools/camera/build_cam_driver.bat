@echo off
REM Build the offline race-camera driver. Links ONLY RaceCamera.cpp + PizReader.cpp,
REM so it is a genuine unit-level exercise of the port -- no renderer, no game state.
REM MSVC x86 to match the port's normal build (mashedmod\build.bat), because the
REM camera is x87-sensitive: RaceCamera.cpp:19 pins the rounding mode and the
REM original's partial results round through float32.
setlocal
set ROOT=%~dp0..\..\..
set VC="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
if not exist %VC% (
  echo ERROR: vcvars32.bat not found at %VC%
  exit /b 1
)
call %VC% >nul
if errorlevel 1 exit /b 1

set OUT=%ROOT%\log\cam_driver
if not exist "%OUT%" mkdir "%OUT%"

cl /nologo /EHsc /O2 /W3 /std:c++17 ^
   /Fo"%OUT%\\" /Fe"%OUT%\cam_driver.exe" ^
   "%~dp0cam_driver.cpp" ^
   "%ROOT%\mashedmod\src\mashed_re\Race\RaceCamera.cpp" ^
   "%ROOT%\mashedmod\src\mashed_re\Piz\PizReader.cpp"
if errorlevel 1 (
  echo === BUILD FAILED ===
  exit /b 1
)
echo === cam_driver OK -^> %OUT%\cam_driver.exe ===
endlocal
