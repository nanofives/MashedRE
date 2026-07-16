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

REM Vendored qhull-2002.1 (RWP-3.7's embedded convex-hull lib; B5b). Build the
REM static lib once (x87 /arch:IA32 for bit-identity with the original) if absent.
REM Linked into both targets; only referenced once the RwpQHullWrapper bridge
REM (FUN_0057ca30) .cpp is compiled in. See deps\qhull-2002.1\build_qhull.bat and
REM re\analysis\B5b_RWP37_QHULL_VENDOR_2026-07-14.md.
set QHULL_LIB=%ROOT%deps\qhull-2002.1\qhull_2002_1.lib
if not exist "%QHULL_LIB%" (
    echo === Building vendored qhull-2002.1 static lib ===
    call "%ROOT%deps\qhull-2002.1\build_qhull.bat"
    if errorlevel 1 (echo [ERROR] qhull lib build failed & exit /b 1)
)

REM B5c: the RwpQHullWrapper bridge (Collision\QhullBridge.cpp) is the ONLY TU that
REM includes the vendored qhull headers. Compile it in ISOLATION with the qhull include
REM path into a per-target .obj so qhull's generic headers (io.h, mem.h, stat.h) never
REM shadow the CRT/other headers of the ~200 other TUs. The .obj is then linked into each
REM target. One .obj per target because the EH model differs (exe /EHa vs asi /EHsc).
set QINC=%ROOT%deps\qhull-2002.1\src
cl /nologo /EHa  /W3 /O2 /c /I "%QINC%" /Fo"%OUT%\QhullBridge_exe.obj" "%SRC%\Collision\QhullBridge.cpp"
if errorlevel 1 (echo [ERROR] QhullBridge exe-obj compile failed & exit /b 1)
cl /nologo /EHsc /W3 /O2 /c /I "%QINC%" /Fo"%OUT%\QhullBridge_asi.obj" "%SRC%\Collision\QhullBridge.cpp"
if errorlevel 1 (echo [ERROR] QhullBridge asi-obj compile failed & exit /b 1)

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
    "Track\TrackData.cpp" ^
    "D3d9Render\TrackRenderer.cpp" ^
    "D3d9Render\RwWorldRender.cpp" ^
    "Ai\AiStandalone.cpp" ^
    "Vehicle\ForceIntegrator.cpp" ^
    "Vehicle\ForceIntegratorStubs.cpp" ^
    "Vehicle\VehicleControl.cpp" ^
    "Vehicle\Integrate2.cpp" ^
    "Vehicle\AeroStabilize.cpp" ^
    "Vehicle\VehicleInit.cpp" ^
    "Vehicle\VehiclePhysicsRun.cpp" ^
    "Vehicle\VehicleCouplingBridge.cpp" ^
    "Collision\ContactStubs.cpp" ^
    "Collision\ContactProducer.cpp" ^
    "Collision\CarWorldContacts.cpp" ^
    "Collision\CarCarContacts.cpp" ^
    "Collision\WheelContactSolver.cpp" ^
    "Collision\RwpIntegrator.cpp" ^
    "Collision\RwpSolverLeaves1.cpp" ^
    "Collision\RwpSolverMath2.cpp" ^
    "Collision\RwpSolverBroadphase3.cpp" ^
    "Collision\RwpSolverCore4.cpp" ^
    "Collision\RwpBuildExterns.cpp" ^
    "Collision\CollisionBodyCreate.cpp" ^
    "Collision\PhysicsWorldBuild.cpp" ^
    "Math\RwV3dTransformPointsCPU.cpp" ^
    "Math\RwMatrixRotate.cpp" ^
    "Math\RwMatrixRotateInner.cpp" ^
    "Math\RwV3dNormalize.cpp" ^
    "Math\Vec3.cpp" ^
    "D3d9Render\RwWorldLoad.cpp" ^
    "D3d9Render\RwWorldLoadStubs.cpp" ^
    "D3d9Render\RwWorldStream.cpp" ^
    "D3d9Render\ParticleSystem.cpp" ^
    "D3d9Render\PickupField.cpp" ^
    "Powerup\PowerupSystem.cpp" ^
    "Powerup\PowerupEffects.cpp" ^
    "Race\RaceCamera.cpp" ^
    "Race\RaceSession.cpp" ^
    "Race\GameFlow.cpp" ^
    "Race\RaceModes.cpp" ^
    "Race\RuleEngine.cpp" ^
    "Audio\RwsBank.cpp" ^
    "Audio\AudioEngine.cpp" ^
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
    "HUD\ScenarioLeaves_sa1.cpp" ^
    "HUD\ScenarioLeaves_sa2.cpp" ^
    "HUD\ScenarioWriters_sa2s2.cpp" ^
    "HUD\ScenarioWriters_sa2s1.cpp" ^
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
    "Vehicle\ShapeOwnerHandlePool.cpp" ^
    "Vehicle\VehicleSeed.cpp" ^
    "Camera\CameraPathPredicates.cpp" ^
    "Ai\VehicleVelocityWorldGet.cpp" ^
    "Physics\SmplFzxStateBlock.cpp" ^
    "Vehicle\SmallLeaves_o5.cpp" ^
    "Vehicle\SmallLeaves_q4.cpp" ^
    "Vehicle\MiscDamping.cpp" ^
    "GameState\StateAccessors.cpp" ^
    "Math\RwSqrt.cpp" ^
    "Math\FPURound.cpp" ^
    "Math\RwV2d.cpp" ^
    "Math\RwV3dTransform.cpp" ^
    "Math\RwV3dTransformPoints.cpp" ^
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
    "Render\RenderStateSettersA.cpp" ^
    "Frontend\FrontendLeaves_ad2.cpp" ^
    "Frontend\MenuLeaves_af5.cpp" ^
    "%OUT%\QhullBridge_exe.obj" ^
    /link /SUBSYSTEM:WINDOWS /BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO ^
    /MAP:"%OUT%\mashed_re.map" ^
    user32.lib d3d9.lib dsound.lib "%QHULL_LIB%"
popd
if errorlevel 1 (echo [ERROR] exe build failed & exit /b 1)

echo === Building mashed_re_dev.asi ===
pushd "%SRC%"
cl /nologo /EHsc /W3 /O2 /LD /Fo"%OUT%\\" /Fe"%OUT%\mashed_re_dev.asi" @"%ROOT%asi_sources.rsp" ^
    "%OUT%\QhullBridge_asi.obj" ^
    /link /DLL /MAP:"%OUT%\mashed_re_dev.map" /MAPINFO:EXPORTS "%QHULL_LIB%"
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
