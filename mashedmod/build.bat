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

REM ===========================================================================
REM Phase C status (2026-05-25):
REM   - LINK GATE MET: an experimental full-source-set build (every reimpl in
REM     mashedmod/src/mashed_re/**/*.cpp minus dll_main.cpp + Core/HookSystem.cpp)
REM     links cleanly with only ONE unresolved external (HookSystem::Register,
REM     stubbed via Stubs/HookSystemNoOp.cpp). DLL imports per Phase B
REM     (re/STANDALONE_DEPS.md) all resolve via /link <libs>.
REM
REM   - RUNTIME GATE NOT MET: the full-set exe crashed at startup with
REM     STATUS_DLL_INIT_FAILED (0xC0000142). Root cause: many reimpls have
REM     static initializers that dereference MASHED.exe RVAs, e.g.
REM       static const GUID g_iid = *reinterpret_cast<const GUID*>(0x005d09dc);
REM     valid only when injected into MASHED's address space. Phase F (RW3
REM     replacement) + Phase G (boot chain) need to neutralize or redirect
REM     these RVA tunnels per .cpp before the full set can run standalone.
REM
REM For now the exe target uses the MINIMAL self-contained set proven to run
REM (Milestones A..B5: window, D3D9 device, Frontend.piz load + TXD decode +
REM 4x2 textured atlas). The full-set link is verified via the .asi target
REM (which exercises every .cpp). When Phase F/G work begins, .cpp files will
REM be added back here individually as their RVA references are neutralized.
REM
REM See `re/STANDALONE_DEPS.md` for the per-region disposition list.
REM ===========================================================================

REM RUNTIME NOTE: when launching mashedmod\build\mashed_re.exe, Windows'
REM safe-DLL-search resolves d3d9.dll and dinput8.dll to the *.bak-able
REM shim copies that build_d3d9_shim.bat / build_dinput8_shim.bat may have
REM left in mashedmod\build\. Those shims expect d3d9_real.dll /
REM dinput8_real.dll alongside (used only by MASHED.exe in original\); when
REM present alongside the standalone they fail to chain and the standalone
REM crashes at process load with STATUS_DLL_INIT_FAILED (0xC0000142).
REM Mitigation: either run mashed_re.exe from a directory without those
REM shim DLLs, or rename them (mv d3d9.dll d3d9.dll.bak) before the run.
REM This file's exe target intentionally does NOT depend on the shim DLLs.
echo === Building mashed_re.exe (B9: frontend + HUD + math + vehicle clusters) ===
pushd "%SRC%"
REM /EHa (not /EHsc) for the exe target: the standalone boot chain relies on
REM __try/__except to survive partial-wedge AVs (steps 2..7, B7/B14 probes);
REM /EHa makes those SEH guards catch hardware exceptions. The .asi target keeps
REM /EHsc (it runs inside MASHED, not the wedge).
cl /nologo /EHa /W3 /O2 /Fo"%OUT%\\" /Fe"%OUT%\mashed_re.exe" ^
    "exe_main.cpp" ^
    "Piz\PizReader.cpp" ^
    "Rws\RwsChunkWalker.cpp" ^
    "Txd\TxdDecoder.cpp" ^
    "Track\TrackWorld.cpp" ^
    "Track\DffModel.cpp" ^
    "D3d9Render\TrackRenderer.cpp" ^
    "Race\RaceCamera.cpp" ^
    "D3d9Render\QuadRenderer.cpp" ^
    "D3d9Render\RwIm2DBridge.cpp" ^
    "D3d9Render\DrawStreamDump.cpp" ^
    "D3d9Render\PngLoader.cpp" ^
    "D3d9Render\MpegVideoTexture.cpp" ^
    "D3d9Render\TextRenderer.cpp" ^
    "D3d9Render\MashedFont.cpp" ^
    "D3d9Render\MenuStringTable.cpp" ^
    "Compat\StandaloneRvaThunks.cpp" ^
    "Stubs\HookSystemNoOp.cpp" ^
    "Frontend\MenuInit.cpp" ^
    "Frontend\MenuButtonDetect.cpp" ^
    "Frontend\FrontendState.cpp" ^
    "Frontend\FrontendNav.cpp" ^
    "Frontend\FrontendMode.cpp" ^
    "Frontend\FrontendAccessors.cpp" ^
    "Frontend\FrontendDispatch.cpp" ^
    "Frontend\DrawQuadPrimitives.cpp" ^
    "Frontend\MenuLeaves_af1.cpp" ^
    "Frontend\MenuSpriteDispatch.cpp" ^
    "Frontend\MenuGetters.cpp" ^
    "Frontend\MenuChrome.cpp" ^
    "Frontend\MenuHelpers.cpp" ^
    "Frontend\MenuNav.cpp" ^
    "Frontend\MenuNavSM.cpp" ^
    "Frontend\MenuRaceEnd.cpp" ^
    "Frontend\MenuScoreGetters.cpp" ^
    "Frontend\MenuScoreSort.cpp" ^
    "Frontend\MenuStateMachine.cpp" ^
    "Frontend\GameModeInit.cpp" ^
    "Frontend\GameModeCarSelect.cpp" ^
    "Frontend\TimerReset.cpp" ^
    "Frontend\SpriteGate.cpp" ^
    "Frontend\Leaves.cpp" ^
    "Frontend\TextMeasure.cpp" ^
    "Frontend\VehicleMeta.cpp" ^
    "Frontend\MenuTime.cpp" ^
    "Frontend\SmallLeaves_n2.cpp" ^
    "Frontend\SmallLeaves_t1.cpp" ^
    "Frontend\SlotZeroers_s1.cpp" ^
    "Frontend\MenuLeaves_s3.cpp" ^
    "Frontend\MenuLeaves_af4.cpp" ^
    "Frontend\GlobalGetters_s4.cpp" ^
    "Frontend\GlobalGetters_s5.cpp" ^
    "Frontend\MenuNearLeaves_s6.cpp" ^
    "Frontend\MenuStateWriters_u1.cpp" ^
    "Frontend\MenuMiscLeaves_t2.cpp" ^
    "Frontend\BucketMixed_t3.cpp" ^
    "Frontend\MenuLeaves_af6.cpp" ^
    "Frontend\Cluster_v3.cpp" ^
    "Frontend\BatchAA_s4.cpp" ^
    "HUD\HudDispatch.cpp" ^
    "HUD\Cluster_v2.cpp" ^
    "Boot\GameStateCluster.cpp" ^
    "Boot\BootLowRvaCluster.cpp" ^
    "Boot\Window.cpp" ^
    "Boot\VideoConfig.cpp" ^
    "Boot\Teardown.cpp" ^
    "Boot\LaunchHandshake.cpp" ^
    "Boot\FrameDispatch.cpp" ^
    "Boot\SubsystemInit.cpp" ^
    "Util\GameStateGetters.cpp" ^
    "Util\EventTable.cpp" ^
    "Util\TimerInit.cpp" ^
    "Util\TimerState.cpp" ^
    "Util\TimerSubarrayInit.cpp" ^
    "Util\TimerSetters.cpp" ^
    "Util\TimerSlot.cpp" ^
    "Util\UtilLeaves.cpp" ^
    "Util\SmallLeaves_o6.cpp" ^
    "Util\UtilLeaves_ab6.cpp" ^
    "Util\UtilLeaves_ac.cpp" ^
    "Vehicle\VehicleState.cpp" ^
    "Vehicle\SmallLeaves_o5.cpp" ^
    "Vehicle\SmallLeaves_q4.cpp" ^
    "Vehicle\MiscDamping.cpp" ^
    "GameState\StateAccessors.cpp" ^
    "Math\Vec3.cpp" ^
    "Math\RwSqrt.cpp" ^
    "Math\RwV2d.cpp" ^
    "Math\RwV3dTransform.cpp" ^
    "Math\RwMatrixScale.cpp" ^
    "Frontend\Cluster_v1.cpp" ^
    "Frontend\BatchAA_s1.cpp" ^
    "Frontend\BatchAA_s3.cpp" ^
    "Frontend\BatchAA_s6.cpp" ^
    "Render\BatchAB_s1.cpp" ^
    "Render\BatchAB_s3.cpp" ^
    "Render\BatchAB_s6.cpp" ^
    "Render\RenderLeaves_ae1.cpp" ^
    "Render\RenderLeaves_ae2.cpp" ^
    "Render\RenderLeaves_ae3.cpp" ^
    "Frontend\FrontendLeaves_ad2.cpp" ^
    "Frontend\MenuLeaves_af5.cpp" ^
    /link /SUBSYSTEM:WINDOWS /BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO ^
    /MAP:"%OUT%\mashed_re.map" ^
    user32.lib d3d9.lib
popd
if errorlevel 1 (echo [ERROR] exe build failed & exit /b 1)

echo === Building mashed_re_dev.asi ===
pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\\" /Fe"%OUT%\mashed_re_dev.asi" ^
    "dll_main.cpp" ^
    "Core\HookSystem.cpp" ^
    "Frontend\MenuAnimTickTwin.cpp" ^
    "Frontend\LogoOverlayTwin.cpp" ^
    "Frontend\MenuDrawLoopTwin.cpp" ^
    "Frontend\PromptStripTwin.cpp" ^
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
    "Audio\AudioLeaves_ab2.cpp" ^
    "Audio\RwsStream.cpp" ^
    "Audio\RwsFmt.cpp" ^
    "Audio\AudioDSound.cpp" ^
    "Audio\AudioMusic.cpp" ^
    "Audio\AudioLeaves_ab4.cpp" ^
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
    "Compat\PizWin32Bypass.cpp" ^
    "Compat\IntroTextNullGuard.cpp" ^
    "Compat\StreamHandlerDispatchGuard.cpp" ^
    "Harness\HarnessStubs.cpp" ^
    "Frontend\Leaves.cpp" ^
    "Frontend\MenuMenusMixed.cpp" ^
    "Util\UtilLeaves.cpp" ^
    "HUD\TextCluster.cpp" ^
    "Frontend\MenuSpriteDispatch.cpp" ^
    "Frontend\MenuMixed.cpp" ^
    "Frontend\DrawQuadPrimitives.cpp" ^
    "Frontend\MenuLeaves_af1.cpp" ^
    "Frontend\SmallLeaves_n2.cpp" ^
    "Frontend\SmallLeaves_t1.cpp" ^
    "Frontend\SlotZeroers_s1.cpp" ^
    "Frontend\MenuLeaves_s3.cpp" ^
    "Frontend\MenuLeaves_af4.cpp" ^
    "Frontend\GlobalGetters_s4.cpp" ^
    "Frontend\GlobalGetters_s5.cpp" ^
    "Frontend\MenuNearLeaves_s6.cpp" ^
    "Frontend\MenuStateWriters_u1.cpp" ^
    "Frontend\BucketMixed_t3.cpp" ^
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
    "Util\UtilLeaves_ab6.cpp" ^
    "Util\UtilLeaves_ac.cpp" ^
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
    "Render\RenderLeaves_ae2.cpp" ^
    "Boot\BootLowRvaCluster.cpp" ^
    "Audio\MixedC3Sweep.cpp" ^
    "Audio\AudioLeaves_ab1.cpp" ^
    "Audio\AudioLeaves_ab3.cpp" ^
    "Frontend\SlotZeroers_s2.cpp" ^
    "Frontend\MenuMiscLeaves_t2.cpp" ^
    "Frontend\SplashGameMode_t5.cpp" ^
    "Frontend\SkeletonAndScatter_t6.cpp" ^
    "Frontend\Cluster_v1.cpp" ^
    "HUD\Cluster_v2.cpp" ^
    "Frontend\Cluster_v3.cpp" ^
    "Frontend\BatchAA_s1.cpp" ^
    "Frontend\BatchAA_s2.cpp" ^
    "Frontend\BatchAA_s3.cpp" ^
    "Frontend\BatchAA_s4.cpp" ^
    "Frontend\BatchAA_s5.cpp" ^
    "Frontend\FrontendLeaves_ad1.cpp" ^
    "Frontend\MenuLeaves_af6.cpp" ^
    "Frontend\BatchAA_s6.cpp" ^
    "Render\BatchAB_s1.cpp" ^
    "Render\BatchAB_s3.cpp" ^
    "Render\BatchAB_s6.cpp" ^
    "Render\RenderLeaves_ae1.cpp" ^
    "Render\RenderLeaves_ae3.cpp" ^
    "Frontend\FrontendLeaves_ad2.cpp" ^
    "Gameplay\GameplayLeaves_ad3.cpp" ^
    "Particle\ParticleLeaves_ad4.cpp" ^
    "Particle\ParticleLeaves_ad5.cpp" ^
    "Frontend\MenuLeaves_af5.cpp" ^
    "Input\MemsetInline_ag1.cpp" ^
    "Vehicle\ReplayRewind_ag.cpp" ^
    "HUD\ViewportDimsSet_ag.cpp" ^
    "Gameplay\SparseGrid_ag.cpp" ^
    "Util\TimeRecordWrite_ag.cpp" ^
    "Audio\AudioList_ag.cpp" ^
    "Gameplay\RangeTable_ah1.cpp" ^
    "Gameplay\ScoreMasks_ah3.cpp" ^
    "Gameplay\Thresholds_ah4.cpp" ^
    "Render\PluginFields_ah4.cpp" ^
    "Audio\TimerGetters_ah5.cpp" ^
    "Ai\PromoLoop_round1.cpp" ^
    "Vehicle\PromoLoop_round2.cpp" ^
    "Render\PromoLoop_round3.cpp" ^
    "Boot\PromoLoop_round6.cpp" ^
    "Boot\PromoLoop_round7.cpp" ^
    "Util\PromoLoop_round8.cpp" ^
    "Util\PromoLoop_round9.cpp" ^
    "Util\PromoLoop_round10.cpp" ^
    "Render\PromoLoop_round11.cpp" ^
    "Boot\PromoLoop_round12.cpp" ^
    "Util\PromoLoop_round15.cpp" ^
    "Render\PromoLoop_round16.cpp" ^
    "Util\PromoLoop_round17.cpp" ^
    "Util\PromoLoop_round18.cpp" ^
    "Util\PromoLoop_round20.cpp" ^
    "Util\PromoLoop_round21.cpp" ^
    "Render\PromoLoop_round22.cpp" ^
    "Util\PromoLoop_round24.cpp" ^
    "Util\PromoLoop_round25.cpp" ^
    "Util\PromoLoop_round26.cpp" ^
    "Util\PromoLoop_round27.cpp" ^
    "Util\PromoLoop_round28.cpp" ^
    "Util\PromoLoop_round29.cpp" ^
    "Util\PromoLoop_round30.cpp" ^
    "Util\PromoLoop_round31.cpp" ^
    "Util\PromoLoop_round32.cpp" ^
    "Util\PromoLoop_round33.cpp" ^
    "Util\PromoLoop_round34.cpp" ^
    "Util\PromoLoop_round35.cpp" ^
    "Util\PromoLoop_round37.cpp" ^
    "Util\PromoLoop_round38.cpp" ^
    "Util\PromoLoop_round39.cpp" ^
    "Util\PromoLoop_round40.cpp" ^
    "Util\PromoLoop_round41.cpp" ^
    "Util\PromoLoop_round42.cpp" ^
    "Util\PromoLoop_round43.cpp" ^
    "Util\PromoLoop_round44.cpp" ^
    "Util\PromoLoop_round45.cpp" ^
    "Util\PromoLoop_round46.cpp" ^
    "Util\PromoLoop_round47.cpp" ^
    "Util\PromoLoop_round48.cpp" ^
    "Util\PromoLoop_round49.cpp" ^
    "Util\PromoLoop_round50.cpp" ^
    "Util\PromoLoop_round51.cpp" ^
    "Util\PromoLoop_round53.cpp" ^
    "Util\PromoLoop_round54.cpp" ^
    "Util\PromoLoop_round55.cpp" ^
    "Util\PromoLoop_round56.cpp" ^
    "Util\PromoLoop_round57.cpp" ^
    "Util\PromoLoop_round58.cpp" ^
    "Util\PromoLoop_round59.cpp" ^
    "Util\PromoLoop_round60.cpp" ^
    /link /DLL /MAP:"%OUT%\mashed_re_dev.map" /MAPINFO:EXPORTS
popd
if errorlevel 1 (echo [ERROR] dll build failed & exit /b 1)

REM Deploy the dev .asi to original\ where the dinput8 loader actually reads it. Without
REM this the loader keeps loading a STALE .asi and rebuilt fixes silently do not apply
REM (footgun hit 2026-06-07: a crasher fix looked broken until the .asi was deployed).
copy /Y "%OUT%\mashed_re_dev.asi" "%ROOT%..\original\mashed_re_dev.asi" >nul
if errorlevel 1 (echo [ERROR] .asi deploy to original\ failed & exit /b 1)
echo deployed mashed_re_dev.asi -^> original\

REM Keep the standalone's output dir free of the dev d3d9/dinput8 PROXY shims. When a
REM proxy d3d9.dll/dinput8.dll sits next to mashed_re.exe, Windows binds it instead of
REM System32's; the proxy forwards to *_real.dll (which live ONLY in original\) and the
REM standalone dies at process load with STATUS_DLL_INIT_FAILED (0xC0000142) /
REM "missing dinput8_real.DLL". The standalone must bind the REAL System32 d3d9/dinput8.
REM Rename any stray shim copies aside so launching %OUT%\mashed_re.exe works clean.
for %%D in (d3d9.dll dinput8.dll) do if exist "%OUT%\%%D" (
    if exist "%OUT%\%%D.bak" del /q "%OUT%\%%D.bak"
    ren "%OUT%\%%D" "%%D.bak"
    echo moved aside dev shim %%D -^> %%D.bak  ^(standalone binds System32^)
)

echo === Build OK ===
dir /b "%OUT%\mashed_re.exe" "%OUT%\mashed_re_dev.asi"
endlocal
