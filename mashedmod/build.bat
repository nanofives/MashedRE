@echo off
REM Mashed RE - top-level build script.
REM Builds both targets via MSVC Build Tools 2022 (x86):
REM   build/mashed_re.exe       - standalone (greenfield)
REM   build/mashed_re_dev.asi   - dev-mode hook DLL
setlocal

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
set ROOT=%~dp0
set SRC=%ROOT%src\mashed_re
set OUT=%ROOT%build

if not exist "%OUT%" mkdir "%OUT%"

call %VCVARS% >nul
if errorlevel 1 (echo [ERROR] vcvars32.bat failed & exit /b 1)

echo === Building mashed_re.exe ===
cl /nologo /EHsc /W3 /O2 /Fo"%OUT%\\" /Fe"%OUT%\mashed_re.exe" ^
    "%SRC%\exe_main.cpp" ^
    /link /SUBSYSTEM:WINDOWS user32.lib
if errorlevel 1 (echo [ERROR] exe build failed & exit /b 1)

echo === Building mashed_re_dev.asi ===
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\\" /Fe"%OUT%\mashed_re_dev.asi" ^
    "%SRC%\dll_main.cpp" ^
    "%SRC%\Core\HookSystem.cpp" ^
    "%SRC%\Math\Vec3.cpp" ^
    "%SRC%\Math\RwSqrt.cpp" ^
    "%SRC%\GameState\StateAccessors.cpp" ^
    "%SRC%\Vehicle\VehicleState.cpp" ^
    "%SRC%\Util\TimerState.cpp" ^
    "%SRC%\Util\GameStateGetters.cpp" ^
    "%SRC%\Util\EventTable.cpp" ^
    "%SRC%\Audio\AudioMemory.cpp" ^
    "%SRC%\Math\RwV3dTransform.cpp" ^
    "%SRC%\Math\RwV2d.cpp" ^
    "%SRC%\Math\RwMatrixScale.cpp" ^
    "%SRC%\Frontend\TimerReset.cpp" ^
    "%SRC%\Frontend\MenuNav.cpp" ^
    "%SRC%\Frontend\FrontendState.cpp" ^
    "%SRC%\Frontend\FrontendNav.cpp" ^
    "%SRC%\Frontend\GameModeInit.cpp" ^
    "%SRC%\Frontend\MenuButtonDetect.cpp" ^
    "%SRC%\Frontend\GameModeCarSelect.cpp" ^
    "%SRC%\Frontend\MenuGetters.cpp" ^
    "%SRC%\Frontend\SpriteGate.cpp" ^
    "%SRC%\Frontend\FrontendAccessors.cpp" ^
    "%SRC%\Frontend\FrontendMode.cpp" ^
    "%SRC%\HUD\HudDispatch.cpp" ^
    "%SRC%\HUD\FontCtx.cpp" ^
    "%SRC%\Frontend\MenuScoreGetters.cpp" ^
    "%SRC%\Frontend\MenuInit.cpp" ^
    "%SRC%\Frontend\MenuScoreSort.cpp" ^
    "%SRC%\Frontend\MenuRaceEnd.cpp" ^
    "%SRC%\Frontend\TextMeasure.cpp" ^
    "%SRC%\Frontend\SpriteDispatch.cpp" ^
    "%SRC%\Frontend\MenuTime.cpp" ^
    "%SRC%\Frontend\MenuHelpers.cpp" ^
    "%SRC%\Frontend\VehicleMeta.cpp" ^
    "%SRC%\Save\GameSaveVFS.cpp" ^
    /link /DLL
if errorlevel 1 (echo [ERROR] dll build failed & exit /b 1)

echo === Build OK ===
dir /b "%OUT%\mashed_re.exe" "%OUT%\mashed_re_dev.asi"
endlocal
