# RESULT D5-FLIP-RFLAME-SORT: MASHED_RFLAME_SORT default ON holds every default gate

Verdict: **GREEN**. Built with `MASHED_RFLAME_SORT` seeded ON (one new `kFaith` row), the exe gives G-CTRL
`8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`, D2 3/3 in the O42 band, and a 13-track table byte-identical to
`run13_reference` (`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1). **Nothing moved**, so no reference is
re-based. Per L-66 (a) (Mariano 2026-10-30) the flip is committed on this branch.

**The gates do not exercise the sorter.** A reachability witness (section 4) shows no slot holds R_FLAME (code 16) at any
frame of the G-CTRL or B window, so every spark record stays dead and the sort skips all of them. The unchanged gates
show the flip disturbs nothing else. They are not evidence that the sort is right. That evidence is d3rflame's replay
(`verify/d3rflame_20261024/RESULT_D3-CF-RFLAME.md`: SORT CHECK 295 / 295 post-sort slot orders equal to the original,
P5 pool bit-equal with the original's births and reference).

Branch `orch/D5-FLIP-RFLAME-SORT` off `orch/integration` @ `af12fc6c5` (descends from `e23f260b7`). LIVING-PC, 2026-10-30.
Private roots `D:/MashedWT/D5-FLIP-RFLAME-SORT_root` (arms) and `..._root13` (run13). No MASHED.exe launched, no .asi
deployed, no .ps1 / .bat edited, no tracker touched, merge_train.json not edited.

## 1. What lands

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp:78`: row `{ "RFLAME_SORT", "1" }` with a comment naming L-66 (a), placed
  after the D-11077 `#endif` and before the INTRO_MOVIES block. That keeps it out of the hunk D5-FLIP-MUSIC-NOCDAUDIO
  (`9586d5b91`, not yet in `orch/integration`) adds after the INTRO_MOVIES row, so the two flips merge without a text
  conflict. The seed loop gives the opt-outs: `MASHED_NO_RFLAME_SORT=1` skips the row, `MASHED_RFLAME_SORT=0` is left as
  given and the site reads it as OFF (`*e != '0'`, `PowerupEffects.cpp:383`), `MASHED_NO_FAITH=1` drops every row.
- `mashedmod/src/mashed_re/Powerup/PowerupEffects.cpp:373-378`: the site comment says default ON and names the opt-outs.
  No code change at the site. `RFlameSortOn()` caches the knob in a function-local static on its first call, from
  `RFlame_Tick` at run time, after the `init_seg(lib)` seed.
- `verify/savelink_20261023/run_arms.py:455`: stack `PUD` = `MASHED_PU_STEPDUMP=@OUT@/@NAME@.pustep.csv` (a log only), for the
  witness arms `H_PUD` / `B_PUD_K0`.
- Standalone only: `FaithDefaults.cpp` is `#ifdef MASHED_STANDALONE`, so the dev `.asi` keeps the knob OFF unless set.
- Why it is faithful: the original's R_FLAME tick `FUN_0045ae80` calls the per-group sorter `FUN_0045ac40` ahead of each
  group's 5-spark scan (CALL 0x0045ac40 @0x0045af41). The port's `RFlame_SortFive` reproduces its post-sort order on the
  original's own records (d3rflame P1, 295 / 295).

## 2. Exe

Worktree build, `MASHED_NO_DEPLOY=1`, rva-lint NEW=0 (16 CROSS-TARGET, all allowlisted). `mashed_re_on.exe` SHA-256
`0daf8b9478fefc76da3e4a583276be2b80cfd7e6a9107d364f0d36bfb3935147`. The measured tree is the committed source.
The string `RFLAME_SORT` occurs 2 times in the exe (the site's `MASHED_RFLAME_SORT` and the `kFaith` row).

## 3. Gates (outputs: `GATES_MEASURE.txt`)

Runner `run_measure.sh` (this folder) = `verify/d5flipmusic_20261030/run_measure.sh` (branch `orch/D5-FLIP-MUSIC-NOCDAUDIO`)
with the step name changed and a `witness` stage added: the merge train's Gate-Default + Gate-Full, same scripts and
arguments, one test-slot, arms through `mashed_lock.py run` (D2 `--solo`), run13 inside a `solo-begin` window. Quiet check
before start: 1 test-slot holder outside Mashed (SimGolf pytest). The arms queued behind two merge-train solo windows
(`merge-train13:20261030_181349`, `merge-train:20261030_185615`, `merge-train13:20261030_185615`) and ran after them.

| Gate | Arms | Result | Reference | |
|---|---|---|---|---|
| G-CTRL | H_DEF | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref | PASS (first run) |
| B | B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref | PASS |
| (b) vsum | B arms | 3/3 arms, all cars A PASS | | PASS |
| (e) ai_speed_env | B_DEF_K0 | v1 / v2 / v3 (e) PASS, launch 1425.7 / 2052.5 / 2055.0 | e_launch | PASS, launch same as D5-FLIP-MUSIC |
| D2 | D2_DEFH_1..3, `score_d2_v2.py port` | drvmed 1917.10 / 1919.62 / 1917.10, slip1500 0.1933 x3, slip2000 0.2515 x3, all `in` | O42 band | 3/3 PASS |
| run13 | 13 tracks, stock budget | table 2431afb1, `^NN_` row diff empty, INSTRUMENT GATES: PASS, 13/13 on attempt 1 | tracks_score_sub_p3.txt 2431afb1 | identical |

D2 arm lengths: first pass 1639 / 1638 / 1591 motion_diag lines. D2_DEFH_3 was under 1600 and was re-run once (the
runner's rule, `d2-arms-are-wall-clock-bound`), giving 1639. D2_DEFH_1 and the re-run D2_DEFH_3 are byte-identical to each
other and to D5-FLIP-MUSIC-NOCDAUDIO's D2_DEFH_3 (`e1029fa7`). D2_DEFH_2 `fe38081b` differs in bytes (wall-clock bound arm)
and is in band.

## 4. Does any gate reach the sorter

Witness arms `H_PUD` and `B_PUD_K0` = `H_DEF` / `B_DEF_K0` plus `MASHED_PU_STEPDUMP` only. Their gate files equal the
default arms' (`H_PUD` 8284be46 / ba407a16 / 31fe043e, `B_PUD_K0` 1d5bf4f9), so the log does not perturb the run.
`pu_reach.py` (this folder) counts, per slot, the dump rows where the slot held each power-up code and where FIRE ran
(`PowerupSystem.cpp` `DumpRow`, columns `code_pre` / `code_post` / `fire_modes`):

| Arm | rows / frames | held | FIRE rows | R_FLAME rows |
|---|---|---|---|---|
| H_PUD (G-CTRL window) | 26852 / 13479 | slot 1 OIL (13373), slot 2 PMINE (13479) | 0 | 0 |
| B_PUD_K0 (B window) | 452 / 279 | slot 1 OIL (173), slot 2 PMINE (279) | 0 | 0 |

No slot holds code 16 at any frame, so `RFlame_Fire` / `RFlame_Step` emit no spark and every `s_sparks` record keeps
`alive == 0`. With the knob ON, `RFlame_Tick` (`PowerupEffects.cpp:476`) still calls `FlameSortRef` (`TrackRenderer.cpp:6467`,
reads the camera through the `const` getter `TrackRenderer::camera`) and `RFlame_SortFive` once per group, and
`RFlame_SortFive` skips every record that is not alive (`PowerupEffects.cpp:421`, `:424`). The ON path is a no-op on these
arms, and the gates confirm it.

Not measured: R_FLAME on the D2 arms and the run13 tracks (no step dump on those runners). Both are byte-identical or
in band, so either the sorter sees no live spark there or its order change reaches no gated output. [UNCERTAIN] which one,
no dump on those runs.

No live arm in this harness makes a car hold R_FLAME: `MASHED_PU_FORCE` only takes `nobox` (`PowerupSystem.cpp:56-57`),
there is no forced-code knob. A live witness of the sort (a flame scenario, knob ON vs `MASHED_NO_RFLAME_SORT=1`) is
**not run: no scenario in the gate harness reaches R_FLAME**. It would need a forced-pickup scenario or an original
capture with flames, and it is not one of the four required gates.

## 5. Witness that the default seeds the knob

**Runtime witness: not run** (no log line echoes the knob, checked across the arm logs). Static evidence:
- `Core/FaithDefaults.cpp:14` `#pragma init_seg(lib)`: `FaithSeed` runs before every user TU initializer. For each `kFaith`
  row it calls `_putenv_s("MASHED_" + name, value)` unless `MASHED_<name>` is already set or `MASHED_NO_<name>` is set. The
  new row is `{ "RFLAME_SORT", "1" }` (`FaithDefaults.cpp:78`).
- `Powerup/PowerupEffects.cpp:379-386` `RFlameSortOn()` reads `MASHED_RFLAME_SORT` with `std::getenv` into a function-local
  static on first call. `_putenv_s` updates the CRT copy `std::getenv` reads (`FaithDefaults.cpp:10-11`).
- `FaithDefaults.cpp` is in the exe link (`mashedmod/exe_sources.rsp:296`).
- The same seed path carries every other `kFaith` row, with runtime witnesses for it (D5-PARTMORPH-FLIPPREP F2).

## 6. Raw data (not committed, `D:/MashedWT/D5-FLIP-RFLAME-SORT_data`)

SHA-256 of every cited raw file is in `GATES_MEASURE.txt` § raw SHA-256 (`raw_sha256.txt` there): `mashed_re_on.exe`,
`arms/H_DEF.{step.csv,gates.csv,ptrace.log}`, `arms/B_DEF_K{0,1,26}.step.csv`, `arms/H_PUD.*`, `arms/B_PUD_K0.*`,
`arms/{H_PUD,B_PUD_K0}.pustep.csv` (`02173d5c...`, `ee8d2419...`), `arms/D2_DEFH_{1,2,3}.motion_diag.log`,
`tracks_score.txt` (2431afb1). Full runner log `measure.log`, arm log `arms/RUNLOG.txt`.

## 7. Merge note

Nothing moved, so this merges in a normal source pass with no ref change. It touches `FaithDefaults.cpp` next to
D5-FLIP-MUSIC-NOCDAUDIO's row but not in the same hunk (section 1).
