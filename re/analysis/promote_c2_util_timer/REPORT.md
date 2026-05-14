# Session Report — promote_c2_util_timer

**Session:** 80  
**Date:** 2026-05-13  
**Model:** claude-sonnet-4-6  
**Slot:** Mashed_pool7 (pre-assigned Mashed_pool1; pool assigned pool7 as first free)  
**Target:** C1→C2 promotion, util subsystem, timer cluster (0x0041xxx–0x0042xxx + 0x00475xxx)

## Summary

20 functions promoted from C1 to C2. All existing analysis notes from prior sessions
(timer_d3_cont1_b, timer_d3_cont1_a, timer_d3) already contained complete mechanical
descriptions with constant citations at address level. 9 spot-check decompilations via Ghidra
confirmed 100% accuracy of notes.

## Promoted functions

| RVA | Name | Note |
|-----|------|------|
| 0x00413f90 | FUN_00413f90 | timer_d3/0x00413f90.md |
| 0x0041cb80 | sub_0041cb80 | timer_d3_cont1_b/0x0041cb80.md |
| 0x0041cbc0 | sub_0041cbc0 | timer_d3_cont1_b/0x0041cbc0.md |
| 0x0041d730 | sub_0041d730 | timer_d3_cont1_b/0x0041d730.md |
| 0x0041d820 | sub_0041d820 | timer_d3_cont1_b/0x0041d820.md |
| 0x0041e130 | sub_0041e130 | timer_d3_cont1_b/0x0041e130.md |
| 0x0041eda0 | sub_0041eda0 | timer_d3_cont1_b/0x0041eda0.md |
| 0x0041f000 | sub_0041f000 | timer_d3_cont1_b/0x0041f000.md |
| 0x00420d40 | sub_00420d40 | timer_d3_cont1_b/0x00420d40.md |
| 0x00422120 | sub_00422120 | timer_d3_cont1_b/0x00422120.md |
| 0x004222c0 | thunk_FUN_00422120 | timer_d3_cont1_b/0x004222c0.md |
| 0x00422b10 | sub_00422b10 | timer_d3_cont1_b/0x00422b10.md |
| 0x00425b10 | sub_00425b10 | timer_d3_cont1_b/0x00425b10.md |
| 0x00426630 | sub_00426630 | timer_d3_cont1_b/0x00426630.md |
| 0x004266f0 | sub_004266f0 | timer_d3_cont1_b/0x004266f0.md |
| 0x00426c10 | sub_00426c10 | timer_d3_cont1_b/0x00426c10.md |
| 0x00426c30 | sub_00426c30 | timer_d3_cont1_b/0x00426c30.md |
| 0x00426c70 | sub_00426c70 | timer_d3_cont1_b/0x00426c70.md |
| 0x00475a60 | FUN_00475a60 | timer_d3_cont1_b/0x00475a60.md |
| 0x00410860 | FUN_00410860 | timer_d3_cont1_a/0x00410860.md |

## Spot-check verification log

Ghidra decompilations confirmed against existing notes:

| RVA | Result |
|-----|--------|
| 0x0041cbc0 | PASS — 12-float table, loop count 0xc, DAT_005f337c, DAT_0063d270 |
| 0x0041d820 | PASS — trivial DAT_0063d558=0 |
| 0x0041eda0 | PASS — stride 0x2ac, base 0x0063dc74, bit 0x8 / mask 0xfffffff7 |
| 0x00426630 | PASS — trivial DAT_0066d6fc=param_1 |
| 0x00413f90 | PASS — returns &DAT_005f2b10 |
| 0x00426c10 | PASS — FUN_0041ea40 gate; 0x00646e58 passed to FUN_0041e920 |
| 0x0041d730 | PASS — loop bound 0x0063d558, stride 0x160, four config pairs |
| 0x00475a60 | PASS — DAT_0069160c count, FUN_004b6520, 0x50 scale, DAT_00691614=0 |
| 0x00422120 | PASS — base 0x0063fb90, stride 0x208, bound 0x6403b0, calls FUN_00421c50 |
| 0x0041cb80 | PASS — DAT_0063cde4/ce18/ce20, stride 0x114, bound 0x0063d270 |

## Additional observation on 0x0041d730

Decompilation revealed the player-index write is:
`*(undefined4 *)(&DAT_0063d298 + *(int *)(&DAT_0063d2a4 + iVar1) * 4 + iVar1) = playerIdx`
where `iVar1 = DAT_007f1a[X8] * 0x160`. The existing note correctly captures this pattern.
The write-offset counter at `&DAT_0063d2a4 + iVar1` = DAT_0063d298+0xC per slot, confirmed.

## No C2 already in range

Count of C2 in the full util C1 query range before this session: 0 (checked via awk).
No abort condition triggered.
