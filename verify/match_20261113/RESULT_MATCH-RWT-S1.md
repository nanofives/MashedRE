# RESULT MATCH-RWT-S1 (2026-11-13)

Row: compiler shaping for the top two rows of `verify/match_20261112/RANKED_MATCH-RWT-REFRESH.tsv`.
Match-only: no build, no gates. Compiler XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`, toolchain `ddk`).
Model: Opus 5.5, medium effort.

| RVA | before | after | flags | status |
|---|---|---|---|---|
| 0x004d40d0 | 89.1% (regs 97.8%) | **100%, BYTE-IDENTICAL** (159 B, 0 unmasked differing bytes), relocheck OK (checked=14, unchecked=0) | `/O2 /Ob1 /G5` | promotion TSV `re/promotions/MATCH-RWT-S1.tsv`, route match |
| 0x004cb4c0 | 90.2% | 90.2% (source kept, notes extended) | `/O2 /Ob1 /G5 /Oy-` | class (b), open |

Flags note: both sources keep their recorded RW-core flags (`/G5`), as do the 42 RW sources in `re/match` at 100%
in the 0x004c..0x004e range (29 with `/O2 /Ob1 /G5`, 13 with `/O2 /GS /Ob1 /G5`). The game-code set (`/O2 /GS /Ob1`, no
`/G5`) was measured on both and scores lower (004d40d0 80.4%, 004cb4c0 70.7% to 83.7%).

## 0x004d40d0: 100%

Two source moves, each measured with `re/tools/match/match.py`:

1. Result into a local, written back on zero: `int r = ...; if (!r) DAT_00911ac8 = r;`. The original's zero test
   is `cmp eax, edi` (edi = 0, `xor edi, edi` in the prologue). `== 0`, `!`, `0 ==`, pointer and unsigned char
   return types, `&&` forms and a zero local all emit `test eax, eax`. The write-back form emits `cmp eax, edi`:
   89.1% -> 91.3% (regs 100%, ops 100%).
2. Vtable read into a local before the call: `VisitorVtbl *t = job->visitor->vt; int r = t->fn4(job->visitor, &ctx);`.
   That moves the vtable to ecx (`mov ecx, [eax]` / `call [ecx+4]`) and the post-call load of job+0x14 to edx
   (`mov edx, [esi+0x14]` / `push edx`), as in the original. 91.3% -> 100%. A visitor local, a function-pointer
   local, a context-pointer local, statement reorders (132 combinations) and the bounded permuter (3231 compiles, walk
   400) stayed at 91.3% before this move.

Checks (worktree `orch/MATCH-RWT-S1`):
```
py -3.12 re/tools/match/match.py --bytes re/match/s20/004d40d0.c
  0x004d40d0 _FUN_004d40d0: 100.0% (regs 100.0%, ops 100.0%)  46/46 ins  flags=/O2 /Ob1 /G5 tc=ddk
  bytes: 159 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s20/004d40d0.c
  0x004d40d0  OK  checked=14  unchecked=0
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S1.tsv     (dry run)
  [1/1] 0x004d40d0 match PASS byte-identical 159 B, relocheck OK (14/0)
  batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
```
The body is plain C (no `__asm`, not naked). `--apply` not run on the branch (HB-TRACKER-AT-MERGE): the merge train
applies the TSV.

## 0x004cb4c0: 90.2%, class (b)

Residual in the cache-compare loop. Original (0x004cb4c0 listing via match.py): `mov eax, 0x618258` /
`mov ecx, edi` / `mov edx, 0x10` / `sub ecx, eax` / loop `mov esi, [ecx+eax]` / `cmp esi, [eax]` / `jne` /
`add eax, 4` / `dec edx` / `jne`. The kept source (index loop `for (i = 0; i < 16; i++)`) compares the pointer against
an end address instead of a down-counter.

Found: a down-counter form, `const int *p = DAT_00618258.m, *q = cache; int n = 16;
do { if (*q != *p) goto set; p++; q++; } while (--n);`, emits the original's loop instruction for instruction under
`/G6 /Oy-`, but the `/G6` scheduler then moves the prologue pushes, the cache-pointer load and one store (87.0%).
Under `/G5 /Oy-` the same source splits the compare into `mov ebx, [eax]` / `cmp esi, ebx` and pushes ebx
(83.5%, 97/92 ins). Every form tried keeps the split under `/G5`:

- 18 loop shapes (do/while/for, two pointers, index, counter + index, break forms, both compare orders), 7 compare
  expressions (`-`, `^`, char-offset pointer, `p[d]`, loaded local, float storage read as int), 5 goto loops, 5 more
  compare forms (embedded assignment, post-increment in the compare).
- 5 structural rewrites: no cache local, a `diff` flag variable, break + `n == 0`, `Mat16 *` cache, cache read after
  the twelve stores.
- volatile p or q, short and unsigned char counters.
- flags `/Oa`, `/Ow`, `/Os`, `/Gr`, `/G3`, `/G4`, `/Og`, `/Ob2`, and toolchain rw9044 (same results as ddk).
- bounded permuter on the counter form: 1295 compiles, no improvement.

Open lever for a next attempt: what keeps c2 from taking ebx for the split in the original (the original uses only
esi and edi as callee-saved registers in this function). Per the row's model rule this attempt ran at Opus medium,
and the next step up is Opus high.

## Files

- `re/match/s20/004d40d0.c`: 100% source and notes.
- `re/match/s20/004cb4c0.c`: notes only (the body is unchanged, still 90.2%).
- `re/promotions/MATCH-RWT-S1.tsv`: one row, 0x004d40d0, route match.
- Scratch variants (not committed): session scratchpad `gen40.py`, `gen42.py`, `gencb*.py`.
