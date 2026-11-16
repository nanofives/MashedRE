# RESULT MATCH-43DFD0-RAISE-R3-LOOPS (round 3, worker "loops")

Row MATCH-43DFD0-RAISE, round 3. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-RAISE-R3-LOOPS` from `orch/integration` 810042e0e. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Final source sha256 `39bf63a2dff5cddea319e6c2d8dbd26031336ab73941fbc857b9b4164d1442e4` (base `34b58998...8aca5e`).
Area: the ebp/esi roles of the five team-slot loops (original 0x0043eb3e.., 0x0043ec65.., 0x0043eddf.., 0x0043ef47.., 0x0043f078..).
Edited: only the lines inside the five `if (0 < iVar8) { ... }` loop blocks. Prologue, declarations, header line 2 and the
0xff1d0000 arm are byte-identical to the base.

## Verdict

GREEN as a small raise, not a match. The ebp/esi roles were NOT fixed: no lever that keeps the rest of the function's
allocation produced the original's roles. No byte-identical claim, so no relocheck and no promotion TSV.

| Measure (match.py, whole function) | before (810042e0e) | after (this branch) |
|---|---|---|
| score | 49.77% (regs 67.02%, ops 88.64%) | **49.95%** (regs 67.20%, ops 86.01%) |
| instructions ours / orig | 2811 / 2817 | 2811 / 2817 |
| `--bytes` | 11098 B, 6419 differing unmasked bytes, first at +0x5f | 11098 B, 6421 differing, first at +0x5f |
| equal instructions in the five loop ranges (difflib, `call 0x46dc00` end inclusive) | 134 (regs ignored 164) | **139** (regs ignored 169) |

Ops falls 2.6 points while the score rises: the ops-only alignment reshuffles, the instruction count is unchanged. The score is
the measure of record.

## The register roles (what the original does, what ours does)

Original copy 1 (same in all five): `mov ebp, 0x7f1a2c` at 0x0043eb4a (pointer at the slot's third word, stores `[ebp-8]`
0x0043eb6c, `[ebp]` 0x0043eba4, `[ebp-4]` 0x0043ebab, `add ebp, 0x10` 0x0043ebb4), the slot number `lea esi, [eax+1]` at
0x0043eb56 (`push esi` 0x0043eb6b, `mov eax, esi` 0x0043ebb2), `piVar18` in edi (0x0043eb74 / 0x0043eb7f), and
`or ebp, 0xffffffff` at 0x0043ebbb restoring the -1 constant ebp holds before the loop (stores at 0x0043eae0..0x0043eb04).
So the pointer reuses the -1 register and the slot number reuses the zero register (esi = 0 at 0x0043eb3c / 0x0043eb44).

Ours (base and after): the pointer in esi and based one word lower (`mov esi, &DAT_007f1a2c` + `sub esi, 4`, stores
`[esi-4]` / `[esi+4]` / `[esi]`), the slot number in ebp, `piVar18` in edi. That costs one extra instruction per copy and the
loop alignment pad differs.

## Lever kept

All five loops: the slot number `iVar16 = iVar9 + 1` written as the expression `(iVar9 + 1)` at its two reads, and
`iVar9 = iVar16` written as `iVar9 = iVar9 + 1`. Our loop tail becomes the original's order
(`mov eax, ebp / add esi, 0x10 / cmp eax, ecx`, as 0x0043ebb2..0x0043ebb7); before it was `add / cmp ebp / mov eax`.
Roles unchanged. 49.77% -> 49.95%.

## Tried, not kept (all five loops unless stated; whole-function score)

| Lever | score | what it does |
|---|---|---|
| store of iVar14 split per branch (`if (iVar14 != DAT_007f1a1c) *p = iVar14; else { search; *p = iVar14; }`) | 44.55% | pointer base becomes `&DAT_007f1a2c` exactly with `[p-8]` / `[p]` / `[p-4]` (the original's), but the case block recolours: pointer edi, `piVar18` esi, slot number ebp, zero in edi |
| index form from one symbol, `(&DAT_007f1a14)[(iVar9 + 1) * 4 + k]` (also from 0x7f1a24 / 0x7f1a2c, and single vs split store, all identical code) | 44.80% | same base as the original, same recolouring as the split store |
| index form from three symbols (`&DAT_007f1a24`, `&DAT_007f1a2c`, `&DAT_007f1a28`) | 43.95% | no pointer IV, `[esi+disp]` form, frame first diff moves to +0x2 |
| pointer at 0x7f1a24 with `[0]/[2]/[1]`, or a struct pointer | 44.69% / 44.80% | c2 rebases to 0x7f1a28 again |
| pointer pre-incremented at the loop top (init 0x7f1a1c) | 44.62% / 47.82% (with split) | extra `add` at the top, recolouring |
| only `piVar17[-2]` through `(&DAT_007f1a24)[iVar9 * 4]` | 50.91% | **rejected**: pointer lands in ebp at 0x7f1a2c, but a second pointer IV appears (ebx, extra `add ebx, 0x10`), `piVar13` is spilled in copy 1 (the original keeps it in ebx, 0x0043eb42), the stack slots shift (`fst [esp+8]` against `[esp+4]` at the 3rd instruction) and the first differing byte moves from +0x5f to +0xb. The score gain is alignment, not fit |
| `piVar17[-2] = local_2c + iVar16` | 44.48% | slot number moves to esi as in the original, but the lea form changes and the block recolours (-1 leaves ebp, `piVar18` in ebp) |
| `[-1]` store through `(&DAT_007f1a28)[iVar9 * 4]` | 44.53% | second IV, first diff +0x2 |
| `piVar17[-1]` store before the call; `local_2c += (cmp)`; for-loop form; increment order; pre-loop order of `piVar17 =` / `iVar9 = 0` / `local_2c = 0` / `piVar13 =`; pointer variable `piVar17` / `puVar12` / `puVar15` / a block local | 44.7-49.95% | no gain; renaming the variable and the pre-loop order give identical code |
| per-copy mix of the forms above (greedy, one copy changed at a time from the kept state) | 44.0-49.9% | every single-copy change costs 0.4-5.9 points: the five copies share one allocation |

Finding for the next round: c2 bases the pointer at the median access offset (`{-8,-4,0}` -> -4, `{-8,0}` -> 0, the split store's
`{-8,-4,0,0}` -> 0). The original's base 0x7f1a2c therefore needs a fourth reference at offset 0 (the split store, or the index
form). Both shapes then lose the ebp/esi/edi assignment to the rest of the function's colouring, which is the REGS worker's area
(0 in ebx, 1 in edi at 0x0043e02e / 0x0043e03b). Retry the split-store form on top of that change.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` before and after (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 vcvars32, syntax only, no object) in
  `mashedmod/src/mashed_re/Match`: exit 0. `iVar16` stays declared and is still used outside the loops.
- Diff: `re/match/s08/0043dfd0.c` (15 insertions, 20 deletions, all inside the five loops), `re/orchestrator/UNCERTAIN_REGISTER.md`
  (one line), this file.
- Scratch tools (session scratchpad, not committed): `loops.py` (per-loop alignment against the original), `var.py` / `var2.py` /
  `sweep.py` / `greedy.py` (variant generation and scoring), `where.py` (equal counts per 0x100 original bucket), `apply.py`.

## [UNCERTAIN]

[UNCERTAIN] The rewrite keeps behaviour. This rests on reading the source: `iVar16` is assigned only as `iVar9 + 1` at each loop
top, and `iVar9` is not written between that assignment and its three reads. Test: `re/tools/match/equiv.py` on this source
against 810042e0e's, or the PROVISIONAL knob-path menu walk ON against OFF. Not run here (match-only row, L-168). One line in
`re/orchestrator/UNCERTAIN_REGISTER.md`.
