# RESULT PORT-COLOURSEL-ICON-FLIP (2026-11-10, L-112 (a))

Branch `orch/PORT-COLOURSEL-ICON-FLIP` from `orch/integration` 086427678. Flip commit 030531b08.

## 1. Answer

**GREEN.** `MASHED_COLOURSEL_ICON` and `MASHED_COLOURSEL_BORDER` are default ON together. Opt-outs are
`MASHED_NO_COLOURSEL_ICON=1` and `MASHED_NO_COLOURSEL_BORDER=1`, the same pattern as the BAR_Y flip
(`ColourSelBarY_SingleRow`: getenv of the `MASHED_NO_...` name, `return !(e && e[0] == '1')`). The default gates are
unchanged. In the 12 captures only walk_04 moves, 4d53d93f87f0332d (OFF) to ba34030d1b7e2fe4 (ON), in both pairs.

## 2. Change (030531b08)

- `Frontend/MenuNavSM.cpp` `ColourSelIcon_On()` reads `MASHED_NO_COLOURSEL_ICON`, `ColourSelBorder_On()` reads
  `MASHED_NO_COLOURSEL_BORDER`, both `return !(e && e[0] == '1')`. Their comments and the `MenuNavSM.h` declarations
  say "default ON since L-112 (a)". No call site changes. ICON gates the icon read of 0x0067f0c0, the left/right
  choice (0x00433486 / 0x004334bb), the confirm commit to DAT_007f1a1c (0x0043e56c..0x0043e57e) plus
  FUN_00431d00 (0x0043e58e) and DAT_0067ea78 = 0 (0x0043e593), the slot 0 livery and the challenge opponent colours.
  BORDER gates the five-rect single-row bar.
- Stacks: new `verify/savelink_20261023/stacks/NOCSIX.txt` = the two opt-outs. `CSI.txt`, `CSIB.txt`, `CSIX.txt`
  carry a comment line saying their `=1` lines are now no-ops. No `STACK` line in `run_arms.py`.
- `verify/porttop_20261108/capflake_runs.py`: walk_04 reference 4d53d93f87f0332d to ba34030d1b7e2fe4 (section 3).
- Driver `verify/portstate_20261110/coloursel_flip/run_flip.py`.

Build a26a4c1b (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint `NEW=0` (10 known, allowlisted), asm-lint OK.

## 3. Evidence

Private root `D:/MashedWT/PORT-COLOURSEL-ICON-FLIP_root`, exe = the a26a4c1b copy, one
`test-slot bash -c 'py -3.12 .../mashed_lock.py run --label PORT-COLOURSEL-ICON-FLIP -- py -3.12 run_flip.py'`.

Gates (`run_arms.py`, A4_OUT/A4_ROOT/A4_EXE set):

| arm | hashes |
|---|---|
| H_DEF (flip ON, the new default) | `8284be46 / 67931834 / 31fe043e` |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| H_NOCSIX (opt-out) | `8284be46 / 67931834 / 31fe043e` |

Equal to `merge_train.json` gctrl.ref / b.ref.

12 captures, `capflake_runs.py <arm> OFF@0` (nav demo, `MASHED_DETERMINISTIC=1`, `MASHED_DET_MPG_PIN=0`, 04E settings
baseline, `MASHED_ORIG_TOP` unset; the script's ON/OFF spec toggles ORIG_TOP, so the colour-select arms are set by
env: OFF = `MASHED_NO_COLOURSEL_ICON=1 MASHED_NO_COLOURSEL_BORDER=1`, ON = nothing). All four runs freeze
`pinned pos=55 hash=28538c74`. Table `coloursel_flip/caps_runs.tsv`.

| run | side | class | walk_04 | other 11 |
|---|---|---|---|---|
| c1_off | OFF | E (12/12) | 4d53d93f87f0332d | = E |
| c2_on | ON | 11/12 | **ba34030d1b7e2fe4** | = E |
| c3_off | OFF | E (12/12) | 4d53d93f87f0332d | = E |
| c4_on | ON | 11/12 | **ba34030d1b7e2fe4** | = E |

Only walk_04 differs. The ON hash equals RESULT_PORT-COLOURSEL-LEFTRIGHT section 5 "ICON + BORDER" (ba34030d1b7e2fe4,
border rows 207 208 231 232, columns 46 47 582 583 = the original's). capflake_runs.py E now carries it, so default
runs classify E again.

## 4. Not changed

- What RESULT_PORT-COLOURSEL-OPPCOL section 6 lists stays unported (other challenge-launch stores, count 2..3 rows, the
  non-cup 0x0042b9e0 team branch). The flip makes the ICON law the default, it adds no new law.
- No tracker edits. No ROADMAP / NEXT_SESSION edits.

## 5. Commits

| commit | content |
|---|---|
| 030531b08 | MenuNavSM.cpp/.h flip, stacks NOCSIX + comments, run_flip.py |
| this commit | capflake_runs.py walk_04 reference, caps_runs.tsv, this RESULT |

## 6. Raw files (`D:/MashedWT/PORT-COLOURSEL-ICON-FLIP_data`, not committed)

| file | sha256 |
|---|---|
| exe/mashed_re.exe | a26a4c1bdebfbef2293be216037b03f13dca4292e2c2004b3179ef48a93dcf74 |
| build.log | a74102a6dfab8b90cfe404eaa6863666bf8719330255d0e3647327ca3bebb946 |
| run_flip.log | 8fb9af04e248e6d753592b561b5172f16165ef25293391e4b7c0205a7cd5ebf2 |
| caps/runs.tsv | 134b46813801b77950a9be635f2ca2e59ff71b11be10f200842459b4fc79d26b |
| arms/H_DEF.step.csv = arms/H_NOCSIX.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv (= K1 = K26 sha8) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| caps/c1_off, c3_off walk_04_colourselect.bmp | 4d53d93f87f0332d3d757d584b0fbaa9bf5ea54a6883b70fd5da920157bba9b2 |
| caps/c2_on, c4_on walk_04_colourselect.bmp | ba34030d1b7e2fe4eb69132a852cd296278fff04f788e3660607ea06670b1e2f |
