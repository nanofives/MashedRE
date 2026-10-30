# RESULT HB-RUN13-PHASE3-START (L-59 (a), harness step before the MASHED_SUBSTATE flip)

Date 2026-10-29, LIVING-PC. Branch `orch/HB-RUN13-PHASE3-START` off `orch/integration` @ `1611794e7`. PREREG
`PREREG_HB-RUN13-PHASE3-START.md` committed alone at `6ede1377c`, before any arm. Change at `8410e8881`. Exe `8575f868`,
built from the worktree with `MASHED_NO_DEPLOY=1`. Private root `D:/MashedWT/HB-RUN13-PHASE3-START_root`.
No MASHED.exe launched, no .asi deployed, no .ps1 edited, no tracker touched, merge_train.json not edited.

## 0. Verdict

> **GREEN. The default 13-track table does not move, and with the replay aligned to the phase-3 entry the
> MASHED_SUBSTATE table has 0 RED rows (39 of 39 CITED).** Run 22's flip table had 18 RED rows. Every row also
> matches more draws than the reference does: +2 to +8 per row, +132 summed.

| arm | knobs (on top of the run13 recipe) | table SHA-256 | result |
|---|---|---|---|
| A, default build | none (recipe sets `MASHED_CAM_REPLAY_P3=1`, SUBSTATE off) | **cf3307d2** | byte-identical to `verify/pregocubeflip_20261029/tracks_score.txt` |
| A0, knob OFF | `MASHED_CAM_REPLAY_P3=0` | **cf3307d2** | byte-identical |
| **B, flip candidate** | `MASHED_SUBSTATE=1 MASHED_P3_RESET_PAIR=1` (the pair D3-SUBSTATE-FLIP seeds) | **2431afb1** | 39 CITED, 0 RED, INSTRUMENT GATES: PASS |
| B1 | `MASHED_SUBSTATE=1` only | 2431afb1 | byte-identical to B |
| gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL 5943c5f1 / 306186d4 / 52052890, B 3a03670c / 9fcdbd3a / 53778664 | = `merge_train.json` refs |

All 52 track runs passed on attempt 1. Arm B ran on the stock frame budget (`max shot + 350`), which run 22 could not use.

**Candidate `run13_reference` for the flip: `verify/run13p3_20261029/tracks_score_sub_p3.txt` (2431afb1).**
The reference does not move: `tracks_score_default.txt` is the arm A table, cf3307d2.

**Merge-train pass kind: SOURCE.** The diff touches `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`, a build input,
plus `verify/rsky_20261016/run13.py` and `verify/run13p3_20261029/`. The new code runs only when `MASHED_CAM_REPLAY_P3`
and `MASHED_CAM_REPLAY` are both set and `MASHED_SUBSTATE` is on, so the default gates do not move (above).

## 1. Change

- `TrackRenderer.cpp` `SubStateCameraChain` (the pre-race chain, `race_cam_.Update` at the old line 8262): new knob
  `MASHED_CAM_REPLAY_P3` (DIAGNOSTIC, default OFF, inert without a loaded `MASHED_CAM_REPLAY` trace). When it is on, the
  chain calls `RaceCamUpdate(cc, reset)` in place of `race_cam_.Update(...)`, so each chain call consumes one trace row (pose
  inputs, ticks, dtb, jit, reset, overhead, the HUD row, the basis and car transplants), exactly as the stand-in countdown
  and the race already do. A call with cam[7] != 0 advances the trace one row and does not update the camera
  (`CamReplayConsumeRow`). No trace has such a row, so this branch only keeps the 1:1 call mapping. The two helpers
  `CamReplayP3On` / `CamReplayConsumeRow` sit next to `CamReplayGoRow`.
- `verify/rsky_20261016/run13.py`: the recipe env sets `MASHED_CAM_REPLAY_P3=1`. The knob argument
  `MASHED_CAM_REPLAY_P3=0` turns it off (arm A0).
- Why one row per chain call is the right mapping (measured on all 13 traces, `camera_probe.py` writes one row per
  FUN_00446520 return): row 0 is ba8c 3 with reset 1, and row 1 is the same original frame (ff4 repeats on rows 0-1 only),
  reset 0. That is the port's phase-3 entry frame, `SubStateCameraChain(true)` (0x004102f8 entry block) followed by
  `SubStateCameraChain(false)` (FUN_0040fc00 -> FUN_0040d470(0)). c7 is 0 on every row of every trace. GO rows are
  554..1064.

## 2. The 13-track tables (`tracks_compare.txt` row by row)

- A and A0 reproduce cf3307d2 byte for byte, so the knob is inert with SUBSTATE off, both in the recipe and outside it.
- B against the reference: every row is CITED. The rows-cited counts drop by 1 to 2 per row (for example Egypt 740: 17 -> 16),
  because more of the original's draws now match. Matched draws rise on all 39 rows (28 rows +3, 7 rows +4, 2 rows +5,
  1 row +8, 1 row +2).
- B against run 22's flip table (`orch/D3-SUBSTATE-FLIP` @ f0fe7783c, `tracks_score_flip.txt`): its 18 RED table rows
  (33 uncited draw rows: helicopter MISSING / EXTRA and IconCube MOVED) are all gone. Run 22's RESULT says 19 RED rows. The
  committed file has 18 rows whose verdict is RED.
- Reading: run 22's cause stands. Its port consumed trace row 0 only at its own GO, so at shot row K its free-running
  animations (`CopterTick`, cube spin, pickup state) ran about one pre-race ahead. With rows consumed from the phase-3
  entry, the port's frame and the trace row line up from the first frame. [UNCERTAIN] Per-frame GO alignment (the
  port's GO frame against the trace's GO row) was not logged. The private root's `mashed_re.log` is rewritten on every run.
  The evidence is the table: 0 RED rows and a higher match count on every row.

## 3. What the flip pass needs (not applied)

- `full.run13_reference` -> `verify/run13p3_20261029/tracks_score_sub_p3.txt` (2431afb1), the other keys as in
  `verify/d3subflip_20261029/RESULT_D3-SUBSTATE-FLIP.md` section 5 (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3,
  D2_DEFH_1..3, the O42 scorer).
- This candidate was measured with the env knobs on an `orch/integration` @ 1611794e7 exe, not with the flip branch's
  seeded default. `FaithDefaults.cpp` seeds the same two knobs. The 13-track pass of the flip should therefore give 2431afb1.
  If integration moves before the flip lands, the train's own run is the check.
- Merge order: this step first, then D3-SUBSTATE-FLIP. Both edit `run_one` in `run13.py`: this step edits the env dict, and
  run 22 edits the `frames =` line (`RUN13_PRE_FRAMES`, default 800). The hunks do not overlap. Run 22 showed that its +800
  is neutral with the opt-out. Arm B shows the stock budget is enough once rows start at phase 3, so with this step merged
  `RUN13_PRE_FRAMES` can be dropped. [UNCERTAIN] I did not run B with +800 frames. The extra frames all come after the last
  shot, so the table should not change.
- No .ps1 change needed.

## 4. Raw data (`D:/MashedWT/HB-RUN13-PHASE3-START_data`, full list with SHA-256 in `raw_sha256.txt`)

| file | SHA-256 |
|---|---|
| mashed_re_p3.exe | 8575f8680331aa0dd45da85ed0636bdbf29a16d1c5a3813bf898afdef4e2ac13 |
| tracks_score_A.txt (dirs `tracks_A/`) | cf3307d20446c2bdc17c08f3f9580594d16e65552df4a322dc7079a54b193d10 |
| tracks_score_A0.txt (`tracks_A0/`) | cf3307d20446c2bdc17c08f3f9580594d16e65552df4a322dc7079a54b193d10 |
| tracks_score_B.txt (`tracks_B/`) | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| tracks_score_B1.txt (`tracks_B1/`) | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 5943c5f1... / 306186d4... / 52052890... (full values in raw_sha256.txt) |
| arms/B_DEF_K0 / K1 / K26 .step.csv | 3a03670c... / 9fcdbd3a... / 53778664... |

Runner: `run_arms.sh` (this dir). Logs `RUNLOG_AB.txt`, `RUNLOG_rest.txt`, `RUNLOG_gates.txt` in the data dir.

## 5. Process

Every run went through `test-slot` and `mashed_lock.py run`. One wrapper slip: the first gates attempt sat inside a 595 s
`timeout`, which cut the runner off after the lock wait. Its game (PID 16788, spawned by this step) was stopped by PID. The
runner's second attempt also failed, because `arms/` did not exist (a FileNotFoundError on the copy). `run_arms.sh` now
creates it, and the third attempt produced the hashes above. Neither failed attempt wrote a hash, and every reported value
comes from the clean run.
