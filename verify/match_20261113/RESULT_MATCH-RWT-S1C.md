# RESULT MATCH-RWT-S1C (2026-11-13)

Row: apply the MATCH-RWT-S1B lever (over-aligned global) to the siblings 0x004cb330 and 0x004d40d0. Match-only: no
build, no gates. Branch `orch/MATCH-RWT-S1C` from `orch/MATCH-RWT-S1B` @ 01d854f23. Model: Opus 5.5.

**Verdict: GREEN.** Both functions are byte-identical under their recorded headers (toolchain `ddk`, and also
`rw9044`), relocheck OK, `c4_reclass.py batch` dry run PASS 2/2. Not naked and not asm-only: plain C, no `__asm`.

| RVA | source | before | after | bytes | relocheck |
|---|---|---|---|---|---|
| 0x004cb330 | `re/match/rwl01/004cb330.c` | 64.9% | **100.0%** | 344 B, 0 differing | OK 16/0 |
| 0x004d40d0 | `re/match/s20/004d40d0.c` | 89.1% | **100.0%** | 159 B, 0 differing | OK 14/0 |

Source SHA-256: 004cb330.c `39a128d592462a878c3de651370490d2cd7c63a95f271bcca7eefe0f1b31e21f`,
004d40d0.c `9e5e8e020a70241de493900b749364eb8a69a286b292db60ebb4a7464c1f3df8`.

## 0x004cb330 (header `/O2 /Ob1 /G5 /Oy-`, tc ddk)

| step | change | score |
|---|---|---|
| 0 | committed source | 64.9% |
| 1 | S1B lever: `DAT_005d8b90` as `Mat16A` (`__declspec(align(8))`), probe `cb330_a8.c` | 85.1% (ops 99.1%) |
| 2 | one shared `return r >= 0;` after `if (param_2 != 0) {...} else {...}` instead of a return per arm (`v1.c`) | 99.1% |
| 3 | `s_cmp(unsigned int n, const int *a, const int *b)`, count first (`p_nab.c`, committed) | **100.0%** |

- Step 1 gives the ebp frame with no ebx and the counter spill at [ebp-4] (0x004cb353, 0x004cb363, 0x004cb36a).
- Step 2 residual it fixed: the original's param_2 arm branches `jne 0x4cb479` (the default arm's tail) on
  `param_1 != 0x100`, and its own copy stores `[0x7d4154] = 0` after `setge dl`. The default arm loads
  `edx = [0x7d4568]`, `eax = [0x7d3ff8]`, and `ecx` for the vtable. A return in each arm gave a separate tail and
  shifted these registers.
- Step 3 residual it fixed: `mov eax, esi` before `mov dword ptr [ebp-4], 0x10` (0x004cb33f..0x004cb344). With
  (a, b, n) the counter store came first.
- Control: the same body without the align (`p_nab_noalign.c`) gives 58.8%, so the lever is still required.
- Also 100%: `p_open.c` (open-coded down-counter loop with a `goto` in the param_2 arm). Not kept: `p_nab.c` keeps
  the helper.
- Other shapes tried at step 3 (all from `v1.c`): helper `*b != *a` 97.4%, swapped args 42.1%, `int n` 99.1%,
  do-while down-counter 99.1% (`*b != *a` order 97.4%), increment order b then a 99.1%, index loop 78.9%, cache in a
  local 99.1%, `__forceinline` 99.1%, not inline 59.6%, open-coded with `n` declared first 99.1%, open-coded `while`
  with q first 41.2%. Flags at step 2: ddk without `/Oy-` 58.8%, rw9044 with or without `/Oy-` 99.1%, `/G6 /Oy-` 79.1%.

## 0x004d40d0 (header `/O2 /Ob1 /G5`, tc ddk)

The S1B lever does not apply: the original has no frame here, and an aligned global would add one. Two source changes:

| step | change | score |
|---|---|---|
| 0 | committed source | 89.1% |
| 1 | `int r = fn4(...); if (r == 0) DAT_00911ac8 = r;` (`e_rstore.c`) | 91.3% (regs/ops 100%) |
| 2 | plus `Visitor *v = job->visitor; VisitorVtbl *t = v->vt; r = t->fn4(v, ...)` (`f_vtloc.c`, committed) | **100.0%** |

- Step 1 residual it fixed: the original's `cmp eax, edi` (edi = 0) after the call at 0x004d4124. Storing a literal
  0 gives `test eax, eax`. Storing `r` gives the zero-register compare.
- Step 2 residual it fixed: the scratch registers. Original vtable load `mov ecx, [eax]` / `call [ecx+4]`
  (0x004d411e) and arg `mov edx, [esi+0x14]` / `push edx` (0x004d4140), ours edx then eax. This is the c2 scratch
  rotation (memory `c2-9044-scratch-register-rotation`): the vtable in a local moves the phase by one.
- Control: step 2 with a literal-0 store (`f_vtloc_ctl.c`) gives 97.8%.
- Tried with no gain (all 89.1% unless given): `== job->state` 83.0%, `!fn4()`, a `z = 0` local for every zero,
  `== flush * 0`, `<= 0` 87.0%, unsigned / pointer / unprototyped / varargs / `__int64` / `char` return,
  `short` 84.8%, `0 == fn4()`, ternary store 49.0%, `&= 0`, store `job->state` 80.9%, `!= 0` with else,
  result local storing 0, goto form. On top of step 1 (91.3% each): visitor local only, ctx pointer local, visitor
  local declared early, owner local, `job->arg` local, `job->state = 0` moved after the visitor load. Context store
  order swapped: 82.6%. Flags on the pre-row source (`d_orig.c`): `/G3` and `/G4` 89.1% on both toolchains; `/Oy-`
  73.5%, `/G6` 80.4%, `/Os` and `/O1` 47.8% on both; plain `/O2 /Ob1` and `/O2 /GS /Ob1` 80.4% under ddk, 89.1%
  under rw9044.

## Checks (run on the branch)

```
py -3.12 re/tools/match/match.py --bytes re/match/rwl01/004cb330.c
  0x004cb330 _FUN_004cb330: 100.0% (regs 100.0%, ops 100.0%)  114/114 ins  flags=/O2 /Ob1 /G5 /Oy- tc=ddk
  bytes: 344 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 (probe p_nab.c, same body)
  bytes: 344 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/rwl01/004cb330.c
  0x004cb330	OK	checked=16	unchecked=0	004cb330.c
py -3.12 re/tools/match/match.py --bytes re/match/s20/004d40d0.c
  0x004d40d0 _FUN_004d40d0: 100.0% (regs 100.0%, ops 100.0%)  46/46 ins  flags=/O2 /Ob1 /G5 tc=ddk
  bytes: 159 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 (probe f_vtloc.c, same body)
  bytes: 159 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s20/004d40d0.c
  0x004d40d0	OK	checked=14	unchecked=0	004d40d0.c
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S1C.tsv      (dry run, no --apply)
  [1/2] 0x004cb330 match PASS byte-identical 344 B, relocheck OK (16/0)
  [2/2] 0x004d40d0 match PASS byte-identical 159 B, relocheck OK (14/0)
  batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
  dry run: nothing written.
```

- Port includes (MATCH_PORT, C++): `mashedmod/src/mashed_re/RwCore/RwCore_004cb330.cpp` and `RwCore_004d40d0.cpp`,
  VS2022 `cl /nologo /Zs /TP /W3 /DMASHED_STANDALONE /std:c++17` on each: no warning, exit 0. No build run.
- Both bodies are still listed as provisional for the RW core knob path (`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`
  lines 74 and 133). Moving them off that list is port-axis work and is not done here.
- Promotion TSV: `re/promotions/MATCH-RWT-S1C.tsv` (CRLF), not applied on the branch.

## Files

- `re/match/rwl01/004cb330.c`, `re/match/s20/004d40d0.c`: new bodies, a MATCH-RWT-S1C note above the preamble.
- `re/promotions/MATCH-RWT-S1C.tsv`.
- `verify/match_20261113/MATCH-RWT-S1C_probes/`: probe sources (264 KB, `.c` only).
