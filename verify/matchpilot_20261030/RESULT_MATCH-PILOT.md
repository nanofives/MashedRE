# MATCH-PILOT: can Mashed's game code be proven by matching decompilation? (2026-10-30)

Branch `orch/MATCH-PILOT` (worktree `.worktrees/MATCH-PILOT`). Measurement only: nothing was promoted, no tracker,
ledger, `original/` or master-Ghidra write, no game run.

## Verdict

**Yes, and cl 13.00.9176 / c2 13.00.9178 (XP DDK) is sufficient to run a matching program. The retail 13.00.9466 is
not needed to start, and nothing in this sample requires it.**

- 43 of 50 stratified random game functions are byte-identical to `MASHED.exe.unpatched` (relocated operands masked,
  same-section jump-table targets compared). 9,754 of the 14,309 sampled bytes.
- The 7 functions left are 51.4-99.3%. 6 of them differ in 1 block or a few instructions of x87/store scheduling or
  operand order. No residual is a missing or extra optimization, a different idiom, a different frame or a
  different calling convention.
- Those 6 are the only place a 9178-vs-9466 back-end difference could hide. They are classed `a?`: the tried
  rewrites did not move them, but the same kinds of differences moved with source changes in other functions
  (see "Residuals"). Testing them needs 9466. It does not block anything.
- Control: the same 42 matched sources built with VS 2022 (cl 19.44.35207, `/O2 /GS- /arch:IA32`) give 4 at 100%,
  mean 46.6% (`control_vc19.tsv`). The period compiler is what makes this work.

## Toolchain identity (evidence)

| fact | evidence |
|---|---|
| Game objects: cl 13.00.9466, C 373 + C++ 18 | Rich header of `MASHED.exe.unpatched`: prodid 28 / 9466 x373, 29 / 9466 x18 |
| The DDK back end is itself in this exe | Rich header: prodid 28 / **9178** x8, 29 / **9178** x49. A plain DDK compile gets `@comp.id` 28/9178 (C) and 29/9178 (C++), the same ids (`match.py` objs, read 2026-10-30) |
| **Not built with /GL** | a DDK `/GL` obj linked `/LTCG` gets Rich prodid **43**/9178, a plain obj **28**/9178 (`gltest/`: source, commands and Rich dump of the 2 test images). MASHED.exe has no 43/44 entry at all |
| Linker | Rich prodid 61 / 9466 x1 (ours 61 / 9210); does not change function bytes |

The DDK has no 13.00.9466 (the 2026-09-09 spike's claim that it does is wrong: `cl` 13.00.9176, `c2` 13.00.9178,
`link` 7.00.9210).

## Flags (found from the code, verified by matched functions)

**`/O2 /GS /Ob1`**, C by default (`/Tc`), x87, no `/Op`. All 43 matched functions are 100% under this one set
(`pilot_collect.py --flags "/O2 /GS /Ob1"` = per-file flags, no score changes).

| flag | evidence |
|---|---|
| `/O2` (or `/Ox`, indistinguishable) | `/O1` and `/Os` break matched functions (`flagscan.py`: 0x00426d00 66.7%, 0x004298e0 21.1%, 0x00424920 17.8%); `/Og /Oi /Ot /Oy /Ob2` = `/O2` |
| frame pointer omitted (`/Oy`, part of /O2) | `/Oy-` breaks 7 of the 11 raw-matched functions (33.3-64.1%). 116 of 3,419 game functions have `push ebp; mov ebp,esp`; 40 game functions contain `and esp,-8/-16` (aligned frames) |
| `/GS` on | ~80 game functions load the cookie at `0x00616038` (written by the CRT init at `0x004a5825`); matched ones with buffers re-emit it (0x00499740 incl. the tail `jmp` to the check) |
| `/Ob1` | static same-file callees must stay calls: 0x00422470 23.2% under `/O2` alone, 100% with `/Ob1`; 0x00457100 73.2 -> 85.5%; 0x0045c820 (34 B, one caller) kept as a call in the original. `/Ob1` changes no other matched function |
| not `/G5` | `/G5` breaks 7 of the 11 raw-matched functions (14.3-99.2%). `/G6` = `/GB` = default. `/G7` does not exist in 9176 (D4002), and the code has inc/dec 4,173 vs add/sub 1 44 |
| not `/Op` | 0x00431b20 37.5% with `/Op` (adds a rounding store), 0x00457100 40% |
| x87, no SSE | 2,146 `fnstsw`, no `fcomi`, 12 scalar-SSE instructions in all game code |
| `/GX` (C++ EH) | 5 game functions have an `fs:[0]` frame. Not needed by any of the 50 (all C) |

`/Gy` is always passed so each function gets its own COMDAT; it does not change code.

## Results

| stratum | n | 100% | 80-99% | under 80% | raw converter at 100% |
|---|---|---|---|---|---|
| small (< 64 B) | 15 | 15 | 0 | 0 | 8 |
| medium (64-400 B) | 20 | 17 | 3 | 0 | 2 |
| large (401-2000 B) | 15 | 11 | 3 | 1 | 1 |
| **all** | **50** | **43 (86%)** | **6** | **1** | **11** |

By group (100% / n): frontend 9/9, physics 8/8, save 7/7, ai 5/7, audio 4/6, race 5/6, render 5/7.

Sample: `sample.tsv`, from `re/tools/match/pilot_select.py` (seed 20261030; round-robin over the 7 groups inside
each size band; the 3 spike functions forced; game objects only, 0x401000-0x49c000, because above that hooks.csv
labels RenderWare, RW audio and CRT functions with game subsystems; large capped at 2,000 B).

Per-function table (`results.tsv`; `raw` = the converter on Ghidra's output, before any hand work):

| rva | size | subsystem | stratum | raw % | best % | regs % | class | source |
|---|---|---|---|---|---|---|---|---|
| 0x00404e50 | 33 | save | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x00407a40 | 17 | ai | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x00422af0 | 21 | render | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x00426d00 | 22 | frontend | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x00426df0 | 7 | frontend | small | 50.0 | 100.0 | 100.0 | - | f_small_a.c |
| 0x00429840 | 24 | ai | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x00431b20 | 15 | audio | small | 10.0 | 100.0 | 100.0 | - | f_small_a.c |
| 0x0045c850 | 16 | gameplay | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x0045cc50 | 47 | audio | small | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x0046bce0 | 54 | gameplay | small | 86.7 | 100.0 | 100.0 | - | a1_0046bce0.c |
| 0x0046cbe0 | 44 | vehicle | small | 83.3 | 100.0 | 100.0 | - | f_small_a.c |
| 0x0046dbe0 | 17 | vehicle | small | 100.0 | 100.0 | 100.0 | - | f_0046dbe0.c |
| 0x00475ef0 | 49 | render | small | 75.0 | 100.0 | 100.0 | - | a1_00475ef0.c |
| 0x00482820 | 56 | vehicle | small | 76.0 | 100.0 | 100.0 | - | a2_00482820.c |
| 0x00498d20 | 53 | save | small | 87.5 | 100.0 | 100.0 | - | a2_00498d20.c |
| 0x00414f00 | 274 | ai | medium | 50.5 | 95.8 | 95.8 | a? | a1_00414f00.c |
| 0x00419090 | 96 | gameplay | medium | 57.9 | 100.0 | 100.0 | - | a6_00419090.c |
| 0x0041c380 | 130 | render | medium | 47.4 | 100.0 | 100.0 | - | a7_0041c380.c |
| 0x00422470 | 94 | frontend | medium | compile | 100.0 | 100.0 | - | m_00422470.c |
| 0x00422b50 | 75 | ai | medium | 25.8 | 100.0 | 100.0 | - | a5_00422b50.c |
| 0x00423e60 | 127 | frontend | medium | 50.0 | 100.0 | 100.0 | - | f_00423e60.c |
| 0x004298e0 | 226 | hud | medium | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x0043df00 | 204 | frontend | medium | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x0044c4f0 | 250 | render | medium | 23.3 | 98.8 | 98.8 | a? | a2_0044c4f0.c |
| 0x00456760 | 279 | vehicle | medium | 47.9 | 100.0 | 100.0 | - | a3_00456760.c |
| 0x00459400 | 128 | gameplay | medium | 20.3 | 100.0 | 100.0 | - | a7_00459400.c |
| 0x0045d250 | 220 | audio | medium | 65.0 | 100.0 | 100.0 | - | a4_0045d250.c |
| 0x004645c0 | 171 | audio | medium | 65.0 | 100.0 | 100.0 | - | a1_004645c0.c |
| 0x0046c5f0 | 210 | vehicle | medium | 50.0 | 100.0 | 100.0 | - | a5_0046c5f0.c |
| 0x0046d510 | 84 | ai | medium | 92.9 | 100.0 | 100.0 | - | a2_0046d510.c |
| 0x00475a00 | 91 | render | medium | 93.1 | 93.1 | 100.0 | a? | a3_00475a00.c |
| 0x00483ca0 | 102 | vehicle | medium | 29.8 | 100.0 | 100.0 | - | a4_00483ca0.c |
| 0x00498d60 | 219 | save | medium | 67.6 | 100.0 | 100.0 | - | a6_00498d60.c |
| 0x00499170 | 128 | save | medium | 92.2 | 100.0 | 100.0 | - | a5_00499170.c |
| 0x00499740 | 100 | save | medium | compile | 100.0 | 100.0 | - | a6_00499740.c |
| 0x00410510 | 812 | save | large | 37.6 | 100.0 | 100.0 | - | a5_00410510.c |
| 0x00423040 | 556 | frontend | large | 53.3 | 100.0 | 100.0 | - | a6_00423040.c |
| 0x00424920 | 608 | frontend | large | 100.0 | 100.0 | 100.0 | - | g2s_raw |
| 0x004260e0 | 525 | render | large | 10.3 | 100.0 | 100.0 | - | a7_004260e0.c |
| 0x0042b310 | 548 | frontend | large | 57.1 | 100.0 | 100.0 | - | a7_0042b310.c |
| 0x00430290 | 987 | save | large | 41.3 | 100.0 | 100.0 | - | a3_00430290.c |
| 0x00432080 | 417 | vehicle | large | 36.4 | 100.0 | 100.0 | - | a1_00432080.c |
| 0x00457100 | 1285 | gameplay | large | 17.8 | 97.7 | 97.7 | a?/b | a2_00457100.c |
| 0x0045cd30 | 563 | audio | large | 29.1 | 51.4 | 62.6 | b | a6_0045cd30.c |
| 0x0045e990 | 1610 | audio | large | 44.6 | 99.3 | 99.3 | a? | a1_0045e990.c |
| 0x004715a0 | 456 | ai | large | 46.3 | 100.0 | 100.0 | - | a3_004715a0.c |
| 0x004783f0 | 482 | ai | large | 19.3 | 90.8 | 90.8 | a?/c | a4_004783f0.c |
| 0x00478cf0 | 824 | track | large | 69.7 | 100.0 | 100.0 | - | a4_00478cf0.c |
| 0x0047f4c0 | 523 | physics | large | 58.0 | 100.0 | 100.0 | - | a5_0047f4c0.c |
| 0x00493710 | 430 | render | large | 68.3 | 100.0 | 100.0 | - | a2_00493710.c |

### Residuals (the 7 below 100%)

- (a?) compiler-version candidates, 6 functions: 0x00414f00 (`fld b; fsubr a` vs `fld a; fsub b` in 2 of 3 vector
  parts), 0x0045e990 (integer loads/stores inside the `fsub`->`fstp` window, 2 spots), 0x00475a00 (SIB base/index
  order `[edi+edx]` vs `[edx+edi]` in 2 stores, registers identical), 0x0044c4f0 (1 store after 2 pushes),
  0x00457100 (one block: operand order of local*global multiplies + push/fstp interleave), 0x004783f0 (one `fld`
  ahead of `lea/lea/push`). Each survived 4-20 hand rewrites and 126-580 permuter compiles.
  Against reading them as 9466 evidence: in this same pilot, block layout (0x004260e0 98.1%, 0x0042b310 95.6%),
  register allocation (0x00478cf0), x87 operand order (0x0044c4f0 92.7 -> 98.8% by literal vs named constants) and
  scheduling around an unrelated store (0x004783f0) all moved with source. These 6 are where to test 9466 if it is
  ever obtained.
- (b) source shape, 1 function: 0x0045cd30 51.4% (a pointer kept in EDX across the call to 0x0045c820 not
  reproduced; loop induction, frame size 0x14 vs 4 and switch-block placement open).
- (c) tool limits, all solved or worked around in this pilot (none left in the table):
  - Ghidra's body size cuts 4 sampled functions short (epilogue after a jump table: 0x00410510; alignment padding
    inside the body: 0x004715a0, 0x00475ef0; 0x00478cf0). `match.py --bytes` now compares the whole obj symbol
    byte for byte, so these count as matched only when every unmasked byte is equal over at least the Ghidra size.
  - `LANG c++` regex bug (helper a6) fixed.

### Findings that change how to match this game

1. **Register-passed arguments are static functions, not /GL.** Ghidra's `in_EAX`, `unaff_ESI`, `unaff_EBX`,
   `extraout_ECX/EDX` (8 of the 50) all came out of plain 9176 once the callee is `static`, defined before its
   caller in the same file, its address not taken, with `/Ob1`. The parameter order picks the registers (0x00422470:
   `(i, seed)` gives EAX/ECX, `(seed, i)` does not). 0x00422470, 0x0041c380, 0x00499170, 0x0046c5f0, 0x00419090,
   0x00498d60, 0x00498d20 and 0x00430290 went from 41-96% (raw or first hand pass) to 100% this way. That is also a static witness of which
   functions shared a source file.
2. A load of a global scheduled before stores through a pointer means the global is file-`static` (0x0041c380:
   `static int DAT_0063cde4`; its 3 .text references all sit in this file's functions).
3. Ghidra's callee prototypes are often wrong in arity (0x004b3f90 takes 1 argument, not 7; 0x005c9d00 is a shared
   `xor eax,eax; ret`). Fixing them fixed the deferred `add esp` merges.
4. The raw converter got 11/50 at 100%. Hand work was 15-40 min per function with the permuter for the last steps.
5. `acos` is the CRT stack-argument entry (0x004a3384), not the `_CIacos` intrinsic 9176 emits for `double acos(double)`.
   Declare it as a plain function.

### Caveats

- 0x00419090 and 0x00499170 were matched with a small stand-in caller in the file (enough to make the compiler pick
  the register convention). The target's bytes are identical, but the file is not the real TU. 0x00457100's best
  file is the permuter's shuffled order (natural order 97.1%).
- `/Ob1` vs `/Ob2` with source-level blocking cannot be told apart from bytes alone. `/Ob1` is the single flag set
  that fits all 43.
- Bonus matches outside the sample (not counted): 0x004223f0, 0x0042a920, 0x0041cb80 at 100%. 0x00498a00 is 96.7%,
  epilogue past Ghidra's size.

## Tools (re/tools/match/, adapted from SimGolf re/tools/, SimGolf untouched)

| file | what |
|---|---|
| `match.py` | compile with `ddk` (tools/vc7ddk/bin of the main checkout) or `vc19`, compare per instruction (+ `regs` / `ops` scores) and `--bytes` whole-symbol |
| `g2s.py` | decomp_pc.py `--port` JSON -> compilable C (typedefs, extern globals, callee conventions from each callee's `ret N`, /GS cookie lines dropped, `X._A_B_` partials, compile-error fixes) |
| `match_permute.py` | SimGolf's permuter with a `--time` budget |
| `flagscan.py`, `codestats.py`, `odis.py` | flag sweep, code-shape counts, original disassembly |
| `pilot_select.py`, `pilot_collect.py` | sample and results |

Sibling note: branch `orch/LIB-IDENTITY` ports the same SimGolf tools for library code. If it also created
`re/tools/match/match.py` or `match_permute.py`, the two need a merge by hand: this `match.py` has `--bytes`, the
`ddk`/`vc19` toolchains and the worktree-to-main path fallback. Not coordinated live, as instructed.

Sources: `re/match/pilot/*.c` (one per function, header comment = best score, what was tried, class); helper notes
`notes/a1..a7.md`; raw converter output `g2s_raw/`.

## What next (for the user to decide; nothing done)

1. A matching lane is viable at about 86% per function with this compiler. C4 by match was approved; promotion
   needs the re-classify path and the rule set for stand-in callers (caveat above) decided first.
2. The 6 `a?` functions are the 9466 test set if the retail compiler is ever obtained.
3. Feed back finding 1 (static callees) and the arity fixes into `g2s.py` before any batch.
