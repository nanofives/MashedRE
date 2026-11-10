# RESULT MATCH-40BB10 (2026-11-10): 0x0040bb10 C4 by match; match-only, 0x0040cda0 stays a stand-in (blocked one level down, at 0x0040bae0)

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/MATCH-40BB10` on
`orch/POWERUPINIT-C` `e4f12233e`. No `mashedmod/` file changed, so there was no build and no gate run.

## Match

| rva | source | match.py --bytes | relocheck | hooks.csv |
|---|---|---|---|---|
| 0x0040bb10 | re/match/s29/0040bb10.c | 100.0% (regs 100.0%), 10/10 ins, 23 B BYTE-IDENTICAL | OK checked=1 unchecked=0 | (none) -> C1 -> C4 |

- **Compiler and flags:** XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C. No inline asm.
- **Body:** `0x0040bae0(atomic, data); return atomic;`, the same shape as 0x004b5560.
- **Row:** added at C1 through re-classify, filed `gameplay` like its caller 0x0040cda0 and its callee 0x0040bae0. C1 cites
  re/console/pc_funcs.csv, which already held the Ghidra function (23 B), and the callback reference in
  re/match/s02/0040cda0.c.
- **Promotion:** `c4_reclass.py batch --apply --strict --result` this file: `[1/1] 0x0040bb10 match PASS
  byte-identical 23 B, relocheck OK (1/0)`. CHANGELOG has the `(none)->C1` and `C1->C4` lines.

## Why 0x0040cda0 is not swapped in

The swap needs every body under 0x0040cda0 to run on what the exe actually holds. One of them cannot:

- **0x0040cda0** (matched, re/match/s02/0040cda0.c): when DAT_00636ad0 == 0, args = {coef, 0x004672d0(0), texture
  "shine"}, then RpClumpForAllAtomics(clump, 0x0040bb10, &args).
- **0x0040bb10** (this row): calls 0x0040bae0(atomic, args).
- **0x0040bae0** (matched, re/match/s02/0040bae0.c, C4) is the blocker:
  - it calls 0x0053fb70(atomic), RpMatFXAtomicEnableEffects
  - it then reads `*(atomic + 0x18)` and, when non-zero, calls 0x004e89a0(that, 0x0040baa0, args), which is
    RpGeometryForAllMaterials
  - 0x0040baa0 is RpMatFXMaterialSetEffects (0x0053fbb0) plus RpMatFXMaterialSetupEnvMap (0x0053fd30) on each material.

+0x18 is the geometry of an RW 3.x RpAtomic: RwObjectHasFrame is 0x14 bytes, then repEntry, then geometry. On this path
the atomics are librw rw::Atomic objects, because the init clones a librw clump and the RpClumpForAllAtomics binding
hands out librw atomics. In rw::Atomic (deps/librw/src/rwobjects.h) the layout is:
- `ObjectWithFrame` = `Object` (type, subType, flags, privateFlags, parent: 8 bytes) + `LLLink inFrame` (8) + `syncCB`
  (4) = 0x14 bytes
- `Geometry *geometry` at +0x14
- `Sphere boundingSphere` from +0x18

So the matched 0x0040bae0 would read boundingSphere.center.x as a geometry pointer and pass it to 0x004e89a0. A
binding for 0x0040bae0 would replace a matched game-code body with hand code, which is not allowed on the knob path.
So 0x0040cda0 stays the logging stand-in `PortTop_PWI_StandIn_0040cda0` (Match/PortTop_004576f0.cpp), unchanged.

Not settled, because the swap stops earlier: the camera record 0x004672d0 reads, `*(DAT_006905b0 + 4)`.
- **Builder:** the matched 0x00467110 (Match/PortTop_00467110.cpp), reached from the matched 0x004921d0.
- **Seen:** POWERUPINIT-C's knob_on_1.log:765 shows 0x006905b0 = 08F558A8 at AppDestroy.
- **Missing:** whether it is set before the 0x0045bae0 type-table loop runs. Re-check that before any future swap.

## L-115

Does not apply. 0x0040bb10 is a byte match, and the blocker is a structure-layout dependency in a matched callee, not
a residual.

## Checks

- `py -3.12 scripts/lint_rva_bodies.py`: OK, 0 new (no exe source changed).
- No `mashedmod/` change, so no build, no boots, no captures and no gates.

## Next (proposed row)

- **MATCH-40BB10-PORT:** run 0x0040cda0 on the knob path. Two ways:
  - (a) An RW-layout atomic view with the geometry at +0x18 for this ForAllAtomics call, plus RW-layout material views
    for RpGeometryForAllMaterials and librw MatFX bindings for 0x0053fb70 / 0x0053fbb0 / 0x0053fd30. This is the
    0x004b3f90 view pattern, extended.
  - (b) A lead decision on a binding for 0x0040bae0.
  Either way, first confirm DAT_006905b0 is built before the 0x0045bae0 loop.
