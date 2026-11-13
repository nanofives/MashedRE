# RESULT HB-RUN13-LIVE-INPUT (HARNESS / DEFAULT-PATH row, harness files only)

Verdict: GREEN. Only `verify/rsky_20261016/run13.py` changed. No exe, `mashedmod/` or default-path file changed, so the default
gates (G-CTRL, B) are not run: the exe bytes cannot move. `merge_train.ps1` needs no edit (it calls run13.py at
`Gate-Full`, lines 484-486, `--out $o13`, so both changes reach the train through run13.py).
`re/orchestrator/mashed_lock_concurrency_test.py` has no mention of run13 or R7 (grep), nothing to change.

## Mechanism (source, branch tree)
- `CookCtrlByteAFromKeyboard` (`mashedmod/src/mashed_re/exe_main.cpp:3086`) is the per-frame writer of ctrl byte
  `0x007f1042 + i*0x4c`: 0xff while DIK 0x1c is down on a keyboard slot (`exe_main.cpp:3096`, `g_keys` filled by a
  keyboard opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, `exe_main.cpp:1207`), or joypad button 0 on a pad slot.
- Its first line is `if (!R7InputOn()) return;` (`exe_main.cpp:3087`). `R7InputOn()` (`exe_main.cpp:1114-1117`) is false when
  env `MASHED_NO_R7_INPUT` is set and its first char is not `0`. The same switch stops `CountGameCtrlCb` from keeping
  joypad devices (`exe_main.cpp:1122`). That is the knob that blocks live input. `MASHED_NO_R7_INPUT=1` is the existing
  opt-out of the default-ON R7INPUT (L-16), not a new knob.
- Consumer of the byte: `CameraEntries.cpp:405-407` (site (b), per RWC-THUNK-BIND-AUDIT section 2).

## Change (run13.py)
1. `run_one` sets `MASHED_NO_R7_INPUT=1` in the exe env unless `RUN13_LIVE_INPUT=1` is set; a knob argument is applied after
   and wins (`MASHED_NO_R7_INPUT=0` restores live input for one run).
2. After each track the private root's `mashed_re.log` is copied to `<out>/<track>/stand/mashed_re.log` (it was rewritten by the
   next track and the next train; the RED run's `R7IN` line was lost that way).

## Evidence
Exe: `D:/MashedWT/RWC-THUNK-BIND-AUDIT_data/city13/tip.exe` (sha256 3dcb0a2f59d000e49d7217cace8fb7d51c6706c3b4004f1fbdae419742edef98,
the tip exe of RWC-THUNK-BIND-AUDIT). Full 13-track run, test-slot outside, mashed_lock inside, 1 attempt each, all 13 OK,
private root `D:/MashedWT/RWC-THUNK-BIND-AUDIT_root`, out `D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run13`.
- Score vs the train reference `verify/run13p3_20261029/tracks_score_sub_p3.txt` (cites `verify/rres_20261016/cites_rres.json`):
  the 13 track rows are IDENTICAL, `INSTRUMENT GATES: PASS`. The score file sha256 is 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3,
  the reference's hash recorded in merge_train.json (2431afb1). Copy: `tracks_score_noinput.txt`.
- Per-track logs kept: `track_logs.txt` lists R7IN line count (0 on all 13: with the knob the writer returns before logging),
  the `[leg2] phase 3 released after N frames` line and each log's sha prefix. 12_City: 618 frames, the GREEN value from RWC-THUNK-BIND-AUDIT.
So the knob changes no cited row on the 13 tracks, and the kept logs show phase-3 release frames after the fact.

## Limits (NO-GUESSING)
- This run is one pass with no live input typed, so it shows the table is unchanged with the knob on, not that an Enter
  press is now harmless. By the code above, with `R7InputOn()` false the function returns before reading `g_keys` or the pad.
  A press-injection arm (`MASHED_R7_INPUT_PRESS`) was not run: that test hook is itself disabled by the knob.
- tip.exe is the RWC-THUNK-BIND-AUDIT tip, not rebuilt from `orch/integration` at ba39eeb6b. The exe is unaffected by this row.
- Default gates (H_DEF, B_DEF_K0/K1/K26) not run: no exe-side change.
- Raw data (not committed): `D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run13/` (dumps, logs), `run13.log`, `score.txt`.
