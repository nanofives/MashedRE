# RESULT RWC-MATCH-S03: RW match slice 3 (0x004f57e0..0x005cb404)

Sources, trackers and evidence: commit `552338c32` on `orch/RWC-MATCH-S03`.

Row kind: match-only (no build, no gates). Branch `orch/RWC-MATCH-S03` from `orch/integration` 0e277caaf.
Input list: `log/orch/run39_rwcmatch/S03.tsv` (73 RVAs). Same method as RWC-MATCH-S01
(`verify/rwcmatch_s01_20261112/RESULT_RWC-MATCH-S01.md`, branch `orch/RWC-MATCH-S01`).

## Verdict

GREEN. 5 rows promoted to C4 by `c4=match`: **0x00543e50, 0x0054b740, 0x0053fa20, 0x0053fb00, 0x00543bb0**.
Two more source edits moved near misses up without reaching 100% (0x00546e70 87.0 -> 92.0, 0x00534b60 94.9 -> 95.6).
16 sources gained `// TOOLCHAIN rw9044` because they score higher under rw9044 than under the file default (ddk).

## The input list was stale

Re-scored at session start (`scores.tsv`, `start_best` = max of `match.py --toolchain ddk` and `--toolchain rw9044` on
the source as it was in `orch/integration` 0e277caaf; `conf_start` / `c4tag_start` from `hooks.csv`):

- 15 of the 73 were already **C4 by `c4=match`** (0x004f8580, 0x00538600, 0x0053eaa0, 0x0053fd30, 0x00540340,
  0x00541b50, 0x00544d20, 0x00546b10, 0x00549900, 0x00551190, 0x00551a50, 0x00551c40, 0x00552920, 0x00553cf0,
  0x005584c0). Not touched.
- 1 is C4 by `c4=frida-replay` (0x00556ca0, 60.0% under both). Not touched.
- 3 skipped as held by sibling rows: 0x00554390, 0x00555360 (MATCH-RT2D-RESID), 0x005405c0 (MATCH-DRIVER-RESID).
  0x00554200 is not in this list.
- 2 have **no hooks.csv row** but already score 100%: 0x00550c50 (rw9044, tagged) and 0x00550da0 (ddk and rw9044).
  `c4_reclass.py` refuses them ("no hooks.csv row (add it first: C1 by the identity)"). Not promoted. Follow-up: a
  re-classify row creation for both, then a one-line c4 batch.
- 2 sources do not compile under either toolchain (0x00538d60 C2440, 0x0053dea0 C2069), 2,960 and 1,415 B. Not worked.
- 0x005cb404 (38 B, `_rwcseg`, census mstate `never`) has no source in `re/match`. Not worked.
- 49 open rows remained.

## Promoted (c4_reclass.py batch, route match)

| RVA | before | after | toolchain | source | change |
|---|---|---|---|---|---|
| 0x00543e50 | 75.9% ddk, 98.9% rw9044 | 100%, 619 B | rw9044 (tag added) | `re/match/rwl04/00543e50.c` | `return -1 < DAT_007dc668` -> `return 0 <= DAT_007dc668` (int): original `test eax,eax; setge cl`, ours was `cmp eax,-1; setg cl` |
| 0x0054b740 | 66.1% ddk, 98.9% rw9044 | 100%, 376 B | rw9044 (tag added) | `re/match/rwl04/0054b740.c` | header buffer `uint buf[5]` and `buf[3] > 0`, `buf[2] > 0`: original `jbe` (unsigned) after `test eax,eax` (`jbe 0x54b857`, `jbe 0x54b884`), ours `je`. For unsigned, `> 0` equals `!= 0`, and the `buf[1] * k` products pass the same bits |
| 0x0053fa20 | 93.8% ddk | 100%, 80 B | ddk (untagged) | `re/match/rwl03/0053fa20.c` | read target is a local `int flag`, not `&param_2`: the original reads into the dead param_1 home (`lea eax,[esp+8]` after `push edi`, param_1 kept in edi), ours read into param_2's slot (`[esp+0xc]`). param_2 is never read in the body |
| 0x0053fb00 | 93.8% ddk | 100%, 80 B | ddk (untagged) | `re/match/rwl03/0053fb00.c` | same change as 0x0053fa20 (twin over DAT_007dc5f0 / FUN_00543b20) |
| 0x00543bb0 | 72.3% ddk, 76.1% rw9044 | 100%, 392 B | rw9044 (tag added) | `re/match/rwl04/00543bb0.c` | (1) name alloc as one call `(*pf)(DAT_007d3ff8->fnF4((int)param_2) + 1, 0x30002)`: the original pushes 0x30002 before the strlen call and pops 4 after it, 78.0%; (2) `local_4 = -1;` moved above `iVar4 = param_1->n;` (match_permute.py, 106 compiles): the original then gives local_4 its own slot (`push ecx`) instead of param_1's home. local_4 is a non-address-taken local, so the order does not change any value |

c4_reclass dry run (`c4_batch.tsv`):
```
[1/5] 0x00543e50 match PASS byte-identical 619 B, relocheck OK (75/0)
[2/5] 0x0054b740 match PASS byte-identical 376 B, relocheck OK (10/0)
[3/5] 0x0053fa20 match PASS byte-identical 80 B, relocheck OK (3/0)
[4/5] 0x0053fb00 match PASS byte-identical 80 B, relocheck OK (3/0)
[5/5] 0x00543bb0 match PASS byte-identical 392 B, relocheck OK (6/0)
batch: 5 pass, 0 refused, 0 skipped (already C4), 5 rows  [by route: match=5]
```
relocheck.py:
```
0x00543e50	OK	checked=75	unchecked=0	00543e50.c
0x0054b740	OK	checked=10	unchecked=0	0054b740.c
0x0053fa20	OK	checked=3	unchecked=0	0053fa20.c
0x0053fb00	OK	checked=3	unchecked=0	0053fb00.c
0x00543bb0	OK	checked=6	unchecked=0	00543bb0.c
```
None of the five sources contains `__asm`.

## Near misses moved up (not promoted)

- **0x00546e70** (quaternion slerp-cache setup) 87.0 -> 92.0% (ddk). The 23 float globals it read
  (0x005cc320..0x005e4888) are all in `.rdata` (0x005cc000..0x005e9044, read-only), so they are written as literals
  (round-trip checked to the same float bits). Residual: the original loads six of the eight final scaled fields first
  (`fld [esi+4]; fmul st(1)`) and pops invSin with `fstp st(0)`, and `df = sq` is `fld st(0); fstp`. Operand order,
  compound `*=`, comma rvalue, double intermediates and rw9044 do not move it. match_permute.py reached 93.1% by moving
  `sq = FUN_004c3b30(z)` below `df = sq`, which reads sq before it is assigned: rejected.
- **0x00534b60** 94.9 -> 95.6% (rw9044). The 0x8000 mask moved into the f8 test
  (`f8 == 1 && (t = param_2 & 0x8000) == 0x8000`), as the original does `and eax,0x8000` after `cmp esi,ebx; jne`.
  Residual: the original re-tests f8 (`test esi,esi`) after the f8 block, ours threads the jump past it. 7 spellings
  of the f8 == 0 test (nested, `!f8`, `!= 1`, else-if, bit tests) and match_permute.py (345 compiles): no gain.

## Retagged rw9044 (score vs the file default up, body unchanged)

0x004f57e0 14.2->32.7, 0x004f6870 23.9->39.9, 0x0052daf0 42.5->44.0, 0x0052e0e0 36.6->37.1, 0x00534b60 81.6->94.9,
0x00539ec0 12.4->12.7, 0x0053d460 41.5->47.5, 0x0053d820 66.7->72.5, 0x00543bb0 72.3->76.1, 0x00543e50 75.9->98.9,
0x00544bf0 74.3->78.3, 0x00546890 24.6->25.8, 0x00547bf0 46.0->71.6, 0x0054b740 66.1->98.9, 0x0054f8d0 54.9->56.7,
0x00551460 37.9->56.9. Each file holds one annotated function. 0x00555360 also scores higher under rw9044 (98.9 vs
94.4) but its file belongs to MATCH-RT2D-RESID and was left alone.

## Tried, no gain (time-boxed)

| RVA | best | what was tried |
|---|---|---|
| 0x00556e90 | 99.0% ddk | not worked: one scheduling residual (integer copy of a spilled delta) after ~4,500 variants in MATCH-RT2D-FONT-2 |
| 0x00535700 | 51.7% ddk, 49.4% rw9044 (regs 97.7) | ecx/edx swapped for param_2 and the switch index: declaration order, default-case position, param_2 copy, rw9044 permute (342 compiles) |
| 0x0053d5a0 | 75.3% | original keeps 0 in ecx and the record pointer in edx: `!= 0` test, uint cursor, single return, else-first (8 variants, both toolchains) |

Rows under ~70% were not worked, per the brief.

## Counts

- Promoted C4 by `c4=match`: 5. Replay-verified (`c4=frida-replay`): 0 new.
- Functions that moved up in score this session: 19 (7 by source edits: 0x00543e50, 0x0054b740, 0x0053fa20, 0x0053fb00,
  0x00543bb0, 0x00546e70, 0x00534b60; 16 by the rw9044 retag, with the 4 retagged-and-edited rows counted once).
- Open rows in the slice after this: 44 (49 - 5).

## Files

- `scores.tsv`: every slice RVA with the census `tsv_best`, `start_best`, `now_*`, the toolchain in the file,
  source, start confidence and C4 tag.
- `c4_batch.tsv`: the promotion batch. `c4_apply.tsv`: the `--apply` report.
- Variant files and permute logs stayed in the session scratchpad. They are not evidence for a status change.
- Port note: none of the edited sources is included by `mashedmod/src` (grep for the file names: no hits), so no port
  build is affected. No build was run (match-only row).
