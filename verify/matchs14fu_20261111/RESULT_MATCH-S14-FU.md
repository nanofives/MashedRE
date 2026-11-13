# RESULT MATCH-S14-FU, 2026-11-11 (run 38)

**Verdict: GREEN (partial yield).** Row kind match-only: no build, no gates. **2 rows C2 -> C4 by route `match`**
(0x00484310, 0x00488320), both byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178,
`/O2 /GS /Ob1`, C, `tc=ddk`), `relocheck.py` OK, plain C (no inline asm), promoted with `re/tools/c4_reclass.py batch`
(`c4_batch.tsv`, dry run `c4_dryrun.tsv`, apply report `c4_apply.tsv`). Both are in the T2 blocking set
(`verify/equivcheck_20261111/blocking_c0c3.tsv`). One source improved without closing (0x00489a40, 78.0% to 78.9%).
No other row closed.

Branch `orch/MATCH-S14-FU`, fast-forwarded from bf44aae7d to orch/integration b7fb2e66e before work (no commits on it).

## Compiler

All 33 rows are game code in 0x004840f0..0x0048b7c0, not RenderWare objects. None carries `// TOOLCHAIN rw9044`
(the L-131 / L-133 Rich prodid 0x30 case in `re/CONFIDENCE.md` applies to RW rows only). Every score below is `tc=ddk`.

## State of the 33 rows at the start (hooks.csv at b7fb2e66e)

Since MATCH-S14 (2026-11-06) other rows closed 13 of them:

| already C4 | rows |
|---|---|
| `c4=match` | 0x004840f0, 0x004841d0 (PORT-TOP-MATCH-01), 0x004844a0, 0x00484580 (MATCH-RACEPATH-FRESH), 0x0048aff0, 0x0048b230, 0x0048b440, 0x00484a50, 0x00485070 (C4R-MATCHBATCH) |
| `c4=frida-replay` | 0x00484f70, 0x00487020, 0x0048a4e0, 0x0048aa50 (still below 100% as match sources) |

**0x00484a50** is C4 by match from its in-file line in `re/match/s14/00484ac0.c` (100%, byte-identical, relocheck OK).
**0x00485070** is C4 by match (C4R-MATCHBATCH, 2026-11-07): under L-93 the C4 gate no longer looks at the .asi install,
so the MATCH-S14 "leg 2" hold no longer applies. What is left for both is port-axis work: a second file plus a naked
EAX/ECX-to-stack adapter for the naked callers in `Race/WorldRegistry_naked.inc` (0x00484b8d, 0x00484c44), and replacing
or dropping the `Lane2/L2_00485070.cpp` .asi body. Both need a build, so they are outside a match-only row, and
`WorldRegistry_naked.inc` is hand-path code (ROADMAP v5 "Frozen until T4"). Not done here.

## Ranking

The T2 blocking set (326 C0..C3 functions in reach of 0x00492370) holds 21 of the 33 rows. Order of work: blocking rows
with no source first (0x00484310, 0x00488320), then blocking near-misses with the least prior effort.

## Closed

| rva | size | source | match | relocheck | how |
|---|---|---|---|---|---|
| 0x00484310 | 68 B | `re/match/s14fu/00484310.c` | 100.0%, BYTE-IDENTICAL | OK checked=11 unchecked=0 | see below |
| 0x00488320 | 108 B | `re/match/s14fu/00488320.c` | 100.0%, BYTE-IDENTICAL | OK checked=6 unchecked=0 | see below |

**0x00484310.** The body reads ESI and never saves it (`push esi` at 0x00484310 is the argument of the call at
0x00484311, and there is no `pop esi`). Its only caller is 0x004846e4 in 0x00484580, which pushes `[esp+0xc]` and holds
the world pointer in ESI (`mov eax, esi` before the 0x004844a0 call at 0x004846f0). That is the same-TU static register
convention cl 13.00.9176 gives 0x004844a0 (world in EAX). A first source with an uninitialized local got 78.3% (the
compiler gives it a stack slot and saves ESI, `push ecx; push esi` at +0x0). The closing form copies
`re/match/racepath/004844a0.c` and adds `static void FUN_00484310(int world, int unused)` with the body of 0x004844a0's
head (callback `FUN_004842d0`, the pushed immediate at 0x00484316). The call becomes `FUN_00484310(world, s)`.
0x004844a0 and 0x00484580 stay BYTE-IDENTICAL in the new file (213 B / 584 B, relocheck OK 22 / 54).

**0x00488320.** `re/match/racepath/00488e70.c` already held a stand-in of this same-TU static (cloud in EAX, `&k` in EDX
at 0x00488fc0..0x00488fc7), written for the calling convention and marked "not scored, no MATCH line". With a
`// MATCH: 0x00488320` line added it is 100.0% and 108 B BYTE-IDENTICAL as written. 0x00488e70 in the same file stays at
99.6% (7 B), unchanged.

## Attempted, not closed

| rva | in blocking set | before | after | what was tried |
|---|---|---|---|---|
| 0x00489a40 | yes | 78.0% | 78.9% | multipliers as float literals 0.3f / 0.6f (.rdata 0x005cc99c / 0x005cc318) kept. A local copy of param_1: no change. `match_permute.py` (8 rounds, 40-step walk, 933 compiles) reached 85.6%, but only by moving `c = m_col[idx]` below its `c >> 24` use, which changes the computed value. **Rejected**, not written. Residual: the `(float)i` and `m_max` fild temps are one stack slot higher than the original's (param_1 / param_2 home slots, 0x00489b02..0x00489b0d), and the pos copy and `sz` are scheduled after the uv stores |
| 0x00488e70 | yes | 99.6% | 99.6% | `volatile float len` (the loop reloads len from memory each iteration): no change. Residual stays the `fstp len` after `mov ecx, e->count` (0x0048902b). Prior: 688 permuter compiles |
| 0x0048ab70 | yes | 91.2% | not changed | read only: two prior rounds (FRESH, FRESH2, 220 permuter variants, 30 store orders). The residual is the vertex pointer base (EBP = &a.y here, &a.z at 0x0071ba38 in the original) |

Not attempted this run, with the prior state re-scored at b7fb2e66e (every one `tc=ddk`):

| rva | blocking | best | note |
|---|---|---|---|
| 0x00484de0 | yes | 99.2% | 195 + 15 prior variants (MATCH-NEARMISS-2-B) |
| 0x004864f0 | yes | 90.1% | 162 prior permuter variants; one fld one slot early |
| 0x00484ac0 | yes | 70.3% | structural (class b) |
| 0x00485b30 | yes | 46.9% | structural (class b) |
| 0x0048a780 | yes | 49.1% | structural (class b) |
| 0x0048b7c0 | yes | 46.4% | partial |
| 0x004847d0 | yes | 34.9% | 10 pointer IVs in the original |
| 0x00485ef0 | yes | 19.8% | partial |
| 0x00486830 | yes | 46.7% | > 900 B (racepath source) |
| 0x00489500 | yes | 48.8% | > 900 B (racepath source) |
| 0x00487280, 0x004883b0 | yes | no source | > 900 B |
| 0x00489dc0 | no | 36.1% | partial |
| 0x0048a130 | no | 93.0% | permuter tried in S14 |
| 0x004854e0 | no | no source | > 900 B |
| 0x00484f70, 0x00487020, 0x0048a4e0, 0x0048aa50 | yes | 68.1 / 96.3 / 98.8 / 85.4% | already C4 by frida-replay |

## Trackers

`re/tools/c4_reclass.py batch --tsv verify/matchs14fu_20261111/c4_batch.tsv --result
verify/matchs14fu_20261111/RESULT_MATCH-S14-FU.md --apply`: hooks.csv 2 rows to C4 with `c4=match`, 2 CHANGELOG lines.
No STUBS / UNCERTAINTIES / DEFERRED change. Count by route: `c4=match` +2, `c4=frida-replay` +0.

## Raw data (not committed), `D:/MashedWT/MATCH-S14-FU_data`

`00489a40_perm.c` (the rejected permuter output) and `perm_00489a40.log`. Hashes are listed in `raw_sha256.txt`.
