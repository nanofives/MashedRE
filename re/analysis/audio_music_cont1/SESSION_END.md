# Session End — audio_music_cont1

**Session ID:** audio_music_cont1-20260512
**Pool slot:** Mashed_pool4 (pre-assigned Mashed_pool2 was locked; pool4 used instead)
**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓ (pool4 confirmed PE/x86 MASHED.exe)
**Parent:** audio_music_d2-20260506 (12 D-rows, all already resolved)

## Drift resolution

All 12 D-rows (D-1780..D-1791) tagged `audio_music-cont1 session` were already analyzed by `audio_music_d2-20260506`. Rows drift-cleared (strikethrough) in DEFERRED.md. No new plates needed for those 12.

## Functions analyzed (new — C1)

| RVA | Name | Notes file | Key finding |
|---|---|---|---|
| 0x005a7460 | FUN_005a7460 | 0x005a7460.md | Dequeue audio node from priority queue; stop sub-obj via param_type=6/9; insert to stopped list DAT_007dca24 |
| 0x005a7560 | FUN_005a7560 | 0x005a7560.md | Optional dequeue then FUN_005a75b0 recompute+re-insert; sets bit-1 at +0xC; increments counters |
| 0x005a75b0 | FUN_005a75b0 | 0x005a75b0.md | Priority score (vol × distance attenuation) + sorted insert into DAT_007dca18 queue; fast-sqrt table at DAT_00633b48 |
| 0x005a6e10 | FUN_005a6e10 | 0x005a6e10.md | Audio parameter query wrapper → FUN_005a6d90 (alt table at param_1[1]+8) |
| 0x005a6d90 | FUN_005a6d90 | 0x005a6d90.md | Indirect dispatch via 8-byte fn-ptr table at param_1[1]+8; query counterpart to FUN_005a6d60 setter |

## Stubs resolved

| ID | RVA | Resolution |
|---|---|---|
| S-2560 | 0x005a7460 | C1 plate written; D-7600 cleared |
| S-2561 | 0x005a7560 | C1 plate written; D-7601 cleared |
| S-2562 | 0x005a75b0 | C1 plate written; D-7602 cleared |

## Uncertainties introduced

| ID | Topic |
|---|---|
| U-3684 | param_type=6 enum values (2→1 transition) in sub-object stop |
| U-3685 | param_type=9 dispatch via FUN_005a6df0 |
| U-3686 | DAT_007dca24 vs DAT_007dca18 list semantic (stopped vs pending) |
| U-3687 | FUN_005a75b0 priority compute — decompiler only; fast-sqrt table |
| U-3688 | DAT_005cc574 ±∞ sentinel float value |
| U-3689 | Dual dispatch table offsets (+4 setter vs +8 query) |

## Audio system globals documented

| Global | Address | Role |
|---|---|---|
| DAT_007dca18 | 0x007dca18 | Sorted priority queue head |
| DAT_007dca1c | 0x007dca1c | Priority queue tail |
| DAT_007dca24 | 0x007dca24 | Stopped/inactive node list head |
| DAT_007dca3c | 0x007dca3c | Queue node count |
| DAT_007dca40 | 0x007dca40 | Accumulated priority sum |
| DAT_007dca44/48/4c | 0x007dca44.. | Listener 3D position (X/Y/Z float) |
| DAT_00633b48 | 0x00633b48 | Fast-sqrt lookup table (128 shorts) |

## Tracker changes

- **hooks.csv**: +5 rows (005a7460, 005a7560, 005a75b0, 005a6e10, 005a6d90) all C1/audio/new
- **STUBS.md**: S-2560 S-2561 S-2562 → resolved
- **DEFERRED.md**: D-1780..D-1791 → drift-cleared; D-7600 D-7601 D-7602 → resolved
- **UNCERTAINTIES.md**: +U-3684..U-3689
- **SCRIBE_QUEUE.md**: queued audio_music_cont1-20260512

## No depth-5 stubs filed

All static callees in the 5 new functions are either already at C1 or analyzed in this session. No new D-rows needed.

## audio_music_state.md NOT drafted

Globals and node layout are documented inline in the plates. A consolidated struct file would need FUN_005a75b0's priority calc verified at listing level (U-3687) before it's trustworthy. Deferred.
