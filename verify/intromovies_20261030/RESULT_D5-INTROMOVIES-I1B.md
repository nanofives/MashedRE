# RESULT D5-INTROMOVIES-I1B: I1 re-run on a quiet machine inside the lock

Branch `orch/D5-INTROMOVIES-I1B` off `orch/integration` @ `9880245d9` (one merge train commit past
the brief's `0cd4241d4`: D4-DC-77b0-VALSRC). Exe `mashed_re.exe` sha256
`62030E052F42171A26D58F56912CDBF4833B6C8AD5E463DA35E3633A2B471E48`, built from the worktree with
`MASHED_NO_DEPLOY=1`. No source change. `MASHED_INTRO_MOVIES` stays default OFF
(`exe_main.cpp:2914` returns unless the env value is 1, call site `exe_main.cpp:9514`).

## Verdict

**GREEN.** Movie 1 (empire, band 6613..10503 ms) took **7766 ms** in I1b and **7782 ms** in I1c.
Every other I1 check passes in both runs, and I0, I2, I3 pass on the same exe. Default gates equal
the `merge_train.json` refs. The run-18 and run-19 I1c misses (34672 / 33547 ms) do not reproduce
at 50-60% system CPU. The flip comes back as a decision row.

## Machine state and timing

Driver: `verify/intromovies_20261030/i1b_driver.py` (committed). It polls system CPU in 30 s
windows of 2 s samples before taking any slot or lock, and needs mean < 40% and max < 65% with no
other `MASHED.exe` / `mashed_re.exe` alive. Stage timestamps: `run_i1b_timeline.txt`, every window:
`idle_wait.txt`.

| stage | time (local) | wait |
|---|---|---|
| driver start | 07:09:29 | - |
| quiet window reached | 07:41:01 | 1892 s (63 windows, 85.9% mean at start: 3 SimGolf pytest runs held 3 of 6 test slots, `MsMpEng.exe`, `SearchIndexer.exe`, a `cl.exe` build) |
| test slot held | 07:41:01 | 0 s |
| `mashed_lock.py run --solo` held | 07:41:02 | 0 s (lock free) |
| I1b + I1c | 07:41:32 .. 07:46:16 | arm wall excludes every queue wait (`run_intro.py` starts its clock at `Popen`) |
| I0 + I2 + I3 | 07:46:16 .. 07:48:14 | - |

The machine was **not idle**: the window that qualified read 40.0% mean / 57.7% max (39.97 before
rounding is the only reading that passes `< 40.0`), and SimGolf's 3 slots stayed held through the
arms. The game ran at `BELOW_NORMAL_PRIORITY_CLASS` (nice `16384`, from the test slot), as in
runs 18 and 19. No other game process ran in any phase (`other_games_max` 0 everywhere).

## I1 rows (band = [0.85, 1.35] x file duration)

| arm | idx | movie | frames | ms | ms/frame | band | phase load sys mean / p95 | others (cores x100) | row |
|---|---|---|---|---|---|---|---|---|---|
| I1b | 1 | empire | 207 | **7766** | 37.5 | 6613..10503 | 58.8% / 62.5% | 661.2 | pass |
| I1b | 2 | supersonic | 255 | 9437 | 37.0 | 8058..12798 | 48.1% / 51.0% | 562.9 | pass |
| I1b | 3 | renderware | 220 | 8156 | 37.1 | 6833..10854 | 55.6% / 57.9% | 655.4 | pass |
| I1b | 4 | intro | 1715 | 63750 | 37.2 | 54170..86035 | 57.4% / 69.7% | 677.6 | pass |
| I1c | 1 | empire | 207 | **7782** | 37.6 | 6613..10503 | 53.5% / 58.9% | 629.6 | pass |
| I1c | 2 | supersonic | 240 | 9437 | 39.3 | 8058..12798 | 61.8% / 71.2% | 726.4 | pass |
| I1c | 3 | renderware | 220 | 8110 | 36.9 | 6833..10854 | 50.2% / 52.9% | 595.0 | pass |
| I1c | 4 | intro | 1710 | 63718 | 37.3 | 54170..86035 | 51.0% / 71.5% | 600.8 | pass |

Both arms: every movie ends on `complete`, small.mpg `BUILT (graph built, not run) video=160x128`,
`F1 frontend.mpg: RUNNING (512x512)` after the INTRO lines, 4 shots at frame 90 with teal fraction
0.0 (per-channel sd 19.2..66.3). `audio_hr` as in run 18 (`0x80040256` on 1, 2, 4, `0x00000001`
on renderware): no audio endpoint on this machine, audio still untested.

Comparison with the earlier runs, movie 1 only:

| run | arm | ms | ms/frame | movie 1 sys mean |
|---|---|---|---|---|
| 18 | I1 | 34672 | 155 | not measured |
| 19 | I1b | 7984 | 38.8 | 34.6% |
| 19 | I1c | 33547 | 200.9 | 98.3% |
| this | I1b | 7766 | 37.5 | 58.8% |
| this | I1c | 7782 | 37.6 | 53.5% |

So movie 1 is in band at 35% and at 54-59% system CPU, and out of band only in the one measured run
at 98%. What saturated the machine in run 19 is still not identified (see the run-19 note in
`verify/intromovies_20261028/RESULT_D5-INTROMOVIES.md`).

## Other rows (no regression)

| arm | result | detail |
|---|---|---|
| I0 (knob OFF, 25 s) | PASS | no INTRO line |
| I2 (`MASHED_INTRO_PRESS=30`) | PASS | 4 x `skip-test` 30 frames, 1204 / 1141 / 1172 / 1328 ms (run 18: 1172 / 1172 / 1140 / 1344), small BUILT |
| I3 (167,414-byte skip files) | PASS | 4 x `complete`, 3/2/2/2 frames, 125 / 78 / 94 / 78 ms (run 18: 157 / 78 / 110 / 125), small BUILT |
| G (knob OFF, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`) | PASS | G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3, equal to `merge_train.json` `gctrl.ref` / `b.ref` |

The first gate attempt inside the solo window (07:48:14) crashed in `run_arms.py:286` on a missing
`A4_OUT` folder after H_DEF ran (the driver did not create `gates/`). The re-run made the folder
and ran all four arms in the lock from 07:56:36 to 08:06:44 (`run_gates.txt`).

## Movie files and original\

Private root `D:\MashedWT\D5-INTROMOVIES-I1B_root` (robocopy of `D:\MashedWT\D5-INTROMOVIES_root`,
which holds the real movies in `original\toastart\pc\movies\backup\`). The shared `original\` was
only read (I3's skip files). `patch_mashed_skip_intro.py --restore` was not run. No original-side
`MASHED.exe` ran. No tracker row changed.

## Files

Committed in `verify/intromovies_20261030/`: this RESULT, `i1b_driver.py`, `intro_score.txt`
(5 scored rows, with per-phase load), `run_i1b_timeline.txt`, `idle_wait.txt`.

Raw data (not committed), `D:/MashedWT/D5-INTROMOVIES-I1B_data/`:

| file | sha256 |
|---|---|
| run_i1b.txt | f37025a8ff7cbe4b642b143652dd81958896473d2c499bed6857029bb5bfdf95 |
| arms/intro_score.txt | cd8fff53ed2561fd2d02629db3e3401c1550653210f18b75275b7ebde6f4a520 |
| arms/I1b.log | 79f067c53dbca688194a38eddf43c4fbd50495ada62d377e436cf3312c300c0a |
| arms/I1c.log | 6daca92d68ec74dcaba0c4d297b089a1b911ce098263660adecb7db9f3f0b513 |
| arms/I0.log | c463c29e0d859fcd304195cd1ebdeafdea8e45386247a7a3c0104b9c6aa8b88a |
| arms/I2.log | aa7e9b3dacc57ff92e3add9f3fe16542f87eafa4898ab1d10ce3e757c2e86430 |
| arms/I3.log | 616f028f54ed4b0a52d1deb460d6454246fc76cfb5d0a0e886e2e5423f386497 |
| arms/I1b_load.csv | f18e1182802b6aa9bd5a842bd9ca0dd9040cb8b012897e4ff0b9293f68f55a8e |
| arms/I1c_load.csv | 3651a170c52325977ffd7f265e387cf92a5406cb011b9592df6042b2dfd1944b |
| arms/I0_load.csv / I2_load.csv / I3_load.csv | 8a30e0cd.. / 8ec1b748.. / 1bf20b4f.. |
| arms/I1b_shot_1..4.bmp | d1aef6f8.. / f6cb1023.. / 46f1eabc.. / 8736556c.. |
| arms/I1c_shot_1..4.bmp | 0b9769cb.. / 4b03deca.. / 57a25d83.. / 8d2b6efe.. |
| run_gates.txt | gate run log |
| gates/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gates/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| gates/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gates/B_DEF_K0 / K1 / K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Decision (filed as a ledger decision row)

Turn `MASHED_INTRO_MOVIES` default ON and strike D-11062? Evidence: this RESULT (I1 in band twice
at 54-59% CPU, I0/I2/I3 PASS, gates unchanged) plus run 19's I1b. Open items if ON: movie audio
untested (no audio endpoint), and movie 1 runs 4.3x long when every core is saturated and the game
sits at BelowNormal (run 19 I1c). Recommendation: ON, with the audio branch carried as a named
residue until a machine with an audio endpoint runs I1.
