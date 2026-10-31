# RESULT D5-FLIP-SAVE-FIRSTRUN: MASHED_SAVE_FIRSTRUN_WRITERS default ON holds every default gate

Verdict: **GREEN**. Built with `MASHED_SAVE_FIRSTRUN_WRITERS` seeded ON (one new `kFaith` row), the exe gives G-CTRL
`8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`, D2 3/3 in the O42 band, and a 13-track table byte-identical to
`run13_reference` (`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1). **Nothing moved**, so no reference is
re-based. Per L-66 (a) (Mariano 2026-10-30) the flip is committed on this branch.

Branch `orch/D5-FLIP-SAVE-FIRSTRUN` off `orch/integration` @ `9824c43af` (descends from `e23f260b7`). LIVING-PC,
2026-10-30. Private roots `D:/MashedWT/D5-FLIP-SAVE-FIRSTRUN_root` (arms, witness) and `..._root13` (run13). No
MASHED.exe launched, no .asi deployed, no .ps1 / .bat edited, no tracker touched, merge_train.json not edited.

## 1. What lands

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp:88`: row `{ "SAVE_FIRSTRUN_WRITERS", "1" }` at the end of `kFaith`,
  with a comment naming L-66 (a). The seed loop gives the opt-outs: `MASHED_NO_SAVE_FIRSTRUN_WRITERS=1` skips the row,
  `MASHED_SAVE_FIRSTRUN_WRITERS=0` is left as given and the site reads it as OFF (`v[0] != '0'`, `Race/GameFlow.cpp:263`
  `FirstRunWritersMode`), `MASHED_NO_FAITH=1` drops every row.
- `mashedmod/src/mashed_re/Race/GameFlow.cpp:247-248`: the site comment says default ON and names the opt-outs. No code
  change at the site. `FirstRunWritersMode()` caches the knob in a function-local static on first call, after the
  `init_seg(lib)` seed.
- Standalone only: `FaithDefaults.cpp` is `#ifdef MASHED_STANDALONE`, and so are all three knob sites.
- What the knob does (from `verify/spanwriters_20261027/RESULT_D4-FU-SPANINIT-WRITERS.md`, GREEN, P1..P6 held):
  (a) `Campaign_LoadProgress` with no save file (`nRead == 0`) sets the counter to 1 (`GameFlow.cpp:476`), the
  original's `0x00409e30` `MOV [0x008a95ac],1`, serialized to file `0x24efc` (`0x00404f03` / `0x00404f23`).
  (b) `SaveProgress` rebuild path clears span `+0x514 + i` for each slot with a derived type != 0 (`GameFlow.cpp:302`),
  the original's `FUN_00496530` `rep stosd` `0x00496568`, gathered by `0x00404ef0` / `0x00404ef2`.
  (c) the pre-WinMain save round-trip harness derives the slot types headless (`GameFlow.cpp:718`), only under
  `MASHED_SAVE_RT_IN`.

## 2. Exe

Worktree build, `MASHED_NO_DEPLOY=1`, rva-lint NEW=0 (16 CROSS-TARGET, all allowlisted). `mashed_re_on.exe` SHA-256
`da73061f7b761763bb06cb0b49623318a7efabbd5bcd4a641f246259a59ee2a8`. The measured tree is the flip commit (`978d38456`):
the same two source files. The string `SAVE_FIRSTRUN_WRITERS` occurs 2 times in the exe (the site's `MASHED_...` name
plus the `kFaith` row).

## 3. Gates (outputs: `GATES_MEASURE.txt`)

Runner `run_measure.sh` (this folder) = `verify/d5flipmusic_20261030/run_measure.sh` with the step name changed, plus a
save-file check after each stage and a `d2short` stage (re-runs only arms under 1600 lines). The merge train's
Gate-Default + Gate-Full, same scripts and arguments, under test-slot, arms through `mashed_lock.py run` (D2 `--solo`),
run13 inside a `solo-begin` window. Quiet check before start: 1 test-slot holder outside Mashed (SimGolf pytest).

| Gate | Arms | Result | Reference | |
|---|---|---|---|---|
| G-CTRL | H_DEF | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref | PASS (first run) |
| B | B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref | PASS |
| (b) vsum | B arms | 3/3 arms, all cars A PASS | | PASS |
| (e) ai_speed_env | B_DEF_K0 | v1 / v2 / v3 (e) PASS | | PASS |
| D2 | D2_DEFH_1..3, `score_d2_v2.py port` | drvmed 1920.25 / 1919.62 / 1920.25, slip1500 0.1930 / 0.1930 / 0.1935, slip2000 0.2514 / 0.2515 / 0.2509, all `in` | O42 band | 3/3 PASS |
| run13 | 13 tracks, stock budget | table 2431afb1, `^NN_` row diff empty, INSTRUMENT GATES: PASS | tracks_score_sub_p3.txt 2431afb1 | identical |

D2 arm lengths: the first `d2` stage ran while the machine was loaded (2 non-Mashed holders: SimGolf pytest and boot,
a Simcity graphaxis lab run, plus two long `find` processes not of this step). Its arms came back 1396 / 1555 / 1539
lines, then after its two reruns 856 / 1399 / 1610 (`score_d2_first.txt`: arms 1 and 2 OUT, arm 3 in). Short arms are
wall-clock artefacts, not measurements. The `d2short` stage re-ran arms 1 and 2 (arm 2 twice) to 1644 / 1643 lines,
arm 3 stays at 1610. The scored table above is those three arms, all over 1600.

## 4. Do the gate arms reach the knob's code path? No, not its outputs

- Every arm root (`_root`, `_root13`) is a robocopy of `original\` and holds no `mashed_re_gamesave.bin`. The arms set
  no `MASHED_SAVE_PATH` / `MASHED_SAVE_ORIGINAL` (`run_arms.py`, `run13.py`: no `SAVE` env), so the save path is the
  default `mashed_re_gamesave.bin` (`Save/GameSave.cpp:127-135`, `GameFlow.cpp:88`) relative to the cwd, which
  `MASHED_ROOT` sets to the root. So every arm boots as a first run with no port save file. The boot also loads
  `original/gamesave.bin` into the live span (`R2-2 gamesave load: LOADED (DEADBEEF save)` in the arm logs), which is a
  different path and not gated by this knob.
- (a) can fire at the boot `Campaign_LoadProgress` (`exe_main.cpp:9982`). It only changes the in-memory counter
  `g_saveCounter`. Readers in the exe: `SaveProgress` (`GameFlow.cpp:294`, `:313`) and the `progress loaded` log line
  (`GameFlow.cpp:482`, into `log/mashed_re.log`, a folder the roots do not have). `GameSaveVFS.cpp`'s autosave guard and
  `GameSaveBuffer.cpp`'s Serialize bind to it but have no exe caller (`GameSaveVFS.cpp:21-22`, no `SerializeToBuffer`
  call outside its TU and the unit test).
- (b) runs only inside `SaveProgress`, which writes the save file. `SaveProgress` callers: Options Save Game
  (`exe_main.cpp:2368`) and the match-end autosave (`exe_main.cpp:4025` -> `GameFlow.cpp:516`).
- After every stage `savefile_check.txt` shows no `mashed_re_gamesave.bin` in either root, and both roots'
  `original/gamesave.bin` are byte-identical to `original\gamesave.bin` (cmp). So no gate arm ran `SaveProgress`.
- (c) needs `MASHED_SAVE_RT_IN`, which no gate arm sets.

So the gates cannot see the writers' effect. They show the flip changes nothing else. This matches the prediction: the
knob is a save-state writer with no sim, AI, physics or draw reader.

## 5. Witness that the default seeds the knob and the writers fire (`WITNESS.txt`)

`run_witness.py` (this folder): the save round-trip harness with `MASHED_SAVE_RT_IN=none` (first run, no file), the
`rt()` recipe of `verify/spanwriters_20261027/run_spw.py`. The exe exits before WinMain (no window, no piz open). Run
under `mashed_lock.py run`, in the gap between two D2 solo windows (witness files 20:57:42-44, D2 rerun window opened
after).

| Arm | Env | file `0x24efc` | gather `0x24f54..57` | SHA-256 (first 8) |
|---|---|---|---|---|
| W_DEF | none (new default) | 1 | 00 01 01 01 | f003d0db = spanwriters `K_0` (knob ON) |
| W_K0 | `MASHED_SAVE_FIRSTRUN_WRITERS=0` | 0 | 01 01 01 01 | 604d373f = spanwriters `N_0` (knob OFF) |
| W_NO | `MASHED_NO_SAVE_FIRSTRUN_WRITERS=1` | 0 | 01 01 01 01 | 604d373f = `N_0` |

W_DEF vs W_K0 / W_NO: 2 bytes differ, `0x24efc` and `0x24f54`, the two writers. W_K0 vs W_NO: 0. Logged slots on this
run: `game controllers 0, types 2 0 0 0, active 1` (slot 0 keyboard only, the same hardware state as spanwriters'
`K_0`). So the default seeds the knob, both writers fire on a first-run save, and both opt-outs restore the old file
byte for byte.

## 6. Not run / `[UNCERTAIN]`

- Not run: a first-run save written from a live game (Options Save Game or a match-end autosave on an empty root). The
  harness calls the same `Campaign_LoadProgress` + `Campaign_SaveNow` pair, but with the slot types derived headless
  (`ExeInput_DeriveSlotsHeadless`) instead of by `InitDirectInput`. Not needed for the four required gates.
- Carried from spanwriters, unchanged by the flip: no 2-controller first-run save measured (the 13-byte case is
  predicted), (a) covers a missing file only, the creation write `0x00409db8` is not ported, the live span seed does not
  model the per-frame record clear.
- `MASHED_SPAN_BOOT_SEED` (the second spanwriters knob) is not part of this step and stays OFF.

## 7. Raw data (not committed), `D:/MashedWT/D5-FLIP-SAVE-FIRSTRUN_data`

| File | SHA-256 |
|---|---|
| mashed_re_on.exe | da73061f7b761763bb06cb0b49623318a7efabbd5bcd4a641f246259a59ee2a8 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0 / K1 / K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/D2_DEFH_1.motion_diag.log | c3624e494158359ff61eafb10bfde300c15921aa5ee58d20049f6011249ce284 |
| arms/D2_DEFH_2.motion_diag.log | 8f7417e9dba2437061061778f8304cd4ae05f02ebd9930015bd1b8fac9b46ce3 |
| arms/D2_DEFH_3.motion_diag.log | 4d75bc634f6a3a7b0155ed9162f7937644597c1f57d183ddc6a181765922489d |
| tracks_score.txt | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| arms/RUNLOG.txt | 892c701bc483848d01466e715fe8d0b2e75c1f6a707a6920c8f706ec1265fdc0 |
| witness/W_DEF.bin | f003d0db7bbd0760a46439239cc25ea777e1921843a7d75f64a9636dbe7d6ecd |
| witness/W_K0.bin, witness/W_NO.bin | 604d373fc3b60fdcaf5903257ab70d62cf32611fdbadcd42226adf8d74a718dd |
| run_measure_console.log | c91efc9c6dc0406920b83afc9b71d505549f62ea3015d09f873d39ab1bc363e5 |
| run_d2short_console.log | 62fabeaaa438484fc74d49017aeff8be615ae62047781faa800af140a6b64fb0 |
| witness_console.log | 09936a0dbeb9da2ab3c7363445f08860252d79d633ca45240c36fabe16473926 |
| build.log | 497b4ce08d433b15bff5fd8af75b07ecf68fdc22174fe30a4d04651a36248e2b |

## Next

Merge in a normal `-Full` source pass (nothing moved). No reference change.
