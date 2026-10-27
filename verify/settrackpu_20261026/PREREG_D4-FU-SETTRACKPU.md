# PREREG D4-FU-SETTRACKPU (2026-10-26): Set_Track_Powerups rows in the first-run save, and a write watch on 0x008a95ac

Committed alone, before any arm. Driver `run_stp.py`, offline model `predict.py`.

## Change under test
- `Save/GameSaveFormat.h`: `kSpanRowCourseId` (the 13 course ids behind `FUN_0040d430` = `[0x005f2770]` =
  `0x005f2728`, read from `MASHED.exe.unpatched` .data: 30, 3, 39, 0, 38, 11, 25, 2, 36, 34, 37, 33, 26),
  `TrackPowerupsRow` (`0x00430250`), `SetTrackPowerups` (`0x0047b280`), and `RunTrackPowerupsScript` (the
  statement set of `POWERUPS_ALL.LUA`: `local NAME = INT`, `--` comments, 7-argument `Set_Track_Powerups` calls).
- `Race/GameFlow.cpp`: `BuildSpanFromProgress` runs `POWERUPS_ALL.LUA` out of `Powerups.piz` over the span after
  `InitSpanDefaults`. Boot order in the original: `FUN_00492370` calls `FUN_004924f0` at `0x0049239f`, then
  `FUN_00402750` at `0x004923d0`, which reaches `FUN_0045bae0`, which calls `FUN_0047b9b0("powerups_all.lua"
  0x005ce628, &local, 0x00440bc0)` at `0x0045bb7d`. `0x00440bc0` registers `0x0047b280` at `0x00440f3b`.
- `re/frida/nav_champ_probe.py`: `NAV_WATCH_WRITE=<va>,..` arms hardware write watchpoints (debug registers
  0..3) on absolute globals right after attach, on every thread and on threads created later.

No knob. The rebuild runs only when there is no valid save file (first run) or with
`MASHED_SAVE_KEEP_IMAGE=0`, and the gate arms never save.

## Static model (analysis, before the PREREG)
`predict.py O_0.bin POWERUPS_ALL.LUA`: SPANINIT model vs `O_0` 269 differ (rows 252, col 4 12, gate 1, gather 3,
`0x24efc` 1). Model + the 39 script calls vs `O_0`: **17** differ (col 4 12, gate 1, gather 3, `0x24efc` 1), rows 0.
0 skipped statements.

## Predictions
| id | arm | prediction |
|---|---|---|
| Q1 | N_0 new exe first run | `N_0` == `predict.py` image byte for byte. vs `O_0`: 17 differ, 0 in `0x24cb0..0x24db3` |
| Q2 | N_P / N_G / N_1, default | output == input byte for byte (KEEP_IMAGE path untouched) |
| Q3 | N_P0, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | informative: vs SPANINIT's `N_P0`, every differing byte is in `0x24cb0..0x24db3` |
| Q4 | GATES, no env | G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (`merge_train.json` refs) |
| Q5 | W, original first-run boot to mode select, watch on `0x008a95ac`, `0x007f105c`, `0x007f10a8`, `0x007f10f4` | the watch arms with no error. Before the save: `[0x008a95ac]` = 1 and the low bytes of the three gather dwords = 0 (as in `O_0`). Every write hit is listed with its pc (the instruction after the store) and backtrace. Informative for which writer: candidates for `0x008a95ac` are `0x00409e30` (`MOV [0x008a95ac],1`) and `0x0040ad34` (`FUN_0040ad30`). `O_W` (the save this run writes) == `O_0` outside the fields the watch names, else the difference is reported |

If Q5 records no hit on an address whose value is non-zero at the save, the writer ran before the attach and the
watch result for that address is `[UNCERTAIN]` (attach timing), not a negative.

RED conditions: Q1 or Q2 or Q4 fails. Q3 and Q5 are informative. Piz-collision crashes are re-runs.
