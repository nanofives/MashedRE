# PREREG D4-WSH-EXEFILE: 0x004325c0 twin rounding under a forced dt

Committed before any arm runs. Base `orch/integration` @ `96e7f1ad2`.

## Why this run

Text diff of the 17 `exe_file != file` C4 rows (RESULT to follow) found 4 real two-body pairs.
For `0x004325c0` the two copies differ in their float-to-int step:

- `.asi` twin `Frontend/MenuAnimTickTwin.cpp:34` `Trunc48(v) = (int)std::nearbyint(v)` (round to nearest).
- exe copy `Frontend/MenuNavSM.cpp` `AnimStep(v) = (int)v` (truncation), plus `kMenuAnimSpeedup = 6.0f`.
- Original `FUN_004a2c48` (decomp, pool10, 2026-10-26): `ROUND(ST0)` then a sign-aware correction that
  subtracts 1 when the positive remainder is negative and adds nothing for the mirrored negative case,
  so the result is truncation toward zero (`_ftol2` shape).

The twin's C4 evidence (`re/analysis/standalone_menu_sm/diff_menu_anim_tick_navdriven.json`, 20/20) ran at
the game's own dt. At one 50 ms tick, `DAT_007f1004 = 50 * 3.3333333e-4 = 0.016666668`, and the branches a
pushed screen exercises give `-2000*dt = -33.33` and `-800*dt = -13.33`. Round and truncation agree on both,
so that evidence cannot separate the two. At two ticks (`dt = 0.033333336`) `-2000*dt = -66.67`, where
round gives -67 and truncation gives -66.

## Arms (each one boot, `re/frida/menu_anim_diff.py`, private root, under `mashed_lock.py`)

| Arm | `MENU_ANIM_DT` | Prediction |
|---|---|---|
| A0 | unset (game dt) | GREEN 20/20 (replicates the 2026-06-10 run) |
| A1 | `0.016666668` (1 tick) | GREEN 20/20, unless a `0xff040000` record has type not in {0, 2, 0x1000}: then RED with `vb - va = +1` at `+0x10` |
| A2 | `0.033333336` (2 ticks) | RED: first differing dword in a `tag 0xff040000` record at `off 0x10`, `va = v0 - 66`, `vb = v0 - 67` |

## Decision rule

- A2 RED with that signature: the `.asi` twin (the body the row's C4 evidence measured) disagrees with the
  original on a reachable dt (a frame of 97 ms or more snaps to 100 ms at `0x0049349b`, giving 2 ticks).
  Demote `0x004325c0` through `re-classify`. The exe copy is not tested by any arm (it is exe-only and
  operates on `g_records`); it stays without evidence either way.
- A2 GREEN: the original does not truncate at this site as read. Record it, no demotion, and file an
  UNCERTAINTIES question on the `FUN_004a2c48` reading.
- A0 RED: harness or state drift, no verdict from A1/A2 until it is explained.
- A piz-collision crash (`MASHED.exe+0x1509b4` / `+0xe66db`) is a re-run.
