# RESULT RWC-MATCH-S01: RW core match slice 1 (0x004c0910..0x004d7100)

Row kind: match-only (no build, no gates). Branch `orch/RWC-MATCH-S01`, source commit `565d9f8c0`.
Input list: `log/orch/run39_rwcmatch/S01.tsv` (74 RVAs). Context: L-124 option (C), `verify/rwsizing_20261111/RESULT_RW-SIZING.md`.

## Verdict

GREEN. 3 rows promoted to C4 by `c4=match`: **0x004c5f60, 0x004caea0, 0x004c5a00**. One near miss moved up by a
source edit (0x004cc820, 95.4 -> 98.0%, not promoted). 13 sources gained a `// TOOLCHAIN rw9044` tag because they score
higher under rw9044 than under ddk (no body change, none at 100%).

## The input list was stale

`S01.tsv` `best_pct` / `conf` predate the rw9044 sweep (MATCH-RW-COMPILER-9044, 2026-11-11). Re-scored at session start
(`scores.tsv`, column `start_best` = max of ddk and rw9044 on the source as it was in `orch/integration` 0e277caaf):

- 20 of the 74 were already **C4 by `c4=match`** in `hooks.csv` (0x004c2fb0, 0x004c5cb0, 0x004c90a0, 0x004c9eb0,
  0x004cb600, 0x004cb8a0, 0x004cba80, 0x004cbb70, 0x004cd550, 0x004cd900, 0x004ce480, 0x004d1640, 0x004d2980,
  0x004d4250, 0x004d42b0, 0x004d4310, 0x004d4dd0, 0x004d5bc0, 0x004d6200, 0x004d6910). Not touched.
- 3 are C4 by `c4=frida-replay` (0x004c1a70, 0x004c1b40, 0x004c4d20). Not promotable by c4_reclass (skips C4 rows);
  a match would still help the object build. 0x004c1b40 sits at 97.6% (one `dec esi` scheduled one x87 pair later).
- 2 skipped as held by sibling rows: 0x004c7a70 (MATCH-DRIVER-RESID), 0x004d1e30 (MATCH-DRIVER-RESID).
- 49 open rows remained. RW core is built by both compilers per object (RESULT_MATCH-RW-COMPILER-9044), so each row
  was scored under both and worked under the better one.

## Promoted (c4_reclass.py batch, route match)

| RVA | tracker name | before | after | compiler | source | change |
|---|---|---|---|---|---|---|
| 0x004c5f60 | rwID_TEXTUREMODULE_ctor | 99.1% (rw9044), 95.6% (ddk) | 100%, BYTE-IDENTICAL 471 B | rw9044 (tag added) | `re/match/rwl01/004c5f60.c` | `iVar2 = DAT_007d3ff8 + DAT_007d4054; *(int *)(iVar2 + 4) = iVar2;` fixes the one `lea` operand order |
| 0x004caea0 | FUN_004caea0 | 77.5% | 100%, BYTE-IDENTICAL 300 B under ddk and rw9044 | ddk (untagged) | `re/match/s20/004caea0.c` | `DAT_007d4578` as `MGLOBALA(int, .., 4, 0x007d4578)` cleared by one `memset(.., 0, 16)`: the original zeroes 0x007d4578/457c/4580/4584 from `eax` (xor eax,eax at 0x004caea0), which a 16-byte inline memset produces |
| 0x004c5a00 | FUN_004c5a00 | 89.7% (permuter 93.1%) | 100%, BYTE-IDENTICAL 90 B under ddk and rw9044 | ddk (untagged) | `re/match/rwl01/004c5a00.c` | store order f0, f4, f10, f30, f54, then dword +0x50 = 0, then `(+0x50 & ~0xff00) \| 0x1100`, then `(+0x50 & ~0xff) \| 1` (24-variant position sweep, one 100%) |

c4_reclass dry run (`c4_batch.tsv`):
```
[1/3] 0x004c5f60 match PASS byte-identical 471 B, relocheck OK (55/3)
[2/3] 0x004caea0 match PASS byte-identical 300 B, relocheck OK (32/0)
[3/3] 0x004c5a00 match PASS byte-identical 90 B, relocheck OK (4/0)
batch: 3 pass, 0 refused, 0 skipped (already C4), 3 rows  [by route: match=3]
```
relocheck.py:
```
0x004c5f60	OK	checked=55	unchecked=3	004c5f60.c	  unchecked: _LAB_004c61b0,_LAB_004c7200,_LAB_004c7490
0x004caea0	OK	checked=32	unchecked=0	004caea0.c
0x004c5a00	OK	checked=4	unchecked=0	004c5a00.c
```
The 3 unchecked symbols in 0x004c5f60 are code labels stored as function-pointer immediates (stores at
`+0x14/+0x18/+0x2c` of the module record, the fourth pointer is `FUN_004c6140`, which relocheck checks); they are
masked by match.py and the bytes around them are identical.
None of the three sources is inline asm.

## Near miss moved up (not promoted)

- **0x004cc820** 95.4 -> 98.0% (regs 99.3%), rw9044. match_permute.py reached 98.7% with two moves. One (two
  independent stores in the block-init loop) is kept. The other moved the `DAT_007d45cc` load after the
  `param_5[8] = &DAT_007d45cc` store. Those can alias, and the original loads first (`mov ecx,[0x7d45cc]` before
  `mov [ebx+0x20],0x7d45cc`), so it was reverted. Residual: the original hoists the `param_4` test above four header
  stores and the `DAT_007d45cc` load above the `lea`. Loop-head variants (`!= 0`, `while`, `(int)` cast, store
  reorders) gave 98.0% or lower.

## Retagged rw9044 (score up, body unchanged, not promoted)

0x004c1b40 92.7->97.6, 0x004c2620 19.0->19.8, 0x004c32b0 62.1->62.6, 0x004c3e90 44.3->44.5, 0x004c4a50 77.8->92.9,
0x004c52f0 12.6->19.6, 0x004c6750 83.3->93.5, 0x004cbbd0 75.5->77.4, 0x004cc050 78.5->80.6, 0x004cc820 74.5->95.4
(then 98.0 above), 0x004cf5a0 59.4->65.2, 0x004cff20 62.5->79.7, 0x004d0290 76.4->79.8 (ddk -> rw9044 on the same
source). Each file holds one annotated function, so no other row could lose.

## Tried, no gain (time-boxed)

| RVA | best | what was tried |
|---|---|---|
| 0x004c1040 | 99.2% ddk (regs 100) | 7 loop spellings for the prev/next walk (for, while, do-while, comma assign, `int **pp`); the copy `mov edx,eax` is always propagated so the load base is `eax`, original `edx`. Also flags /G4 /G6 /Ox /Op /Oy-: same or lower. Prior session tried 3 more. |
| 0x004d4f00 | 98.5% | moving `e = param_1->elems` under `if (n != 0)` (where the original loads it) gives regs 100% but swaps esi/edi (74.2%); 6 variants plus permuter (228 compiles from that start) did not swap them back |
| 0x004cd500 | 83.3% (regs 100) | 5 operand-order spellings of `RwEngineInstance + offset`; cl canonicalises them all |
| 0x004d40d0 | 89.1% (regs 97.8) | original compares the vcall result with `cmp eax, edi` (0 held in edi), ours `test eax,eax`; return-type and compare-form variants unchanged |
| 0x004d1d70 | 72.1% (regs 98.4) | 48 variants: all 24 case orders x ext computed before / inside the parent test; best 75.4% with regs 95.1%, not kept |
| 0x004cc050 | 80.6% | original pushes `edi` only inside case 1/2 (the file-stream arm); not reproduced, not pursued |
| permuter, no gain | | 0x004c4a50 92.9, 0x004cb4c0 90.2, 0x004c6750 93.5, 0x004c4dc0 91.2, 0x004d40d0 89.1, 0x004c1b40 97.6, 0x004cd500 83.3 (4 rounds, walk 40, 500 s each) |

Rows under ~70% (0x004c1f80, 0x004c2620, 0x004c51a0, 0x004c52f0, 0x004cdf20, 0x004d2340, 0x004d3090, 0x004d4380,
0x004d7100 and others) were not worked, per the brief.

## Counts

- Promoted C4 by `c4=match`: 3. Replay-verified (`c4=frida-replay`): 0 new.
- Functions that moved up in score this session: 17 (4 by source edits: 0x004c5f60, 0x004caea0, 0x004c5a00,
  0x004cc820; 13 by the rw9044 retag, with 0x004cc820 counted once).
- Open rows in the slice after this: 46 (49 - 3).

## Files

- `scores.tsv`: every slice RVA with `tsv_best`, `start_best`, `now`, `now_regs`, toolchain, source, confidence and
  C4 tags before this row.
- `c4_batch.tsv`: the promotion batch. `c4_apply.tsv`: the `--apply` report.
- Port note: `mashedmod/src/mashed_re/RwCore/RwCore_004c5f60.cpp` and `RwCore_004cc820.cpp` include the edited
  sources. Both edits keep the computed values (one value named once, one swap of two independent stores). No build
  was run (match-only row). `RwCore_004c32b0/004c3e90/004cff20/004d0290.cpp` include sources that only gained the tag;
  `ddkobj.py` honours `// TOOLCHAIN`, and none of them is at 100%.
