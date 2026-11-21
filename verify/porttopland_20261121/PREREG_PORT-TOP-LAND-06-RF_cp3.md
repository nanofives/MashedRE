# PREREG PORT-TOP-LAND-06-RF checkpoint 3 (0x00429290 title tick)

Committed before its arms (WORKER_RULES "What a change may do").

## Change

The matched 0x00402750 AppInitialiseOnBootup (`Match/PortTop_00402750.cpp`, `re/match/s01/00402750.c:122`) passes
0x00429290 as a callback to 0x004b6560: `FUN_004b6560(FUN_00429290, 0)`. On the port `FUN_00429290` is the stand-in
`PortTop_StandIn04E_00429290` (`Match/PortTop_00402750_Pieces.cpp:228`) and 0x004b6560 is
`PortTop_PizFs_SetIdle_004b6560`, which records the pointer and never calls it (the exe piz reader loads synchronously,
`PortTop_00402750_Pieces.cpp:140-146`).

Checkpoint 3:
1. Adds 0x00429290 as a second root of the renderframe profile (`re/tools/bind/state/renderframe/roots.txt`, data only)
   and lets `porttop_bind.py write --profile renderframe` bind its closure (dry plan on the landing tree: +7 binds incl.
   the root from `re/match/s06/00429290.c` (c4=match), +16 reuses, +1 stop).
2. Swaps the caller only: `FUN_00429290` in the 0x00402750 wrapper becomes `PortTop_Sel_00429290`
   (`Match/PortTop_00402750_Pieces.cpp`), a selector on a new sub-knob:

| `MASHED_OT_TITLETICK` | the pointer 0x00402750 passes to 0x004b6560 calls |
|---|---|
| unset or not `1` (default) | `PortTop_StandIn04E_00429290`, unchanged |
| `1` | `PortTop_Bound_00429290`, the byte-matched body, with its callees bound by the renderframe profile |

The selector runs only if something calls the recorded pointer. On this tree nothing does, so the bound closure is built
and linked, not executed, whatever the knob says. Reached only under `MASHED_ORIG_TOP=1`. No default-path code changes.

## Arms (knob boot, `verify/porttop_20261108/knob_boot.py`, test-slot outside, mashed_lock inside)

1. VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`: rc 0, `WinMain 0x00492370 returned 0`.
2. The same plus `MASHED_OT_TITLETICK=1`: rc 0, the same end line, and the `[PIZ_FS] 0x004b6560 callback <ptr> arg 0
   recorded` line shows the selector's address in both arms (the selector is the pointer in both).

Pass: both arms rc 0 with the end line; ddkobj check shows the new bound units IDENTICAL where their source is 100%.
G-CTRL / B / 12 captures not run (top-down row, the knob is off for those hashes).
