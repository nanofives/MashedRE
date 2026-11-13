# RESULT RO-CTL-68BC0C (run 42, 2026-11-13): 0x0068bc0c is a frame-matrix pad word copied at boot, MASKED in prov_compare.py

Verdict: **MASKED.** Writer: the `rep movsd` at **0x0045a736** in FUN_0045a6b0, observed live (2 write hits from boot to race end,
pc 0x0045a736, main thread). With the mask, control `wss_ctl_g04` against r1/r2 is **IDENTICAL-FOR-N 2724 of 2724** (was DIVERGED
at tick 0 on block 0x0068bc00). r1/r2 are not re-recorded. Row kind: race-path diagnosis. No build, no G-CTRL, no B, no 12 captures,
no default-path change, no `hooks.csv` / `re/promotions` change.

## 1. What the two words are (static, cited)

- FUN_0045a6b0 is C4 by match (`hooks.csv`, source `re/match/s29/0045a6b0.c`). Line 73: `r->mat = *FUN_004c0ed0(atoms[n]->frame);`
  into the 0x68-byte records `DAT_0068bb60[4]`, field `mat` (16 dwords) at record +0x28 (struct at `0045a6b0.c:31-38`).
- Disassembly: `lea edi, [ebx + 0x28]` 0x0045a729, `mov ecx, 0x10` 0x0045a72c, `rep movsd` 0x0045a736, source `esi` = return of
  `call 0x4c0ed0` 0x0045a724. The record loop starts at `mov ebx, 0x68bd00` 0x0045a6dd, `sub ebx, 0x68` 0x0045a6e3 and ends at `cmp ebx, 0x68bb60` 0x0045a768.
- FUN_004c0ed0 (C4, plate `re/analysis/render_3_c1_to_c2_s6/FUN_004c0ed0.md`) returns param_1 + 0x50.
- Record 1 base = 0x0068bb60 + 0x68 = 0x0068bbc8. 0x0068bc0c = record +0x44 = `mat` dword 7 (byte +0x1c). 0x0068bc1c = record +0x54 =
  `mat` dword 11 (byte +0x2c). The neighbouring `mat` words hold the same values in every run: dword 0 / 5 / 10 = 0x3f800000
  (0x0068bbf0, 0x0068bc04, 0x0068bc18), dword 3 = 3 (0x0068bbfc).

## 2. Who writes it (live, `--watch-from install`)

New harness option (no default-path change): `detorig.py --watch-from install` arms the existing `--watch` debug-register write
watch at the install (boot mode: the PE entry point), re-arms it at the warp, and keeps the old arm at the race anchor
(`re/frida/js/detorig.js` watchArm / install / doWarp). The arm message now prints each watched dword's value.

| run | mode | 0x0068bc0c at ckpt 0..14 | write hits on 0x0068bc0c / 0x0068bc1c |
|---|---|---|---|
| `w1` | warp at the menu (PROV-INSITU config, `run_watch.sh`) | 0 | none from the menu to the end (both 0 at install, warp and anchor) |
| `b1` | boot (`--boot`, watch from the PE entry, `run_watch_boot.sh`) | 0 | **2 hits, pc 0x0045a736, tid = main thread, N = 177, bt `0x5f9aac\|0x45a736`, values 0x0 / 0x0** |

`b1` shows the copy is the only write to both words from boot to the race end, and that it runs before the menu (N = 177, warp at
N = 10136). The trap reports the `rep movsd` address itself (a rep string instruction traps per element with EIP still on it).
0x005f9aac in the backtrace is the type-table slot that points at FUN_0045a6b0 (`hooks.csv` note: slot 4 of 0x005f9998, +0x14 at 0x005f9aac).

## 3. Why it differs between runs (population, existing dumps)

`survey.py` / `survey.txt`: value of 0x0068bc0c at ckpt 00 in every run folder under `D:/MashedWT/*_data` (417 folders; 6 are
planted copies, 4 from §4 of this row and 2 from RO-BOOT-BLOCK-MASK, so 411 recorded runs).

- **4** in 160 runs, all in PROV-INSITU_root, recorded 2026-11-11 between 13:25 and 22:50 (r1 16:45, r2 16:46).
- **0** in 251 runs: every run before 13:24 (REPLAY-ORACLE-DESIGN, RO-WARP-NORMALIZE, PROMOTE-DETORIG-*), 4 PROV-INSITU runs inside
  the window (ref_a 13:24, x_ref / x_cnt / x_b1 14:05..14:14, same root, same config), and every run from 22:51 on (PROV-DIVERGED-18,
  PROV-WRITESET-114 / -SCENARIO, PROV-CRASH-12, PROV-FIX-9, RO-MENU-REPLAY, RO-MENU-DIV-43DFD0, RO-BOOT-CRASH-3, this row's w1 and b1).
- The PROV-INSITU root and this row's fresh root hold the same files (same SHA-256 for MASHED.exe b9977dab..., gamesave.bin bd187881...,
  videocfg.bin f87df3c8..., d3d9.dll, dinput8.dll; file lists equal apart from logs), so the root content does not select the value.
- `separ.py` / `separ.out` (195 runs of PROV-INSITU, PROV-DIVERGED-18, PROV-FIX-9; 160 holding 4, 35 holding 0): at ckpt 00, 07 and 14
  **no other .data dword** (0x005ea000.. dump) takes values disjoint between the two groups. Only 0x0068bc0c (4 / 0) and 0x0068bc1c
  (non-zero / zero) split them. In the 4 group 0x0068bc1c is a pointer-range value that differs between runs (r1 0x0f0e0038, r2
  0x0f0f0038). The value never changes after the copy (same at all 15 checkpoints) and no reader of record +0x44 / +0x54 was found at the
  five `imul ..., 0x68` + `0x68bb60` index sites in .text (`sweep68.py` run, 0x0045a7cf / 85a / 89a / 8bc / 91a, no +0x44 / +0x54 access).
- The menu clock at the warp does not select it either (`warpclk.out`: both values at every 0x007f0ff0 between 8500 and 9450).

So the two words carry copied matrix dwords 7 and 11 whose value is not a function of the scenario, the root or the harness
configuration. What makes them 4 / pointer in the 11-11 window is **[UNCERTAIN]**: no run since 22:51 on 11-11 has reproduced it, so
the source dword (frame +0x50 + 0x1c / + 0x2c of the cloned atom's frame) was not watched at a nonzero value. Missing evidence: one
run that produces 4, with a watch on the source frame matrix before 0x0045a736.

## 4. The mask (`re/frida/prov_compare.py`)

- RO-BOOT-BLOCK-MASK's single boot block is generalised to `VARIANT_BLOCKS = {0x00773c00: (BOOT_WORDS, strict), 0x0068bc00:
  (PAD_WORDS, pointer pairs equal)}`, `PAD_WORDS = {0x0068bc0c, 0x0068bc1c}`. When a variant block's hash differs, the two runs'
  checkpoint dumps must agree on every dword of the block outside its words (for 0x0068bc00, a pair of pointer-range dwords counts as
  equal, the rule the tick hash already applies: `detorig.js` tick hash, `detorig.is_ptr`), and the hash must move at the same ticks
  in both runs (unchanged rule). A raw-range run drops `VARIANT_WORDS` per dword. Notes now read "pad-variant block 0x0068bc00".
- Only the two observed words are masked. The other pad-position dwords of the four copied matrices (dwords 7 / 11 / 15 at
  0x0068bba4 / bbb4 / bbc4, bc0c / bc1c / bc2c, bc74 / bc84 / bc94, bcdc / bcec / bcfc) were 0 in every surveyed run and stay scored.
- BOOT-CLASS and the 0x00773c00 check are unchanged (`verify/roBootBlockMask_20261113/plant_test.py` re-run: PASS).

Proof (`plant_test.py`, `plant_test.out`, PASS):

| case | verdict | block 0x0068bc00 reported |
|---|---|---|
| ctl `wss_ctl_g04` vs r1/r2 | IDENTICAL-FOR-N 2724 | no ("15 checkpoints equal outside the 2 pad words") |
| ctl copy, 0x0068bc10 xored in all ckpts | DIVERGED 0 | **yes** ("ckpt00 dword 0x0068bc10 differs") |
| ctl copy, 0x0068bc04 (1.0f) xored | DIVERGED 0 | **yes** |
| ctl copy, 0x0068bc0c and 0x0068bc1c xored | IDENTICAL-FOR-N 2724 | no |
| r1 copy with the two words zeroed | IDENTICAL-FOR-N 2724 | no |

Regression (`regress.py`, `regress.out`): all 58 runs of PROV-WRITESET-SCENARIO, PROV-FIX-9, PROV-DIVERGED-18 and w1, old prov_compare
(HEAD) against new, both against r1/r2. All 58 were DIVERGED at tick 0 with 0x0068bc00 in the list before. After: 40 IDENTICAL-FOR-N
2724 (among them ctl, ref1..ref6 of PROV-FIX-9, w1), 13 DIVERGED at a later tick on other blocks (first divergence 1 to 2162), 5 still
DIVERGED at tick 0 on their other blocks (wsb_g05 / g06a / g06b on 0x00912800 alone, d18_r1 and wss_g04 on a 20+ block set; 0x0068bc00
is the only block removed from those lists). In 21 runs the old tick-0 list also held 0x007f1000: that block is now cleared by the
existing TIMER-PHASE rule (`loadlen_rule`), which applies only when it is the one bad block at tick 0, so it was blocked by 0x0068bc00
before. These verdicts against r1/r2 are information for the rows that own those runs, not scored here.

## 5. Consequence for the hold

ctl and today's fresh warp-at-menu run (w1, private root, current harness) both score IDENTICAL-FOR-N 2724 against r1/r2 with the mask,
so r1/r2 stay usable as the reference pair and new `c4=frida-replay` runs can be scored against them again. Re-recording is not needed
for this word.

## 6. Follow-up: read watch from boot to race end (run 42, the 0 case)

The lead dequeued the mask because §3's reader evidence was static only (`sweep68.py` over the five `imul ..., 0x68` sites), and a
pointer-based reader would not show there. This section records a live read-or-write watch.

Harness change (default-off): `detorig.py --watch-mode {w,rw}`, default `w` (old behaviour unchanged). With `rw` the two debug
registers are armed for read or write (x86 has no read-only data breakpoint). Hits are de-duplicated per pc: the first 4 per pc
are sent, then one line at each power-of-two count (`cnt`). The agent's own reads would trap inside Frida's guarded read, so the
`.data` checkpoint dump runs with this thread's two slots cleared and then re-armed (`watchQuiet` in `re/frida/js/detorig.js`).
Re-arming clears the slots before it reads the current values. The first attempt `brw1` stopped at the install for this reason
(`single-step triggered` from the arm message's own read, rc 6, no race).

Run `brw2`: the boot configuration of `b1` (`run_watch_boot.sh brw2 --watch-mode rw`, private root, under test-slot and
`mashed_lock.py run`). Armed at the PE entry 0x004a4bb7 on 6 threads (`meta.json` boot.install), re-armed at the warp (N = 10127)
and at the race anchor (N = 10238) on 34 threads. rc 0, 15 checkpoints, 3624 ticks (menu 0..899, race 900..3623).

**Value case: 0.** 0x0068bc0c = 0x0 and 0x0068bc1c = 0x0 at every arm message and in ckpt00 and ckpt14. The 4 / pointer case was not
produced (§3: no run since 2026-11-11 22:51 holds it).

Every hit (`brw2_watch.jsonl`, 16 lines):

| pc | hits | thread | what it is |
|---|---|---|---|
| 0x0045a736 | 2 (N = 164) | main 6772 | the `rep movsd` in FUN_0045a6b0 that writes `mat` (§1, §2). Backtrace `0x5f9aac\|0x45a736`, as in `b1`. A write, not a read |
| 0x04180259 | at least 4096 (count line 4096 at N = 22891) | main 6772 | outside MASHED.exe (image 0x00400000..0x00995000, SizeOfImage 0x595000). Every backtrace passes 0x00493485, the return address of `call 0x493390` at 0x00493480. 0x00493390 is the `--tick-va` the harness hooks (`detorig.py` default, `install` entry `['tick', '493390']`). So this pc is the harness's own per-tick block-hash CModule (`detorig.js` cfg.tick), which hashes block 0x0068bc00 once per tick. The count grows at 2 per tick (2048 crossed at N = 11617 ≈ tick 1090, 4096 at N = 22891) |

**No pc inside MASHED.exe read 0x0068bc0c or 0x0068bc1c from the PE entry to the race end.** There is no reader, so there is no
behaviour-changing reader. That includes a whole-matrix copy out of record +0x28: none was observed either. An x86 data breakpoint
fires on any access that overlaps the watched dword, so wider reads (x87, `rep movs`, 16-byte moves) over the matrix are covered.

Limits, stated as missing evidence:
- **[UNCERTAIN] the 4 / pointer case.** Read sites are code, and no reader executed with value 0. A reader behind a branch that tests
  one of these words or a neighbour would not run here. No run produced the nonzero value, so that case is not observed.
- Threads created after the install arm and before the warp (6 threads at the install, 34 at the warp) carried no watch from the
  install until the warp. The main thread was armed for the whole run. From the warp on, all 34 threads were armed.
- The process itself: `brw2` against `b1` (same boot configuration, write-only watch) is DIVERGED at tick 0 on 0x0067d800 and the timer
  block, with a BOOT-CLASS mismatch at 0x007f1030 (`prov_compare.py`: "verdict not body evidence"). Block 0x0068bc00 is not among the
  differing blocks at any tick. Boot-mode runs are not scored against r1/r2 (warp-at-menu configuration); `b1` and `brw2` are both
  DIVERGED at tick 0 there, on the same 559-block set.

Verdict for the mask: **no behaviour-changing reader in the 0 case.** The mask in §4 stands. r1/r2 are not re-recorded.

## Files

Committed (`verify/replayoracle_20261113/`): this RESULT, `run_watch.sh`, `run_watch_boot.sh`, `plant_test.py` / `.out`, `regress.py` /
`.out`, `survey.py` / `survey.txt`, `separ.py` / `separ.out`, `warpclk.py` / `warpclk.out`, `sweep68.py`, `raw_sha256_RO-CTL-68BC0C.txt`.
Code: `re/frida/prov_compare.py` (mask), `re/frida/detorig.py` + `re/frida/js/detorig.js` (`--watch-from`, and `--watch-mode rw`
from §6). §6 adds `brw2_watch.jsonl`. Raw: `D:/MashedWT/RO-CTL-68BC0C_data/brw2/` (watch.jsonl 1ef8da5a..., meta.json 6b845f03...,
ticks.bin d79df8c0..., ckpt00 366fe1ce..., ckpt14 093937ae...), `brw2.log` 331a6a73..., the failed attempt `brw1.log` 2ca0afc0....

Raw (not committed, SHA-256 in `raw_sha256_RO-CTL-68BC0C.txt`): `D:/MashedWT/RO-CTL-68BC0C_data/b1/watch.jsonl` 5c814135...,
`b1/meta.json` b1c449c6..., `b1.log` 5f4f27fd..., `w1/meta.json` fe410d8c..., `w1/ticks.bin` 0dd2d731..., `w1/ckpt00.data.bin`
8570cb97..., `w1.log` 6fc580ab..., `decomp_bb60.txt` 0bcb7b57...; references `PROV-INSITU_data/r1/ckpt00.data.bin` fc2931d8...,
`r2/ckpt00.data.bin` 026eae14..., `r1/ticks.bin` 64850d1a..., `r2/ticks.bin` fb219feb..., `PROV-WRITESET-SCENARIO_data/wss_ctl_g04/ckpt00.data.bin`
73452702..., `wss_ctl_g04/ticks.bin` 5118f084.... Planted copies: `D:/MashedWT/RO-CTL-68BC0C_data/plant/`. Private root `D:/MashedWT/RO-CTL-68BC0C_root`.
