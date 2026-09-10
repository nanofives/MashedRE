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

REM `build.bat clean` drops the per-TU object cache (see build_objs.ps1); the next
REM run recompiles everything. MASHED_BUILD_FULL=1 does the same without deleting.
if /i "%~1"=="clean" (
    if exist "%OUT%\obj" rmdir /s /q "%OUT%\obj"
    echo cleaned %OUT%\obj
)

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

REM Vendored librw (MIT, aap) -- the SHIPPING renderer per gate D2 (user-decided
REM 2026-07-31). Build the static lib once if absent. EXE-ONLY: deliberately NOT
REM linked into the .asi, which runs inside MASHED.exe and already has its own RW
REM engine + D3D9 device. See deps\librw\build_librw.bat, deps\librw\PINNED_REV.txt
REM and re\analysis\LIBRW_SIZING_2026-08.md.
set LIBRW_LIB=%ROOT%deps\librw\librw_d3d9.lib
REM Rebuild when the lib is MISSING **or STALE**. "Build once if absent" was not
REM enough: we carry local patches to the vendored snapshot
REM (deps\librw\MASHED_PATCHES.md), and editing those sources left a stale .lib
REM that still linked, so the build failed with unresolved externals for the newly
REM added entry points -- which is exactly what merging this branch into a tree
REM with a pre-patch .lib did. Staleness is a source-newer-than-lib check.
set LIBRW_STALE=0
if not exist "%LIBRW_LIB%" (
    set LIBRW_STALE=1
) else (
    for /f %%s in ('powershell -NoProfile -Command ^
        "$l=(Get-Item '%LIBRW_LIB%').LastWriteTime;" ^
        "if (Get-ChildItem '%ROOT%deps\librw\src' -Recurse -Include *.cpp,*.h -ErrorAction SilentlyContinue |" ^
        "    Where-Object { $_.LastWriteTime -gt $l } | Select-Object -First 1) { '1' } else { '0' }"') do set LIBRW_STALE=%%s
)
if "%LIBRW_STALE%"=="1" (
    echo === Building vendored librw static lib ^(missing or sources newer^) ===
    call "%ROOT%deps\librw\build_librw.bat"
    if errorlevel 1 (echo [ERROR] librw lib build failed & exit /b 1)
)

REM E1': LibRw\RwBridge.cpp is the ONLY TU that includes librw's <rw.h>. Compile it
REM in ISOLATION with the librw include path into its own .obj, exactly as
REM QhullBridge.cpp is handled above -- librw's headers drop a large `namespace rw`
REM and their own rw*.h names into scope, which must not reach the other ~200 TUs.
REM Only an exe .obj is produced (no asi counterpart): librw is exe-only.
set LIBRWINC=%ROOT%deps\librw
cl /nologo /EHa /W3 /O2 /c /I "%LIBRWINC%" /I "%LIBRWINC%\src" /DRW_D3D9 ^
    /Fo"%OUT%\RwBridge_exe.obj" "%SRC%\LibRw\RwBridge.cpp"
if errorlevel 1 (echo [ERROR] RwBridge exe-obj compile failed & exit /b 1)
cl /nologo /EHa /W3 /O2 /c /I "%LIBRWINC%" /I "%LIBRWINC%\src" /DRW_D3D9 ^
    /Fo"%OUT%\RwRasterBridge_exe.obj" "%SRC%\LibRw\RwRasterBridge.cpp"
if errorlevel 1 (echo [ERROR] RwRasterBridge exe-obj compile failed & exit /b 1)
cl /nologo /EHa /W3 /O2 /c /I "%LIBRWINC%" /I "%LIBRWINC%\src" /DRW_D3D9 ^
    /Fo"%OUT%\RwSceneBuild_exe.obj" "%SRC%\LibRw\RwSceneBuild.cpp"
if errorlevel 1 (echo [ERROR] RwSceneBuild exe-obj compile failed & exit /b 1)
REM E2'b step 3: the in-loop submit path. Same isolation as the three above --
REM it includes <rw.h>, so it must NOT join the plain source list below (those
REM TUs compile without the librw include path). EXE-ONLY: never asi_sources.rsp.
cl /nologo /EHa /W3 /O2 /c /I "%LIBRWINC%" /I "%LIBRWINC%\src" /DRW_D3D9 ^
    /Fo"%OUT%\RwRaceSubmit_exe.obj" "%SRC%\LibRw\RwRaceSubmit.cpp"
if errorlevel 1 (echo [ERROR] RwRaceSubmit exe-obj compile failed & exit /b 1)

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
REM
REM   - D0.7 TRIAGE, 2026-08-18 (read_fleet runs/w1_relink). 124 of the 235
REM     unlinked .cpp were classified. Two findings correct the picture above:
REM
REM     (a) RH_ScopedInstall is NOT a boot hazard and never was. It expands to a
REM         file-scope object whose ctor calls HookSystem::Register(RVA, &fn) --
REM         the RVA is passed as an INTEGER, never dereferenced -- and in the exe
REM         Register is the no-op from Stubs\HookSystemNoOp.cpp. Util\UtilLeaves.cpp
REM         has the identical shape and has been linked and booting all along.
REM         The real trigger is narrower: a file-scope initializer that DEREFS an
REM         absolute address, i.e. the leading '*' in the GUID example above.
REM         Across 124 files there are exactly TWO offenders, both in Audio\:
REM         AudioDSound.cpp:95-96 (confirmed, the GUID pattern verbatim) and
REM         AudioRws.cpp:477-490 (RVA-bound globals; binds only, will not fault
REM         the loader, held out for its thunks to the original RW audio engine).
REM
REM     (b) Load-safe is NOT the same as functional, and this is the load-bearing
REM         caveat. Because Register is no-op'd, a linked reimpl is a DEAD EXPORT
REM         unless the standalone call graph invokes it by name; and its body
REM         still derefs MASHED addresses (0x004xxxxx code, 0x006xxxxx-0x008xxxxx
REM         data) that are unmapped in an exe based at 0x10000, so it AVs if it
REM         ever does run. Bulk-adding the class-B files would grow the binary and
REM         the tracker without shipping one working feature.
REM
REM     Therefore add-backs are gated on NO MASHED ADDRESS IN ANY CODE PATH, not
REM     merely on booting. Batch 1 (the six Save\/Input\/Particle\ files at the end
REM     of the list below) meets that bar. Everything else in the backlog needs its
REM     RVA tunnels neutralized first -- that is porting work, not a list edit.
REM
REM     D0.7 batch 2, 2026-08-19 (save-rva-neutralize): the FIRST neutralized
REM     tunnel add-back. Save\GameSaveBuffer.cpp DOES touch MASHED addresses (the
REM     save globals in 0x007xxxxx/0x008xxxxx), so it did not qualify for batch 1.
REM     It is added here because those tunnels are now neutralized at compile time:
REM     the new /DMASHED_STANDALONE on the exe cl line below resolves every base to
REM     a NAMED standalone symbol -- 7 logical globals split 3-vs-4 (see the header
REM     block in Save\GameSaveBuffer.cpp): 3 are PRIVATE to save (own local scratch)
REM     and 4 are SHARED engine state (BOUND by name -- trackTable->g_save_span,
REM     counter->g_saveCounter; strideRecords/profile bind to the standalone's
REM     documented ABSENCE of that state, not a duplicate). The dev .asi (no macro)
REM     keeps the original absolute globals so the diff-original reference is
REM     byte-identical. Repeatable pattern for the rest of Save\. NOTE: in the exe,
REM     HookSystem::Register is no-op'd (Stubs\HookSystemNoOp.cpp), so the two funcs
REM     are DEAD EXPORTS -- LINKED and load-safe but NOT reached on the default path
REM     (the live save/load runs through Save\GameSaveFormat.h BuildImage/ParseImage
REM     in Race\GameFlow.cpp; nothing calls Serialize/DeserializeToBuffer by name).
REM     Wiring a real standalone save/load call site is a separate slice.
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
REM Per-TU object cache (2026-09-09): the source list moved to exe_sources.rsp (one
REM quoted path per line, link order preserved). build_objs.ps1 compiles only the
REM TUs whose .obj is missing/older than the .cpp or any repo header it includes
REM (cl /sourceDependencies), into build\obj\exe\, then writes link.rsp with every
REM object. A flag change wipes the cache; `build.bat clean` or MASHED_BUILD_FULL=1
REM force a full rebuild. NEW EXE TUs GO IN exe_sources.rsp, NOT HERE.
REM The SAME x87 list applies to the exe: these 10 Collision/ TUs are in BOTH source
REM lists, and compiling them SSE2 here while the .asi compiles them x87 would make the
REM two targets compute physics differently -- an A/B verified under the .asi would not
REM transfer to the standalone. Keep the float model identical across targets.
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%build_objs.ps1" -Target exe ^
    -SrcRoot "%SRC%" -ObjDir "%OUT%\obj\exe" -Rsp "%ROOT%exe_sources.rsp" ^
    -ClFlags "/EHa /W3 /O2 /DMASHED_STANDALONE" -RepoRoot "%ROOT%.." -X87List "%ROOT%x87_tus.txt"
if errorlevel 1 (popd & echo [ERROR] exe compile failed & exit /b 1)
cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /Fe"%OUT%\mashed_re.exe" @"%OUT%\obj\exe\link.rsp" ^
    "%OUT%\QhullBridge_exe.obj" ^
    "%OUT%\RwBridge_exe.obj" ^
    "%OUT%\RwRasterBridge_exe.obj" ^
    "%OUT%\RwSceneBuild_exe.obj" ^
    "%OUT%\RwRaceSubmit_exe.obj" ^
    /link /SUBSYSTEM:WINDOWS /BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO ^
    /MAP:"%OUT%\mashed_re.map" ^
    user32.lib d3d9.lib dsound.lib gdi32.lib "%QHULL_LIB%" "%LIBRW_LIB%"
popd
if errorlevel 1 (echo [ERROR] exe build failed & exit /b 1)

echo === Building mashed_re_dev.asi ===
pushd "%SRC%"
REM Per-TU object cache (2026-09-09): build_objs.ps1 compiles only the stale subset of
REM asi_sources.rsp into build\obj\asi\ and writes link.rsp (every .obj, list order).
REM -X87List (2026-09-10, decision D-11070): the TUs named in x87_tus.txt get /arch:IA32
REM appended -- x87 codegen instead of MSVC's default /arch:SSE2. They are verbatim
REM transcriptions of x87 instruction streams, and under SSE2 a `float` expression rounds
REM to 32 bits after EVERY operation where the original rounds once at the FSTP.
REM MEASURED on RwpSolverBroadphase3.cpp: 0x0055b750 DIVERGENT 13/48 -> CLEAN 48/48 and
REM 0x0055c2d0 5/24 -> CLEAN 24/24, with its two already-CLEAN rows still CLEAN 48/48.
REM Per-TU, NOT global, so librw (the shipping renderer) and every non-physics TU keep SSE2.
REM Rationale, caveats and scope: re/analysis/float_model_is_sse2_not_x87_20260910.md
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%build_objs.ps1" -Target asi ^
    -SrcRoot "%SRC%" -ObjDir "%OUT%\obj\asi" -Rsp "%ROOT%asi_sources.rsp" ^
    -ClFlags "/EHsc /W3 /O2" -RepoRoot "%ROOT%.." -X87List "%ROOT%x87_tus.txt"
if errorlevel 1 (popd & echo [ERROR] asi compile failed & exit /b 1)
cl /nologo /EHsc /W3 /O2 /LD /Fe"%OUT%\mashed_re_dev.asi" @"%OUT%\obj\asi\link.rsp" ^
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
