# RESULT RWC-MATCH (run 40): near misses, compile failures, S01-S03 follow-ups

Row kind: match-only (no build, no gates). Branch `orch/RWC-MATCH` from `orch/integration` b34672ee2.
Inputs: the run 39 follow-up list in the ledger note, `log/orch/run39_rwcmatch/S01-S03.tsv` and the run 39 score
tables `verify/rwcmatch_s0{1,2,3}_20261112/scores.tsv`. Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 (`ddk`) and the
rw9044 toolchain, flags `/O2 /Ob1 /G5` from each source's `// FLAGS`, scored with `re/tools/match/match.py`.

## Verdict

GREEN. 1 row to C4 by `c4=match`: **0x004d4f00**. Two near misses moved up by source edits, three sources that did not
compile now compile, 0x005cb404 reported as unmatchable from C. Promotion file `re/promotions/RWC-MATCH.tsv`
(not applied on the branch, the merge train applies it).

## Promoted (route match)

| RVA | name | before | after | compiler | source | change |
|---|---|---|---|---|---|---|
| 0x004d4f00 | FUN_004d4f00 | 98.5% (ddk) | 100%, BYTE-IDENTICAL 142 B | ddk | `re/match/s21/004d4f00.c` | `if (param_1->count != 0) { n = param_1->count; e = param_1->elems; ...` in place of loading count and elems before the test. The original loads elems inside the branch after the param_3 load (0x004d4f00 listing); the earlier placement swapped esi/edi. count is read twice with no store between, same values. |

`c4_reclass.py batch --tsv re/promotions/RWC-MATCH.tsv` (dry run):
```
[1/1] 0x004d4f00 match PASS byte-identical 142 B, relocheck OK (1/0)
batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
```
relocheck: `0x004d4f00 OK checked=1 unchecked=0`. The source is plain C, no inline asm. No port file includes it.

## Near misses (brief item 1)

| RVA | before | after | what happened |
|---|---|---|---|
| 0x004d4f00 | 98.5 | **100** | above |
| 0x004cc820 | 98.0 | 99.3 (rw9044) | the list-add tail written in `rwLinkListAddLLLink` order on `param_5 + 7` (`p[7] = head; p[8] = &list; head->prev = p+7; head = p+7`) reloads the head into edx as the original does. Load stays before the `p[8]` store (the order S01 kept for aliasing). Residual: the original hoists the `param_4` test above the four header stores. 15 loop-head / store-order spellings and the permuter (474 compiles): no gain. Port includes this source (`RwCore_004cc820.cpp`, `PortTop_004cc820.cpp`): same constructs as before, no build run (match-only row). |
| 0x004c1040 | 99.2 | 99.2 | 5 more loop spellings (pointer, struct `next` field, `while (cur->next != child)`), RemoveChild as `__inline` / `__forceinline` helper under both compilers, permuter 376 compiles: the copy is always propagated, so the load base stays eax (original edx). |
| 0x004d1e30 | 99.1 | 99.1 | levels as inline call arguments, function scope, int, two-step, via a format local, before the texture test: 99.1 or lower under both compilers. MATCH-DRIVER-RESID's branch (975911b07) source scores the same 99.1. |

## Other rows from the lists

| RVA | before | after | note |
|---|---|---|---|
| 0x00534b60 | 95.6 | 98.5 (rw9044) | with `f8 == 0 && f1 == 0`, cl threads the f8 == 1 arm past the second f8 test (an extra jmp the original lacks). `f1 == 0 && f8 == 0` stops the threading (pure tests, same values). Residual: the original tests f8 first there, unthreaded. 49 compare-form x flag-type variants and the permuter (425 compiles) did not reach it. |
| 0x004f0ae0 | 81.8 (regs 100) | 81.8 | esi/edi naming swap only. 8 spellings (no size local, declaration orders, unsigned size, no result temp, early returns) and the permuter (436 compiles): no gain. |
| 0x004dba60 | 87.6 | 87.6 | converting ox before oy matches the original's order (regs 97.8%) but drops exact to 82.6%. Not kept. |
| 0x00556e90 | 99.0 | not worked | MATCH-RT2D-FONT-2 already ran about 4,500 variants on it. |
| 0x00546e70 | 92.0 | not worked | x87 schedule residual, S03 note lists what was tried. |
| 0x00555360, 0x00554200, 0x00554390, 0x00553e80 | | not touched | owned by MATCH-RT2D-RESID (lead note). |
| 0x004db3e0 | 95.2 | not touched | MATCH-DRIVER-RESID found its lever is a third compiler. |

## 100% rows with no hooks.csv row (brief item 2)

0x00550c50 and 0x00550da0 already went through MATCH-RECLASS-550C50 (applied by the merge train, `eac388006`):
both are C4 in `hooks.csv` now. Nothing to do here.

## Sources that did not compile (brief item 3)

All three were source defects in raw g2s output: wrong callee prototypes or a stack array typed as pointers.

| RVA | error | fix | score now |
|---|---|---|---|
| 0x004f1fb0 | C2069 cast of void | `FUN_004f0970` declared `void *` (its body, `re/match/rwl03/004f0970.c`, tail-returns the allocator result in eax, and this caller uses it) | 37.0% ddk |
| 0x0053dea0 | C2115 x2, C2069 x3 | prototypes of `FUN_0053e430` (int, int, float, int *), `FUN_0053e730` and `FUN_0053e760` (return `undefined4 *`) copied from their own sources in `re/match/rwl03` | 28.1% rw9044 (tag added), 24.1% ddk |
| 0x00538d60 | C2440 x8 | five reads and writes of `undefined1 *` stack slots written `*(float *)&slot`: `odis.py 0x00538d60` shows no `fild` in the 756-instruction body, so the slots are float bit patterns, not integer conversions | 12.3% |

**0x005cb404 (no source):** `odis.py` shows a 39-byte stub `push eax / wait / fnstcw [0x915000] / mov eax,[0x915000] /
mov [0x915004],eax / and eax,0xfffffcff / or eax,0x3f / mov [0x915000],eax / fldcw [0x915000] / pop eax / ret`. It saves
and restores eax with no C frame, so no C source compiles to it, and an inline-asm body is barred from C4 by match.
Remaining route: libid, if the vendored RW library object holds it. No source written.

## Counts

- C4 by `c4=match`: 1 (0x004d4f00). Replay-verified (`c4=frida-replay`): 0.
- Score gains from source edits: 3 (0x004d4f00, 0x004cc820, 0x00534b60). Sources made to compile: 3.

## Files

- `re/promotions/RWC-MATCH.tsv`: the promotion row (dry run only on the branch).
- `verify/rwcmatch_20261112/scores.tsv`: before and after for every row worked.
- Edited sources: `re/match/s21/004d4f00.c`, `re/match/rwl01/004cc820.c`, `re/match/rwl03/00534b60.c`,
  `re/match/rwl03/004f1fb0.c`, `re/match/rwl03/0053dea0.c`, `re/match/rwl03/00538d60.c`. Each carries a header note.
