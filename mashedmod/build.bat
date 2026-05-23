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
    "Vehicle\MiscDamping.cpp" ^
    "Vehicle\Replay.cpp" ^
    "Vehicle\Damage.cpp" ^
    "Vehicle\Physics.cpp" ^
    "Util\TimerState.cpp" ^
    "Util\TimerSubarrayInit.cpp" ^
    "Util\GameStateGetters.cpp" ^
    "Util\EventTable.cpp" ^
    "Util\TimerInit.cpp" ^
    "Audio\AudioMemory.cpp" ^
    "Audio\AudioRws.cpp" ^
    "Audio\RwsStream.cpp" ^
    "Audio\RwsFmt.cpp" ^
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
    "HUD\HudBatch.cpp" ^
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
    "Frontend\MenuMenusB.cpp" ^
    "Frontend\IntroSplash.cpp" ^
    "Save\GameSaveBuffer.cpp" ^
    "Save\Race_Guard.cpp" ^
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
    "Save\SettingsAndIO.cpp" ^
    "Save\SettingsCfg.cpp" ^
    "Save\SettingsDialog.cpp" ^
    "Save\RwStream.cpp" ^
    "Save\CareerEvents.cpp" ^
    "Save\ReplayThunk.cpp" ^
    "Boot\CrtStartup.cpp" ^
    "Boot\Boot.cpp" ^
    "Boot\SubsystemInit.cpp" ^
    "Util\TimerSetters.cpp" ^
    "Util\TimerSlot.cpp" ^
    "Util\UtilBatch.cpp" ^
    "Util\UtilMid.cpp" ^
    "Frontend\MenuStateMachine.cpp" ^
    "Frontend\FrontendDispatch.cpp" ^
    "Frontend\SpriteCluster.cpp" ^
    "Boot\FrameDispatch.cpp" ^
    "Boot\GameStateCluster.cpp" ^
    "Input\DirectInput.cpp" ^
    "Input\DInput.cpp" ^
    "Input\InputWave3.cpp" ^
    "Compat\PizWin32Bypass.cpp" ^
    "Harness\HarnessStubs.cpp" ^
    "Frontend\Leaves.cpp" ^
    "Frontend\MenuMenusMixed.cpp" ^
    "Util\UtilLeaves.cpp" ^
    "Util\UtilWave3.cpp" ^
    "HUD\TextCluster.cpp" ^
    "HUD\HudWave3.cpp" ^
    "Frontend\MenuSpriteDispatch.cpp" ^
    "Frontend\MenuMixed.cpp" ^
    "Frontend\DrawQuadPrimitives.cpp" ^
    "Frontend\SmallLeaves_n2.cpp" ^
    "Frontend\FrontendWave3.cpp" ^
    "Render\HighAB3Helpers_p6.cpp" ^
    "Render\TrackNodeLeaves_o1.cpp" ^
    "Render\LowRvaSetters_o2.cpp" ^
    "Render\RwPluginHelpers_o3.cpp" ^
    "Render\RwPluginHelpers_q1.cpp" ^
    "Render\RenderSubmit_o4.cpp" ^
    "Render\D3D9Helpers_p4.cpp" ^
    "Vehicle\ReplayCluster.cpp" ^
    "Vehicle\SmallLeaves_o5.cpp" ^
    "Vehicle\SmallLeaves_q4.cpp" ^
    "Util\SmallLeaves_o6.cpp" ^
    "Render\TrackLoaderMicros_p3.cpp" ^
    "Render\FrameHelpers_q2.cpp" ^
    "Render\LowRvaMixed_q3.cpp" ^
    "Render\D3D9Helpers_q5.cpp" ^
    "Render\TextureLoader_q6.cpp" ^
    "Render\TextureLoaderCluster.cpp" ^
    "Boot\LaunchHandshake.cpp" ^
    "Render\FrameWorldPasses.cpp" ^
    "Render\PerModeRender.cpp" ^
    "Render\MixedC3Sweep.cpp" ^
    "Boot\BootLowRvaCluster.cpp" ^
    "Audio\MixedC3Sweep.cpp" ^
    "Audio\AudioWave3.cpp" ^
    /link /DLL
popd
if errorlevel 1 (echo [ERROR] dll build failed & exit /b 1)

echo === Build OK ===
dir /b "%OUT%\mashed_re.exe" "%OUT%\mashed_re_dev.asi"
endlocal
