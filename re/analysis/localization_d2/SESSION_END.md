# Session: localization_d2-20260506

## Summary
Session LLLLL — localization depth-2, batch 2.

Slot: Mashed_pool5 (read-only).
Binary anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Functions analyzed (8)

| RVA | Name | Size | C | Notes |
|---|---|---|---|---|
| 0x0042c300 | FUN_0042c300 | 176B | C1 | int→decimal formatter (1-3 digits + space) |
| 0x0042c3c0 | FUN_0042c3c0 | 329B | C1 | ratio formatter (0.XX / 1.0 / 0.0); "%.2f" path |
| 0x0042c7c0 | FUN_0042c7c0 | 408B | C1 | player-name concatenator; scans 4-slot pair array |
| 0x00429660 | FUN_00429660 | 434B | C1 | name-label drop-shadow renderer; solo + multi-path |
| 0x0040dc80 | FUN_0040dc80 | 6B | C1 | float getter: DAT_0063b910 |
| 0x0040dc90 | FUN_0040dc90 | 23B | C1 | conditional slot-index getter; excl. DAT_007f0fd0∈{5,10} |
| 0x00429620 | FUN_00429620 | 57B | C1 | X slide-in animator; 470-(1-clamp30*0.033)*500 |
| 0x00429300 | FUN_00429300 | 6B | C1 | float getter: DAT_008991b8 |

## ID ranges used
- U: 2267..2279 (13 entries)
- S: none (all callees were already mapped)
- D: 6700..6703 (4 deferred callers from name-label cluster)

## Deferred to localization_d2-cont1
| D-ID | RVA | Reason |
|---|---|---|
| D-6700 | 0x004295a0 | caller of FUN_0040dc80; 0x7c B |
| D-6701 | 0x00429bd0 | caller of FUN_0040dc80; 0x23e B |
| D-6702 | 0x00424eb0 | caller of FUN_0040dc90; 0x40e B |
| D-6703 | 0x00442440 | caller of FUN_0040dc90; 0x186 B |

## MashedRunner framing check
Language codes 0=EN/1=FR/2=DE/3=ES/4=IT confirmed by SciLor/frmMashed.cs:20-24. No ID 5 — confirms U-0869 (USA.dat).

## Notes
FUN_0042c3c0 and FUN_00429660 have calling-convention decompiler artefacts (U-2270, U-2275) that require listing verification. FUN_0042c7c0 references the DAT_007f1a18 pair array; its layout (playerID→nameIndex mapping at stride 0x10) is well-determined but semantics of the key field require callers.
