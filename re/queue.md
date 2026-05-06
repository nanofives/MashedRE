
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

