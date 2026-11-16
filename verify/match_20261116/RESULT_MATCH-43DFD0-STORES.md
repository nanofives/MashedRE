# RESULT MATCH-43DFD0-STORES (round 2, worker "stores")

Row MATCH-43DFD0-RAISE, round 2. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-STORES` from `orch/MATCH-43DFD0-FRAME` 0048e45cb. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Lines touched: the declarations (4 new locals after `local_20`), source lines ~1503-1615 (store block and switch 1), and the
header score on line 2. The dispatch worker's lines (register roles at 0x0043e02e / 0x0043e03b, the uVar10 tree at 0x0043e503) are untouched.

## Verdict

GREEN as a raise, not a match. No byte-identical claim, so no relocheck and no promotion TSV.

| Measure | before (0048e45cb) | after (this branch) |
|---|---|---|
| match.py whole function | 45.9% (regs 57.9%, ops 81.1%), 2811/2817 ins | **47.0%** (regs 58.3%, ops 81.7%), 2821/2817 ins |
| instructions equal (equal opcodes of match.py's own alignment) | 1294 | **1326** |
| `--bytes` | 11046 B, 6421 differing unmasked bytes, **first at +0x5f** | 11114 B, 6451 differing unmasked bytes, **first at +0x5f** |

The first differing byte is unchanged at +0x5f: the 0/1 register swap at 0x0043e02e / 0x0043e03b (dispatch worker's range).
The differing-byte count rises by 30 because our symbol grew by 68 bytes (offsets after the switch shift). Score and equal count
are the measures of fit.

## Levers kept (each measured on top of the one before)

| Step | score | equal |
|---|---|---|
| base 0048e45cb | 45.94% | 1294 |
| 1. switch 1 (line ~1520) case groups in the order 4, 0xf, 2, 8, 3/9, 0x15, 0/5, 1 | 46.45% | 1309 |
| 2. + loads of 0x007f0f44/48/4c/50 into locals `uS0..uS3` before the 0x1f/0x21 tests | 46.69% | 1318 |
| 3. + the three case-8 tests (0x1a, 0x13, 0x20) read `*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40)` in place of `iVar8` | **47.00%** | **1326** |

1. **Switch 1 order.** The original lays out the case blocks 4 `0x0043fcfc`, 0xf `0x0043fd0f`, 2 `0x0043fd1c`, 8 `0x0043fd3c`,
   3/9, 0x15, 0/5 (`0x0043fe7a`), with case 1 at `0x0043fc7e` and the join `0x0043fc89` ahead of them
   (slice 3's table). Slice 3 measured this order at -3 points before the frame fix. With the frame at 0x2c it now gains 0.51.
   Case 1 last (with default and the `switchD_0043f78b_caseD_6` label) scores 47.00%; case 1 first scores 46.94%; case 1 then
   0/5 first scores 46.51%. Our compile still puts [case 1, join, bVar3 block] after the cases, where the original puts them
   before (see "Tried, not kept").
2. **Store-block loads.** The original reads the four sources once, before the `0x1f` test and before the `0x67e840` / `0x67f1a0`
   stores: `mov ecx,[0x7f0f50]` 0x0043f6fd, `mov esi,[0x7f0f48]` 0x0043f703, `mov edi,[0x7f0f44]` 0x0043f709,
   `mov edx,[0x7f0f4c]` 0x0043f71d, then stores registers in both blocks (0x0043f73a..0x0043f74c, 0x0043f757..0x0043f769).
   Unconditional loads need the reads in the source. With the locals our build also holds `DAT_0067e9f8` in ebp
   (`mov ebp,[0x67e9f8]`, as 0x0043f6f7) and builds the switch index as `lea ecx,[ebp-2]` (as 0x0043f76f).
   Load order and declaration order of the locals change nothing (12 variants, identical code): our registers stay mirrored
   (ours 0x7f0f44 in ecx, 0x7f0f48 edx, 0x7f0f4c esi, 0x7f0f50 edi, original edi / esi / edx / ecx).
3. **Case 8 tests.** The original tests `eax` at 0x0043fd3c / 0x0043fd59 / 0x0043fd9a and falls through every test
   (`jne` to the next test), then reloads `mov ebp,[0x67e9f8]` at 0x0043fdab after the call and tests
   `cmp [ebp+0x67ed3c],0x1c` at 0x0043fdb7. With `iVar8` in the tests our c2 threaded the jumps (`jmp` past the remaining
   tests after a hit). With the memory expression c2 still keeps the value in eax but no longer threads: our case 8 is now
   the original's instruction sequence, ebp included. Alone (without step 2) this lever costs 0.12; on top of step 2 it gains 0.31.
   The same rewrite in cases 0/5 and 0x15 (`iVar8 == 8`, `iVar8 != 8`) produces identical code and is not applied.

## Tried, not kept (gain below cost)

| Lever | result on the kept state | note |
|---|---|---|
| **two separate store blocks for 0x1f / 0x21** (area a): both tests, or only the second, read the 0x0067ed3c word | 46.92% / 1325 equal (-0.08 / -1), bytes 6447 (-4) | Produces the original's shape exactly: `cmp eax,0x1f; ... jne; 4 stores; cmp eax,0x21; jne; 4 stores` (0x0043f71a..0x0043f769), same eax, no threading. Costs 0.08 because it adds 3 instructions while the function is already 4 over the original. Worth retrying once the dispatch worker's register swap lands. |
| loop count of the =6 loop in a register local (`iCnt = 0; if (DAT_0067ea98 != 0) iCnt = 1; ...; if (0 < iCnt) { local_28 = iCnt; ...`) | 47.00% (0), regs +0.11, ops +0.25, bytes +13 | Gives the original's form at 0x0043f5d6..0x0043f63e (`xor eax,eax` .. `je` / `mov eax,ecx` / `inc eax` x2 / `jle`, one `mov [esp+0x14],eax`) in place of our three `mov [esp+0x14],eax`. Score-neutral, so not kept. |
| switch 1 bodies out of line after the bVar3 block (`case n: goto SW1_n;`, bodies after `LAB_0043fc9a`'s block, `goto SW1_JOIN` for break) | 47.04% (+0.04), 1325 equal (-1), bytes +23 | Gotos everywhere for 0.04 points and one equal instruction less. |
| `if (bVar6 != 0)` / `if (bVar7 != 0)` at 0x00440457 / 0x004409c8 | identical code | see below |

## Flag tests at 0x00440457 / 0x004409c8 (area b)

The original tests `cmp dword ptr [esp+0x30], ebx` (0x00440457) and `cmp dword ptr [esp+0x34], ebx` (0x004409c8), ebx = 0.
Ours emits `mov eax,[esp+0x30]; test eax,eax` (and `[esp+0x34]`). The join has the same shape on both sides (the original has
30 jumps into 0x00440457 and 3 into the `call 0x42f6b0` tail at 0x00440452, ours 34 into the test and 3 into the same tail). Where our
build has its 0 register live at the test it uses the same `cmp [esp+n], reg` form (0x0043f6d4 `cmp [esp+0x20], edi`,
0x0043fed7 / 0x0043fef9 / 0x0043ff1f the same with edi). So these two follow the 0/1 register roles (ours 0 in edi, 1 in ebx;
the original 0 in ebx, 1 in edi), which is the dispatch worker's lever. [UNCERTAIN] that the register swap alone changes them:
no source edit tried here moves them.

## Region 0x0043f63e / 0x0043f6d4 (area b)

- 0x0043f63e: our three `mov [esp+0x14],eax` against the original's one are the count lever above (score-neutral, not kept).
- 0x0043f6d4: the join is preceded in the original by the `=6` loop case ending `push [0x67ea84]; push 0; call 0x46dc00` at
  0x0043f6be..0x0043f6cc and `or ebp,-1; xor ebx,ebx` at 0x0043f6cf. In ours the join sits at +0xdb7, after a different uVar10
  case (the 0x7f1a54 loop ending `mov [0x67f1a8],1`), and the `=6` case jumps back to it. Which uVar10 case falls into the
  join is set by the uVar10 decision-tree layout at 0x0043e503, the dispatch worker's range. Not edited.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` before and after (table above). Equal instructions counted
  from match.py's own `SequenceMatcher` opcodes (`score_function`), same count as the `=` rows of `--show` (1294 at the base).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022, syntax only, no object, no link) in
  `mashedmod/src/mashed_re/Match`: exit 0.
- Behaviour, by reading the source ([UNCERTAIN], one line in `re/orchestrator/UNCERTAIN_REGISTER.md`): every switch-1 case group
  still ends in `break` or `goto`, so the reorder adds no fallthrough (case 1 falls into `default:` as before); the four loads
  read plain globals that nothing writes between the read and the stores; `iVar8` keeps its assignment, and the case-8 expression
  equals it because nothing between writes 0x0067ed3c or 0x0067e9f8 (the stores in between go to 0x007f0eec..0x007f0f10). Test:
  `re/tools/match/equiv.py` on this source against 0048e45cb's, or the PROVISIONAL knob-path menu walk ON against OFF. Not run
  here (match-only row, L-168).
- Final source sha256 (LF working copy) `2cf73bddfc037d518915e76e627813b1e8eba8c3c9fd347a7efd1b5986bf3b15`.
- Scratch tools (session scratchpad, not committed): `t.py` (score, aligned and raw listings of ours with relocation names and of
  the original from `original/MASHED.exe.unpatched` via match.py), `exp.py` + `v1.py`..`v9.py` (variant runs), `apply.py`.

## Handoff to the merge of the round-2 branches

- This branch and the dispatch branch both edit `re/match/s08/0043dfd0.c` line 2 (the header score). Lines 1503-1615 are this
  branch's only body edits plus 4 declarations after `int local_20;`.
- After the register swap lands, retry the two-block store form (both 0x1f / 0x21 tests on the 0x0067ed3c word) and the `iCnt`
  count local: both reproduce the original's instruction sequence and were held back only by the whole-function score.
