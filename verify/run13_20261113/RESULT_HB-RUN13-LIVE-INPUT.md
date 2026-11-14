# RESULT HB-RUN13-LIVE-INPUT (HARNESS / DEFAULT-PATH row, harness files only)

Verdict: the table result is GREEN (no cited row moved with the knob on, below). The claim "a live key press is now harmless"
is **[UNCERTAIN]**: no positive test has been run. Run 45 queued it (arm A below) and the lead stopped it before it started,
because the merge train held a solo window on the machine (see "Run 45: positive test not run").

Only harness files changed (`verify/rsky_20261016/run13.py`). No exe, `mashedmod/` or default-path file changed, so the default
gates (G-CTRL, B) are not run: the exe bytes cannot move. `merge_train.ps1` needs no edit (it calls run13.py at
`Gate-Full`, lines 484-486, `--out $o13`, so the changes reach the train through run13.py).
`re/orchestrator/mashed_lock_concurrency_test.py` has no mention of run13 or R7 (grep), nothing to change.

## Mechanism (source, branch tree)
- `CookCtrlByteAFromKeyboard` (`mashedmod/src/mashed_re/exe_main.cpp:3086`) is the per-frame writer of ctrl byte
  `0x007f1042 + i*0x4c`: 0xff while DIK 0x1c is down on a keyboard slot (`exe_main.cpp:3103`, `g_keys` filled by a
  keyboard opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, `exe_main.cpp:1207`), or joypad button 0 on a pad slot.
- Its first line is `if (!R7InputOn()) return;` (`exe_main.cpp:3087`). `R7InputOn()` (`exe_main.cpp:1114-1117`) is false when
  env `MASHED_NO_R7_INPUT` is set and its first char is not `0`. The same switch stops `CountGameCtrlCb` from keeping
  joypad devices (`exe_main.cpp:1122`). `MASHED_NO_R7_INPUT=1` is the existing opt-out of the default-ON R7INPUT (L-16), not a new knob.
- Consumer of the byte: `CameraEntries.cpp:405-407` (site (b), per RWC-THUNK-BIND-AUDIT section 2).

## Change (run13.py)
1. `run_one` sets `MASHED_NO_R7_INPUT=1` in the exe env unless `RUN13_LIVE_INPUT=1` is set; a knob argument is applied after
   and wins (`MASHED_NO_R7_INPUT=0` restores live input for one run).
2. After each track the private root's `mashed_re.log` is copied to `<out>/<track>/stand/mashed_re.log` (it was rewritten by the
   next track and the next train; the RED run's `R7IN` line was lost that way).
3. (run 45) `RUN13_POST_KEYS=1` starts `start_key_poster`: every 2 s it posts WM_KEYDOWN/WM_KEYUP (Enter, then Down arrow,
   alternating) to the top-level windows whose `GetWindowThreadProcessId` equals the PID `run_one` spawned (EnumWindows filter).
   No focus change, no SendInput, no image name. It writes `<track>/stand/key_poster.txt` (post count, hwnds, pid). Untested:
   it never ran against a game process.

## Evidence (earlier pass, no input typed)
Exe: `D:/MashedWT/RWC-THUNK-BIND-AUDIT_data/city13/tip.exe` (sha256 3dcb0a2f59d000e49d7217cace8fb7d51c6706c3b4004f1fbdae419742edef98,
the tip exe of RWC-THUNK-BIND-AUDIT). Full 13-track run, test-slot outside, mashed_lock inside, 1 attempt each, all 13 OK,
private root `D:/MashedWT/RWC-THUNK-BIND-AUDIT_root`, out `D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run13`.
- Score vs the train reference `verify/run13p3_20261029/tracks_score_sub_p3.txt` (cites `verify/rres_20261016/cites_rres.json`):
  the 13 track rows are IDENTICAL, `INSTRUMENT GATES: PASS`. The score file sha256 is 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3,
  the reference's hash recorded in merge_train.json (2431afb1). Copy: `tracks_score_noinput.txt`.
- Per-track logs kept: `track_logs.txt` lists R7IN line count (0 on all 13: with the knob the writer returns before logging),
  the `[leg2] phase 3 released after N frames` line and each log's sha prefix. 12_City: 618 frames, the GREEN value from RWC-THUNK-BIND-AUDIT.
So the knob changes no cited row on the 13 tracks when nobody types.

## Run 45: positive test not run [UNCERTAIN]
Missing evidence for "a live key press is now harmless":
1. **No run with a press injected.** The earlier pass typed nothing. Arm A (13 tracks, knob ON, presses injected) was queued under
   test-slot (slot 2/8 after a 600 s memory wait) and then waited at `mashed_lock.py run` behind the merge train's solo window
   (`merge-train:20261113_204549`, since 21:08:39, ~57 min left, PRIORITY-HOLD). The lead stopped it before any game process spawned.
   Nothing was launched, so no table, hash or log exists for it. Stopped PIDs: only the test-slot wrapper chain this session started.
2. **No control showing the input is read.** Not run either.
3. **A posted WM_KEYDOWN cannot be the positive test as specified (code reading, not observed).** The game's keyboard state is
   `g_kbd->GetDeviceState` on a DirectInput device (`exe_main.cpp:1286`, opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE` at
   `exe_main.cpp:1207`), which reports device state and not the window's message queue. The window procedure's `WM_KEYDOWN`
   case does nothing (`exe_main.cpp:1026-1030`, `break`). By this reading a PostMessage Enter changes neither `g_keys` nor
   the ctrl byte with the knob on or off, so a PostMessage arm cannot show that a live press is harmless, and a
   `RUN13_LIVE_INPUT=1` control with posted keys only would not show the input read. This is unobserved: it is the
   reason the posted-key arm is paired below with an in-process device-level press.
4. The merged exe was built (VS2022 default, `MASHED_NO_DEPLOY=1`, from this worktree, whose base is `orch/integration` @ ba39eeb6b,
   0 commits behind): sha256 fb3738aa3e873998887314b3a516a233832b5a785418d64c47477a2f1cb02c68 at
   `D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run45/merged.exe`, 0 compile or link errors. It was not run. Private root
   `D:/MashedWT/HB-RUN13-LIVE-INPUT_root` (copy of `original\`) is ready.

Press paths that exist in the exe (source): `MASHED_DET_TEST_KEY=<dik>@<det_frame>[+<frames>]` sets `g_keys[dik]=0x80` inside
`DetAmbientKeys` (`exe_main.cpp:1243-1263`) and logs `DET-AMBIENT-KEY ... test=1` (`exe_main.cpp:1272`), upstream of
`CookCtrlByteAFromKeyboard`; `MASHED_R7_INPUT_PRESS` is inside the function the knob disables (`exe_main.cpp:3087-3088`).

## Command to run the test later (when mashed_lock shows no merge-train SOLO or PRIORITY-HOLD)
Build is done; reuse `merged.exe` or rebuild from this branch. From the worktree
`C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/HB-RUN13-LIVE-INPUT`, three runs, each `test-slot` outside and `mashed_lock` inside
(`L=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py`,
`E=D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run45/merged.exe`, `R=D:/MashedWT/HB-RUN13-LIVE-INPUT_root`):
- Arm A (positive, knob ON, all 13 tracks; background, timeout 7200000):
  `RUN13_POST_KEYS=1 test-slot bash -c "py -3.12 $L run --label HB-RUN13-LIVE-INPUT -- py -3.12 verify/rsky_20261016/run13.py run --exe $E --root $R --out D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run45/armA MASHED_DET_TEST_KEY=0x1c@300+600"`
  then `py -3.12 verify/rsky_20261016/run13.py score --out D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run45/armA --cite verify/rres_20261016/cites_rres.json`.
  Pass: 13 rows identical, `INSTRUMENT GATES: PASS`, score sha256 2431afb1..., 0 `R7IN` lines per track, and `DET-AMBIENT-KEY ... test=1`
  present in each track's `mashed_re.log` (the press reached `g_keys` and the knob still blocked it). `key_poster.txt` lists the posts.
- Control C1 (knob OFF, one track): same command with `RUN13_LIVE_INPUT=1`, `--tracks 12`, `--out .../ctrl12`. Pass: an `R7IN byte0xa ... test=0`
  line appears and the `phase 3 released after N frames` value differs from 618, or the log shows it was read. If it does not, record that.
- Control C2 (posted only): `RUN13_LIVE_INPUT=1 RUN13_POST_KEYS=1`, `--tracks 12`, no `MASHED_DET_TEST_KEY`. Expected by item 3 above: no `R7IN` line.
  Either outcome is a finding.

## Limits (NO-GUESSING)
- tip.exe (earlier pass) is the RWC-THUNK-BIND-AUDIT tip, not rebuilt from `orch/integration` at ba39eeb6b. The exe is unaffected by this row.
- Default gates (H_DEF, B_DEF_K0/K1/K26) not run: no exe-side change.
- Raw data (not committed): `D:/MashedWT/HB-RUN13-LIVE-INPUT_data/run13/` (dumps, logs), `run13.log`, `score.txt`; run 45: `run45/` (build log, merged.exe, armA.log with the queue lines only).
