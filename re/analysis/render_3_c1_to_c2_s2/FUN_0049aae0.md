# FUN_0049aae0 — RainMode1Init C1→C2

**RVA:** 0x0049aae0
**Body:** 0x0049aae0..0x0049ab01
**Session:** batch-render-3-s2
**Pool:** Mashed_pool10
**U-id:** U-4732

## Decompilation summary

Mode-1 initialiser pair. Two calls:
1. `FUN_004cbad0(&DAT_005d0438, &DAT_00773974)` at 0x0049aaea — initialises resource into DAT_00773974 using template at DAT_005d0438
2. `FUN_004cbb20(&DAT_005d0518, &DAT_0077397c)` at 0x0049aaf4 — initialises resource into DAT_0077397c using template at DAT_005d0518

Returns void.

Global reads/writes:
- `DAT_005d0438` at 0x0049aaeb — template/descriptor for resource 1
- `DAT_00773974` at 0x0049aaf0 — output handle for resource 1
- `DAT_005d0518` at 0x0049aaf5 — template/descriptor for resource 2
- `DAT_0077397c` at 0x0049aafb — output handle for resource 2

## Callers (1)
- `FUN_0049a0e0` (0x0049a0e0) — RainModeSelect, mode-1 branch

## Callees (2)
- `FUN_004cbad0` (0x004cbad0) — resource initialiser / allocator pair
- `FUN_004cbb20` (0x004cbb20) — resource initialiser / allocator pair (variant)

## C2 evidence
- Full decompilation read; two-call init structure with concrete addresses cited.
- Caller documented in this session.
- No UNCERTAIN items.

## Line count
~10 decompiled lines — within 300-line cap.
