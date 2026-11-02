# RESULT HB-GATE-FLAKE-GAMEFLOW (2026-11-02)

Verdict: **cause found from existing logs. The fix is built but NOT verified on the exe**, because the display was asleep
from about 09:20 and every boot returned D3D9 `0x8876086A`. Arm status is in "Arms" below and the next steps are
in "Handoff".

**Cause: a real keyboard Enter pressed somewhere on the machine reaches a deterministic G-CTRL arm.** The exe's
keyboard is opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE` (`exe_main.cpp` InitDirectInput), so it reports keys
pressed in any window. The R7 cook (`CookCtrlByteAFromKeyboard`, `g_keys[0x1c]`) reads that state with no focus gate
and no `g_det_clock` gate, and so does the powerup fire read (`g_keys[DIK_SPACE]`). Every G-CTRL arm ends round 1 at
det_frame 1098 and then holds on the round-result screen until frame 14400, about 13,300 frames. An Enter at any time
in that window starts round 2, and from then on the arm does not reproduce the refs.

The cause is not wall-clock time, lock contention, a file race or uninitialised state. The 280 s vs 193 s gap is a
consequence of the press: g1 raced a second round with four cars for about 9,000 frames, and g1b sat on the cheap result
screen.

PREREG: `verify/harness_20261102/PREREG_HB-GATE-FLAKE-GAMEFLOW.md`, committed alone at `8a98d064b` before any arm ran.

## Evidence (existing raw data, no new arm needed)

1. **g1 vs g1b, same exe `fd0211e9`.** The step files are byte-identical through line 4,969 (frame 4527). At frame
   4528, cars 1 and 2 reappear in g1 (round 2 respawn). In g1b, car 3 continues alone in substate 7.
2. **g1 `H_PSGO.log` lines 235-246:**
   `R7IN byte0xa slot=0 val=0xff r7frame=4308 test=0 det_frame=5406 type=2`, then
   `R7X EXIT det_frame=5406 round 1 -> 2 match_winner=-1`, then 9 more `R7IN ... test=0` lines (det_frame 5407..5415).
   - That is a key held for 10 frames. At this arm's average of 51 frames/s, 10 frames is about 0.2 s.
   - `test=0` means the byte came from `g_keys[0x1c]` (DIK_RETURN, the action-4 binding), not from
     `MASHED_R7_INPUT_PRESS`.
   - g1b's log has no `R7IN` line, and neither does any other H arm of g1 (H_DEF, H_PSG).
   - The rest of the two logs differ only in boot-time noise: the image commit size, a `.data` page protect, and intro
     movie frame counts and ms.
3. **The same signature happened earlier, in a separate step.** D4-DC-77b0-MATCHEND-SUBSTATE `H_ME_try1` (2026-11-01
   20:34, exe `7b389f80`, 362 s) logged `R7IN ... r7frame=11550 test=0 det_frame=12648` and
   `R7X EXIT det_frame=12648 round 1 -> 2`. Its hashes were 937d89d8 / 277b95f7 / 1925768a. Try 2 on the same exe
   reproduced 8284be46 / ba407a16 / 31fe043e in 142 s. That RESULT voided the run as "external input" and did not fix
   the cause (`verify/d4dc77b0matchend_20261101/RESULT_D4-DC-77b0-MATCHEND-SUBSTATE.md:40-45`).
4. **Frequency.** I checked every `H_*.log` under `D:/MashedWT/*/*/` and `*/*/*/` (696 logs, 488 written since
   2026-10-25, the R7 input flip). Exactly 2 contain an `R7IN ... test=0` line: the two arms above.
5. **Wall clock alone does not move the hash.** I took every RUNLOG row since 2026-10-30 for a Training H arm with
   14400 frames (178 rows). 164 reproduce step 8284be46, at durations from 121 s to 805 s (median 480 s). The other
   14 rows are knob-ON behaviour arms, apart from the two ambient-Enter rows (3cc38c6b, 937d89d8). Table:
   `duration_table.txt`.

**Display-state transition as an alternative (lead's question): ruled out for g1.** g1 exited 0 after its full
14,400 frames, with every capture written. A display that is asleep or locked fails the boot outright
(`CreateDevice` `0x8876086A`, as at 09:20 today). Render state also does not feed the simulation. The one input that
changes is the `R7IN test=0` keyboard byte at the exact frame where the step files part. A keypress is also what wakes
a sleeping display, so a person at the keyboard around 00:45 fits the evidence. Who or what pressed Enter is
[UNCERTAIN]: the log records the key, not the source process.
- No repo tool injected the key. The repo's only OS-level injector is `re/tools/sa_capture.py`, which calls
  `keybd_event` for Enter, Down, Up and Esc, holds each 70 ms, and is not part of any gate arm.
- Any global injector on the machine (sa_capture, or a person typing) produces this signature.

## Change (`59634b943`, both files)

- `mashedmod/src/mashed_re/exe_main.cpp`: new `DetAmbientKeys()`, called at the end of `PollKeyboard`. It returns at
  once unless `MASHED_DETERMINISTIC` is set.
  - Witness, log only: one `DET-AMBIENT-KEY det_frame=<n> dik=<first> keys_down=<n> test=<0|1> LIVE|dropped` line per
    frame on which the device reports any key down, capped at 64 lines. From now on a contaminated arm says so in its
    log, whichever key was pressed.
  - `MASHED_DET_NO_AMBIENT_KEYS=1` (**default OFF in the exe**) zeroes `g_keys` after the device read, which is the
    state a run reads when nobody is at the keyboard. In-process drivers (nav demo, cfgedit, drive hold) write
    `g_keys` after `PollKeyboard` returns, so they are unchanged.
  - `MASHED_DET_TEST_KEY=<dik>@<det_frame>[+n]` (test only) makes the device report a key, so an ambient press can be
    reproduced without touching the real keyboard. The real-keyboard alternative, `keybd_event`, would also type into
    the user's focused window.
  - With the drop knob OFF and no key pressed, nothing changes except for the code layout. Training G-CTRL has been
    stable across layouts (memory `sim-trajectory-is-code-layout-sensitive`).
- `verify/savelink_20261023/run_arms.py`:
  - `base_env` sets `MASHED_DET_NO_AMBIENT_KEYS=1` for every H / B / W / LR / CR / R arm. Exes from before this
    change ignore it, and an arm can remove it with `-MASHED_DET_NO_AMBIENT_KEYS`.
  - New repro stacks: `R7P`, `PSGOR7P`, `TK`, `TKD`, `NAKOFF`, `PSGOTK`.
  - This is a harness default, not a game default. The merge train's gate arms pick it up once this merges. **For the
    lead:** if a harness env change counts as a flip, drop that one `base_env` line and file a decision row. The exe
    knob stays OFF either way.
- Build: `MASHED_NO_DEPLOY=1`, `=== Build OK ===`, `[rva-lint] ... allowlisted=2 NEW=0`, `[asm-lint] OK`. Exe
  `88356f08`.

## Arms

Prediction ids are the PREREG's Q1..Q7. **No arm in this step has reached the simulation.**

| attempt | time | arms | outcome |
|---|---|---|---|
| 1 | 09:21 | fd02: H_PSGOR7P H_R7P H_PSGO-r1 H_DEF-r1 H_PSGO-r2. new: H_TK H_TKD H_DEF B_DEF_K0/K1/K26 H_NAKOFF H_PSGOTK | all 13 exit 3 in 1-8 s, `InitD3D9: CreateDevice ... fail hr=0x8876086A` on all 8 device paths. Display asleep (L0 notice, `diag.py probe-render` BLOCKED). Not data, kept under `failed_sandboxed/` (the folder name records my first wrong reading, which was the sandbox) |
| 2 | 09:33 | new: B_DEF_K0 (PowerShell probe) | exit 3, same hr |
| poll | 09:36, 09:42, 09:47 | `poll_and_run.sh`: `probe-render` every 300 s | BLOCKED `0x8876086A` all 3. Poller stopped by hand after probe 3, so no arm starts unattended after this session closes |

Rerun count: 0 completed arms, 14 failed boots (INFRA) plus 4 BLOCKED probes. Hashes: none.

## Handoff

- Branch `orch/HB-GATE-FLAKE-GAMEFLOW`, worktree `.worktrees/HB-GATE-FLAKE-GAMEFLOW`.
- The exes are already built and copied out: `exe_new` = `88356f08` (this branch), `exe_fd02` = `fd0211e9`.
- The root `D:/MashedWT/HB-GATE-FLAKE-GAMEFLOW_root` is ready. Its `videocfg.bin` is copied from the GAMEFLOW root.
- When `py -3.12 scripts/diag.py probe-render` passes, run `bash D:/MashedWT/HB-GATE-FLAKE-GAMEFLOW_data/poll_and_run.sh`
  in the background. It wraps every set in `test-slot` and `mashed_lock`, takes about 35 min of lock time, and writes
  `gates_new.log`, `gates_fd02.log` and `gates_new2.log`.
- Score Q1..Q7 into the "Arms" table, then set the row to review.
- GREEN needs Q4 + Q7. Q6 is the strong form (the injected press reproduces g1's hashes). If Q6 moves the hashes
  without matching g1 exactly, the cause still stands as long as the step file diverges at frame 4528 and the log
  shows `R7X EXIT det_frame=5406`.

**Arms still to run** (one command, after `py -3.12 scripts/diag.py probe-render` passes):
`bash D:/MashedWT/HB-GATE-FLAKE-GAMEFLOW_data/poll_and_run.sh`. It runs:
- new exe `88356f08`: H_TK (Q6, expect g1's 3cc38c6b / 75d81021 / 8c8ba1f1 or a step file equal to g1's through line
  4,969), H_TKD (Q7, expect refs and 10 `DET-AMBIENT-KEY ... dropped` lines), H_DEF and B_DEF_K0/K1/K26 (Q4, refs), then
  H_NAKOFF (Q5) and H_PSGOTK.
- exe `fd0211e9`: H_R7P and H_PSGOR7P (Q1/Q2: the press alone reproduces g1's round change at det_frame 5406), then
  H_PSGO-r1 and H_DEF-r1 (Q3, refs).

## Filed (not done here)

- **Train INFRA precheck.** The merge train's gate wrapper should run `diag.py probe-render` before the G-CTRL / B arms.
  On `0x8876086A` it should report INFRA and wait or retry, not report MISSING hashes as RED. The lead proposed this;
  it is not implemented in this branch.
- **Live play is exposed too.** The R7 cook and the powerup fire read have no focus gate in normal play either: the
  menu (`UpdateMenuSelection`) and driving (`s_live_input_ok`) reads check `g_active`, these two do not. The original's
  keyboard cooperative level is [UNCERTAIN]: no analysis note records the `SetCooperativeLevel` flags of the
  original's keyboard device. A faithful fix needs that read first (a decomp of the keyboard create path next to
  0x00495faf). That would be a behaviour change and needs its own PREREG.
- `UNCERTAINTIES.md` / trackers: none touched. No RVA changes level.

## Raw data (D:/MashedWT, not committed)

```
96162cea5153d3b58b02bd8ecf1659d7ed0343474719d86ad1d9be70452b2a08  MATCH-PORTSTATE-GAMEFLOW_data/arms_g1/H_PSGO.log
f8b6d42d836b4373d8f8a7e9fbc1f2b63158c64a36efa09c17476b57c38ca169  MATCH-PORTSTATE-GAMEFLOW_data/arms_g1b/H_PSGO.log
3cc38c6b0e0c0df627f336e55bcb45d997fca623c64c97329485b8feaa2a1b69  MATCH-PORTSTATE-GAMEFLOW_data/arms_g1/H_PSGO.step.csv
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  MATCH-PORTSTATE-GAMEFLOW_data/arms_g1b/H_PSGO.step.csv
5519da70220a4553b61118462d6025c7723fa2a529506fe29eee5a77065fde3c  D4-DC-77b0-MATCHEND-SUBSTATE_data/arms/H_ME_try1.log
fd0211e93b286efb3b3a034d2d21eeaf06b69fde3c77d469c9e4421b42b68789  HB-GATE-FLAKE-GAMEFLOW_data/exe_fd02/mashed_re.exe
88356f0815807af7124076149073cb9e7402f9c21c4942979ed53c9b5ebbb45e  HB-GATE-FLAKE-GAMEFLOW_data/exe_new/mashed_re.exe
49d225075409a7a01019d2564ff4054accaaaf0a6a0bbe59dcbdab52f6135e84  HB-GATE-FLAKE-GAMEFLOW_data/build1.log
49ac4171cb8445abdf8d3a028ff8b00e98c1f861aee6777e96e320bb1412e49e  HB-GATE-FLAKE-GAMEFLOW_data/probe_ps/B_DEF_K0.log
```

Committed extract: `duration_table.txt` (the 178-row summary of evidence item 5).
