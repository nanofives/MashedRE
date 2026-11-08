# RESULT MATCH-RWR01 (MATCH-RWLIB-2 slice R01, RW core 0x004c0910..0x004e5ef0)

Verdict: GREEN (honest partial). Match-only: no build, no game, no gates, no tracker edits, no promotion.

## Counts

- Rows now at 100% byte match with relocheck OK: **11 / 79** (before 0 / 79).
- Rows improved but not at 100%: 3 (0x004c3e20 85.2 -> 92.6, 0x004e5d30 91.2 -> 94.1, 0x004e5bf0 70.6 -> 78.6).
- Rows unchanged: 65 (every one re-scored in `results.tsv`, no regression).
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, flags `/O2 /Ob1 /G5` on every row
  (0x004e5820 moved from its old `/O1` line to the slice flags, where it matches).
- Every 100% file: `relocheck.py` OK (`relocheck.tsv`), C++ check (VS2022 /TP, MATCH_PORT) clean, no `__asm`,
  no naked function.

## The 11 rows at 100% (C4 candidates by route match)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x004ccc50 | 138 | 98.2 | 100 | bitmap byte pointer set before the sum loop gives the [ecx+eax+8] index/base order |
| 0x004e15f0 | 128 | 96.2 | 100 | tail block through the typed `end` pointer, the same shape as the first block |
| 0x004ccba0 | 169 | 95.9 | 100 | one bitmap byte pointer for both accesses + a fresh for-loop index for the sum |
| 0x004e5700 | 288 | 89.8 | 100 | stack walk push via a saved node, `ff8 + 71cc` global order, flag test before the +0x1c read |
| 0x004e55d0 | 129 | 88.2 | 100 | same stack walk; `DAT_007d71cc + 4 + DAT_007d3ff8` scored 100% but relocheck BAD (globals swapped), `DAT_007d3ff8 + DAT_007d71cc + 4` is OK |
| 0x004c5a60 | 121 | 83.7 | 100 | `refCount--; if (refCount <= 0) { refCount++; ...` |
| 0x004e4f70 | 165 | 81.8 | 100 | double recursion; the compiler turns the second call into the loop Ghidra showed |
| 0x004e5c30 | 62 | 76.7 | 100 | count hoisted into a local, array pointer reloaded per iteration |
| 0x004e4380 | 38 | 72.2 | 100 | returns param_1 (Ghidra typed it void) |
| 0x004e4130 | 89 | 67.6 | 100 | match_permute.py swap: `cur = nx` before the free callback (o already computed, same semantics) |
| 0x004e5820 | 21 | 57.1 | 100 | returns p, under the slice flags `/O2 /Ob1 /G5` |

## Findings that apply to the other R0n slices

1. **Address operand order follows variable identity, not spelling.** `[ecx+eax]` vs `[eax+ecx]` did not move under any
   reordering of the same expression (14 spellings on 0x004ccc50). It moved when the base went through its own named
   pointer variable (`m = (byte *)(p + 2); m[i]`) or the loop index was a fresh variable.
2. **Ghidra drops return values.** Two rows were typed `void` but leave a parameter in eax at the ret (0x004e4380,
   0x004e5820). Returning it removes a duplicated tail. `verify/matchrwr01_20261108/tools/retscan.py` lists the
   remaining void-typed rows of R01 with the original's last instructions before each ret.
3. **Loops Ghidra renders with a top test and an inner return can be tail recursion** (0x004e4f70).
4. **The KD-tree stack walk** in 0x004e55d0 / 0x004e5700 (and the round-1 0x004e51e0) is
   `cur = stack[sp--]` on pop and `old = cur; cur = cur[2]; stack[++sp] = old[3]` on push.
5. **A 100% score can hide swapped globals**: run relocheck on every candidate, not only at the end (0x004e55d0).

## Rows tried and left below 100%

- 0x004c1040 (99.2) and 0x004c1210 (98.4): one `[+0x9c]` load base register in the sibling search loop
  (`mov edx,eax; mov eax,[edx+0x9c]` vs ours `[eax+0x9c]`). Tried 10 loop shapes (RW `while (cur->next != child)`,
  fresh prev/cur variables, typed frame struct, inline helper). Permuter 316 + 238 compiles, no gain. Class (a).
- 0x004e4450 (98.1): `lea ebp,[eax+edi]` scheduled one slot later. 120 statement permutations + 12 pointer spellings.
  Permuter 298 compiles. Class (a) scheduling.
- 0x004e2470 (93.6): param_5 loaded into edx before the f8 store; original reloads it into ecx after f4. Store orders,
  struct form, pointer return tried. Permuter 318 compiles. Class (b).
- 0x004e5d30 (94.1, was 91.2): original keeps one `ret` per failing call; ours jumps to a shared ret. 12 return shapes.
- 0x004c3e20 (92.6, was 85.2): the two stores after the free call keep `[eax+edx]` (original `[edx+eax]`) under 16
  load/store spelling pairs and a `char *` typed global.
- 0x004c5a00 (89.7): /G5 store scheduling of `[+0x50]` dword vs `[+0x51]` byte. The permuter's 93.1% variant stores the
  dword after the byte (clobbers +0x51), so it was discarded as a semantics change.
- 0x004e1d20 (84.8): the shared `return 0` block sits after the first test in the original. Separate returns and an
  `__inline` helper for the write loop (not inlined, 53/92 insns) did not move it.
- 0x004e1b60 (86.5): the RW plugin-reader shape with real locals gets ops 100% but the frame is 0x18 against 0x10: the
  original keeps the chunk type/length in the dead param_2/param_3 slots. Mimicking that in source scored 76.8%.
  Kept the round-1 source.
- 0x004e51e0 (90.7), 0x004e5ef0 (88.0), 0x004e5bf0 (78.6): see results.tsv reasons.
- Rows below 85% other than those named above: not reworked (budget).

## Per-row before/after

All 79 rows: `results.tsv` (columns rva size best% regs% flags by sha256 class reason before%).
Rows with a change: listed above. No row went down.

## Files

- Sources changed (14): `re/match/rwl01/{004c3e20,004c5a60,004ccba0,004ccc50,004e15f0}.c`,
  `re/match/rwl02/{004e4130,004e4380,004e4f70,004e55d0,004e5700,004e5820,004e5bf0,004e5c30,004e5d30}.c`.
  Each gets a `/* MATCH-RWR01 2026-11-08: ... */` header line after FLAGS that supersedes the older header.
- `tools/`: `vtry.py` (variant scorer: OLD snippet + VARIANTS dict, `--apply NAME`), `collect.py`, `retscan.py`,
  `headers.py`. Write variant spec files with the Write tool, not a Bash heredoc (heredocs mangled `\n` in Python
  string literals here).
- Raw, not committed (D:/MashedWT/MATCH-RWR01_data):
  - `permute1.log` sha256 cdf0f6e6a78a565d477fa156de86173e46bc9616db1dcfad2c3fb140c5978dac
  - `permute2.log` sha256 64910471e793eccae3481b38b488d114519228536e99edc92e4a1c6dc3bd57f6
  - `permute3.log` sha256 5d5bd40fc097e2443dfcae69eb33e4336a70392b61759bd43414f25670f72b65
  - `collect.log` sha256 f797e14667bc68a84b2d1b4b7639b8e2d2fbed8e91a474adb7f8ac31bb20e473
  - `scratch/` variant sources.

## Next

A C4R batch can promote the 11 rows above by route match from `relocheck.tsv`. Remaining near misses for a later
slice: 0x004c1040, 0x004c1210, 0x004e4450 (one instruction each, class (a)).
