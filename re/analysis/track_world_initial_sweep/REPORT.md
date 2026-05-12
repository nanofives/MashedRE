# Session 13 — track_world_initial_sweep — REPORT
Date: 2026-05-12  
Pool: Mashed_pool7 (fallback; Mashed_pool12 had stale OS lock)  
Model: Sonnet 4.6  
RVAs analyzed: 18 unique (19 effective; FUN_00420420 counted as 2)  
Cap: 20 effective — within budget  

## Entry-point search strategy

String xrefs searched:
- "TRACKS\\" / "toastart/tracks/" → found in FUN_00462950 (audio loader, already tracked) and FUN_00426e10 (TRACK_LOAD_FN, already tracked)
- "LAPDATA.LUA" / "COURSE.LUA" → inside FUN_00426e10 (already tracked)
- "Lap_Line_Change" / "Lap_Line_End" / "Lap_Line" / "Lap_Variations" → at 0x00440d9f–0x00440dcf, no Ghidra function object — part of the COURSE.LUA command registration block (LAB_00440bc0)
- "Course_Name" / "Course_Id" → also inside LAB_00440bc0 registration block
- "loading courseA\n" / "loading course\n" → inside FUN_00462950 (audio loader, already tracked)
- "course LoadTime is %u (%.3f)\n" → inside FUN_0040d270 (NEW — course load orchestrator)
- "TrackImages.txd" → FUN_0040bbb0 (boot/C1, already tracked)
- "Set_Track_Powerups" → at 0x00440f40, inside LAB_00440bc0 block, no function object
- TRACKFEATURE* strings (0x00603bb0+) → zero xrefs; data table only

Caller-chain approach (decisive):
- `FUN_00426e10` (TRACK_LOAD_FN) has ONE caller: `FUN_0040d020` (NEW)
- `FUN_0040d020` has ONE caller: `FUN_0040d270` (NEW — orchestrator)
- `FUN_0040d270` has ONE caller: `FUN_0040d440` (NEW — public entry, already had Ghidra plate but missing from CSV)
- `FUN_0040d440` called from `FUN_004929d0` (game_state, already tracked)

## Functions found

| RVA | Name | Subsystem | Notes |
|---|---|---|---|
| 0x0040d440 | Course::LoadCurrent | track | Only public entry; calls orchestrator with current track index |
| 0x0040d270 | Course::Load | track | Full orchestrator; cache-hit path + full reload path; times load |
| 0x0040d020 | load-by-index thunk | track | Dereferences table[idx] → TRACK_LOAD_FN |
| 0x0040d040 | cache validity check | track | Checks player track groups; returns 1 if reload needed |
| 0x0040d110 | post-load per-player | track | Assigns per-player track IDs; opens vehicle piz; checkpoint init |
| 0x0040cea0 | surface setup | track | dirt.piz; surface handle pair per track → per-player |
| 0x0040cf80 | teardown players | track | Pre-reload player/vehicle/audio teardown |
| 0x0040cfd0 | teardown world | track | Physics+audio+subsystem teardown; clears DAT_0080332c |
| 0x00426c00 | track ID getter | track | Returns DAT_00644158; had Ghidra plate |
| 0x00462510 | clear audio flag | track | _DAT_00603868=0 |
| 0x00462500 | set audio flag | track | _DAT_00603868=1 |
| 0x004264d0 | powerups loader | track | Medal-gated powerups_gold.lua or powerups.lua |
| 0x00426340 | KTCScript.lua loader | track | Registers COURSE.LUA cmd block at LAB_00440bc0 |
| 0x0041e980 | track descriptor lookup | track | scans s_training_005f33f8 by +0x10 field |
| 0x00420420 | per-player vehicle+CP init | track | BSP sectors; special objects; stride 0x2AC; heavy decomp |
| 0x00431d70 | tournament override getter | track | Returns DAT_0067ea94 |
| 0x00420230 | vehicle piz path builder | track | "toastart/vehicles/"+name |
| 0x004ccde0 | memory pool GC | util | Walks pool list; 3× in orchestrator |

## Track subsystem structure discovered

```
FUN_004929d0 (game_state)
  └─ FUN_0040d440 (Course::LoadCurrent)
       └─ FUN_0040d270 (Course::Load orchestrator)
            ├─ FUN_00462510  clear audio flag
            ├─ [cache hit path]
            │    └─ FUN_0040d040  cache check
            ├─ [full reload path]
            │    ├─ FUN_004ccde0  memory pool GC (×3)
            │    ├─ FUN_00462500  set audio flag
            │    ├─ FUN_004264d0  powerups loader
            │    ├─ FUN_0040cf80  teardown players
            │    ├─ FUN_0040cfd0  teardown world
            │    └─ FUN_0040d020  load by index
            │         └─ FUN_00426e10  TRACK_LOAD_FN (existing)
            ├─ FUN_0040d110  post-load per-player setup
            │    └─ FUN_00420420  per-player vehicle+CP init
            │    └─ FUN_0040cea0  surface setup
            └─ [ghost mode path]
                 └─ FUN_00426340  KTCScript.lua loader
                      └─ FUN_0047b9b0  RegisterScriptCommand (existing)
                           └─ LAB_00440bc0  COURSE.LUA command block
```

## Key globals identified

| Global | Address | Meaning |
|---|---|---|
| `DAT_0063ba7c` | 0x0063ba7c | Current track index |
| `DAT_0063ba78` | 0x0063ba78 | Previous track index |
| `DAT_0063ba8c` | 0x0063ba8c | Track loaded flag (set to 1 after load) |
| `DAT_00644158` | 0x00644158 | Current active track ID (returned by FUN_00426c00) |
| `_DAT_00603868` | 0x00603868 | Audio course active flag |
| `DAT_007f1a1c` | 0x007f1a1c | 16-entry player track-slot array |
| `DAT_0080332c` | 0x0080332c | Physics world handle (cleared on teardown) |
| `DAT_0063ba90/94` | 0x0063ba90/94 | Surface TXD handle pair |
| `DAT_0063d9e0` | 0x0063d9e0 | Player track state base (stride 0x2AC) |
| `DAT_0063d830` | 0x0063d830 | Per-player init complete flags |
| `DAT_0067ea94` | 0x0067ea94 | Tournament mode override track index |
| `PTR_PTR_005f2770` | 0x005f2770 | Track name table pointer |
| `s_training_005f33f8` | 0x005f33f8 | Track descriptor table (0x48B/record) |

## Stubs filed: S-3574..S-3611 (38 stubs)
## Uncertainties filed: U-3573..U-3576 (4)
## Deferrals: none (no cap overflow; 19/20 effective budget used)

## STOP-AND-ASK not triggered
- Track strings ARE present in MASHED.exe (no string-in-piz scenario).
- Callee tree did NOT explode past 60 (38 stubs, well within bound).
- Pool12 lock was an OS-held lock (not stale); fell back to pool7 autonomously per prior-session pattern.

## subsystem_map.md update
- `### track` entry updated with 5 entry-point RVAs and fingerprint strings.
