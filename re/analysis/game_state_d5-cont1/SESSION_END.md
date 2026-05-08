# SESSION_END — game_state_d5-cont1

**Date:** 2026-05-08
**Slot used:** Mashed_pool12
**Session ID:** game_state_d5-cont1-20260508-0324
**SHA-256 anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Items processed

| D | RVA | File | Confidence | Notes |
|---|---|---|---|---|
| D-8689 | 0x00405460 | 0x00405460.md | C1 | spline-step; guards+ratio build+FUN_00404fa0 eval+accumulate DAT_00639d74; S-2929 resolved; S-2960; U-2967..U-2969 |
| D-8690 | 0x0040e590 | 0x0040e590.md | C1 | race-start coord; mod-12 counter; 4-slot assignment; score-sort path; per-player grid placement; S-2930 resolved; S-2961..S-2975; U-2970..U-2974 |

## Stubs resolved

| S | RVA | Resolution |
|---|---|---|
| S-2929 | 0x00405460 | Analyzed as C1 — D-8689 complete |
| S-2930 | 0x0040e590 | Analyzed as C1 — D-8690 complete |

## New stubs

| S | D | RVA | Description |
|---|---|---|---|
| S-2960 | D-8800 | 0x00404fa0 | spline matrix eval; 1068 bytes; callee of FUN_00405460 |
| S-2961 | D-8801 | 0x00408b00 | grid-pos lookup; 0x1D2 bytes; callee of FUN_0040e590 |
| S-2962 | D-8802 | 0x00409290 | pos-handle writer; 0x6D bytes; callee of FUN_0040e590 |
| S-2963 | D-8803 | 0x0040b250 | no-arg pre-placement; 0x33 bytes; callee of FUN_0040e590 |
| S-2964 | D-8804 | 0x0040b410 | player readiness getter; 0xB bytes; callee of FUN_0040e590 |
| S-2965 | D-8805 | 0x0041ede0 | zeroing-loop call A; 0x6F bytes; callee of FUN_0040e590 |
| S-2966 | D-8806 | 0x0041ee50 | zeroing-loop call B; 0x58 bytes; callee of FUN_0040e590 |
| S-2967 | D-8807 | 0x0041ef80 | zeroing-loop call C; 0x36 bytes; callee of FUN_0040e590 |
| S-2968 | D-8808 | 0x0041f000 | score-array call; 0x24 bytes; callee of FUN_0040e590 |
| S-2969 | D-8809 | 0x00429820 | no-arg init call; 0x14 bytes; callee of FUN_0040e590 |
| S-2970 | D-8810 | 0x0046b1c0 | score-array call B; 0x329 bytes; callee of FUN_0040e590 |
| S-2971 | D-8811 | 0x0046b540 | post-b1c0 call; 0x157 bytes; callee of FUN_0040e590 |
| S-2972 | D-8812 | 0x0046c6d0 | score writer; 0x23 bytes; callee of FUN_0040e590 |
| S-2973 | D-8813 | 0x004704c0 | 6-arg vehicle-placement; 0x1AA bytes; callee of FUN_0040e590 |
| S-2974 | D-8814 | 0x0048f680 | no-arg init A; 0x20 bytes; callee of FUN_0040e590 |
| S-2975 | D-8815 | 0x0048f740 | no-arg init B; 0x30 bytes; callee of FUN_0040e590 |

## New uncertainties

| U | Function | Topic |
|---|---|---|
| U-2967 | 0x00405460 | DAT_00639d70 struct layout; field +0xc is float upper bound |
| U-2968 | 0x00405460 | Double-call of FUN_004c1480 in conditional path — intentional or decompiler artifact |
| U-2969 | 0x00405460 | _DAT_005cc94c added when param_1 < 0 — sign normalization or wraparound correction |
| U-2970 | 0x0040e590 | DAT_008a94d0 semantics: player count vs game-mode code |
| U-2971 | 0x0040e590 | FUN_0040b410 readiness code semantics (0/1/2/0xffffffff/0xfffffffe) |
| U-2972 | 0x0040e590 | local_2c[5..8] score metric and 0→0xfffffffe remap purpose |
| U-2973 | 0x0040e590 | PTR_PTR_005f2770 struct layout (+0x34/+0x38/+0x3c/+0x40) |
| U-2974 | 0x0040e590 | DAT_007f0fd0 placement-mode code values (0/1/2/4/8/9) |

## Queue changes

- Removed: D-8689 (0x00405460), D-8690 (0x0040e590) — both drained here.
- Added: game_state_d5-cont2 → D-8800..D-8815 (16 RVAs).

## IDs used

- U-2967..U-2974 (8 used; 12 remaining in U=2967..2986)
- D-8800..D-8815 (16 used; 44 remaining in D=8800..8859)
- S-2960..S-2975 (16 used; 4 remaining in S=2960..2979)

## Scribe outcome

Queued for sweep (see re/SCRIBE_QUEUE.md row game_state_d5-cont1-20260508-0324).
