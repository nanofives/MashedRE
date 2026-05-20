# Subsystem Map

The Phase 1 deliverable. Identifies the major code areas in `MASHED.exe` and their entry points. Filled in as Phase 1 progresses — empty until then.

## Format

For each subsystem: a short prose description of its role, the RVAs of its top entry points (≤ 5), and the strings/imports that fingerprint it.

## Subsystems

### boot
- **Role:** WinMain, command-line parsing, language selection, video config load, RenderWare engine init.
- **Entry points:**
  - `entry` @ `0x004a4bb7` (PE OS entry — MSVC CRT startup; calls into WinMain)
  - WinMain RVA: TBD (downstream of `entry`)
- **Fingerprints:** strings `"RwEngineInit"`, `"videocfg.bin"`, language code constants from MashedRunner; MSVC EH runtime at `0x004a3d7e` / `0x004a8f33`.

### audio
- **Role:** RenderWare audio (RWS) loading, DirectSound playback, music streaming.
- **Entry points:**
  - `AudioBitScanForward` @ `0x005aee20` (C4 — canonical leaf; bsf wrapper used across the audio engine)
  - `AudioRwsChunkHeaderRead` @ `0x005ab380` (C3 — RWS 12-byte chunk header reader; entry to the RWS load chain)
  - `AudioDSoundSecondaryInit` @ `0x005bbfc0` (C3 — DirectSound secondary-buffer creation)
  - `AudioDSoundQIChain` @ `0x005bc400` (C3 — DSound COM QueryInterface chain on init)
  - `AudioContextLookup` @ `0x005ac900` (C3 — context-table lookup used by the SFX/mixer dispatchers)
- **Fingerprints:** RWS chunk header reader at `0x005ab380` calls `0x004cbd30` (RW stream reader); DirectSound COM references in `0x005bbfc0`/`0x005bc400`; `toastaudio/` path strings; cluster `re/analysis/audio_dsound*`, `audio_rws_loader*`, `audio_music*`, `audio_sfx_dispatch*`.

### ai
- **Role:** Driver AI for the 4 opponent slots — spline following, race-angle tracking, behavior modes, powerup gating.
- **Entry points:**
  - `FUN_00418860` @ `0x00418860` (C2 — AI tick loop; guards `DAT_00801ca0>3`, iterates 4 AI slots, dispatches per-vehicle update)
  - `FUN_00418560` @ `0x00418560` (C2 — per-vehicle AI update; selects spline (race/inside/slow/cheat) by type)
  - `FUN_004177b0` @ `0x004177b0` (C2 — AI pre-tick; updates `DAT_0089a880` race-angle array, mode-4/9 rubber-banding)
  - `FUN_00416250` @ `0x00416250` (C1 — primary AI control step; behavior-mode 0..10 decision tree)
  - `FUN_00423540` @ `0x00423540` (C2 — `AI%%d.AI` saver; writes 0x11884-byte block from `DAT_007f1a9c` with magic `0x13269902`)
- **Fingerprints:** `AI%%d.AI` format string referenced from `0x00423480`/`0x00423540`; AI data magic `0x13269902`; per-vehicle race-angle array `DAT_0089a880`; per-vehicle stride 0x30c at `0x008a9640`; cluster `re/analysis/ai_update*`, `ai_path_following*`, `ai_powerup_decisions*`.

### vehicle
- **Role:** Per-slot vehicle entities — slot lookup, dynamics integration, damage accumulation, replay/ghost recording.
- **Entry points:**
  - `VehicleSlotGetter` @ `0x0046c7b0` (C3 — `DAT_008815a4[idx*0x341]` slot lookup; canonical vehicle-by-index entry)
  - `VehicleRacePositionGet` @ `0x0046dbe0` (C3 — public race-position getter)
  - `VehicleWheelForceIntegrator` @ `0x0046ddb0` (C2 — per-wheel force integration step)
  - `VehicleCollisionBroadPhase` @ `0x004709a0` (C2 — broad-phase collision detection)
  - `ReplayRecordFrame` @ `0x00411600` (C3 — Replay system frame recorder; public replay entry)
- **Fingerprints:** vehicle slot stride `0x341` at `DAT_008815a4` (referenced by `0x0046c7b0`); per-vehicle stride `0x2AC` at `DAT_0063dc38` for triggers; replay/ghost cluster at `0x004110xx..0x00411dxx`; cluster `re/analysis/vehicle_damage*`, `vehicle_dynamics*`, `vehicle_update*`, `vehicle_promote_c2*`.

### track
- **Role:** Track loading, geometry, checkpoints, per-player track assignment, Lua scripted course events
- **Entry points:**
  - `FUN_0040d440` @ `0x0040d440` — Course::LoadCurrent (no args; loads `DAT_0063ba7c`-indexed track; sole public entry)
  - `FUN_0040d270` @ `0x0040d270` — Course::Load (table, idx; full orchestrator; called only from FUN_0040d440)
  - `FUN_00426340` @ `0x00426340` — KTCScript.lua loader (registers COURSE.LUA Lua command block at LAB_00440bc0)
  - `FUN_004264d0` @ `0x004264d0` — Track powerups loader (per-track powerups_gold.lua or powerups.lua)
  - `FUN_00420420` @ `0x00420420` — Per-player vehicle+checkpoint init (stride 0x2AC; BSP sectors)
- **Fingerprints:** strings `"toastart/tracks/"` (0x005cd4c0), `"LAPDATA.LUA"` (0x005cd578), `"COURSE.LUA"` (0x005cd584), `"TOTALNUMTRACKS-3=%d\n"` (0x005cd8b0), `"course LoadTime is %u (%.3f)\n"` (0x005ccd48), `"KTCScript.lua"` (embedded in FUN_00426340)

### render
- **Role:** RenderWare 3.x graphics core driving the D3D9 backend — matrix/vector math, device setup, primitive submission, viewport rendering.
- **Entry points:**
  - `RwV3dTransformPoint` @ `0x004c3730` (C4 — `out=mat4x4*in+translation`; default fn-ptr A in RW path table at `DAT_007d3ffc+DAT_007d3ff8+0x08`)
  - `RwV3dTransformVector` @ `0x004c3880` (C4 — vector-only transform, no translation)
  - `RwMatrixScale` @ `0x004c5010` (C4 — RW SDK `RwMatrixScale(mat, scale, combineOp)`)
  - `RwRenderPrimitiveSubmit` @ `0x004cd070` (C2 — primitive submission to the RW immediate device)
  - `PerPlayerViewportRender` @ `0x00420050` (C2 — per-player viewport render; split-screen renderer entry)
- **Fingerprints:** RW path-table base at `DAT_007d3ffc+DAT_007d3ff8`; `_rwDeviceSystemFn` at `0x004c7a70`; RW string anchors `"RwEngineInit"` / `"RwEngineOpen"` from boot; D3D9 device creation lives under `re/analysis/render_d3d9_device*` and is intercepted by the dev-time d3d9 shim; cluster `re/analysis/render_frame*`, `render_d3d9_device*`, `render_pipeline*`, `render_lighting*`.

### hud
- **Role:** In-race HUD — viewport sub-modes, font/text rendering, lap/position overlays, per-player HUD slot dispatch.
- **Entry points:**
  - `HudIngameDispatch` @ `0x0040dfc0` (C3 — primary in-race HUD dispatcher)
  - `Sub00403160_SubMode0BViewport` @ `0x00403160` (C4 — sub-mode 0xB viewport setup, 0.8f scale)
  - `FontText_StringTableLookup` @ `0x00427780` (C4 — packed-string-table lookup at `0x66d828`)
  - `FontText_UTF16WidenCopy` @ `0x00427840` (C4 — `__fastcall` UTF-16 widen-copy used by text draw paths)
  - `FontCtx_SetTranslation` @ `0x00552df0` (C4 — font-context translation setter; canonical-scenario verified)
- **Fingerprints:** font string table base `0x66d828` (referenced by `0x00427780`); font-context table at `0x00552xxx`; HUD slot dispatchers cluster at `0x0041axxx..0x0041cxxx`; cluster `re/analysis/hud_ingame*`, `hud_frontend*`, `hud_promote_c2*`.

### frontend
- **Role:** Main menu and front-end screens — sprite tables, button detectors, mode/race-end gates, player-slot checks.
- **Entry points:**
  - `FrontendGlobalGet` @ `0x0040ad20` (C4 — pure getter `DAT_008a95ac`; canonical front-end global)
  - `FrontendArrayGet` @ `0x0040b6c0` (C4 — indexed read `DAT_008a94f0[idx]`; front-end array base)
  - `FrontendPlayerSlotCheck` @ `0x0042ebe0` (C4 — player-slot fullness check; iterates `0x7f0a48` stride `0x1e0`)
  - `MenuAlphaGet` @ `0x0042b930` (C4 — pure getter `DAT_0067ecb0`)
  - `SpriteSlotGate` @ `0x0042ee00` (C4 — sprite-slot dispatcher gating `FUN_0040bb50` on slot {0,1,2})
- **Fingerprints:** front-end array `DAT_008a94f0` and global `DAT_008a95ac`; menu-alpha global `DAT_0067ecb0`; sprite tables at `0x0040bbxx`; button-detector cluster at `0x0042axxx..0x0042bxxx`; `Frontend.piz` asset references; cluster `re/analysis/frontend_promote_menus*`, `frontend_c0_promote*`, `frontend_unmapped_a*`.

### input
- **Role:** DirectInput8 device creation, controller config load, Lua-driven joypad remap.
- **Entry points:**
  - `CreateDInputObject` @ `0x00495530` (C3 — DI8Create wrapper; callees include `DirectInput8Create` and `0x004987b0`)
  - `DInputInitPredicate` @ `0x004955b0` (C3 — DInput-init predicate, gates device enumeration)
  - `ControllerConfigLoad_j5` @ `0x004971b0` (C3 — controller config loader; reads remap data)
  - `JoypadStrcpy` @ `0x00495830` (C3 — joypad-name strcpy helper)
  - `DirectInput8Create` @ `0x0049b300` (C1 — import thunk to `dinput8.dll!DirectInput8Create`)
- **Fingerprints:** `dinput8.dll` import (`DirectInput8Create`); `remap.lua` string anchor; controller-config block referenced by `0x004971b0`; cluster `re/analysis/input_dinput*`, `input_lua*`.

### save
- **Role:** Game-save read/write, `videocfg.bin` settings persistence, video-settings dialog.
- **Entry points:**
  - `SaveLoad` @ `0x00404e50` (C3 — gamesave reader; primary save entry)
  - `SaveWrite` @ `0x00404f50` (C3 — gamesave writer)
  - `SaveFileExists` @ `0x00404f80` (C3 — gamesave-presence probe)
  - `ConfigLoad` @ `0x00498950` (C3 — `_fsopen("videocfg.bin","rb",_SH_DENYNO)`; fread 512B into `0x773208`)
  - `ConfigSave` @ `0x004989b0` (C3 — `videocfg.bin` writer)
- **Fingerprints:** `"videocfg.bin"` string referenced by `0x00498950`; `videocfg.bin` 512-byte buffer at `0x773208`; gamesave-state cluster at `0x00404exx..0x00404fxx`; VFS helpers `VfsFileExists` @ `0x00550b00`, `VfsStreamRead` @ `0x00550980`; cluster `re/analysis/save_gamesave*`.

### net
- **Role:** Multiplayer networking — **no evidence of network multiplayer in this build**.
- **Entry points:** N/A — `hooks.csv` has zero rows tagged `subsystem=net` (0/6058). Local split-screen "multiplayer" mode exists (see `IsMultiplayerMode` @ `0x00430760`, `SplitScreenTrackAssignment` @ `0x00430830` under `frontend`) but it is purely shared-screen, not networked.
- **Fingerprints:** absent — no `WSAStartup`, `wsock32.dll`, or `ws2_32.dll` imports observed in the analysis corpus; no `socket()`/`bind()`/`recv()` import thunks tagged. If a net subsystem ever surfaces, look first for WS2 imports and multiplayer lobby strings.

### util
- **Role:** Cross-subsystem leaf helpers — fast math (sqrt/invsqrt/magnitude), timer dispatch, pending-op queue, generic accessors.
- **Entry points:**
  - `Vec3Magnitude` @ `0x004c3ac0` (C4 — `FastSqrt(x²+y²+z²)`; canonical-scenario verified)
  - `FastSqrt` @ `0x004c3b30` (C4 — table-driven fast sqrt)
  - `FastInvSqrt` @ `0x004c3b90` (C4 — table-driven fast inverse sqrt)
  - `TimerSlotTickDispatcher` @ `0x0043c000` (C4 — per-slot timer tick dispatcher)
  - `PendingOpQueueFlush` @ `0x00475a60` (C4 — pending-op queue flush; cross-subsystem deferred-work drain)
- **Fingerprints:** fast-math lookup tables referenced by `0x004c3b30`/`0x004c3b90`; `TimerGetBasePtr` @ `0x00413f90`; timer dispatcher family `TimerDispatch10/30/70` @ `0x00426c10..70`; cluster `re/analysis/util_c0_promote*`.

### unknown
- **Role:** Functions where the subsystem is genuinely indeterminate from current evidence.
- **Entry points:** TBD — only 2 of 6058 rows in `hooks.csv` carry `subsystem=unknown`, and neither is yet a named public API. Promote them out of `unknown` as soon as the calling context is identified.
- **Fingerprints:** N/A by definition; this bucket is residue, not a real subsystem.
