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
pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /Fo"%OUT%\\" /Fe"%OUT%\mashed_re.exe" ^
    "exe_main.cpp" ^
    "Piz\PizReader.cpp" ^
    "Rws\RwsChunkWalker.cpp" ^
    "Txd\TxdDecoder.cpp" ^
    "D3d9Render\QuadRenderer.cpp" ^
    /link /SUBSYSTEM:WINDOWS user32.lib d3d9.lib
popd
if errorlevel 1 (echo [ERROR] exe build failed & exit /b 1)

echo === Building mashed_re_dev.asi ===
pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\\" /Fe"%OUT%\mashed_re_dev.asi" ^
    "dll_main.cpp" ^
    "Core\HookSystem.cpp" ^
    "Math\Vec3.cpp" ^
    "Math\RwSqrt.cpp" ^
    "GameState\StateAccessors.cpp" ^
    "Vehicle\VehicleState.cpp" ^
    "Vehicle\MiscDamping_j3.cpp" ^
    "Vehicle\Replay_j3.cpp" ^
    "Util\TimerState.cpp" ^
    "Util\TimerSubarrayInit.cpp" ^
    "Util\GameStateGetters.cpp" ^
    "Util\EventTable.cpp" ^
    "Util\TimerInit.cpp" ^
    "Audio\AudioMemory.cpp" ^
    "Audio\AudioRws.cpp" ^
    "Audio\RwsStream_i1.cpp" ^
    "Audio\RwsFmt_j1.cpp" ^
    "Audio\AudioDSound.cpp" ^
    "Audio\AudioMusic.cpp" ^
    "Math\RwV3dTransform.cpp" ^
    "Math\RwV2d.cpp" ^
    "Math\RwMatrixScale.cpp" ^
    "Frontend\TimerReset.cpp" ^
    "Frontend\MenuNav.cpp" ^
    "Frontend\FrontendState.cpp" ^
    "Frontend\FrontendNav.cpp" ^
    "Frontend\GameModeInit.cpp" ^
    "Frontend\MenuButtonDetect.cpp" ^
    "Frontend\GameModeCarSelect.cpp" ^
    "Frontend\MenuGetters.cpp" ^
    "Frontend\SpriteGate.cpp" ^
    "Frontend\FrontendAccessors.cpp" ^
    "Frontend\FrontendMode.cpp" ^
    "HUD\HudDispatch.cpp" ^
    "HUD\FontCtx.cpp" ^
    "HUD\HudBatch_h4.cpp" ^
    "Frontend\MenuScoreGetters.cpp" ^
    "Frontend\MenuInit.cpp" ^
    "Frontend\MenuScoreSort.cpp" ^
    "Frontend\MenuRaceEnd.cpp" ^
    "Frontend\TextMeasure.cpp" ^
    "Frontend\SpriteDispatch.cpp" ^
    "Frontend\MenuTime.cpp" ^
    "Frontend\MenuHelpers.cpp" ^
    "Frontend\VehicleMeta.cpp" ^
    "Frontend\MenuChrome.cpp" ^
    "Frontend\RaceResults.cpp" ^
    "Save\GameSaveVFS.cpp" ^
    "Boot\CrtCompilerSupport.cpp" ^
    "Boot\CrtInit.cpp" ^
    "Boot\Window.cpp" ^
    "Boot\RwEngineInit.cpp" ^
    "Boot\Teardown.cpp" ^
    "Boot\VideoConfig.cpp" ^
    "Save\GameSave.cpp" ^
    "Save\VfsStream.cpp" ^
    "Save\FsOpen.cpp" ^
    "Save\SettingsConfig.cpp" ^
    "Save\SettingsAndIO_i3.cpp" ^
    "Save\SettingsCfg_i4.cpp" ^
    "Save\SettingsDialog_j5.cpp" ^
    "Boot\CrtStartup.cpp" ^
    "Boot\Boot_j5.cpp" ^
    "Util\TimerSetters.cpp" ^
    "Util\TimerSlot.cpp" ^
    "Util\UtilBatch_h5.cpp" ^
    "Util\UtilMid_h6.cpp" ^
    "Frontend\MenuStateMachine.cpp" ^
    "Frontend\FrontendDispatch.cpp" ^
    "Boot\FrameDispatch.cpp" ^
    "Input\DirectInput.cpp" ^
    "Input\DInput_j5.cpp" ^
    "Compat\PizWin32Bypass.cpp" ^
    "Harness\HarnessStubs.cpp" ^
    /link /DLL
popd
if errorlevel 1 (echo [ERROR] dll build failed & exit /b 1)

echo === Build OK ===
dir /b "%OUT%\mashed_re.exe" "%OUT%\mashed_re_dev.asi"
endlocal
