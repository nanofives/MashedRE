
## title_screen-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/title_screen_d2/
**Parent:** title_screen-20260506-VVVVV (D=7300..7306, bucket title_screen-cont1)
**Subsystem:** frontend

Depth-2 deferred callees from title_screen session VVVVV:

| D | Address | Description |
|---|---------|-------------|
| D-7300 | 0x00428450 | FUN_00428450 — ticker/overlay; args (0x20, 0xffffffe0) in title context, (0x10, 0x100) in lobby; S-2460 U-2470 |
| D-7301 | 0x004288a0 | FUN_004288a0 — dark/blank screen renderer; called from FUN_00428a30 when DAT_0067d84c==0; S-2461 |
| D-7302 | 0x00428320 | FUN_00428320 — text renderer; renders build date + string 0x222 in title and attract; S-2462 |
| D-7303 | 0x0042e590 | FUN_0042e590 — sprite draw; car sprite at (320.0, 260.0) in attract renderer; S-2463 U-2469 |
| D-7304 | 0x0040d250 | FUN_0040d250 — lobby visual element (gradient/line/logo path); depth-2 of lobby renderer; S-2464 |
| D-7305 | 0x00401ee0 | FUN_00401ee0 — lobby visual element; depth-2 of lobby renderer; S-2465 |
| D-7306 | 0x0042f0b0 | FUN_0042f0b0 — lobby UI sub-renderer at position (347.5, 168.0); S-2466 |

---

## game_state_d3-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/game_state_d4/
**Parent:** game_state_d3 (D=7240..7242, bucket game_state_d3-cont1)
**Subsystem:** util

Depth-4 deferred callees from game_state_d3 session UUUUU:

| D | Address | Description |
|---|---------|-------------|
| D-7240 | 0x00445aa0 | FUN_00445aa0 — type-0 handler; called with (entry_ptr, param_1) from FUN_004464c0; entry from array at 0x008964c0 stride 0xd8; S-2440 |
| D-7241 | 0x00441d40 | FUN_00441d40 — type-1 handler; called with (entry_ptr, param_1) from FUN_004464c0; same array; S-2441 |
| D-7242 | 0x00442440 | FUN_00442440 — type-2 handler; called with (entry_ptr, param_1) from FUN_004464c0; same array; S-2442 |

---

## intro_splash_d2-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/intro_splash_d3/
**Parent:** intro_splash_d2 (D=6940..6941, bucket intro_splash_d2-cont1)
**Subsystem:** frontend

Depth-3 deferred callees from intro_splash_d2 session PPPPP:

| D | Address | Description |
|---|---------|-------------|
| D-6940 | 0x004d8000 | FUN_004d8000 — list insertion function (82b); called from FUN_004c77c0 as `FUN_004d8000(&DAT_00618180, alloc_result)` on successful video texture allocation; referenced in rw_engine_init and powerups contexts; S-2340 |
| D-6941 | 0x004d8c40 | FUN_004d8c40 — no-arg function (117b); called immediately before vtable slot 38 dispatch in FUN_004c7730; purpose unknown; S-2341 |

---

## leaderboard-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/leaderboard/
**Parent:** leaderboard-20260506-JJJJJ (D=6580..6589)
**Subsystem:** util

Depth-2 deferred callees from leaderboard session JJJJJ:

| D | Address | Description |
|---|---------|-------------|
| D-6580 | 0x0040e560 | FUN_0040e560 — intermediate caller between TimeTrial tick and Replay::LapFinish; confirm call chain |
| D-6581 | 0x0040fc00 | Race main tick (620b, ~30 callees); calls TimeTrial::Tick; broad |
| D-6582 | 0x00430b00 | FUN_00430b00 — HUD time display updater (S-2220); resolve split/best display path |
| D-6583 | 0x0042f790 | FUN_0042f790 — ghost-mode flag getter (S-2225) |
| D-6584 | 0x0040d040 | FUN_0040d040 — car finish validator (S-2226) |

---

## audio_music_d2-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/audio_music_d3/
**Parent:** audio_music_d2 (D-6520..D-6521, bucket audio_music_d2-cont1)
**Subsystem:** audio

Depth-3 callees from audio_music_d2. All items deferred:

| D | Address | Description |
|---|---------|-------------|
| D-6520 | 0x005a7520 | Audio state dispatcher: mode 0→FUN_005a7560, 1→FUN_005a7460, 2→FUN_005a75b0 (conditional on node+0xc bit-0) |
| D-6521 | 0x005a6d60 | Actual audio parameter setter (4 args; param_4 by address); resolve U-2209 U-2210 |

---

## track_loader_d3-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/track_loader_d4/  
**Parent:** track_loader_d3 (D-5740..D-5755, bucket track_loader_d3-cont1)  
**Subsystem:** render/track  

Depth-4 callees from track_loader_d3. All 16 stubs deferred:

| D | Address | Description |
|---|---------|-------------|
| D-5740 | 0x004b3c60 | BSP/RpWorld stream reader |
| D-5741 | 0x00558df0 | UVAnim chunk loader |
| D-5742 | 0x004b3cc0 | Spline stream reader |
| D-5743 | 0x004b3de0 | Animation stream reader |
| D-5744 | 0x00479030 | Course post-load callback table |
| D-5745 | 0x00474fb0 | DFF clump node iterator |
| D-5746 | 0x00474f30 | Sky dome per-node callback |
| D-5747 | 0x0047f4c0 | Physics world constructor |
| D-5748 | 0x0047d080 | Activate physics body slot |
| D-5749 | 0x0047d100 | Secondary enable physics body |
| D-5750 | 0x00487280 | Broadphase body registration |
| D-5751 | 0x0047be80 | Triangle mesh init |
| D-5752 | 0x0047bcc0 | Collect portal/neighbor list |
| D-5753 | 0x004b53b0 | Bounding sphere builder |
| D-5754 | 0x004c3d90 | Sector bounding geometry builder |
| D-5755 | 0x00546380 | Audio waypoint set constructor |

---

## input_dinput_d2-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/input_dinput_d3/
**Parent:** input_dinput_d2 (D=6760..6766, bucket input_dinput_d2-cont1)
**Subsystem:** input

Depth-3 deferred callees from input_dinput session MMMMM:

| D | Address | Description |
|---|---------|-------------|
| D-6760 | 0x004972b0 | FUN_004972b0 — called 3rd each frame in FUN_004967e0 before keyboard read; purpose unknown |
| D-6761 | 0x0045b350 | FUN_0045b350 — called 2nd each frame; cross-subsystem RVA range; purpose unknown |
| D-6762 | 0x00499720 | FUN_00499720 — HINSTANCE getter (called by FUN_00498510 for LoadStringA) |
| D-6763 | 0x00495830 | FUN_00495830 — per-slot default button mapping setup (called by FUN_00498510) |
| D-6764 | 0x004971b0 | FUN_004971b0 — saved config comparison per slot (called by FUN_00498510) |
| D-6765 | 0x004a2c48 | FUN_004a2c48 — button state byte reader (called by FUN_00497310 analog path + FUN_00496530) |
| D-6766 | 0x00495ee0 | LAB_00495ee0 — EnumDevices callback; Ghidra label only; needs function_create first |


## librw_plugin_compat  [queued 2026-05-06]

**Bucket:** re/analysis/librw_plugin_compat/
**Parent:** rw_engine_init/004c32b0 (C1 mapped)
**Subsystem:** render
**Driver:** librw integration feasibility study

Goal: validate per-plugin offset compatibility between Mashed's RW plugin registry and librw before committing to librw-as-RW-substitute strategy.

Mashed registers 14 plugins at FUN_004c32b0 via FUN_004d7de0 calls. The (size, id) pairs from re/analysis/rw_engine_init/004c32b0.md:

| Slot | Size  | ID    | Notes |
|------|-------|-------|-------|
| 1    | 8     | 0x40f | |
| 2    | 0x18  | 0x401 | |
| 3    | 0     | 0x40d | size-0; sentinel? |
| 4    | 0x18  | 0x402 | |
| 5    | 4     | 0x403 | |
| 6    | 4     | 0x404 | |
| 7    | 4     | 0x405 | |
| 8    | 0x220 | 0x406 | largest — likely vehicle/track data |
| 9    | 100   | 0x407 | |
| 10   | 0x34  | 0x408 | |
| 11   | 0x60  | 0x409 | |
| 12   | 4     | 0x412 | |
| 13   | 0x74  | 0x40a | |
| 14   | 0x28  | 0x40b | |

**Tasks:**
1. For each ID in `0x401..0x40f` and `0x412`: identify the plugin name from RW SDK conventions (these are toolkit-range plugin IDs).
2. Cross-check librw source (`librw/src/`) for each ID: present? same size? same constructor/destructor pattern?
3. Identify which IDs are RW-standard (matfx/anim/skin/etc.) vs Mashed-specific extensions.
4. Output: a compat matrix `(id, size, mashed_callbacks, librw_match, gap_action)` to seal the librw decision.

**Blocks:** any decision to vendor librw as a dependency.

---

## piz_fsmanager_handler  [queued 2026-05-06]

**Bucket:** re/analysis/piz_fsmanager_handler/
**Parent:** launch_handshake (FUN_004955d0 = HardwareInstallFileSystem, C1)
**Subsystem:** util
**Driver:** librw integration feasibility study

Goal: trace Mashed's RtFSManager file-system handler to determine whether `.piz` archive reads route through the FSManager VFS layer (clean shim point) or are bolted in elsewhere.

**Known anchors:**
- `FUN_004955d0` (HardwareInstallFileSystem, 0x004955d0) — installs an FS handler via `FUN_00551190(0x14, ...)` for cwd and per-drive (re/analysis/launch_handshake/0x000955d0.md).
- `FUN_00551190` is RW's RtFSManager install routine; first arg `0x14` is the FS type/size descriptor.
- `&PTR_DAT_005cfee0` is passed as third arg — this is the FS handler vtable (open/read/close/etc.).

**Tasks:**
1. Decompile `PTR_DAT_005cfee0` as an RtFSManager FS handler vtable; enumerate function pointers (open/close/read/write/seek/stat/etc.).
2. For each vtable entry, identify the implementation function and check whether it dispatches to `.piz` archive reader (re/tools/piz_extract.py shows the format).
3. Determine: is `.piz` reading (a) inside the FS handler vtable (clean shim), or (b) called separately from game code (more invasive shim).
4. If (a): document the FS handler signature so a librw-side stub can call into our `.piz` reader.
5. If (b): identify each `.piz` open/read site and assess refactor cost.

**Blocks:** librw integration design; cannot decide on `RtFSManager` shim approach without knowing the call graph.
