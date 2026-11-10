# RESULT MATCH-RACEPATH-X87B (race-path pool B, 33 rows), 2026-11-09

**Verdict: RED on yield. 1 of 12 worked rows closed (8%), under the 30% stop rule, so the slice stopped after
row 12. 0x0041f120 promoted to C4 by route match.**

Branch `orch/MATCH-RACEPATH-X87B`, base `orch/integration` @ 3fbf77214. Every score uses `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with the file's own line-1 FLAGS, unchanged. No `__asm`, no pragma, no
mashedmod/ edit, no game run, no Ghidra write, no tool code change. The one changed source
(`re/match/s04/0041f120.c`) holds one `// MATCH:` row and is not `#include`d by mashedmod/ (grep over mashedmod/src
for `0041f120`: 0 hits), so the default gates are unaffected and were not run.

Pool: `D:/MashedWT/MATCH-RACEPATH_data/poolB.tsv`. Per-row class, before/after, Ghidra body vs compiled symbol,
compile count and sha256: `results.tsv` (column `worked` = `hand` for the 12 worked rows, `auto` for rows that got
only the mechanical passes). "before" is re-measured on this branch's base (0x0041f770 measures 110 B / 250 B in
`re/match/s05/0041f770.c`, the pool listed 147 B from the other source file).

## Yield

| scope | closed |
|---|---|
| worked by hand (first 12, smallest x87 residue first) | **1/12** |
| x87 among them | 1/9 (0x0041f120) |
| cf among them | 0/2 (0x00420290, 0x0041f770) |
| order among them | 0/1 (0x00477be0) |
| whole pool, incl. the mechanical passes | 1/33 |

Classes over the 33 rows (from the base `match.py --show` diff): x87 16, regalloc 6, cf 5, order 3, stack 3.
None is a pure size artefact: every row whose Ghidra body differs from the compiled symbol also has differing
instructions inside the body (`results.tsv` column `ghidra_body/symbol_B`). Where the symbol is longer than the
Ghidra body (Ghidra body short, accepted by `c4_reclass.py`) or shorter (our code is shorter) is listed per row.
The extent-override route was not checked for the 21 rows that were not worked by hand.

| rva | subsystem | before | after | source |
|---|---|---|---|---|
| 0x0041f120 | util | 115 B differ, 89.1% | 150 B BYTE-IDENTICAL, 100% | `re/match/s04/0041f120.c` |

## What closed it

0x0041f120: the converse of the pilot's temp lever. The original loads `s->pos.x` before `mov ecx,[ecx+4]` and
subtracts from `[ecx]` after `add ecx,0x40`. A named float local `sx = s->pos.x` read before `p` is formed gives
exactly that order. Same value (a read of the same field, no store in between). relocheck OK, 2 checked, 0
unchecked.

## Passes run over the pool

1. **Double-intermediate greedy** (pilot `dblgreedy.py`, D/E/N/T/U per statement, 6 rounds) on all 33 rows:
   0 closed, gains on 11 rows. One false gain refused: 0x00401da0 51 -> 43 only through `(double)(1.0f)`, a qword
   constant the original never has.
2. **Permuter** (`match_permute.py`, 4 to 6 rounds plus a 40 to 60 step walk, 278 to 1138 compiles each) on the 7
   best greedy variants: 0 byte gains. Every permuter output scored equal or worse bytes, so none was kept or
   read for semantics.
3. **Comma rvalue greedy** (the lead's lever from MATCH-RACEPATH-BIG: wrap one member access as `(0, x)`, or
   `L op= R` as `L = (0, L) op (R)`; scratch tool `tools/commagreedy.py`, rejects any variant with more qword
   operands than its base) on 31 rows, run after the lead's note arrived and before the stop: 0 closed, 3 large
   gains, 0x0048d360 and its twin 0x0048d540 136 -> 38 B (one `(0, p->t)` that fixes an early length difference,
   lever (2) of the note), 0x00459000 55 -> 32 B, plus 0x0041faf0 59 -> 12 B by hand with the same lever.
4. **Hand attempts**, bounded, on the 12 worked rows (detail per row in `results.tsv`).

## Residues left on the worked rows (one line each)

- 0x0041faf0 12 B: the `d.y` load sits before `push eax` in the original, after it in ours (21 spellings).
- 0x0048aa50 17 B: one `fld` of the `out[]` products before `push ebp` (14 spellings, permuter).
- 0x00459000 30 B: the double clamp compares, the original loads the constant (`fld; fcomp st(1)`, `fcomp dword`),
  ours compares against memory (34 spellings, notes list 60+ earlier).
- 0x00477be0 31 B: `fld/fchs` of the negated param one slot off, and `inc dword ptr [count]` against load/inc/store
  (7 B, symbol 542 vs Ghidra 535).
- 0x004c4d20 35 B (RwMatrixRotate): the original keeps one `recip` copy and does `fld [axis+n]; fmul st(1)` then
  pops it. Our forms keep a second copy or fold the last use. Pads (memory note) do not enter: this is a static
  byte compare.
- 0x004b5080 35 B, 0x004b42c0 38 B, 0x00480b70 42 B: cross-product / scale loads in the other operand order, or
  issued one slot off around call-argument pushes.
- 0x005a8750 82 B: the original keeps the zero-extended qword store of the uint divisor but divides with
  `fidiv dword`. No source spelling found. An `int` divisor drops the zero store and is a different value for
  divisors >= 2^31, so it was not adopted.
- 0x00420290 and 0x0041f770: one clamp idiom whose out-of-range block the original places after the `ret`.
  25 spellings (if/else nestings, ternaries, 8 `static __inline` clamp helpers, two goto layouts) give the same
  layout. cl 13.00.9176 decides the block order itself here.

## What this says for the next slices

- On this pool the double-intermediate lever closes nothing by itself. The pilot's 45% x87 rate came from
  near-misses of 9 to 23 B. Pool B starts at 40 B, and its residues are 2 to 4 independent scheduling spots each.
  The lever removes one spot and the next one stays.
- The comma rvalue lever is the stronger of the two here (136 -> 38, 59 -> 12, 55 -> 32). Run it first on any
  x87 row with high insn% and many differing bytes.
- Partial variants are kept under `variants/` (16 files, each scored in `results.tsv`, no qword false gain). Every
  rewrite in them keeps the C value: double intermediates of float operands (one rounding of an exact double
  product or sum), temps, comma rvalues, statement reorders of independent stores, and a clamp with the compare
  mirrored (same result for NaN). None was written back to `re/match/`.
- Regalloc, stack and cf rows (14) should stay parked. They got only the mechanical passes, with no gain.

## Gate checks

- `match.py --bytes` on the changed file's only `// MATCH:` (0x0041f120): BYTE-IDENTICAL. `relocheck.py`: OK
  (`relocheck.tsv`).
- `c4_reclass.py batch --tsv c4_batch.tsv` dry run: 1 PASS, 0 refused (`c4_dryrun.tsv`), then `--apply --result`
  this file.
- Tracker lint: see the commit (NEW = 0 required).

## Files

| file | what |
|---|---|
| `results.tsv` | 33 rows: class, worked, before/after, body/symbol, verdict, detail, compiles, variant, sha256 |
| `c4_batch.tsv`, `c4_dryrun.tsv` | promotion batch and gate dry run |
| `relocheck.tsv` | relocheck of the changed source |
| `variants/` | 16 partial variants (not closed) |
| `tools/` | `tryv.py`, `dblgreedy.py` (pilot copies), `commagreedy.py` (new, scratch) |

Raw logs (not committed), `D:/MashedWT/MATCH-RACEPATH-X87B_data/`:

| file | sha256 |
|---|---|
| `dg_all.log` (double-intermediate greedy, 33 rows) | 501bbdd2eef74cc834649d8aa9bcffaba732f7d08688bc98728c05a8b9c60f22 |
| `perm_all.log` (permuter, 6 rows) | 2ae2420f3550c859e09015d69796db7da652f3ada8ee2061688a6df0f9e81da0 |
| `cg_all.log` (comma greedy, 31 rows) | 55ba27d3f9369758d25903cf0f540a2557fd15c3cfae2dadc9f6bcf86d1f5d69 |
| `base/*.txt` | base `match.py --show` diff per row (33 files) |
