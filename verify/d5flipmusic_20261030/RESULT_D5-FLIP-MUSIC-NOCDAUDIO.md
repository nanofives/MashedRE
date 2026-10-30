# RESULT D5-FLIP-MUSIC-NOCDAUDIO: MASHED_MUSIC_NO_RACE_CDAUDIO default ON holds every default gate

Verdict: **GREEN**. Built with `MASHED_MUSIC_NO_RACE_CDAUDIO` seeded ON (one new `kFaith` row), the exe gives G-CTRL
`8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`, D2 3/3 in the O42 band, and a 13-track table byte-identical to
`run13_reference` (`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1). **Nothing moved**, so no reference is
re-based. Per L-66 (a) (Mariano 2026-10-30) the flip is committed on this branch.

Branch `orch/D5-FLIP-MUSIC-NOCDAUDIO` off `orch/integration` @ `e23f260b7`. LIVING-PC, 2026-10-30. Private roots
`D:/MashedWT/D5-FLIP-MUSIC-NOCDAUDIO_root` (arms) and `..._root13` (run13). No MASHED.exe launched, no .asi
deployed, no .ps1 / .bat edited, no tracker touched, merge_train.json not edited.

## 1. What lands

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp`: row `{ "MUSIC_NO_RACE_CDAUDIO", "1" }` at the end of `kFaith`, with a
  comment naming L-66 (a). The seed loop gives the opt-outs: `MASHED_NO_MUSIC_NO_RACE_CDAUDIO=1` skips the row,
  `MASHED_MUSIC_NO_RACE_CDAUDIO=0` is left as given and the site reads it as OFF (`b[0] == '1'`, `AudioEngine.cpp`
  `NoRaceCdaudio`), `MASHED_NO_FAITH=1` drops every row.
- `mashedmod/src/mashed_re/Audio/AudioEngine.cpp`: the site comment says default ON and names the opt-outs. No code change
  at the site. `NoRaceCdaudio()` caches the knob in a function-local static read at the first `MusicSetState` call, after
  the `init_seg(lib)` seed.
- Standalone only: `FaithDefaults.cpp` is `#ifdef MASHED_STANDALONE`, so the dev `.asi` keeps the knob OFF unless set.
- Why it is faithful (from `verify/wsjmusic_20261026/RESULT_D4-WSJ-MUSIC.md`): the original's race tick `FUN_00466b50`
  case 3/7 starts no music voice, no reader of the `CDAUDIO` string `0x00604170` exists, and the three
  `pcdics\` path builders (`0x004625ec`, `0x00462991`, `0x0045d470`) cannot yield `cdaudio.rws`.

## 2. Exe

Worktree build, `MASHED_NO_DEPLOY=1`, rva-lint NEW=0 (16 CROSS-TARGET, all allowlisted). `mashed_re_on.exe` SHA-256
`54fd54d78fa5ce05d73abc9d4985b33eb4a09e074149ad397598c375c44cd6ea`. This is the committed source: the measured tree and the
flip commit are the same two files.

## 3. Gates (outputs: `GATES_MEASURE.txt`)

Runner `run_measure.sh` (this folder) = `verify/d5partmorphflip_20261030/run_measure.sh` with the step name changed: the
merge train's Gate-Default + Gate-Full, same scripts and arguments, one test-slot, arms through `mashed_lock.py run`
(D2 `--solo`), run13 inside a `solo-begin` window. Quiet check before start: 2 test-slot holders outside Mashed (SimGolf
pytest), the step's limit.

| Gate | Arms | Result | Reference | |
|---|---|---|---|---|
| G-CTRL | H_DEF | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref | PASS (first run) |
| B | B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref | PASS |
| (b) vsum | B arms | 3/3 arms, all cars A PASS | | PASS |
| (e) ai_speed_env | B_DEF_K0 | v1 / v2 / v3 (e) PASS, launch 1425.7 / 2052.5 / 2055.0 | e_launch | PASS, launch same |
| D2 | D2_DEFH_1..3, `score_d2_v2.py port` | drvmed 1917.10 x3, slip1500 0.1930 / 0.1930 / 0.1933, slip2000 0.2515 x3, all `in` | O42 band | 3/3 PASS |
| run13 | 13 tracks, stock budget | table 2431afb1, `^NN_` row diff empty, INSTRUMENT GATES: PASS, 13/13 on attempt 1 | tracks_score_sub_p3.txt 2431afb1 | identical |

D2 arm lengths 1641 / 1641 / 1639 motion_diag lines (min 1600), no rerun. D2_DEFH_2 and D2_DEFH_3 motion_diag logs are
byte-identical to D5-PARTMORPH-MEASURE's (`e078ab49`, `e1029fa7`).

## 4. Why nothing moved

Every gate arm runs `MASHED_MUTE=1` (`run_arms.py` base env, `run13.py:84`). Under mute `AudioEngine` skips DirectSound
init (`AudioEngine.cpp:214`), `g_ds` stays null, and `MusicStart` returns before any decode (`if (!g_ds || g_music) return;`).
So with the knob OFF the race `cdaudio.rws` start was already a no-op on these arms, and with it ON `MusicSetState`
returns after `MusicStop()`. Neither path touches sim, AI, physics or draw state. The gates therefore cannot see the
flip, and they show it changes nothing else.

## 5. Witness that the default seeds the knob

**Runtime witness: not run.** The three audio-event arms (`run_port.py` in this folder, a copy of
`verify/wsjmusic_20261026/run_port.py` with EXE / ROOT / OUT changed: W_DEF knob unset, W_KNOB0 `ARM_KNOB=0`, W_NO
`ARM_NOKNOB=1`, menu path, 80 s) were queued behind the `mashed_machine` lock: first another step's solo window
(D5-KNOBS-E1-PHYS-A4A5), then the merge-train run13 solo window (`merge-train13:20261030_181349`). No arm booted. On the
lead's decision the waiters (my PIDs only) were killed and the step closes on the four required gates.

Static evidence that the default seeds the knob:
- `Core/FaithDefaults.cpp:14` `#pragma init_seg(lib)`: `FaithSeed` runs before every user TU initializer. For each `kFaith`
  row it calls `_putenv_s("MASHED_" + name, value)` (`FaithDefaults.cpp:98`) unless `MASHED_<name>` is already set or
  `MASHED_NO_<name>` is set. The new row is `{ "MUSIC_NO_RACE_CDAUDIO", "1" }` (`FaithDefaults.cpp:82`).
- `Audio/AudioEngine.cpp:435-438` `NoRaceCdaudio()` reads `MASHED_MUSIC_NO_RACE_CDAUDIO` with `GetEnvironmentVariableA` into a
  function-local static on first call. `_putenv_s` updates the process environment, which `GetEnvironmentVariableA`
  reads (the TU header comment, `FaithDefaults.cpp:10-11`). The first call is from `MusicSetState`
  (`AudioEngine.cpp:447`), at run time, after the seed.
- `FaithDefaults.cpp` is in the exe link (`mashedmod/exe_sources.rsp:296`).
- Binary: the string `MUSIC_NO_RACE_CDAUDIO` occurs 2 times in `mashed_re_on.exe` (the site's `MASHED_...` name plus the
  `kFaith` row) against 1 time in D5-PARTMORPH-MEASURE's exe (`cb1084f5`, site only).

The same seed path carries every other `kFaith` row, and runtime witnesses exist for it (D5-PARTMORPH-FLIPPREP F2: the
`MASHED_FLIP_D11077` rows seeded both knobs, measured by their law logs). [UNCERTAIN] No run of this exe logged a
`MusicSetState -> 2 (no cdaudio)` line or a missing `music_start` event at race entry. Evidence needed: W_DEF from
`run_port.py` here (expect no `music_start cdaudio.rws` after `music_state 2`) and W_KNOB0 / W_NO (expect it). The merge
train or a later step can run it in about 5 minutes of machine time.

## 6. Raw data (not committed), `D:/MashedWT/D5-FLIP-MUSIC-NOCDAUDIO_data`

| File | SHA-256 |
|---|---|
| mashed_re_on.exe | 54fd54d78fa5ce05d73abc9d4985b33eb4a09e074149ad397598c375c44cd6ea |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0 / K1 / K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/D2_DEFH_1.motion_diag.log | e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258 |
| arms/D2_DEFH_2.motion_diag.log | e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258 |
| arms/D2_DEFH_3.motion_diag.log | e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a |
| tracks_score.txt | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| arms/RUNLOG.txt | 9aaa4cbff0bd30ffee3f25b1f6ac2b702759bb52a69ca55a63e4de0280dcce57 |
| RUNLOG_measure.txt | f828050cdd3291fadc54ab893d0b214e1d0e9139b5f96c10c96293b4ae2de1d2 |
| build.log | 44c0f65d2003b92d690c116f9bcf0966bcf45bcc7c537a83e18dba2aa29d759e |
