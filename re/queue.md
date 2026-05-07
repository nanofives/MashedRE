
## title_screen_d2-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/title_screen_d3/
**Parent:** title_screen_d2 (D-7540, bucket title_screen_d2-cont1)
**Subsystem:** render

Depth-4 callee from title_screen_d2:

| D | Address | Description |
|---|---------|-------------|
| D-7540 | 0x004c52f0 | FUN_004c52f0 — called by FUN_004c1480 as FUN_004c52f0(param_1+0x10, param_2, param_3); likely RW matrix/frame operation; purpose unknown; S-2543 |

---

## game_state_d4-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/game_state_d5/
**Parent:** game_state_d4 (D=7420..7421, bucket game_state_d4-cont1)
**Subsystem:** util

Depth-5 deferred callees from game_state_d4 session:

| D | Address | Description |
|---|---------|-------------|
| D-7420 | 0x00441c80 | FUN_00441c80 — interpolated XYZ getter from two vehicle slots; reads FUN_0040e180 for defaults; lerp by _DAT_005cc32c; S-2500 |
| D-7421 | 0x004430a0 | FUN_004430a0 — 9B setter: DAT_00897fe0 = param_1; called with 0 in 40-entry player-slot loop in FUN_00445aa0; S-2501 |

---

## title_screen-cont1  [COMPLETED 2026-05-06 — title_screen_d2 session]

All 7 items (D-7300..D-7306) drained. S-2460..S-2466 cleared. U-2469 partially resolved (U-2547 filed); U-2470 resolved. New depth-3: D-7540 (FUN_004c52f0); S-2540..S-2544 queued in title_screen_d2-cont1.

---

## game_state_d3-cont1  [COMPLETED 2026-05-06 — game_state_d4 session]

Drained: D-7240/7241/7242 analyzed as C1. New stubs S-2500 (D-7420), S-2501 (D-7421) queued in game_state_d4-cont1 below.

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

## librw_plugin_compat  [COMPLETED 2026-05-07 — session librw_plugin_compat-20260507-1950]

REPORT.md written to re/analysis/librw_plugin_compat/REPORT.md.
14/14 plugin IDs identified (all rwVENDORID_CRITERIONINT range). 4 present in librw (all size-mismatched). 10 absent.
Decision: librw-as-substitute NOT viable as-is. 14 stubs required.
Gating next step: D-8560 (decompile 28 callbacks) → bucket librw_plugin_compat-cont1.
IDs used: U-2887..U-2891, D-8560. Scribe queued.

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
