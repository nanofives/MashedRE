# PREREG RACE-RWLAYER-00543D40 (run 53, 2026-11-21)

Committed alone, before any build or arm. Row kind: top-down / race path. Branch `orch/RACE-RWLAYER-00543D40` = `orch/RACE-WRITER-0067EA74`
`4946232ea` with `orch/RACE-RWGLOBALS-FRAMELIST` `9f6685512` merged (`f26fdac1e`, no conflict). No code change in this PREREG.

## Step 1, answered from existing evidence (no run)

On both knob-on arms of RACE-WRITER-0067EA74 (`D:/MashedWT/RACE-WRITER-0067EA74_data/RO_ROLOAD-pr1.log`):
- line 836: `[RW_BIND] RpClumpStreamRead(0x11CE4CA8) -> rw::Clump::streamRead 11295B58 (11890 bytes, 17 atomics)`. So the
  clump and its 17 atomics are built by the librw binding `MashedRw_ClumpStreamRead_004e7420` (`LibRw/RwApiBind.cpp:980`),
  which calls `RwCore_004e7420` only under `RwCore_FrameRoute()` (`MASHED_RWC_CORE=1` + `MASHED_RWC_FRAME=1`).
- Call chain to it, by source: 0x004264d0 -> 0x0042a5d0("powerups_gold.dff") (`Match/PortTop_0042a5d0.cpp:10`) ->
  `Orig_004b3bf0` (`re/match/s18/004b3bf0.c:48` `FUN_004e7420(stream)`), with `#define FUN_004e7420
  MashedRw_ClumpStreamRead_004e7420` (`Match/PortTop_004b3bf0.cpp:13`). Not a call under 0x004b5190: 0x004b5190 only
  receives the atomics afterwards (0x00426460 -> 0x004b3fc0 collects them, then `FUN_004b5190(list[i], 0, 0)`).
- RWMOD: off on that arm (line 133 `0x00493600: missing (MASHED_ORIG_TOP_RWMOD off: no image module)`, line 200
  `0x00427ca0: RWMOD off: stand-in only`). The ROLOAD stack is `@ROIMG` + `MASHED_OT_RACELOAD=1`.
- What 0x00543d40 reads: `re/match/rwl04/00543d40.c`: `*(DAT_007dc634 + p)` and `*(DAT_007dc634 + 4 + p)`, with p =
  `*(atomic + 0x18)` (`re/match/s18/004b5190.c`). That is a geometry pointer read at an RW 3.x atomic offset, plus a
  plugin offset `DAT_007dc634`, inside an object librw built. [UNCERTAIN] What `DAT_007dc634` holds on the exe at the
  fault and what librw keeps at atomic + 0x18 are not measured. The fault itself (read of 0, eax 0, ecx 0) is measured.

## Arm requested by L0: the same knob-on arm with `MASHED_ORIG_TOP_RWMOD=1`

Stack `verify/savelink_20261023/stacks/ROLOADRW.txt` = `@ROLOAD` + `MASHED_ORIG_TOP_RWMOD=1`. One arm, `RO_ROLOADRW-pr1`,
object build of this branch, root `D:/MashedWT/RACE-RWLAYER-00543D40_root`.

Predictions (from source: `RwModKnob()` is read in `LibRw/RwApiBind.cpp` (module offsets, string and freelist slots,
image formats), `Match/PortTop_00427620.cpp`, `PortTop_0045b930.cpp`, `PortTop_00493600.cpp`, `PortTop_00552b60.cpp`.
None of them is `MashedRw_ClumpStreamRead_004e7420`, which tests `RwCore_FrameRoute()` only):
1. The clump read for `powerups_gold.dff` still goes to the librw binding (`[RW_BIND] RpClumpStreamRead(...) ->
   rw::Clump::streamRead`, 17 atomics).
2. The arm faults at `RwCore_00543d40+0xd` (read of 0) inside 0x004264d0's call to 0x00426460, as without RWMOD, unless
   one of the RWMOD paths above faults first (then that fault is the result, with its address).
3. The frame open line of RACE-RWGLOBALS-FRAMELIST appears, and the 0x00458c70 bound body runs and returns before the
   piz mount (it is called before 0x0042a5d0 in 0x004264d0).

Outcomes, as agreed with the lead: if RWMOD routes the clump read to the core, the row becomes "which RW knobs the race-load
arms carry". If the clump still comes from the librw binding, or the arm faults earlier, the row sizes the move of the
race load's DFF read to the RW core (functions, bound, stops, C4 / provisional / unmatched) and stops. No move in this run.
