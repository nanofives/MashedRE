# RESULT RWC-MATCH-S02: RW match slice 2 (0x004d7480..0x004f3e90)

Row kind: match-only (no build, no gates). Branch `orch/RWC-MATCH-S02` from `orch/integration` 0e277caaf.
Input list: `log/orch/run39_rwcmatch/S02.tsv` (74 RVAs). Method and lessons from `RESULT_RWC-MATCH-S01.md`.

## Verdict

GREEN. 3 rows promoted to C4 by `c4=match`: **0x004f3be0, 0x004e5d30, 0x004e66d0**. All three reach 100% only under
rw9044 (VC6 SP5 cl 12.00.8804 + c2 13.00.9044, `/O2 /Ob1 /G5`). The same source edits score 88.2 / 91.2 / 78.6% under
ddk, which is why the earlier ddk-only sessions did not keep them. 10 more sources gained `// TOOLCHAIN rw9044` with no
body change. Two sources had wrong semantics and were corrected, which lowered their scores (below).

## The input list was stale

Re-scored at session start under both toolchains (`scores.tsv`, columns `start_ddk`, `start_rw9044`):

- 12 of the 74 were already **C4 by `c4=match`** (0x004d7480, 0x004d80d0, 0x004d8480, 0x004dc5b0, 0x004e1df0,
  0x004e5ef0, 0x004e6650, 0x004e6710, 0x004e6760, 0x004e6920, 0x004ebb00, 0x004f2e70). Not touched.
- 0 were C4 by `c4=frida-replay`.
- 1 skipped as held by a sibling row: 0x004db3e0 (MATCH-DRIVER-RESID). 0x004d1e30 is outside this slice.
- 61 open rows remained.

## Promoted (c4_reclass.py batch, route match)

| RVA | tracker name | before | after | compiler | source | change |
|---|---|---|---|---|---|---|
| 0x004f3be0 | FUN_004f3be0 | 95.6% (ddk = rw9044) | 100%, BYTE-IDENTICAL 193 B | rw9044 (tag added) | `re/match/rwl03/004f3be0.c` | `iVar1 = param_1[2] * 4 + 0x50` moved below the grow test: the original computes `lea edi,[eax*4+0x50]` after the `jle`, on the fail path only. No store sits between the load of `param_1[2]` and the use. |
| 0x004e5d30 | FUN_004e5d30 | 94.1% (ddk = rw9044) | 100%, BYTE-IDENTICAL 237 B | rw9044 (tag added) | `re/match/rwl02/004e5d30.c` | the nested if chain is now early returns of the zero result (`r = f(); if (r == 0) return r;`) after FUN_004e3470 and FUN_004e65c0: the original returns eax=0 with `test; jne; ret` after each call. 16 callback names widened from `FUN_4xxxxx` to `FUN_004xxxxx` so relocheck checks them (was 11 checked / 16 unchecked, now 27 / 0) |
| 0x004e66d0 | RpClumpForAllAtomics | 70.6% (ddk = rw9044) | 100%, BYTE-IDENTICAL 57 B | rw9044 (tag added) | `re/match/s21/004e66d0.c` | `return clump;` inside the loop in place of `break`: the original reloads the clump from the arg slot at the single return (`mov eax,[esp+0x14]`) and holds `cur` in eax. Same exits, same value. This is the shape of the C4 sibling 0x004e6710. |

c4_reclass dry run (`c4_batch.tsv`):
```
[1/3] 0x004f3be0 match PASS byte-identical 193 B, relocheck OK (4/0)
[2/3] 0x004e5d30 match PASS byte-identical 237 B, relocheck OK (27/0)
[3/3] 0x004e66d0 match PASS byte-identical 57 B, relocheck OK (0/0)
batch: 3 pass, 0 refused, 0 skipped (already C4), 3 rows  [by route: match=3]
```
relocheck.py:
```
0x004f3be0	OK	checked=4	unchecked=0	004f3be0.c
0x004e5d30	OK	checked=27	unchecked=0	004e5d30.c
0x004e66d0	OK	checked=0	unchecked=0	004e66d0.c
```
0x004e66d0 has no relocations (the callback and pData come from the arg slots). None of the three sources is inline asm.

## Semantic fixes (score went down, the source is now right)

- **0x004d8c40** (C3, frontend list splice) 69.4 -> 27.0%. The non-empty-A arm set `a->prev` to the old head of B.
  The original's else arm (0x004d8c6d: `mov edx,[eax+4]; mov [edx],ecx; mov [ecx+4],edx`) sets `a->prev = b->prev`
  after `b->prev->next = a`. The C3 hand body (`Util/PromoLoop_sessionB.cpp`, naked transcription) shows the same thing.
  The wrong order let both arms tail-merge, which scored higher. The 27.0% equals the raw form's score in the file
  header, and the raw form had the right splice. A permuter run on the corrected source (686 compiles) only got back
  to 69.4% by moving `a->prev = n` above the reload again. That move was rejected.
- **0x004e5fc0** 77.3 -> 75.5%. The flag clear read `*(uint *)0xfffffffd & (param_1 + 0x4c)`, a load from address
  0xfffffffd. The original does `mov edx,[eax+0x4c]; and edx,0xfffffffd; mov [eax+0x4c],edx`, now written that way.
  Residual: x87 product order (`fld m; fmul st(1)` against our `fld st(0); fmul m`) and the epilogue schedule.

Neither file is included by a port `.cpp`. Both have a header line that records the fix.

## Retagged rw9044 (score up, body unchanged, not promoted)

0x004df120 46.3->52.8, 0x004e2090 29.7->30.7, 0x004e2bf0 38.2->38.3, 0x004e5850 57.4->60.7, 0x004e6ab0 46.1->58.3,
0x004e8c70 28.9->32.2, 0x004e99b0 45.5->61.7, 0x004f0f10 9.3->50.7, 0x004f3440 67.1->68.5, 0x004f36c0 32.0->33.1.
Each file holds one function, so no other row could lose.

## Tried, no gain (time-boxed)

| RVA | best | what was tried |
|---|---|---|
| 0x004e51e0 | 90.7% (ddk = rw9044) | one `axis` load in a block (64.8%, regs swap), `axis` local after `hi` (70.4%, spills), stack index form `bstack[sp]` (77.8%: no reload, but integer copies in place of fld/fstp and a byte-offset IV). The original keeps `node->axis` in ecx across the store through `hp`. Permuter 292 compiles, 0 gains. |
| 0x004f0ae0 | 81.8% (regs 100) | esi/edi naming swap only. Declaration order (3 forms), no `sz` temp, inverted NULL test: no change or worse. Permuter 216 compiles, 0 gains. |
| 0x004e1b60 | 86.5% ddk | error-record store before the call (80.4%), `iVar4 = param_2` before the first call (77.5%): the original loads the stream into esi after `push esi`, ours into edx before the frame. Permuter 233 compiles, 0 gains. |
| 0x004d8300 | 51.6% (regs 100) | while, if+do-while, explicit head pointer, param as loop variable (5 forms): f always gets edi, the original uses ebp. Permuter reached 77.4% only by moving the flag clear onto `f->next`, not equivalent, rejected. |
| 0x004daff0 | 77.8% | 8 flag sets (/G6, /Op, /Ox, /Oy-, /Ob2, /GB, ...) under both toolchains: none better. Residual is x87 operand order, as in the file note. |
| permuter, no gain | | 0x004dba60 87.6, 0x004e5fc0 77.3 (before the fix), 0x004dcbb0 77.5, 0x004e8640 77.9, 0x004e4c30 71.3 (0 variants generated), 0x004dc8a0 58.3 |

Rows under ~50% and the asm/CPUID leaves (0x004dc370, 0x004dc410, 0x004dcf90, 0x004dcff0, 0x004dd050, 0x004dd0b0)
were not worked, per the brief. 0x004f1fb0 does not compile (`C2069 cast of 'void' term` at line 196) and was not
worked.

## Lesson for the next slices

Score every hand edit under **both** toolchains. Three of the three closures here were edits that lose under ddk and
reach 100% under rw9044. In this slice the ddk and rw9044 scores of an unchanged source were often equal, so the
toolchain difference shows only after an edit.

## Counts

- Promoted C4 by `c4=match`: 3. Replay-verified (`c4=frida-replay`): 0.
- Functions that moved up in score under their own toolchain: 13 (3 by source edits to 100%, 10 by the rw9044 retag).
  Two moved down by the semantic fixes (0x004d8c40, 0x004e5fc0).
- Open rows in the slice after this: 58 (61 - 3), plus 0x004db3e0 held by MATCH-DRIVER-RESID.

## Files

- `scores.tsv`: every slice RVA with `tsv_best`, start and end scores under ddk and rw9044, the file's tag, the delta
  under its own tag, start confidence and C4 tags.
- `c4_batch.tsv`: the promotion batch. `c4_apply.tsv`: the `--apply` report.
- Commit SHA: see the REPORT (the commit that carries this file).
