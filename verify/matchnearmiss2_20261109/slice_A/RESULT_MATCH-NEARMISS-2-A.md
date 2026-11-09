# MATCH-NEARMISS-2-A: hand rewrites on slice A (23 near-miss rows, 2026-11-09)

Branch `orch/MATCH-NEARMISS-2-A`, base `orch/MATCH-NEARMISS-2` @ 55d7eb08a. Toolchain `re/tools/match/match.py --show
--bytes --only` (XP DDK cl 13.00.9176 / c2 13.00.9178), each file's own line-1 FLAGS, unchanged. No `__asm`, no naked,
no pragma. No mashedmod/ edit, no game run, no Ghidra write, no tracker edit.

**Verdict: 0 of 23 closed. 22 NOT-CLOSED, 1 PORT-INCLUDED (0x0042f530, not closed on its copy), 0 SIZE-ARTEFACT.**
Three rows moved closer (variants kept under `variants/`, committed sources unchanged):

| rva | before | after (variant) | lever |
|---|---|---|---|
| 0x004627f0 | 96.8% / 61 B | 98.9% / 8 B | float stores through a block-local `float *` keep the two RMW `or [eax+0x70]` separate (instruction count now equals the original, 94) |
| 0x0052fdb0 | 96.2% / 33 B | 97.5% / 25 B | index form `idx[i*4 + k]`: the compiler builds the base+2 induction pointer itself, `add ecx,2` lands after the count test |
| 0x004c5f60 | 95.6% / 7 B | 96.5% / 6 B | typed RW plugin-globals macro + `rwLinkListInitialize` on the list head fixes the `lea` operand order |

## Closure per miss type

| miss type | rows | closed |
|---|---|---|
| order | 14 | 0/14 |
| count | 5 | 0/5 |
| const (const+regalloc, const+other, const+other+regalloc) | 4 | 0/4 (1 of them PORT-INCLUDED) |

## What was tried, by lever

1. **Callee prototypes against their own matched sources** (`arity.py`-style scan of every declaration in the 22 files
   against `re/match/**`). No arity or type error in any slice row. The mismatches the scan reported are either call
   text (not a declaration) or a callee whose own source reads fewer args than the binary call site pushes
   (0x0044e0a0 pushes 3 to 0x00481750; the declaration follows the binary). **One real error was outside the slice in a
   slice file:** `re/match/s05/00420050.c` row 0x0041f8f0 (C4, route match) called
   `FUN_00422570(key, m, FUN_00426070(float))`, while `re/match/s06/00426070.c` takes no args and
   `re/match/s05/00422570.c` takes 4 (`float alpha` last). Fixed to `FUN_00422570(key, m, FUN_00426070(), float)`.
   The function stays BYTE-IDENTICAL and relocheck OK (`relocheck_s05_00420050.tsv`, 0x0041f8f0 OK checked=26),
   the other three functions in the file score as before (0x0041faf0 90.8, 0x0041fcc0 98.9, 0x00420050 99.3). The
   byte compare cannot see this class: under cdecl the float push and the merged `add esp` are the same bytes for both
   arities (same class as the pilot's 0x005b0df0).
2. **RW list-link / typed link idiom.** Only 0x004c5f60 has a list head in this slice. The typed form fixed one
   instruction. The remaining residual in 0x004c5f60 and 0x004cd900 (both RW module-open functions) is the same: the
   first failure `return NULL` stays inline in the original and is cross-jumped into a later failure's
   `xor eax,eax; pop esi/ebx; ret` tail in ours. No return spelling moved it.
3. **Statement order, temporaries, loop form, polarity.** 20 of the 23 rows went through the 11-07 bounded permute (all but the three RW rows);
   their residuals are one-slot scheduling of an x87 store / load / pop (0x0040ad90, 0x00412cf0, 0x0041e6c0,
   0x0041fcc0, 0x00420050, 0x00442a60, 0x0044e0a0, 0x00458e00, 0x0045c030, 0x0045e990, 0x00426810), a register choice
   (0x00408610, 0x0042f530, 0x004046a0), block layout / tail-merge choice (0x004332a0, 0x00461e90) or one
   store-vs-load/push order (0x0044c4f0, 0x00407800, 0x00429bd0). The diffs were read for each and the hand log names
   the forms compiled; rows marked 0 compiles had no lever left that the prior notes had not already tried.

## Count rows: size artefact check

`verify/c4rnearmiss_20261107/size_triage.py` over the 5 count rows: 0x004c5f60 471/471, 0x004cd900 608/608,
0x00461e90 1250/1250 and 0x004627f0 351/351 (pc_funcs body_size = contiguous). 0x00408610 is 1066 vs 1073, and at
1073 B it still has 32 differing bytes (97.3%), so it is a code difference. No row is SIZE-ARTEFACT.

## Port check

`grep -rIn "<dir>/<rva>.c" mashedmod` per row: only `s07/0042f530.c` is referenced (`Match/PortTop_0042f530.cpp`
`#include`, `PortTop_0042f660.cpp`, `exe_sources.rsp`). It was worked on `port_included/0042f530.c` only. The file
changed in this branch, `re/match/s05/00420050.c`, is not referenced by mashedmod.

## Gate dry run

0 CLOSED rows, so `c4_reclass.py make-tsv` / `batch` had no input: gate dry run 0/0. 0x0041f8f0 (corrected source) is
already C4: `batch` on it reports `SKIP already C4 (no write)`. Its match + relocheck evidence after the fix is above.

## Read for the lead

- Slice A predicted yield was low and measured 0/23: 20 rows were permute residue and the two RW rows share one
  compiler tail-merge choice. The pilot's levers (callee arity, param-to-field mapping, list-link idiom) found no error
  inside these 23 functions.
- The arity scan is cheap and finds errors that the byte compare masks. A pool-wide run over every BYTE-IDENTICAL
  source (not only near misses) would catch further C4 rows like 0x0041f8f0.
- The three variants are better than the committed sources and may be adopted as the new best (the file headers would
  need their score lines updated). 0x004627f0 at 8 bytes and 0x0052fdb0 at 25 bytes are the nearest.

## Files

| file | sha256 |
|---|---|
| results.tsv | e12d6c9534c308e8cb606a1fe93b661cf8c92e3e0610b90171f1f71bbe4cd330 |
| hand_log.tsv | e77e3d001ee5c0bf89b4710d8b56fc1fa375da058f76a28dca9ec78db6596787 |
| build_results.py (rebuilds results.tsv from hand_log.tsv, re-scores sources and variants) | 65f1ee37dd09c27a9cb324f77b4c55c30a32d7d3e48044653dd3ddebf43aa7bf |
| relocheck_s05_00420050.tsv | 29b92af642a3da5b2ac79e311df05f8fb182a4bfc40c3728c43418e14dd98c87 |
| variants/004627f0.c | 6825ad70fd4ed3568a8f68b10bf60c0a0fd9678606f50a1f595089753233a07e |
| variants/0052fdb0.c | be8d554c884280bb342bf54a8fd83ae5d59b0b63d0a9d7ea2daf8ed90f7a80c6 |
| variants/004c5f60.c | f6b6f550b21aac53abaec6d7a61581e31f02405b0998f9ee3773f4360e7576e0 |
| port_included/0042f530.c (unchanged copy) | 392b17e272948dbfa81b29dff9566231631bfb0231df514bda8b74e3ce0b5020 |
| re/match/s05/00420050.c (after) | 1762c99c427981bf17ee7b57aadddd3f9450790dc89c9cb3f8089a24e3bbb9ab |
