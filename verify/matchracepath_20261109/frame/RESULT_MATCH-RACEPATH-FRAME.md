# RESULT MATCH-RACEPATH-FRAME (2026-11-09)

**Verdict: GREEN for 0x004111c0 (C2 -> C4, route match, extent override).** Tasks 2 and 3: see sections 4-5.
Text-only: no build of mashed_re, no game run, gates not run. Branch `orch/MATCH-RACEPATH-FRAME` from `orch/integration`
(3fbf77214).

## 1. Why the gate counted 124 original instructions against 105

`re/console/pc_funcs.csv` row `0x004111c0,0x00411322,1498,FUN_004111c0`: `body_end` is the body's max address
(0x00411322, inclusive), so entry..end is 355 B, but `body_size` is Ghidra's address count (1498). `match.ghidra_size`
caps 1498 at the next function entry (0x00411350), so `score_function` read 400 B from 0x004111c0: the 355 B of code,
the `nop` at 0x00411323 and the 11-entry jump table at 0x00411324..0x0041134f disassembled as instructions. Those 19
garbage "instructions" are the 105/124. The whole symbol was already BYTE-IDENTICAL (400 B).

The 1143 B above 355 are not part of the 400 B window at all: they sit **below** the entry.
Fresh read of `Mashed_pool0` (read-only, `-noanalysis`, `FuncRangesPC.java` from C4R-AC80-EXTENT, slot released),
`ghidra_extent.txt`:

| range | len | reached from 0x004111c0 by |
|---|---|---|
| 0x0040dd80..0x0040ddad | 46 | `jmp 0x40dd80` at 0x004112f2 (case 8) |
| 0x0040ddf0..0x0040ddff | 16 | `jmp 0x40ddf0` at 0x004112fb (case 9) |
| 0x0040feb0..0x004100eb | 572 | `jmp 0x40feb0` at 0x0041125f (case 1) |
| 0x00410130..0x004102b6 | 391 | `jmp 0x410130` at 0x00411272 (case 2) |
| 0x00410ab0..0x00410b25 | 118 | `jmp 0x410ab0` at 0x0041131e (case 11) |
| 0x004111c0..0x00411322 | 355 | the function itself |

46+16+572+391+118+355 = 1498 = `num_addresses`, 6 ranges. Probes: 0x0040dd80, 0x0040ddf0, 0x0040feb0, 0x00410130,
0x00410ab0 are `function_at=-`, `containing=FUN_004111c0`: Ghidra has no entry at any of them and filed them under
0x004111c0 because the dispatcher reaches them by tail JMP. The three jump tables (0x004100ec, 0x004102b8, 0x00411324)
are owned by no function. 0x0040ddb0 (called by 0x0040ddf0) is its own function (FUN_0040ddb0, 50 B).

So this is the C4R-AC80-EXTENT shape (a dispatcher plus functions it tail-jumps to), route (a). Route (b) does not
apply: the 1143 B are real code of 0x004111c0's Ghidra body, not a stale row.

## 2. The five members (re/match/t01g/004111c0.c)

Added as `// HIDDEN:` statics, DDK side only (`#ifndef MATCH_PORT`; the `#else` keeps the five extern declarations,
so the port compile of `mashedmod/src/mashed_re/Match/MatchT01G_004111c0.cpp` sees the same code: resolving the
MATCH_PORT conditionals of the old and new file and diffing gives only the 5 declarations moving below 0x004103a0,
still before their first use). Every member scored at its Ghidra range size, same calls as `c4_reclass.eval_extent`:

| member | size | insn | whole symbol |
|---|---|---|---|
| FUN_004111c0 (MATCH) | 355 | 100.0% (105/105) | 400 B, 0 bad |
| _FUN_0040dd80 | 46 | 100.0% (11/11) | 46 B, 0 bad |
| _FUN_0040ddf0 | 16 | 100.0% (3/3) | 16 B, 0 bad |
| _FUN_0040feb0 | 572 | 100.0% (156/156) | 628 B (code + index table + jump table), 0 bad |
| _FUN_00410130 | 391 | 100.0% (89/89) | 448 B, 0 bad |
| _FUN_00410ab0 | 118 | 100.0% (30/30) | 118 B, 0 bad |

What moved them:
- 0x0040feb0 / 0x00410130: case order 2, 6/10, 3/4/5, default (0x0040fc00's order), case 2 and 6/10 leave by `goto`
  past the shared tail, case 3/4/5's inner switch falls into the default tail (both call the 0x0042f500 if/else).
- 0x0040ffc2 calls the bare-RET stub 0x0045b350 with no argument (0x0040ffd1 then pushes 1): the DDK side declares it
  without a prototype (`void FUN_0045b350();`), the port side keeps `(int p0)`.
- 0x00410ab0: `!(x < K)` gives `test ah,5 / jnp`; a **non-static** HELD copy of 0x0040e460 (body:
  re/match/s02/0040e460.c) defined before it lets cl keep ECX (slot pointer) and EDX (0) live across the call at
  0x00410b0c, as the original does (a static copy got an EAX argument instead of the push); a `volatile` read of
  0x0063b91c gives `fld [0x63b91c]; fadd [0x7f100c]`. Without it every form tried (a+b, b+a, +=, temp, assignment in
  the condition, pointer cast, inline accessor, struct field, double, declaration order, symbol name) loads 0x007f100c
  first. The byte compare masks that swap (both operands are relocations); relocheck caught it. `volatile` is the same
  steer 38 re/match sources already use.

## 3. Gate and tracker

`re/tools/match/extent_overrides.tsv` row:
`004111c0  re/match/t01g/004111c0.c  FUN_004111c0@004111c0:355,_FUN_0040dd80@0040dd80:46,_FUN_0040ddf0@0040ddf0:16,_FUN_0040feb0@0040feb0:572,_FUN_00410130@00410130:391,_FUN_00410ab0@00410ab0:118`
(data row only, no tool change).

`c4_reclass.py batch` (`batch_frame.tsv`), dry run `batch_dryrun.txt`:
`[1/1] 0x004111c0 match PASS extent override: 6 members = 1498 B, each byte-identical, relocheck OK (179/0)`.
Applied with `--apply --result` (this file): `batch_apply.txt`. The file's other rows are unchanged:
`match.py --bytes` 0x0040fc00 100.0% 684 B BYTE-IDENTICAL, 0x004102f0 100.0% 172 B BYTE-IDENTICAL; relocheck OK for
0x0040fc00 (75/1 `__ftol`), 0x004102f0 (14/0), 0x004111c0 (37/0).

0x0040feb0 / 0x00410130 / 0x00410ab0 / 0x0040dd80 / 0x0040ddf0 have no hooks.csv rows (Ghidra has no function there),
so they are certified only as members of 0x004111c0's extent. Port axis unchanged: the port compiles none of the five
bodies (MATCH_PORT keeps the extern declarations); a port body for them is port work, not part of this row.

## 4. Task 2: 0x00416250

(filled below)

## 5. Task 3: 0x0040e590

(filled below)

## Files
`ghidra_extent.txt`, `FuncRangesPC.java` (copy of verify/c4rac80extent_20261107/), `batch_frame.tsv`,
`batch_dryrun.txt`, `batch_apply.txt`, `results.tsv`, `lint_before.txt`, `lint_after.txt`.
