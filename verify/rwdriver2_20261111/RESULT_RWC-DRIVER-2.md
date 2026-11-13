# RESULT RWC-DRIVER-2 (L-124 C follow-up): RW D3D9 driver near misses under rw9044

Row: RWC-DRIVER-2, branch `orch/RWC-DRIVER-2` (from `orch/integration` @ bf44aae7d), L2 worker, 2026-11-11.
Kind: match-only (WORKER_RULES "Match-only, tracker and text rows"): no build, no gates. Prior row:
`verify/rwdriver_20261111/RESULT_RWC-DRIVER.md`.

## Verdict

- **3 functions matched and promoted to C4 by match** under TOOLCHAIN `rw9044` (VC6 SP5 cl 12.00.8804 + Processor
  Pack c2 13.00.9044, L-131 / L-133), `c4_reclass.py batch --apply`, all PASS (byte-identical + relocheck OK, no
  naked asm, no asm-only body): **0x004d1640** (1,760 B symbol), **0x004c9eb0** (147 B), **0x004d2980** (758 B).
- **0x004d7480** is 100% under rw9044 with its existing source and was already C4 before this row (commit
  9e36314ac, MATCH-RW-COMPILER-9044). Not re-promoted.
- **0x004c7a70** `_rwDeviceSystemFn`: 93.2% -> **97.0%** (ops 100%, 821/821 ins). Not matched, residual is one
  jump-target choice (below).
- **0x004d1e30** 99.1% and **0x004db3e0** 95.2%: unchanged.
- **0x005405c0** (3,339 B, 23.8%): not attempted again (budget, see Follow-ups).
- No equiv.py runs: RWC-DRIVER showed every COM-calling function of this driver is NOT-PROVEN `ret-esp` under
  equiv.py rule A4 (stdcall vtable calls), so no admission is possible without the rule change filed there.

Driver count, recomputed on this branch from `verify/rwsizing_20261111/RW_FUNCS.tsv` (`d3d_com` set, phase not
`not-run`) against hooks.csv: 57 rows, minus the 8 RWC-DRIVER `NOROW` functions (all still below C4) = 49 (RWC-DRIVER
wrote 48 for the same filter; the one-row difference is not resolved here). Of the 49: **35 C4 before this row's
apply** (RWC-DRIVER's 31 plus 0x004d7480, 0x004cb8a0 and others promoted by MATCH-RW-COMPILER-9044), **38 after**.
Still below C4 (11): 0x004c7a70, 0x004cb330, 0x004cb4c0, 0x004cff20, 0x004d0290, 0x004d1e30, 0x004db3e0, 0x004dba60,
0x004e0920, 0x005405c0, 0x005412d0.

## Step 1: re-score under rw9044 (sources as on `orch/integration`, before any edit)

Command: `py -3.12 re/tools/match/match.py <src> --toolchain {ddk,rw9044} --bytes`.

| rva | source | ddk | rw9044 |
|---|---|---|---|
| 0x004d1640 | re/match/s20/004d1640.c | 99.8% | 99.8% |
| 0x004d1e30 | re/match/s20/004d1e30.c | 99.1% | 99.1% |
| 0x004d2980 | re/match/rwl01/004d2980.c | 97.1% | 97.1% |
| 0x004d7480 | re/match/s21/004d7480.c | 96.2% | **100% BYTE-IDENTICAL** (file already `TOOLCHAIN rw9044`, C4) |
| 0x004db3e0 | re/match/rwl01/004db3e0.c | 95.2% | 95.2% |
| 0x004c9eb0 | re/match/s20/004c9eb0.c | 94.8% | 94.8% |
| 0x004c7a70 | re/match/rwd01/004c7a70.c | 93.2% | 93.1% |
| 0x005405c0 | re/match/rwl03/005405c0.c | 23.8% | 23.8% |

So the existing sources, shaped against the DDK, gained nothing from the compiler switch alone. The gains below
came from **undoing DDK-specific workarounds**: the natural forms the DDK punished are the ones 9044 matches.

Flag sweep under rw9044 on the five unmatched sources (`--flags`, run with `MSYS_NO_PATHCONV=1` so Git Bash does not
rewrite `/Ob2` into a path; this is why RWC-DRIVER saw "a --flags override did not take effect"): `/O2 /Ob1 /G5`,
`/O2 /Ob2 /G5`, `/O2 /G5`, `/O2 /Ob1`, `/Ox /Ob1 /G5` all give the same score; `/G6`, `/Oy-`, `/O1` are worse on all
five. So /Ob2 is measured now and changes nothing here. 0x004db3e0 also tried `/Ogisyb1`, `/Os`, `/G3`, `/G4`, `/GB`,
`/Oa`, `/Ow`, `/Op`, `/GF`: none better.

## Step 2: per function

| rva | size | start | end | status | what moved it / residual |
|---|---|---|---|---|---|
| 0x004d1640 | 1734 | 99.8% | **100%** | **C4 match** | the 16-bit early return in type 5 as the plain `format \|= 0x200; break;`. The DDK cross-jumps that block into the 0x100 path's `or esi, 0x200` tail (91.7%), so RWC-DRIVER wrote `format += 0x200` (99.8%, `add` for `or`). 9044 keeps the block with its own tail. `format = format \| 0x200` is also 100%. |
| 0x004c9eb0 | 146 | 94.8% | **100%** | **C4 match** | `best = 0` inside `if (param_1 != 0)` (the DDK scored this form 91.4%). 9044 pushes ebx before the branch and zeroes it after push esi/edi, as the original. Early return, a block-scoped `best`, `i = 0; best = 0` also give 100%. |
| 0x004d2980 | 757 | 97.1% | **100%** | **C4 match** | the two RWERROR sites written out, each `err.pluginID = 1; err.errorCode = FUN_004d7ff0(2, <msg>); FUN_004d8480(&err); return 0;`. 9044 cross-jumps the common tail after the per-site `push <msg>`, as the original (0x004d29cd / 0x004d2bfa). The DDK compiles the same text to 82.1% (first site shrink-wrapped), hence RWC-DRIVER's goto + `msg` local (97.1%). Scoped per-site `RwError` locals: 81.8%. |
| 0x004c7a70 | 2810 | 93.2% | **97.0%** | provisional candidate | see below |
| 0x004d1e30 | 1269 | 99.1% | 99.1% | provisional candidate | case 5 schedule unchanged: the original copies the format to edi between the argument pushes and runs `not / shr 0xf / and 1` on edi after them, ours on edx before. 9044 tried: `int` levels (gives `sar`), `unsigned long`, mask-then-shift, xor-mask (88.4%), copy then in-place ops, a format local, `~` first, a usage temp first, ternary / `!` forms (88.4%), RW `RwRasterGetFormat` macro with `& 0xff` (88.0%), the expression at each call through a macro (61.2% / 70.5%), permuter 431 compiles: 0 gains. |
| 0x004db3e0 | 367 | 95.2% | 95.2% | provisional candidate | in the failed-IB path, the original's DAT_007d70a8 == NULL branch (`je 0x004db4f6`) jumps to the **success path's** `mov eax, esi` return tail, and the DAT_007d70a8 release block has its own tail copy with `mov eax, esi` scheduled before the store; the two Release calls use edx then ecx. Ours: `return 0` there gives `xor eax, eax` (95.2%); without it (93.3%) the NULL branch goes to the later copy. 9044 tried: `return uVar3` (constant-folded), `if (!uVar3)` / `== 0` after the success block, the cleanup as `__inline` / `__forceinline` / plain static helper under /Ob1 and /Ob2 (93.3%), helper plus `return 0` / `return uVar3` (56.2%), fail branch first (72.4%, 55.2%), `return uVar3` in the success branch (56.2%), permuter 245 compiles: 0 gains. |
| 0x005405c0 | 3339 | 23.8% | 23.8% | not attempted (budget) | raw g2s source, unchanged |

### 0x004c7a70 (93.2% -> 97.0%)

Edits, all under rw9044:
1. `in >= 0` in case 6 (the DDK-only `in > -1` size trick is gone): 93.1 -> 93.8.
2. Case 0x11 MXCSR update as a 5-instruction inline `__asm` block (`stmxcsr csr / mov eax, csr / or eax, 0x8000 /
   mov csr, eax / ldmxcsr csr`): the original has `or eax, 0x8000` (0x004c8440 area) and every intrinsic form gives
   `or ah, 0x80` under both compilers. The body stays compiled C (re/CONFIDENCE.md C4 gate 3: inline `__asm` inside
   real C work is allowed; c4_reclass.py `asm_verdict` accepts it). 93.8 -> 93.9.
3. Case 0 returns early when FUN_004dd140 (Direct3DCreate9 thunk) gives NULL: 9044 then emits that return's own
   epilogue right after the test, as the original at 0x004c7c95..0x004c7ca2.
4. Case 7 written in the switch (`if (DAT_007d4120 == 0 && in >= 0 && in < DAT_007d4128) { ...; return 1; }
   break;`) instead of the `D3D9UseMode` helper. With the helper, 9044 returned its value through a stack temp and
   added an epilogue (`mov eax, [esp+0xc]`). With 3 + 4: 97.0%, ops 100%, 821/821.

Residual (only jump displacements): case 7's two multisample-failure branches (`jl` / `jbe` after the vtable +0x2c
call with type 1) go to **case 16's** copy of the `mov eax, edi` return tail in ours (near jumps, 8 bytes more than
two short jumps); the original sends them to case 7's own copy at 0x004c7bcd and sends case 16's branches there as
well. Every later target is then 8 bytes off. Tried: case 16 placed first / right after case 7 / last (79.8-81.7%),
case 16 as an int helper, MultisampleLevels expanded in either case or returning int, a goto from case 16 into case 7
(85.0%, drops case 16's copy), all helpers plain `static` under /Ob2 or `__inline` (97.0%), permuter 319 compiles:
0 gains.

0x004db3e0 shows the same pattern: where a return tail is duplicated, the original branches to the **earlier** copy
and 9044 with our sources to the **later** one. [UNCERTAIN] whether this is a source-shape difference or a compiler
difference (another 9044-era c2 build or a front-end difference: the Rich prodid 0x30 / build 9044 entries name the
c2 build only); no source form tried here flips it.

## Port side (no build run, match-only row)

`mashedmod/exe_sources.rsp` compiles 0x004c7a70, 0x004c9eb0 and 0x004d2980 verbatim as C++ in
`mashedmod/src/mashed_re/RwCore/RwCore_<rva>.cpp` (provisional bodies, knob path `MASHED_ORIG_TOP=1` +
`MASHED_RWC_CORE=1` only). A compile-only check of those three TUs with VS2022 (`cl /EHa /W3 /O2 /DMASHED_STANDALONE
/c`) passes, `__asm` block included. No TU references the removed `D3D9UseMode` helper. 0x004d1640 has no RwCore TU.
The object build (`ddkobj.py`) reads each source's TOOLCHAIN line (`compile_obj(..., info["toolchain"])`), so it
compiles the three new 100% sources with 9044. Not run here: the merge train or a top-down row runs both builds and
`ddkobj.py check`. The PROVISIONAL_RWC.tsv entries for 0x004c9eb0 / 0x004d2980 can now move to the matched set; that
is a port-row edit, not done here.

## Files

- Changed sources: `re/match/s20/004d1640.c`, `re/match/s20/004c9eb0.c`, `re/match/rwl01/004d2980.c` (each now
  `// TOOLCHAIN rw9044`, 100%), `re/match/rwd01/004c7a70.c` (`// TOOLCHAIN rw9044`, 97.0%, unused `D3D9UseMode`
  helper and the dead trailing `return 0` of `D3D9Open` removed). Each carries an `RWC-DRIVER-2` comment.
- Evidence: `c4_batch.tsv` (batch input), `c4_dryrun.tsv` (dry-run report), `c4_apply.tsv` (apply report).
- Trackers: hooks.csv + re/analysis/CHANGELOG.md through `c4_reclass.py batch --apply` (3 rows).

## Follow-ups

- 0x004c7a70 and 0x004db3e0: the duplicated-tail branch choice above. A next try could reproduce it on a two-function
  toy (two identical `return` tails) to tell source shape from compiler build.
- 0x005405c0 (3,339 B, raw g2s 23.8%): needs a full rewrite from the disassembly. Write it under rw9044 from the start.
- Lever for every RW near miss shaped against the DDK: re-try the forms an earlier row rejected under the DDK (goto
  shared tails, `+=` for `|=`, `> -1` for `>= 0`, hoisted initialisations, merged error sites). Candidates from
  RWC-DRIVER's table: 0x004cb4c0 90.2, 0x004dba60 87.6, 0x004d0290 76.4, 0x004cb8a0 71.3, 0x004cb330 64.9,
  0x004cff20 62.5, 0x004e0920 46.3 (0x005412d0 stays: `fistp` from RW inline asm).
- equiv.py callee-pops extension for stdcall vtable calls (RWC-DRIVER's decision-row candidate) still blocks any
  admission of the unmatched driver sources.
