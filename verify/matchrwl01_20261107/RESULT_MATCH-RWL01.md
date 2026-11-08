# RESULT MATCH-RWL01 (2026-11-07, run 35)

Verdict: GREEN (partial slice, honest counts). Slice 0x004c0790..0x004e1e70, 132 `third-party-library[renderware]` C2 rows
(RW core). Match-only: no trackers, no build, no gates, no `mashedmod/` edits.

## Counts
| class | rows |
|---|---|
| byte-identical (match.py 100% regs 100%) and relocheck OK, no asm | 70 / 132 |
| below 100% | 62 |
| no Ghidra size | 0 |
| C++ check (VS2022 /TP, MATCH_PORT) | 132 / 132 pass |

39 of the 70 came from the raw converter (`re/tools/match/g2s.py` under `/O2 /Ob1 /G5`, best of three flag runs, ties to /G5);
the other 31 are hand-matched by three sonnet helpers (notes in `notes/h1.md` h2 h3). `relocheck.tsv` lists the 70.

## Flags evidence
`// FLAGS: /O2 /Ob1 /G5` on 129 files. Raw converter runs under `/O2 /Ob1 /G5`, `/O2 /GS /Ob1`, `/O1` over all 132 rows: 39 rows at 100%,
all under /G5 (no row was 100% only under the game flags). Three SSE-family rows (class c, below) are kept at `/O1` because it
scores higher on a placeholder body; they are not identical, so this does not affect the identical count. Files at /O1: see `results.tsv` flags column.

## Tool fix (re/tools/match/relocheck.py, one line)
`RE_G`'s optional middle group `(?:[^,]+,\s*)?` could cross a `;` and read the next statement, so a file whose last `MGLOBAL(...)` was followed by
a call with a trailing hex argument resolved that global to the argument (e.g. DAT_007d3e70 -> 0x3000e, rows 0x004c0b30 / 0x004c57a0 reported BAD
while correct). Changed to `(?:[^,;]+,\s*)?`. Re-run on all 132 files.

## Below-100% rows (class per S24 BRIEF: a compiler-version codegen, b source shape, c tool limit)
See `results.tsv` (`class`, `reason` columns) and `notes/h*.md`. Summary:
- (c) 6 rows: 0x004dc300, 0x004dc370, 0x004dc410, 0x004dc4a0, 0x004dc530 (hand-written SSE 3x3/4x3 transform loops, `movups/shufps/mulps`; cl 13.00.9176 has
  no `__m128`, `__asm` is banned, the C is scalar placeholder semantics) and 0x004dd0b0 (`pushf/popf` + `cpuid`, needs inline asm).
  These are the naked-asm shape the L-93 rule refuses: not promotable by match.
- (a) about 10 rows at 95-99% whose only residual is the commutative base/index order in an address operand (`[ecx+eax]` vs `[eax+ecx]`) or a callee-saved
  register choice: 0x004c1040 (99.2), 0x004c1210 (98.4), 0x004ccc50 (98.2), 0x004ccba0, 0x004e15f0, 0x004db3e0, 0x004c5f60, 0x004c3e20, 0x004cd900.
  Regs are 100% on six of them. Same classes as flags_brief.md; no source form moved them.
- (b) the rest: register allocation / frame layout / epilogue placement; large rows (0x004d4380 1412 B, 0x004df120 1420 B, 0x004de390, 0x004cdf20,
  0x004d3090) are raw converter form only, out of budget.

## Static callees and notes
- 0x004c1210 and 0x004c1040 each hold the same `static` helper 0x004c0ad0 (collides if both are included in one TU).
- 0x004c1d30 holds a non-real stand-in body of 0x004c1a70 under `#ifndef __cplusplus` (no `// MATCH:` line).
- 15 files declare `code` as `typedef int code(...)` under `#ifdef __cplusplus` (C keeps `typedef int code()`); DDK scores unchanged.
- 0x004cc820 and 0x004c78e0 use `#pragma intrinsic(memset)` (rep stosd), allowed by the form rules.
- Helper permuter outputs that were semantically wrong (uninitialised local, moved increment, `*(int*)0`) were discarded.
- Rows decoded with `decomp_pc.py` from the main checkout (the worktree has no `mashed_pool`); data in `D:/MashedWT/MATCH-RWL01_data/`.

## Follow-ups
- A later C4R batch (`re/tools/c4_reclass.py`) can promote the 70 identical rows by route match.
- Next MATCH pass on this slice: the (a)/(b) rows at >=95% first (0x004c1040, 0x004c1210, 0x004ccc50), then mid-size (b) rows; the SSE rows need a different route (libid or asm-aware identity).
