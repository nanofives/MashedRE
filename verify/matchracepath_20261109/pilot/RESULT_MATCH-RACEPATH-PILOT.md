# RESULT MATCH-RACEPATH-PILOT (race-path near-miss pilot), 2026-11-09

**Verdict: GREEN. 6 of 20 closed (30%), all 6 promoted to C4 by route match.**

Branch `orch/MATCH-RACEPATH-PILOT`, base `orch/integration` @ 3fbf77214. Every score uses `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS, unchanged in every row. No `__asm`, no naked
body, no pragma used to force bytes. No mashedmod/ edit, no game run, no Ghidra write, no tool code change. None of the
6 changed sources is `#include`d by mashedmod/ (grep over mashedmod/src for each file name and for `re/match/` includes
of these paths: 0 hits), so the default gates are unaffected and were not run.

Row set: the 20 race-path rows from the lead's inventory (`D:/MashedWT/MATCH-RACEPATH_data/inv.tsv`, gate log
`gate_all.log`). Per-row before/after, residual, compile count and source sha256: `results.tsv`.

## Yield

| miss type | closed | rows closed |
|---|---|---|
| x87 | 5/11 | 0x00477920 0x00476320 0x00475210 0x00415e20 0x00417640 |
| order | 1/5 | 0x005b10e0 |
| regalloc | 0/3 | none |
| stack | 0/2 | none |

| rva | subsystem | before | after | source |
|---|---|---|---|---|
| 0x00477920 | util | 9 B differ, 93.3% | 231 B BYTE-IDENTICAL | `re/match/s12/00477920.c` |
| 0x00476320 | render | 11 B, 91.7% | 103 B BYTE-IDENTICAL | `re/match/s12/00476320.c` |
| 0x00475210 | render | 12 B, 93.1% | 182 B BYTE-IDENTICAL | `re/match/s12/00475210.c` |
| 0x00415e20 | ai | 18 B, 93.8% | 567 B BYTE-IDENTICAL | `re/match/s03/h2_00415e20.c` |
| 0x00417640 | ai | 23 B, 94.6% | 237 B BYTE-IDENTICAL | `re/match/s03/h3_00417640.c` |
| 0x005b10e0 | audio | 25 B, 42.9% | 38 B BYTE-IDENTICAL | `re/match/s26/005b10e0.c` |

0x00477920 and 0x00415e20 score insn 96.0% / 96.3% only because the Ghidra body is shorter than the compiled symbol
(228 vs 231 B, 551 vs 567 B). The whole-symbol byte compare is identical and `c4_reclass.py` accepts that case
(`insn ... (Ghidra body short, n/m)`), as it did for 0x00402b70 in MATCH-NEARMISS-2.

## What moved the closed rows (this decides the next slices)

1. **Double intermediate, cast back to float (4 of 6: 0x00476320, 0x00475210, 0x00415e20, 0x00417640).**
   `x = (float)(a + (double)b)` in place of `x = a + b` on one statement. With a float expression whose one operand
   is a stack slot (a spilled temp or a struct field of a local), cl 13.00 loads the slot first whatever the operand
   order of the source. In double, it loads the memory field first and makes the slot the `fadd`/`fsub` operand, as
   the original does. The same change also moves the scheduling of an `fld` or `fstp st(0)` across the next
   `add esp` (0x00476320, 0x00417640 spot 2). Earlier slices had read these residues as "x87 operand order the source
   cannot reach" (0x00476320, 0x00475210, 0x00415e20 class (b)) or as "compiler-version scheduling" (0x00417640
   class (a)). Both readings are refuted for these rows. Guard: a double literal (`(double)0.95f`) turns the
   constant into `fmul qword`, which the original never has. Two such false gains (0x00458a40 to 10 B, 0x00425ca0 to
   18 B) are not counted.
2. **Compiler temporary instead of a named local (0x00477920, and spot 1 of 0x00417640).** 0x00477920: the original
   stores the spill before the field (`fst [esp+0xc]; fstp [edi+8]`). That is the compiler's CSE spill of
   `t->age`, not a source local `a`. Writing `t->age = DAT_007f100c + t->age` and reading `t->age` in the next
   compare gives it. 0x00417640 spot 1 is the converse: a float local for `obj->x` makes the load order
   `fld [eax+0x30]; fsubr [esi]`.
3. **Return type and the width of a local (0x005b10e0).** `int` return plus an `int` local for the u16 table field
   gives `xor eax,eax; mov ax,...` ahead of the `lea`. With `void` the same body stays at 42.9%.

Partial gains from the same two levers, not closed and not adopted (kept under `variants/`): 0x004584e0 31 -> 2 B,
0x00408590 12 -> 6, 0x00474de0 12 -> 7, 0x00477f50 29 -> 10, 0x00458a40 29 -> 14. All five residues are one x87
`fld`/`fstp` placed one slot off around an integer op or an `add esp`.

## Not closed (14)

- x87, 6 rows: the 5 partials above plus 0x00425ca0 (no legal gain).
- regalloc, 3 rows: 0x004770a0 (SIB base/index, 1 B), 0x0047cea0 (lea base/index, 3 B), 0x00441b30 (frame slots of
  V3 copies). Neither lever applies to integer code.
- stack, 2 rows: 0x005aaac0 (shrink-wrap past the empty-list exit), 0x00422ba0 (one frame slot, 22 B).
- order, 3 rows: 0x004c1b40, 0x00458b10 and 0x0055ae50.
- **0x0055ae50 needs a decision, not more permuting.** A `void` return is 27 B BYTE-IDENTICAL
  (`variants/0055ae50_void.c`). It is refused here because the matched caller `re/match/s13/0047def0.c` (lines 159,
  166, 174) compares the return value with 6 and 7, so the void source would be a wrong reconstruction that the byte
  compare cannot see (the same class of error as MATCH-NEARMISS-2's 0x0041f8f0 note). Every non-void return type
  tried keeps 71.4%.

## Recommendation for the next slices

Run the double-intermediate / temp rewrite over the race-path x87 near-misses first. It closed 5 of 11 x87 rows
here, 45% for that type. The other types closed 1 of 9 (11%). The search is mechanical: one statement at a time,
three forms (`(float)(A op (double)B)`, `(float)((double)A op B)`, a float temp for one operand), greedy on the
differing-byte count, with variants that introduce a qword constant thrown out. The helpers used here are
`tools/tryv.py` (named string-replacement variants) and `tools/dblgreedy.py` (the greedy search). They are scratch
tools, copied for reproducibility. They are not part of `re/tools/match/`, and promoting them there needs the lead's
approval. Regalloc and stack rows should stay parked.

## Same-file and gate checks

- `re/match/s12/00477920.c` also holds 0x004778e0 (already C4): still 100%, 56 B BYTE-IDENTICAL, relocheck OK. The
  other 5 changed files hold one `// MATCH:` each.
- `relocheck.py` OK on all 6 (`relocheck.tsv`). 0x00417640 has 4 unchecked externs (`_DAT_005cc568`,
  `_DAT_005cc72c`, `_DAT_005ccad0`, `_DAT_0089a52c`), the same declarations as the base source.
- `c4_reclass.py batch --tsv verify/matchracepath_20261109/pilot/c4_batch.tsv` dry run: 6 PASS, 0 refused
  (`c4_dryrun.tsv`), then `--apply --result` this file.

## Files

| file | what |
|---|---|
| `results.tsv` | per-row before/after, verdict, residual, compiles, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run |
| `relocheck.tsv` | relocheck of the 6 changed sources |
| `variants/` | improved but not closed (5), plus the refused `0055ae50_void.c` |
| `tools/` | `tryv.py`, `dblgreedy.py` (scratch helpers) |
