# PREREG RACE-RWGLOBALS-FRAMELIST (run 53, 2026-11-21)

Committed alone, before any build or arm. Row kind: top-down / race path. Branch `orch/RACE-RWGLOBALS-FRAMELIST` from
`orch/integration` `b24bba46c`.

## Evidence the change rests on

- The fault (RESULT_PORT-OT-RACE.md 7.2): the matched 0x00458c70 calls `RwCore_004c15c0`, which faults at original
  0x004c1645 writing `[[RwGlobals+0xbc]+4]`. On the ORIG_TOP arm `[0x007d3ff8]` is the librw record of
  `LibRw/RwApiBind.cpp` (`MashedRw_InstallRwGlobals`), whose +0xbc is 0.
- The writer of +0xbc / +0xc0 in the RW core: `re/match/s19/004c07b0.c` (0x004c07b0, hooks.csv C4 c4=match, exe body
  `RwCore/RwCore_004c07b0.cpp`): `DAT_007d3e70 = offset`, `[RwGlobals + offset] = FUN_004cc9e0(DAT_00617f78,
  DAT_00617f90, 4, DAT_00617f94, &DAT_007d3e78, 0x4000e)`, then `[RwGlobals+0xbc] = [RwGlobals+0xc0] = RwGlobals+0xbc`,
  `DAT_007d3e74++`.
- The original's record (original-side probe `verify/portorigtop_20261109/ORIG_RWG.json`, RwGlobals `0x2b499a8`, t =
  33.39 s, `rwg_block_0x200`): +0xbc = +0xc0 = `0x02b49a64` = RwGlobals + 0xbc (the self-linked head 0x004c07b0 leaves),
  +0x164 = `0x007d3e78` (the in-place freelist 0x004c07b0 passes). So the frame module's offset in the original is 0x164.
- File-backed values (`MASHED.exe.unpatched`): `DAT_00617f78` = 0xa4, `DAT_00617f90` = 0x32, `DAT_00617f94` = 1,
  `DAT_006182b0` = 1. `DAT_007d3e70` is BSS (0).

## Change (default OFF, ORIG_TOP only, exe only, no new librw binding)

1. Knob `MASHED_OT_RACELOAD=1` (existing, default OFF): at the first `MashedRw_InstallRwGlobals` that puts the librw
   record at 0x007d3ff8, the RW core's frame-module open `RwCore_004c07b0(record, 0x164, 4)` runs once on that record
   (L-124: the matched RW core body, not a librw call). Logged before and after.
2. 0x00458c70 bound: `Match/PortTop_00458c70.cpp` includes `re/match/s09/00458c70.c` (C4 c4=match) as
   `PortTop_Bound_00458c70`, its callee 0x004c15c0 = `RwCore_004c15c0`. Callers (the matched 0x0040d270 and the bound
   0x004264d0) call `PortTop_RL_00458c70` (`Match/PortTop_StandInsRL.cpp`): when `[[0x007d3ff8]+0xbc]` is 0 it keeps the
   counting stand-in and logs it, else it logs and calls the bound body. Generator ALIAS updated the same way.

## Predictions

1. Knob-on log: the frame open line with result = record, then `[rec+0xbc]` = `[rec+0xc0]` = rec + 0xbc,
   `[rec+0x164]` = `0x007d3e78`, `DAT_007d3e70` = 0x164, `DAT_007d3e74` = 1, header 0x007d3e78 words +0x0 0xa4, +0x4 0x32,
   +0xc 4, +0x18 3 (object build arm with the file-backed image; the knob boot without the image may log 0 for the
   three `DAT_00617f..` inputs, which changes only the header words).
2. Knob boot to the menu (VS2022, `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000`): exit 0, 3000 frames.
3. Object-build arm `RO_ROLOAD-pr1`: the bound 0x00458c70 runs (log line), 25 `RwCore_004c15c0` calls, no fault, then
   0x004264d0 returns and 0x00426e10 is entered as at checkpoint 3. The arm then ends at the 0x0047a020 stop (exit
   4694048) unless PORT-OT-RACE-BIND-PROV4 has merged by then. So the load length is predicted **not reached**
   (`race.reached` false) on this row alone; the DONE target "above 3" depends on 0x0047a020.
4. Knob-off control `RO_ROIMG-pr1`: unchanged (exit 0, 600 ticks, load length 1, self-tests PASS).
5. First divergence against ref1: record 0 (tick 1), `0x00604604`, unchanged. The frame open writes 0x007d3e70 /
   0x007d3e74 / 0x007d3e78.. before tick 1, so the record-0 VALUE count may move by a few dwords; not predicted exactly.
6. Object build: the new TU's unit 0x00458c70 IDENTICAL in `BYTECHECK.tsv`; no newly DIFFERENT unit.

## Known limit, stated before the run

The frames 0x004c15c0 marks dirty are librw frames (the pool clumps come from `MashedRw_ClumpStreamRead_004e7420`).
They go into the RW core's dirty list on the record, which no sync on the librw route walks. Not measured by the
oracle; register line in the RESULT.
