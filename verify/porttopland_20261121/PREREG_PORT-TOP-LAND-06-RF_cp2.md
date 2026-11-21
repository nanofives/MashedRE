# PREREG PORT-TOP-LAND-06-RF checkpoint 2 (0x0040d110 caller swap)

Committed alone, before its arms (WORKER_RULES "What a change may do").

## Change

The matched course load 0x0040d270 (`Match/PortTop_0040d270.cpp`, `re/match/top07/0040d270.c`, calls at lines 107 and
115) calls 0x0040d110 through `PortTop_StandInRL_0040d110`, a counting stand-in. RACE-BIND-C4-BULK cp1 bound the body
`PortTop_Bound_0040d110` (`Match/PortTop_0040d110.cpp`, `re/match/topm01/0040d110.c`, c4=match), held so far only by a
keep-alive anchor (`Match/PortTop_BulkRefs.cpp`). Checkpoint 2 swaps the caller only: `FUN_0040d110` in the 0x0040d270
wrapper becomes `PortTop_RL_0040d110` (`Match/PortTop_StandInsRL.cpp`), a selector on a new sub-knob:

| `MASHED_OT_RL_D110` | 0x0040d270 calls |
|---|---|
| unset or not `1` (default) | `PortTop_StandInRL_0040d110`, the stand-in, unchanged |
| `1` | `PortTop_Bound_0040d110`, the byte-matched body; its callees 0x0040cea0, 0x0041a8d0, 0x00420420 are named stops of `Match/PortTop_BulkStops.cpp` (exit with code = rva, or log and return 0 under `MASHED_OT_RL_STOPS=count`) |

The selector is reached only under `MASHED_ORIG_TOP=1` + `MASHED_OT_RACELOAD=1` with a file-backed .data table at
[0x005f2770] (`PortTop_Bound_0040d270`, `Match/PortTop_StandInsRL.cpp`). No default-path code changes.

## Arms (knob boot, `verify/porttop_20261108/knob_boot.py`, test-slot outside, mashed_lock inside)

1. VS2022 exe, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`: rc 0, `WinMain 0x00492370 returned 0`.
2. The same plus `MASHED_OT_RL_D110=1 MASHED_OT_RL_STOPS=count`: rc 0. Expected: the same end line. If the log carries
   `rva 0x0040d270: MASHED_OT_RACELOAD=1, matched body Orig_0040d270 ... entered`, the selector line
   `rva 0x0040d110: matched body PortTop_Bound_0040d110` must follow; if 0x0040d270 does not run in 3000 frames the
   change is built and linked but not executed, and the RESULT says so.

Pass: both arms rc 0 with the end line. G-CTRL / B / 12 captures not run (top-down row, the knob is off for those hashes).
