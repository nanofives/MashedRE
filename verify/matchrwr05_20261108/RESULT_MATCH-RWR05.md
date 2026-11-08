# RESULT MATCH-RWR05 (MATCH-RWLIB-2 slice R05, RenderWare-Physics 0x00565780..0x0057c0d0)

Verdict: GREEN (honest partial). Match-only: no build, no game, no gates, no tracker edits, no promotion.

## Counts

- Rows now at 100% byte match with relocheck OK: **5 / 79** (before 0 / 79).
- Rows improved but not at 100%: 13 (table below).
- Rows unchanged: 61 (every one re-scored in `results.tsv`, no regression).
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, flags `/O2 /Ob1 /G5` on every row.
- Every 100% file: `relocheck.py` OK (`relocheck.tsv`), C++ check (VS2022 /TP, MATCH_PORT, `verify/matchrwl06_20261108/tools/cxxcheck.bat`)
  clean, no `__asm`, no naked function. The 13 improved files also pass the C++ check.

## The 5 rows at 100% (C4 candidates by route match)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x0057ae30 | 348 | 90.7 | 100 | 1/3 as the literal `0.33333334f`; first vertex sum through pointer locals `a = param_1`, `b = param_1 + 4`; sort index built in `idx` first, then `0x21312300 >> (idx * 4)` |
| 0x00568c80 | 109 | 73.0 | 100 | arena struct, each aligned value written straight to `ar->cur` and read back for `out[i]` (a temp gave the wrong register assignment) |
| 0x00565cd0 | 118 | 68.3 | 100 | literal `0.5f`, endpoints via pointer locals `a`, `b = param_2 + 4`, radius `(float)sqrt(...) * 0.5f` |
| 0x005687d0 | 129 | 58.7 | 100 | one `return` expression summing eight `AL16(x)` terms in source order (locals or an accumulator let the compiler reorder the sum) |
| 0x0056d350 | 158 | 55.9 | 100 | typed 32-byte element table, `(*tab)[c + (b + a*4)*2].v[k]` (the Ghidra int-offset form swapped eax/ecx) |

## Rows improved, not at 100%

| rva | before | after | residual |
|---|---|---|---|
| 0x00578b20 | 81.2 | 90.6 | `-1.0f` literal fixes the fmul order; the param_1 copy is loaded before `push esi` (original loads it into edi after `push edi`). Real locals give ops 100% but a 0xc frame (69.8%), 60 declaration orders tried |
| 0x00575120 | 69.3 | 86.7 | typed structs + `e->pos = *p` struct copy fix the tail; the original's `fxch st(1)` before the first `faddp` of both sums and the private epilogue of the first `return 0` remain (12 orders x 2 associativities, 8 cast mixes) |
| 0x0056fad0 | 76.5 | 85.3 | `-1.0f` literal; cross-product fmul operand order is canonicalised (4096-way greedy over operand orders and V3/V4 field forms: no change) |
| 0x00579e50 | 60.4 | 83.3 | struct S + row pointers fix the callee-saved registers; the original anchors its strength-reduced row pointer at `p[i][1]` and emits the dot z,x,y (48 term/operand orders in three spellings compile to one order) |
| 0x00566b40 | 76.7 | 81.9 | literal for `DAT_005cc320` |
| 0x005685f0 | 69.1 | 78.3 | clean struct C (Island/World), `nB > 0` (jbe); the original pushes edi at entry, ours shrink-wraps it, so every later param offset differs by 4 |
| 0x00578cb0 | 70.3 | 78.4 | float literals |
| 0x005667c0 | 63.4 | 73.7 | float literals |
| 0x00579c00 | 72.2 | 74.1 | float literals |
| 0x00578bd0 | 41.7 | 50.0 | float literals |
| 0x00566200 | 39.7 | 43.4 | float literals |
| 0x0057a9a0 | 34.1 | 35.0 | float literals |
| 0x00566420 | 31.1 | 31.9 | float literals |

## Findings that apply to the other slices

1. **Float constants as literals, not globals.** RW-Physics reads its float constants from `.rdata` (`DAT_005cc33c` = -1.0,
   `DAT_005cc32c` = 0.5, `DAT_005ccac8` = 1/3, ...). Those addresses are the compiler's own `__real@` constants, so the
   source had literals. With a global the compiler loads the constant first (`fld [const]; fmul [x]`); with a literal it
   loads the variable first, as the original does. `relocheck.py` compares a `__real@` constant by value, so a literal
   passes. `tools/lit.py <src> <0xrva> [--apply]` reads each float/double `DAT_` from the unpatched exe, substitutes it
   (all, each, subsets) and keeps the best. It raised 11 rows here. Comparisons against `DAT_005d757c` (0.0) did not move.
2. **Pointer locals change x87 operand order.** Sums like `p[0] + p[4]` load the lower offset first; through
   `a = p; b = p + 4; a[0] + b[0]` the compiler loads `b` first, as the original does (0x0057ae30, 0x00565cd0).
3. **Typed structs fix register assignment** that int-offset Ghidra forms get wrong (0x0056d350, 0x00568c80, 0x00579e50,
   0x00575120). Write results straight into the struct field when the original reloads it.
4. **Sums of aligned sizes**: one expression in source order; separate locals or `+=` let the compiler reorder the terms.
5. **x87 sums are reassociated by the compiler**: for most rows the emitted term order did not follow the source term
   order, operand order, names, or `/TP`. `/Op` made every row worse. The remaining x87 term-order residue (0x0056fad0,
   0x00575b60, 0x00578f20, 0x00579e50) needs a different base-pointer shape, not a term permutation.

## Rows tried and left unchanged

- 0x0057c0d0 (98.7) and 0x005735f0 (98.0): the original loads the next pointer one slot earlier (mov before the first
  fmul). Return-expression forms, pointer copies, if/else, float/double temps did not move it.
- 0x00566c10 (88.9): the `fstp st(0); mov eax,1; ret` block sits before the zero-store block in the original. Five return
  layouts, all at or below 88.9.
- 0x00575b60 (79.8): greedy over all 6 term orders of five sums plus a trailing return, and a typed matrix/shape
  re-derivation (70.2). The original pushes edi at entry and emits row 1 as z,x,y.
- 0x00578f20 (64.2): 12 term orders x 2 pointer forms, and renamed identifiers: one output. The original keeps the loop
  counter in memory and param_3 in ebp.
- 0x005672c0 (18.8): seven expression forms (single expression, factored, fully expanded, `M / 4`) compile to one code.
- 0x00567350 (8.2): not reworked. It is the carve twin of 0x005672c0 (`dst = off + sz <= total ? base + off : 0`) and
  should be re-derived with a carve macro.
- Rows below 60% other than those named above: not reworked (budget).

## Per-row before/after

All 79 rows: `results.tsv` (columns rva size best% regs% flags by sha256 class reason before%). No row went down.

## Files

- Sources changed (18), all under `re/match/rwl06/`: 0057ae30, 00568c80, 00565cd0, 005687d0, 0056d350 (100%);
  00578b20, 00575120, 0056fad0, 00579e50, 00566b40, 005685f0, 00578cb0, 005667c0, 00579c00, 00578bd0, 00566200,
  0057a9a0, 00566420 (improved). Each gets a `/* MATCH-RWR05 2026-11-08: ... */` header line after FLAGS that supersedes
  the older header.
- `tools/`: `lit.py` (literal substitution), `greedy.py` (greedy coordinate search over source slots), `vtry.py`
  (variant scorer from R01), `collect.py`.
- Raw, not committed (D:/MashedWT/MATCH-RWR05_data):
  - `collect.log` sha256 c69b2d748f2ade091f16c1fde51dc27a5e5c6ec92a4a51a91674404dea79599a
  - `lit_all.log` sha256 e44c3e46fda8ff53e9f9f1bd491dae2444fb74312b2b5b7d30006b7de7442a72
  - `scratch/` (472 variant sources), `spec_*.py` variant specs.

## Next

A C4R batch can promote the 5 rows above by route match from `relocheck.tsv`. Best follow-ups: 0x00578b20 (90.6, one
param copy), 0x00575120 (86.7), 0x00567350 / 0x005676f0 with a carve macro, and `lit.py` over the R0n slices that hold
other RW-Physics rows.
