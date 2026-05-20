# Skeleton Call Tree

Phase 1 deliverable. Documents the boot-to-frame-tick call path through
`MASHED.exe`. Each node cites its RVA against the version anchor
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`
(size 2,846,720).

Format per node: `**Name** @ 0x00xxxxxx — short mechanical description`.
Where no C3/C4 symbol exists the Ghidra `FUN_…` label is used. `[UNCERTAIN]`
marks nodes whose role is inferred from caller context only (no direct
hooks.csv or analysis-note attestation of the labelled role).

## 1. Boot path

PE entry through the MSVC CRT to the WinMain wrapper and the three-phase
app lifecycle (`AppInitialiseOnBootup` / app run / `AppDestroy` /
`SoftwareTidyUpBeforeExiting` / `HardwareExitApplication`).

- **entry** @ `0x004a4bb7` — PE OS entry; MSVC CRT startup; `GetVersionExA`→5
  OS globals; PE header parse; heap+TLS+file-handle init; cmdline/env/argv;
  tail-calls the WinMain wrapper.
  - **CrtPreInit** @ `0x004a31f3` — indirect pre-init fn-ptr at 0x00616044;
    fn-ptr tables 0x005ea03c (7 entries) + 0x005ea000 (14 entries);
    `_atexit(0x004a78f4)`.
    - **CrtPreInitLoop** @ `0x004a78b0` — pre-init loop over table at
      0x005e7b84.
  - **CrtStackProbe** @ `0x004a3440` — VS2003 `__chkstk` page-probe (FidDB).
  - **CrtSehProlog** @ `0x004a5984` / **CrtSehEpilog** @ `0x004a59bf` —
    `__SEH_prolog` / `__SEH_epilog` (FidDB).
  - **`__heap_init`** @ `0x004aa3fe` — `HeapCreate(0x1000)` → 0x008aa69c;
    `___heap_select` → 0x008aa6a0; `___sbh_heap_init(0x3f8)` when mode==3.
  - **FUN_004a8a04** @ `0x004a8a04` — `__mtinitlocks`; `TlsAlloc`→0x00616658;
    `_calloc(1,0x88)`.
  - **FUN_004ac04a** @ `0x004ac04a` — CRT file-handle table (32 slots/block,
    stride 36); `GetStartupInfoA` inheritance; 3× `GetStdHandle`.
  - **`__setenvp`** @ `0x004abc53` — env-ptr two-pass build (C3 GREEN).
  - **`___crtGetEnvironmentStringsA`** @ `0x004abf28` — ANSI/wide env
    fetch (C3 GREEN).
  - **FUN_004abe86** @ `0x004abe86` — `GetModuleFileNameA`→0x00773c40;
    `malloc(param_1+argc*4)`; writes argv globals at 0x007739b0/b4/cc.
  - **sub_00492370 (WinMain wrapper)** @ `0x00492370` — 4-param `__cdecl`;
    three-phase init/run/teardown with literal log strings
    `"Calling AppInitialiseOnBootup\n"` / `"Calling AppDestroy\n"` /
    `"Calling SoftwareTidyUpBeforeExiting\n"` / `"Calling HardwareExitApplication\n"`.
    - **sub_00499ba0 (WindowCreate)** @ `0x00499ba0` — `CoInitialize` +
      `RegisterClassA("MASHED")` + `CreateWindowExA` → `DAT_007e9584`
      (style 0xcf0000, `WS_OVERLAPPEDWINDOW`).
    - **sub_00493900 (CmdlineParse)** @ `0x00493900` — tokenises cmdline on
      0x20; `-vs0/1`, `-cs0/1`, `-l0..-l5` switches.
    - **thunk_FUN_00495150** @ `0x00493540` — language/codeset gate
      (DAT_007719e8 lang + DAT_006147c0 cs); returns bool.
    - **sub_004924f0 (DataZeroFill)** @ `0x004924f0` — zero-fills 0xdce9
      DWORDs at DAT_007f0f60; nested data-array init loops.
    - **sub_00492270 (SubsystemInit)** @ `0x00492270` — `FUN_00493710(0)`
      RW init gate; if zero, fail-fast.
      - **FUN_00493710 (RW_INIT_FN)** @ `0x00493710` — sequential
        `RwEngineInit` → `RtFSManagerOpen` → `RwEngineOpen` →
        `RwEngineStart`; returns 1 on full success.
      - **FUN_004921d0 (DisplayInit)** @ `0x004921d0` — video-mode pick
        (FUN_004c2f00) → info (FUN_004c2ed0); `FUN_0042f660`
        DefaultViewportCameraInit; `LoadIcon`→DAT_00771960.
      - **FUN_00428590 (ViewportInit)** @ `0x00428590` — viewport
        bg-colour 0xff000000; sets DAT_0067d974 to 0x3fc90fdb (π/2).
    - **WindowShow** @ `0x004996f0` — `ShowWindow(DAT_007e9584, nCmdShow)`
      + `UpdateWindow`. **(C3 GREEN)**
    - **sub_00402750 (AppInitialiseOnBootup)** @ `0x00402750` — gate
      `FUN_004d8560`; loads PIZ/TXD assets via 57 callees; returns 0/1.
    - **sub_00492290 (MainLoop)** @ `0x00492290` — the message-pump /
      tick loop, expanded in section 2 below.
    - **sub_00402a40 (AppDestroy)** @ `0x00402a40` — teardown; destroys
      DAT_00636ac8 via FUN_004c5930; 43 callees; void.
    - **thunk_FUN_004938c0 (SoftwareTidyUpBeforeExiting)** @ `0x00493550`
      — 5 sequential void calls (FUN_00558470/00550390/004c2f60/004c3040/
      004c3270); the engine-stop dispatch (cmd 0x12 + cmd 0x03).
    - **thunk_FUN_004954f0 (HardwareExitApplication)** @ `0x00493560` —
      `FUN_00498bf0` gate, then `ShowCursor(1)` + 3 thunks +
      `FUN_00496010`.
    - **sub_00499cc0 (WindowDestroy)** @ `0x00499cc0` —
      `DestroyWindow(DAT_007e9584)`; returns DAT_007e95a8 (exit code).

## 2. Main loop

The application's per-frame tick loop. Lives inside the WinMain wrapper's
success path; iterates until either the exit flag DAT_00828300 is set, or
the message pump returns non-zero (WM_QUIT path).

- **sub_00492290 (MainLoop)** @ `0x00492290` — body of the while loop
  `while (DAT_00828300 == 0 && FUN_00499690() == 0) { … }`; returns 1.
  - **FUN_00492770 (MainLoopInit)** @ `0x00492770` — pre-loop: clears
    DAT_00828300 (exit flag); DAT_00771968 := 1 (game state); registers
    FUN_0042b920 → FUN_00433240; FUN_004c57a0 → DAT_007e9dc0.
  - Loop header (per-iteration):
    - **WindowMsgPump** @ `0x00499690` — `PeekMessageA(MSG, NULL, 0, 0,
      PM_REMOVE)`; `TranslateMessage` + `DispatchMessageA`; `WaitMessage`
      if DAT_0077391c==0; returns `DAT_00773918 != 0` (the WM_QUIT
      sentinel that drives loop exit). **(C3 GREEN)**
  - Loop body (6 sequential void calls per frame, in order):
    1. **FUN_004929d0 (StateDispatch)** @ `0x004929d0` — switch on
       DAT_00771968 (7 cases); session-phase dispatcher; calls
       course-load in case 2.
    2. **sub_00492d20 (IntroSplashShim)** @ `0x00492d20` — shim →
       `FUN_004967e0`; returns 1 (caller discards).
    3. **FUN_00492d30 (TimerTick)** @ `0x00492d30` — 7-case dispatch on
       DAT_00771968; calls FUN_0043dfd0; QPC timing; returns 1.
    4. **FUN_00492e90 (FrameDispatch)** @ `0x00492e90` — the per-frame
       render/simulate state machine, expanded in section 3 below.
    5. **FUN_004926c0 (AudioTickAndAvg)** @ `0x004926c0` — QPC sample;
       `FUN_00496930(3/4)` input toggle → audio skip (FUN_0045d3f0 +
       FUN_00466b50 + FUN_0045d3f0/430); 60-frame average → DAT_0077197c.
    6. **FUN_00493480 (FpsDiscretise)** @ `0x00493480` — FPS tick
       discretiser; snaps 50/100/150/200 Hz buckets; writes DAT_007f1000
       (tick count) and DAT_007f1004 (scaled float).

## 3. Frame tick

The render/simulate state machine called once per main-loop iteration.
Switches on the game-state global DAT_00771968. The body has a clear
single-switch shape (states 1, 2, 3/7, 4, 5/6, plus an always-run
post-switch tail).

- **FUN_00492e90 (FrameDispatch)** @ `0x00492e90` — per-frame render
  dispatcher; reads frame timer; clears backbuffer (default colour
  DAT_006147b4 or local alt 0x000000ff); switch on DAT_00771968.
  - Pre-switch (timing + clear):
    - **FUN_004950b0 (TimerSample)** @ `0x004950b0` ×2 — `QueryPerformanceCounter`
      reads; delta scaled by `_DAT_005cc558` into `_DAT_00771988`.
    - **StatePhaseIsIdle** @ `0x0042b8d0` — pure-leaf predicate
      `return DAT_0067eca4 == 0`. **(C3 GREEN)**
    - **StatePhaseIsFinal** @ `0x0042b8f0` — pure-leaf predicate
      `return DAT_0067eca4 == 5`. **(C3 GREEN)**
    - **FUN_004671a0 (CameraDispatch)** @ `0x004671a0` — RW camera/op
      dispatcher (multi-call site; arg0 selects camera handle).
    - **FUN_004c1bb0 (RwCameraClear)** @ `0x004c1bb0` — clear backbuffer.
  - State 1 — frontend/menu pipeline:
    - **MenuAlphaGet (FUN_0042b930)** @ `0x0042b930` — menu alpha
      getter. **(C3)**
    - **sub_00492e60 (SetDefaultViewWindow)** @ `0x00492e60` — sets view
      window 0.8 via FUN_004671a0(0xffffffff).
    - **FRONTEND_FN (FUN_0043c5b0)** @ `0x0043c5b0` — frontend HUD draw.
    - **RwVtableSlot07Call** @ `0x004c19f0` — `(**(code**)(p+0x1c))()`;
      indirect RW vtable slot 7 trampoline. **(C3 GREEN)**
    - **Vehicle0HandleGet** @ `0x0042f510` — pure-leaf getter
      `return DAT_0067f190`. **(C3 GREEN)**
    - **sub_00426030 / sub_00426670 / sub_004266b0 / sub_00410b30** @
      `0x00426030`, `0x00426670`, `0x004266b0`, `0x00410b30` —
      `WorldRenderPrePass`, `WorldRenderDispatch_Begin`,
      `WorldRenderDispatch_End`, `InGameRenderDispatcher`.
    - **sub_00433f40 (RaceEndFadeOverlay)** @ `0x00433f40` — RGBA fade
      panels + text (state-1/4 tail).
  - State 2 — sets DAT_007719c8 := 4 and breaks.
  - States 3 / 7 — in-race render pipeline (gated on DAT_007719c8==0):
    - **sub_00426030** @ `0x00426030` — WorldRenderPrePass.
    - **sub_00426670** @ `0x00426670` — WorldRenderDispatch_Begin (camera A/B).
    - **sub_00410b30 (InGameRenderDispatcher)** @ `0x00410b30` —
      sequential sub-passes.
    - **sub_0040df60** @ `0x0040df60` — ConditionalRenderSubPass
      (3-gate on DAT_0063ba8c / FUN_0042f6a0 / DAT_007f0fd0).
    - **sub_00404320 (PerModeRenderMachine)** @ `0x00404320` — modes
      5/8/9/10; EndUpdate + SetViewWindow(0.8) + BeginUpdate; mode
      dispatch.
    - **sub_0040de30 (MinimapCameraOrthoSetup)** @ `0x0040de30` — saves
      frame matrix + viewwindow + projection; sets ortho 0.6×0.45.
    - Renderer vtable: `[DAT_007d3ff8+0x20](6,0)` and `(8,0)`.
    - **HudIngameDispatch** @ `0x0040dfc0` — per-frame HUD draw
      dispatcher; double-switch on sub-mode + game-mode. **(C3 GREEN)**
    - Renderer vtable: `[DAT_007d3ff8+0x20](6,1)` and `(8,1)`.
    - **sub_0040df20 (MinimapCameraRestore)** @ `0x0040df20` — restores
      saved camera state.
    - **sub_004266b0 (WorldRenderDispatch_End)** @ `0x004266b0`.
  - State 4 — race-entering:
    - **sub_00492e60** + **FRONTEND_FN (FUN_0043c5b0)**.
    - `[UNCERTAIN]` **FUN_00430b90** @ `0x00430b90` — invoked only when
      `FUN_0042b930() == 0x20` (sub-mode 0x20 semantics not attested;
      U-0642).
  - States 5 / 6 — post-race / results: render sequence mirrors state
    3/7 but with `FUN_0043c5b0` substituted in the tail position.
  - Post-switch (always runs — input read, swap, profile):
    - **DisplayActiveFlagGet** @ `0x00498bf0` — pure-leaf getter
      `return DAT_00773204`. **(C3 GREEN)**
    - **FUN_004c1be0 (RwCameraEndUpdate / swap)** @ `0x004c1be0` — RW
      end-update + buffer swap (via FUN_004671a0(0,0,1)).
    - Profiler accumulator loop: `for (puVar6 = 0x00868320; puVar6 <
      0x00869ca0; puVar6 += 0x44) FUN_00492440(puVar6);` — 96 stride-0x44
      slots through **sub_00492440 (RenderStatsAccumulate)** @
      `0x00492440` (60-frame rolling average per slot).
    - Increment DAT_0077198c (frame counter), snapshot DAT_00771998,
      increment DAT_007719a4 (per-second counter).
    - Returns 1.

## Confidence

C3+ nodes (Frida A/B verified, bit-identical or pure-leaf exemption):
- `0x004a31f3` CrtPreInit, `0x004a3258` CrtExitCore, `0x004a31b1`
  CrtExitProcess_j5, `0x004a332b` / `0x004a334d` CrtExit\*\_j5,
  `0x004a3440` CrtStackProbe, `0x004a4b93` CrtFastErrorExit,
  `0x004a5984` / `0x004a59bf` CrtSehProlog/Epilog, `0x004a78b0`
  CrtPreInitLoop, `0x004abc53` `__setenvp`, `0x004abf28`
  `___crtGetEnvironmentStringsA` — boot/CRT path verified.
- `0x004996f0` WindowShow, `0x00499690` WindowMsgPump — boot/window
  pair verified at main menu.
- `0x0040dfc0` HudIngameDispatch, `0x0042b8d0` StatePhaseIsIdle,
  `0x0042b8f0` StatePhaseIsFinal, `0x0042f510` Vehicle0HandleGet,
  `0x00498bf0` DisplayActiveFlagGet, `0x004c19f0` RwVtableSlot07Call,
  `0x0042b930` MenuAlphaGet — frame-dispatch leaves verified.

C2 nodes (Ghidra-mechanical, end-to-end re-read against decomp; no
runtime diff yet): the entire WinMain wrapper chain
(`0x00492370`, `0x00492270`, `0x00492290`, `0x004924f0`, `0x00492e90`,
`0x004921d0`, `0x00428590`, `0x00493600`), the main-loop init/tick
callees (`0x00492770`, `0x004929d0`, `0x00492d20`, `0x00492d30`,
`0x004926c0`, `0x00493480`), the AppInit/AppDestroy bookends
(`0x00402750`, `0x00402a40`), the launch-handshake thunks
(`0x00493540`, `0x00493550`, `0x00493560`, `0x00493900`,
`0x004938c0`), the RW engine init gate (`0x00493710`) and its
sub-callees (`0x004c2c90`, `0x004c2ed0`, `0x004c2f00`, `0x004c2fb0`).

C1 nodes (mechanical Ghidra read only): most of the post-CRT
`__heap_init` / `__mtinitlocks` block (`0x004aa3fe`, `0x004a8a04`,
`0x004ac04a`), the file-handle plumbing, the world-render sub-passes
in state 3/7 (`0x00426030`, `0x00426670`, `0x004266b0`, `0x00410b30`,
`0x0040de30`, `0x0040df20`, `0x0040df60`, `0x00404320`), and the
state-1/4 frontend tail (`0x00433f40`, `0x0043c5b0`, `0x00492e60`).

`[UNCERTAIN]` nodes:
- `0x00430b90` (state-4, sub-mode 0x20) — U-0642 open; the call is
  attested in `re/analysis/physics_collision/00492e90.md`, but the
  function itself (1693 bytes) has no decomp note and its role in the
  frame tick is inferred only from "called when `FUN_0042b930()` returns
  0x20".

Overall coverage: 23 nodes in the tree are C3+ (boot, msg pump,
frame-dispatch leaves); ~32 nodes are C2 (WinMain wrapper, main loop,
frame switch, RW engine init); ~17 nodes are C1 (CRT plumbing,
world-render sub-passes); 1 node is `[UNCERTAIN]`. Boot path is the
strongest column (CRT pre-init through window create is mostly C3);
frame tick is the weakest (most render sub-passes are still C1, awaiting
Phase 5 frontend/HUD sweep).
