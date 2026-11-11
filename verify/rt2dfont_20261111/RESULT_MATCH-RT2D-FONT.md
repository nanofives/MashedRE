# RESULT MATCH-RT2D-FONT (2026-11-11)

Row: byte-match the unmatched Rt2d font toolkit nodes under 0x004275d0 (6 with no source in
verify/porttop_20261108/RESULT_PORT-TOP-04G.md:148-149) and the C2 nodes it names (0x00554390, 0x00556e90), so the
font loaders 0x00427ca0 / 0x004275d0 can later run matched under MASHED_ORIG_TOP. Match-only row: no build, no gates.

Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), flags as recorded in each source: `/O2 /Ob1 /G5`
(RenderWare code; `/O2 /Ob1` without `/G5` scores lower on every row, e.g. 0x005c4ad0 88.5% against 96.7%).
Tools: `re/tools/match/match.py` (insn score + `--bytes`), `relocheck.py`, `match_permute.py`, branch base
`orch/integration` 778b8b91d.

## Verdict

5 of 8 at 100% byte-identical with relocheck OK. 4 promoted to C4 by route match (`c4_reclass.py batch`),
1 (0x00552b50) given a C1 row first (it had none) and then promoted. 3 improved but not matched.

| RVA | before | after | relocheck | source | note |
|---|---|---|---|---|---|
| 0x00552920 | 79.8% | **100%**, 316 B BYTE-IDENTICAL | OK 17/0 | re/match/rwl04/00552920.c | width/height as one V2d local, products written `sz.x * scale * k` |
| 0x00552b50 | no source | **100%**, 6 B BYTE-IDENTICAL | OK 1/0 | re/match/rt2d/00552b50.c (new) | `jmp [0x00912c58]`: tail call through the callback 0x00552a60 saved |
| 0x00553cf0 | 40.9% | **100%**, 262 B BYTE-IDENTICAL | OK 12/0 | re/match/rwl04/00553cf0.c | 0x00553590 body as a __forceinline helper; if/else on the -1 test |
| 0x005c4ad0 | 96.7% | **100%**, 205 B BYTE-IDENTICAL | OK 7/0 | re/match/s28/005c4ad0.c | SList struct; entrySize stored after alloced (permute "move") |
| 0x005c4c60 | 93.0% | **100%**, 186 B BYTE-IDENTICAL | OK 3/0 | re/match/s28/005c4c60.c | return value `*p + p[1]*p[3]` computed before `p[1] += n` |
| 0x00556e90 | 97.5% | 98.0% | n/a | re/match/s24/00556e90.c | 0.0f literal; residual x87 scheduling, permuter 395 compiles no gain |
| 0x00554200 | 69.5% | 69.5% | n/a | re/match/s24/00554200.c (unchanged) | result kept in its own register (ebp) in the original, joined at one exit |
| 0x00554390 | 33.5% | 62.6% | n/a (layout differs) | re/match/rwl05/00554390.c | rewritten from the disassembly; residual = per-return epilogues |

Byte-identical lines come from `py -3.12 re/tools/match/match.py <src> --bytes` and the `c4_reclass.py batch` dry run
(below). relocheck: `py -3.12 re/tools/match/relocheck.py <src>`. None of the five is inline asm.

## Needs Opus-high shaping (not matched here)

- **0x00554390** (Rt2d font stream read, 1110 B, 62.6%). The structure is now right (Font 0x144 B / Glyph 0x20 B,
  memset intrinsics, outline branch first, bitmap branch with TXD save/restore at 0x004c5ca0 / 0x004c5c80). The
  original pushes ebx/ebp/esi/edi at entry (0x00554393..0x00554396) and gives every early `return 0` its own epilogue
  that returns the zero already in eax (e.g. 0x005543c2..0x005543c9); DDK 9176 on this source shrink-wraps ebp/esi/edi
  past the first return and cross-jumps the later returns into one block with `xor eax,eax`. Tried: the read helper
  typed as returning a pointer, 8 flag sets.
- **0x00554200** (Rt2d path node stream read, 69.5%). The original keeps the result in ebp, separate from the node in
  esi (`mov ebp, esi` at 0x00554256), sets it to 0 on each failure and joins at 0x0055432d (`mov ebx, ebp`). Ours
  coalesces result and node. Tried here: three final-return shapes (68.8 / 69.5 / 68.8); the source header lists the
  earlier attempts (permuter 422 compiles).
- **0x00556e90** (Rt2d brush set RGBA, 98.0%). Two integer copies of spilled deltas (`mov edx,[esp+k]`) and one esi
  reload scheduled one x87 instruction away from the original; permuter 395 compiles, no gain.

## Promotion

Dry run then apply, batch file `verify/rt2dfont_20261111/c4_batch.tsv`:

```
py -3.12 re/tools/c1_add.py --rva 0x00552b50 --subsystem "third-party-library[renderware]" --file re/match/rt2d/00552b50.c --why "..." --result <this RESULT> --tag MATCH-RT2D-FONT
py -3.12 re/tools/c4_reclass.py batch --tsv verify/rt2dfont_20261111/c4_batch.tsv
py -3.12 re/tools/c4_reclass.py batch --tsv verify/rt2dfont_20261111/c4_batch.tsv --apply --result verify/rt2dfont_20261111/RESULT_MATCH-RT2D-FONT.md
```

c1_add: `[1/1] 0x00552b50 FUN_00552b50 third-party-library[renderware] (none)->C1`. Apply output
(`verify/rt2dfont_20261111/c4_apply.txt`): 5 pass, 0 refused; hooks.csv 5 rows -> C4 (0x00552920, 0x00552b50,
0x00553cf0, 0x005c4ad0, 0x005c4c60), 5 CHANGELOG lines.

## Test fixtures

`re/tools/match/test_equiv.py` used the live sources of 0x005c4ad0 (expected EQUIV, the imul-order case) and
0x00556e90 (expected NOT-PROVEN, x87 scheduling) as reference vectors. Both sources changed, so the pre-change copies
are frozen at `re/tools/match/equiv_tests/refs/005c4ad0_held.c` / `00556e90_held.c` and the test points at them.
Result: 2 passed (test_cases, test_references), `verify/rt2dfont_20261111/test_equiv.txt`.

## Port axis

Nothing here touches `mashed_re.exe` or the knob path. What the font loaders still need on the top-down path:
0x00554390, 0x00554200 and 0x00556e90 matched (or admitted under L-126 / provisional), then an exe Rt2d port drawn
through the exe's Im2D path (RESULT_PORT-TOP-04G.md:153-154).

## Rebase onto d8f323c9a (MATCH-REBASE-3, 2026-11-11)

The merge train set this branch aside on a hooks.csv / CHANGELOG conflict. Rebased onto `orch/integration`
d8f323c9a with integration's `hooks.csv` and `re/analysis/CHANGELOG.md` taken as-is, then the promotions re-applied
with the batch tool. `c1_add.py` for 0x00552b50 was not re-run: integration already carries its C1 row (RWC-CENSUS,
`verify/rwccensus_20261111/RESULT_RWC-CENSUS.md`), so the batch promotes that row C1->C4. Re-apply output
(`verify/rt2dfont_20261111/c4_apply_rebase.txt`): 5 pass, 0 refused, relocheck OK on all five against the current
relocheck (named data operands compared): 0x00552920 17/0, 0x00552b50 1/0, 0x00553cf0 12/0, 0x005c4ad0 7/0,
0x005c4c60 3/0. `test_equiv.py` re-run after the rebase: see `verify/rt2dfont_20261111/test_equiv_rebase.txt`.
