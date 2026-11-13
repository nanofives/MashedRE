# RESULT MATCH-RWT-S4 (2026-11-13)

Row: compiler shaping of ranks 9-11 of `verify/match_20261112/RANKED_MATCH-RWT-REFRESH.tsv`. Match-only: no build, no
gates. Branch `orch/MATCH-RWT-S4` from `orch/integration` @ e47e10795. Model: Opus 5.5.

**Verdict: GREEN.** 2 of 3 functions byte-identical under their recorded headers, relocheck OK, `c4_reclass.py batch`
dry run PASS 2/2. One semantic fix on the third (score drops, see below). Not naked and not asm-only: plain C, no `__asm`.

| RVA | source | listed | re-score | after | bytes | relocheck |
|---|---|---|---|---|---|---|
| 0x0053d5a0 | `re/match/rwl03/0053d5a0.c` (ddk) | 75.3 | 75.3 | **100.0** (ddk and rw9044) | 231 B, 0 differing | OK 3/0 |
| 0x004d0290 | `re/match/s20/004d0290.c` (TOOLCHAIN rw9044) | 73.4 | 79.8 | **100.0** (rw9044; ddk 74.2) | 656 B, 0 differing | OK 11/0 |
| 0x004ce970 | `re/match/s20/004ce970.c` (ddk) | 71.5 | 71.5 (wrong body) | 56.2 (semantically fixed) | 392 B vs 375 B, first differing byte +0xe | BAD (layout) |

Source SHA-256 (working tree, LF): 0053d5a0.c `5415a727c01e9b8eb8a3ea13f83136a71ba86272baa5bd6cdc30f14b14a20684`,
004d0290.c `3baa129800cebbb5e3c943c325bf7c8deff6333a3fbc68db2de7536911d7e6a9`,
004ce970.c `cb24dff57a88957697e21a5349a5cdd7f7c9791ba941f51ab420b2e1c9ff89a3`.

## 0x0053d5a0 (header `/O2 /Ob1 /G5`, tc ddk): 75.3% -> 100%

Residual before: ours kept the allocation result in eax for the whole body, the cursor in ecx, and loaded the zero
register before the NULL check (`xor edx,edx; cmp eax,edx`). The original tests `test eax,eax` at 0x0053d5f6, then
after the header copy moves the result to edx (`mov edx,eax` at 0x0053d640), creates the zero register
(`xor ecx,ecx` at 0x0053d642) and advances the cursor in eax (`add eax,0x28`, `and al,0xf0`, `and al,0xfe`).

Lever: hold the allocation in one byte-pointer variable `mem`, make the header pointer a copy (`h = (H *)mem`) and
advance `mem` past the 0x28-byte header (`mem += sizeof(H)`). c2 then coalesces the cursor with the call result.

| probe (`MATCH-RWT-S4_probes/`) | change | score |
|---|---|---|
| `d5a0_v1_base.c` | struct `H` for the header, `h + 1` cursor | 75.3 |
| `d5a0_v1_noret.c` | `!h`, `NULL` stores | 75.3 |
| `d5a0_v1_zlocal.c` | result copied to a second local for the stores | 75.3 |
| `d5a0_v2_hfirst.c` | `h` = call result, `mem = (uchar *)h` | 75.3 |
| `d5a0_v2_memvar.c` | `mem` = call result, `h = (H *)mem`, `mem += sizeof(H)` | **100.0** |
| `d5a0_v2_memvar_u.c` | same with `uint mem` | **100.0** |

The committed source keeps the `uint *` return and parameter types its callers declare (`re/match/rwl03/0053d820.c:40`,
`0053d920.c:46`), and adds `typedef struct H` (flags, S24, two ushort counts, two pointers, 0x28 bytes).

## 0x004d0290 (header TOOLCHAIN rw9044, `/O2 /Ob1 /G5`): 79.8% -> 100%

The re-score is 79.8, not the listed 73.4: the prologue residual named in the RWC-DRIVER note (ebp/edi pushes) is gone
under the current tool. Three source changes, all order-only (same stores, same calls):

| step | change | score |
|---|---|---|
| 0 | committed source | 79.8 |
| 1 | case 0/4: `privateFlags &= ~6` before `lockedMipLevel = 0xff` (original `and al,0xf9` at 0x004d033a reuses al from the +0x22 test, then `mov byte ptr [edi+0xb],0xff` at 0x004d033f) | 81.1 |
| 2 | cube branch: `tex = topExt->texture` inside each arm (original reads the cube byte at 0x004d03a5 before the texture at 0x004d03a9) | 82.8 |
| 3 | `pt.x = param_2->nOffsetX; pt.y = param_2->nOffsetY;` instead of `pt.x = rect.left; pt.y = rect.top` | **100.0** |

Step 3 stops c2 hoisting the `surface` load above the `top == param_2` test (original loads it after `cmp ebp,esi` at
0x004d03d4) and gives the original rect schedule from 0x004d041e. Other rect orders at step 3 (`d0290_v2_r*.c`):
right before top 83.7, x/y locals 85.0, every field re-read 85.0, pt first 82.4. The previous note's step (0xff store
before the flag clear, a permuter pick) is reverted by step 1. Under ddk the same body is 74.2 (ddk c2 sinks the
ebp/edi pushes past the early exit). The recorded toolchain is rw9044, as for 0x004cbbd0 in MATCH-RWT-S3.

## 0x004ce970 (header `/O2 /Ob1 /G5`, tc ddk): semantic fix, 71.5% -> 56.2%, RED

**Semantic bug fixed.** On the failure path (FUN_004cde50 or FUN_004cdf20 returns 0) the original destroys image
first, then mask: `test byte ptr [esi],1` at 0x004cea03 through the freelist free at 0x004cea42 on esi (image), then
`mov al,[edi]` at 0x004cea48 through 0x004cea8a on edi (mask). esi is the first FUN_004ce390 result (0x004ce97d),
edi the second (0x004ce9a6). The committed body destroyed mask first. That body was the permuter's pick and its 71.5%
measured a wrong body. Fixing the order drops the score to 56.2%. The rest of the body matches the original paths:
NULL image -> 0 (0x004ce986 -> 0x004cea93), no mask name -> image (0x004ce992 / 0x004ce99a -> 0x004ceae1), NULL mask
-> destroy image, 0 (0x004ce9af..0x004ce9df), success -> destroy mask, image (0x004cea99..0x004ceae1).

First divergent bytes (fixed source, `--bytes`): +0xe, the `mov` after the first call: original `8b f0`
(`mov esi,eax`, 0x004ce97d), ours `8b f8` (`mov edi,eax`). 392 B against 375 B, 137 against 127 instructions.

Residual: (a) register roles swapped (image edi / mask esi, original esi / edi). (b) The original cross-jumps the
NULL-mask path into the failure path's freelist-free tail (`push esi; jmp 0x004cea7b` at 0x004ce9de), ours emits a
full copy and its own epilogue. (c) Block order: the original falls through to the failure path after FUN_004cdf20 and
places the success path last, sharing `mov eax,esi` (0x004ceae1) with the no-mask-name return.

Levers tried (all with the destroy order fixed; scores ddk, rw9044 identical where run):

| probe file | change | score |
|---|---|---|
| `ce970_v1_fixed.c` (committed) | committed structure, order fixed | 56.2 |
| `ce970_v1_retimg.c` | single `return image` at the end | 56.2 |
| `ce970_v1_nomask_first.c` | `if (!mask) { destroy image; return 0; }` first | 50.4 |
| `ce970_v1_elseform.c` | if/else, `image = 0`, one return | 45.3 |
| `ce970_v1_elseret.c` | early `if (!image) return 0`, inverted test | 46.1 (rw9044 47.9) |
| `ce970_v2_g1..g6{im,mi}.c` | goto `ret0` single exit (g1), early-return chain (g2), mask-null first (g3), inverted test (g4), shared `return 0` (g5), merged outer condition (g6), each with both declaration orders | 41.6 .. 56.2, declaration order never changes the code |
| `ce970_v3_*.c` | destroy as `static __forceinline int ImgDestroy(RwImage *)` | same as the macro (41.6 .. 56.2) |
| `ce970_v4_*.c` | destroy as `__inline` | not inlined under /Ob1, 21.3 |
| `ce970_flags.log` | g2 / g3 under `/G6`, `/G4`, `/Ob2`, `/O1`, `/Oy-`, `/O2 /G5` on ddk and rw9044 | best 56.2 (g2 `/G4`, `/Ob2`, `/O2 /G5`); none swaps the registers |

Earlier rows' levers (in the source's old note): nested RW-style form 56.2, early-return form 41.6, if/else with a
shared victim free 44.5, static `__inline` 19.7, permuter 90 compiles. Stopped at the lead's time-box. Not tried: a
permuter run on the fixed body, scratch-rotation shifters (memory `c2-9044-scratch-register-rotation`) for the
cross-jump, a separate `RwImageDestroy`-shaped function body with a `RwBool` result used in a condition.

## Checks (run on the branch)

```
py -3.12 re/tools/match/match.py --bytes re/match/rwl03/0053d5a0.c
  0x0053d5a0 _FUN_0053d5a0: 100.0% (regs 100.0%, ops 100.0%)  85/85 ins  flags=/O2 /Ob1 /G5 tc=ddk
  bytes: 231 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 re/match/rwl03/0053d5a0.c
  0x0053d5a0 _FUN_0053d5a0: 100.0% (regs 100.0%, ops 100.0%)  85/85 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 231 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/rwl03/0053d5a0.c
  0x0053d5a0	OK	checked=3	unchecked=0	0053d5a0.c
py -3.12 re/tools/match/match.py --bytes re/match/s20/004d0290.c
  0x004d0290 _FUN_004d0290: 100.0% (regs 100.0%, ops 100.0%)  233/233 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 656 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s20/004d0290.c
  0x004d0290	OK	checked=11	unchecked=0	004d0290.c
py -3.12 re/tools/match/match.py --bytes re/match/s20/004ce970.c
  0x004ce970 _FUN_004ce970: 56.2% (regs 38.7%, ops 83.2%)  137/127 ins  flags=/O2 /Ob1 /G5 tc=ddk
  bytes: 392 B whole symbol, 266 differing unmasked bytes (first at +0xe)
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S4.tsv      (dry run, no --apply)
  [1/2] 0x0053d5a0 match PASS byte-identical 231 B, relocheck OK (3/0)
  [2/2] 0x004d0290 match PASS byte-identical 656 B, relocheck OK (11/0)
  batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
  dry run: nothing written.
```

VS2022 `cl /nologo /Zs /TP /W3 /DMASHED_STANDALONE /std:c++17 <TU>` in `mashedmod/src/mashed_re/RwCore/` (after
vcvars32), exit 0 for the three TUs that include the changed sources (`RwCore_0053d5a0.cpp`, `RwCore_004d0290.cpp`,
`RwCore_004ce970.cpp`) and for the five that call them (`RwCore_0053d820.cpp`, `RwCore_0053d920.cpp`,
`RwCore_004c6750.cpp`, `RwCore_004c8e50.cpp`, `RwCore_004d1020.cpp`). No build run (match-only row).

Port note: these bodies run on the RW core knob path as listed provisional bodies
(`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`, per the TU headers). Moving the two matched ones off that list, and
the changed 0x004ce970 body (now the original's destroy order), are port-axis facts not handled here.

Promotion TSV: `re/promotions/MATCH-RWT-S4.tsv` (2 rows, route match), not applied on the branch.

Probe files: `verify/match_20261113/MATCH-RWT-S4_probes/` (`vrun.py` variant runner, `vgen.py` replacement generator,
`*_s1.py` / `*_s2.py` / `ce970_gen*.py` specs, every probe `.c`, `*.show.txt` listings, `ce970_flags.log`,
`c4_reclass_dryrun.txt`).
