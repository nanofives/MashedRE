# RESULT D5-KNOBS-E5-REST (2026-10-31)

Verdict: **GREEN**. All 9 named knobs are deleted: their read sites (8 of them) and their 9 `kFaith` entries
(GRID_DIR's entry had no reader since D5-KNOBS-RUNARMS-STACKS). None had a .asi seed entry. Both targets build, rva-lint
reports 0 NEW, and no `MASHED_<name>` / `MASHED_NO_<name>` string of the 9 is left in either binary. No knob stays in as a
documented opt-out: none is an input of a committed gate arm (G-CTRL, B, D2 DEFH, run13.py), SUBSTATE and INTRO_MOVIES
included (sections 1.1 and 1.2).
G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is 3/3 in the 42 s band. The 13-track
table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS).

Step: the rest batch (E5) of the seeded knobs outside class d, same pattern as D5-KNOBS-E2-PHYS-A6FIX,
D5-KNOBS-E3-GEOM-AI and D5-KNOBS-E4-MODE-REG. Branch `orch/D5-KNOBS-E5-REST` from `f50fa777b` (orch/integration with E4
merged), source commit `1b199c1bc`. No default flipped. All 9 were seeded ON in the standalone by
`Core/FaithDefaults.cpp` (`kFaith`, exe-only TU). None was in the .asi seed list `Vehicle/PhysicsChainHooks.cpp`
(`A6aAsiDefaults`: A6A_DRIVE_NORM_LUT, A6B_ACOS_CRT, A6B_NORM_LUT, A5_DRAFT_LUT only).

## 1. Audit, one line per knob (read site at `f50fa777b`)

`S` = `mashedmod/src/mashed_re/`. Seed = the `kFaith` line in `S/Core/FaithDefaults.cpp` at `f50fa777b`. Read forms:
`nz` = `e && e[0] && e[0] != '0'` on `MASHED_<name>`, `on1` = `e[0] == '1'`, `eq1` = the value is exactly `"1"`,
`set` = `getenv(...) != nullptr` (any value is ON), `int1` = `GetEnvironmentVariableA` + `atoi == 1`. No site read a
`MASHED_NO_<name>` itself, so the opt-outs were `MASHED_NO_<name>=1` (the seeder `FaithSeed` skips the knob),
`MASHED_NO_FAITH=1`, or an explicit `MASHED_<name>` value the form reads as OFF (not for `set`).
Script check: `git grep -E "MASHED_(NO_)?<name>\b"` over `scripts/`, `re/tools/`, `re/frida/`, `re/orchestrator/`
(`*.md` / `*.json` / `*.jsonl` excluded): 0 hits for all 9. "verify files" = files under `verify/` naming
`MASHED_<name>` at `f50fa777b`, with the `MASHED_NO_<name>` count in brackets. "run_arms" =
`verify/savelink_20261023/run_arms.py` line numbers. `verify/rsky_20261016/run13.py` and `verify/d2band42_20261029/*.py`
set none of the 9 (run13.py names MASHED_SUBSTATE in two comments only, :70 and :95).

| Knob | Read site (form), uses at `f50fa777b` | Seed | TU in .asi build | verify files (NO_) | run_arms stacks | Gate arms use it |
|---|---|---|---|---|---|---|
| ROUND_GRID | `S/Race/RoundGrid.cpp:43` (nz) in `RoundGrid_Enabled`, guards :51 (`RaceInit`), :59 (`RoundEnd`), :122 (`Anchor`), :257 (`SlotOrder`) | :34 | no | 171 (1) | RG / TGTRG :38-39, CCB :44 (and stacks built on CCB) | no |
| TGT3 | `S/Ai/AiTargetClosest.cpp:276` (nz) in `TargetClosest_Enabled`, uses `S/Ai/AiStandalone.cpp:1066` (inside `#ifdef MASHED_STANDALONE`), `S/D3d9Render/TrackRenderer.cpp:7091` / :7216 (step-dump columns `ret14a70,tg3_los`) | :35 | AiStandalone yes, but the use is standalone-only | 157 (1) | TGT / TGTRG :37-39, CCB :45 | no |
| GRID_DIR | none (its opt-out reader went with D5-KNOBS-RUNARMS-STACKS, `S/Race/RoundGrid.cpp:137` comment) | :35 | n/a | 157 (94) | CCBD :49, CCD :51, G :85, BTGTRGD :176 | no |
| SLOT_TYPE_LIVE | `S/D3d9Render/TrackRenderer.cpp:145` (nz) in `SlotTypeLiveEnabled`, use :149 (`aib_veh_type`) | :48 | no | 11 (48) | NOK :393 | no |
| SUBSTEP_OUTER | `S/Vehicle/VehiclePhysicsRun.cpp:2513` (on1) in `VehiclePhysics_OuterEnabled`, uses `S/D3d9Render/TrackRenderer.cpp:5682`, :5711 | :51 | no | 53 (48) | SX :412, NOSX :417 | no |
| SUBSTATE | `S/Race/SubStateCallees.cpp:72` (set) in `SubState_Enabled`, uses SubStateCallees :97 (`StoreStep`, called from `S/Ai/AiStandalone.cpp:2173` `Ai_AdvanceClock` and TrackRenderer), :523 (`SeedCd`), `S/D3d9Render/TrackRenderer.cpp:192`, :652, :5227, :5375, :5613, :7670, :10027 | :66 | SubStateCallees and AiStandalone yes (`Ai_AdvanceClock` is outside `#ifdef MASHED_STANDALONE`); .asi unseeded = OFF | 98 (13) | NOSUB :442, SUB :446 | no |
| PARTMORPH | `S/D3d9Render/TrackRenderer.cpp:4379` (eq1) in `PartMorphEnabled`, use :10378 | :72 (`#if MASHED_FLIP_D11077`) | no | 13 (5) | none | no |
| CAR_STATE0 | `S/D3d9Render/TrackRenderer.cpp:4525` (eq1) in `CarState0Enabled`, uses :4554 (`CarState0Latch`), :10190 | :72 (same block) | no | 10 (5) | none | no |
| INTRO_MOVIES | `S/exe_main.cpp:2945` (int1) in `RunBootIntroMovies` | :82 | no | 12 (1) | none | no |

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26` set stack `DEF` (empty), `D2_DEFH_1..3` set `DEFH` (`MASHED_DRIVE_HOLD_MS`
only), and `run13.py` sets none of the 9. So the gates only ever saw the seeded values, and no committed gate harness
uses any of the 9 as an input.

### 1.1 SUBSTATE: deleted, not kept as an opt-out

`MASHED_NO_SUBSTATE=1` is not an input of any gate arm or gate scorer: `merge_train.json` names `H_DEF`, `B_DEF_*`,
`D2_DEFH_1..3` and `verify/d2band42_20261029/score_d2_v2.py`, none of which sets it, and the D2 old-band scorer
(`d2rebase_20261019`, retired by the D3-SUBSTATE-FLIP train) scores logs, not arms. The only committed stacks naming it
are the report-only `NOSUB` (`run_arms.py:442`, copies in `verify/d5partmorphflip_20261030/run_arms.py:438` and
`verify/d3subph3_20261028/run_arms.py`) and `SUB` / `SUBNS`. Since D5-KNOBS-E4-MODE-REG deleted MASHED_P3_RESET_PAIR,
`NOSUB` already turned off only the sub-state machine with the reset pair on (`Race/RaceCamera.cpp` forces a reset on the
first call, not behind `SubState_Enabled`), so it no longer reproduced the pre-flip `old_ref` of
`verify/d3subflip_20261029/expected_refs.json` (G-CTRL `5943c5f1 / 306186d4 / 52052890`, B `3a03670c / 9fcdbd3a /
53778664`). A kept opt-out would have measured a state that is neither the pre-flip default nor anything gated.

Consequences, recorded:
- `NOSUB` now runs the default code (both of its names are gone), like `NOP3R` after E4. The 30 s D2 band
  (`d2rebase_20261019`), which applied only with `MASHED_NO_SUBSTATE=1`, can no longer be reproduced on a current exe.
  The 42 s band (`d2band42_20261029`) is the D2 gate and is unaffected.
- The .asi never seeded MASHED_SUBSTATE. `SubState_Enabled()` is now `true` under `MASHED_STANDALONE` and `false` in
  the .asi, with no env read, so `Ai_AdvanceClock`'s step store stays a no-op in the .asi (no .asi default moved). The one
  .asi change: an explicit `MASHED_SUBSTATE=1` no longer turns the step store on in the .asi (same kind as E3's
  `MASHED_ROTATE_LUT=1`). `MASHED_SUBSTATE_CD` (measurement) still works in the exe.
- The standalone still writes its `[substate] on: step store ...` line to `mashed_re.log` on the first query (the text
  said `MASHED_SUBSTATE=1` before; `mashed_re.log` is not a gate file).

### 1.2 INTRO_MOVIES: deleted, not kept as an opt-out (for D5-INTROMOVIES-FLIP's owner)

No committed harness needs the opt-out. The gate arms already play the boot movies on the default exe: D5-INTROMOVIES-FLIP
measured G-CTRL / B / D2 / 13 tracks with the movies ON and cited `arms/H_DEF.log (INTRO lines)`
(`verify/intromovies_20261030/RESULT_D5-INTROMOVIES-I1B.md`); the private roots and `original\` carry the 167,414-byte
`skip_intro` stubs, so a boot spends seconds there, not minutes. `verify/intromovies_2026102{8,0}/run_intro.py` set
`MASHED_INTRO_MOVIES=1` (now a no-op, same result). The LAN release ships the real movies, and the player skips them with
the bound key as in the original (FUN_00495350's skip scan). What is lost: `MASHED_NO_INTRO_MOVIES=1`,
`MASHED_INTRO_MOVIES=0` and `MASHED_NO_FAITH=1` no longer skip the movies. `MASHED_INTRO_PRESS` / `MASHED_INTRO_SHOT`
(arm-test knobs, never seeded) stay. If a dev root with the real movies ever needs an unattended skip,
`MASHED_INTRO_PRESS=1` presses the skip key on frame 1 of every movie.

### 1.3 The other report-only stacks

These now run the default code for the deleted names: `RG` / `TGTRG` / `TGT` / `CCB` (and every stack built on them) for
ROUND_GRID / TGT3, `CCBD` / `CCD` / `G` / `BTGTRGD` for GRID_DIR (already no-ops before this step), `NOK` (both halves now
gone: its MODE_LIVE half went with E4), `SX` / `NOSX`, `NOSUB` / `SUB` / `SUBNS` (`SUBNS` keeps
`MASHED_NO_SUBSTATE_SETTLE_FIRST`), and `NF` / `OLDNF` for these 9. The frozen runner copies under `verify/` are left
unedited, as in E1-E4.

## 2. Change (commit `1b199c1bc`, 13 files, 113 insertions, 197 deletions)

- `Core/FaithDefaults.cpp`: the 9 entries and the `MASHED_FLIP_D11077` macro / `#if` block removed, with a note naming
  them. Left in `kFaith`: AI_COUNTDOWN_CLOCK, AI_CONTACT_EVENT, CC_PC24, A5_DRAFT_LUT, A6B_ACOS_CRT, A6B_NORM_LUT,
  RFLAME_SORT, MUSIC_NO_RACE_CDAUDIO, SAVE_FIRSTRUN_WRITERS (other steps / class d).
- **ROUND_GRID** (`Race/RoundGrid.cpp/.h`): `RoundGrid_Enabled()` deleted with its four guards. FUN_0040fef0's
  DAT_0063ba80 = 0 (0x0040fef0), FUN_00410860's survivor node (0x004108c5), the FUN_00409290 snap + FUN_00408b00 anchor
  and FUN_0040e590's per-car slot order always run. `RoundGrid_SlotOrder` now always returns true. The
  `GridDir_Anchor` fallback in TrackRenderer stays (it runs only when `RoundGrid_Anchor` finds no record).
- **TGT3** (`Ai/AiTargetClosest.cpp/.h`, `Ai/AiStandalone.cpp`, `TrackRenderer.cpp`): `TargetClosest_Enabled()`
  deleted. FUN_00414a70 (call 0x00416394..0x0041640f) runs whenever mode is 0 and v is 0..3. The AI step dump always
  writes the `ret14a70,tg3_los` columns (as the seeded default did, so `H_DEF.step.csv` keeps its layout).
- **SLOT_TYPE_LIVE** (`TrackRenderer.cpp`): `SlotTypeLiveEnabled()` deleted. `aib_veh_type` reads the live slot table
  (0x0040e474 / 0x0040e47a) for v 0..3; the stand-in 0 / 2 is gone (kept only for v outside 0..3, as before).
- **SUBSTEP_OUTER** (`Vehicle/VehiclePhysicsRun.cpp/.h`, `TrackRenderer.cpp`): `VehiclePhysics_OuterEnabled()` deleted.
  The A4 pre-phase always records each AI car's io, and the player step always goes through
  `VehiclePhysics_StepCarsOuter` when it has more than one car (FUN_004709a0's substep-outer / car-inner order); the
  `StepPlayer` fallback for a single car or a bad list stays.
- **SUBSTATE** (`Race/SubStateCallees.cpp/.h`, `TrackRenderer.cpp`, `Ai/AiStandalone.cpp` comment): the env read is
  gone (section 1.1). In TrackRenderer every `SubState_Enabled()` test is removed: FUN_0040e350 returns DAT_0063ba8c when
  non-zero, race setup always runs FUN_004425d0 and DAT_005f29b8 = 100000 (0x004100d3), the sub-state block runs on every
  countdown frame, the FUN_0041d930 tail runs every race frame, every round writes DAT_0063ba8c = 0, and the HUD reads the
  real DAT_0063ba8c when non-zero. The knob-OFF HUD start-light adapter (the 113-tick stand-in zeroing, U-9704 note) is
  deleted (29 lines).
- **PARTMORPH / CAR_STATE0** (`TrackRenderer.cpp/.h`): `PartMorphEnabled()` and `CarState0Enabled()` deleted.
  FUN_0041f770 (via FUN_00420870) and the record-state-0 producer (CarState0Latch / CarState0Apply) always run. The
  diagnostics `MASHED_DBG_PARTMORPH`, `MASHED_DBG_CAR_STATE0`, `MASHED_DBG_PARTSTATE0` stay.
- **INTRO_MOVIES** (`exe_main.cpp`): `RunBootIntroMovies` returns early only without a device.
- **GRID_DIR**: only the `kFaith` entry (no reader).

Built binary check (Python `bytes.count`): no `MASHED_<name>\0` or `MASHED_NO_<name>\0` for the 9 names in
`mashed_re.exe` (sha256 `e5b7f049`) or `mashed_re_dev.asi` (`f6154e39`). The only `PARTMORPH\0` / `CAR_STATE0\0` hits
in the exe are the tails of the diagnostics `MASHED_DBG_PARTMORPH` / `MASHED_DBG_CAR_STATE0`.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` gave `=== Build OK ===` for both targets, 0 `error C` /
`LNK` lines. The shared `.asi` was not deployed. rva-lint: `scripts/lint_rva_bodies.py` rc 0,
`48 finding(s): CROSS-TARGET=33, DUP-IN-TARGET=15 | allowlisted=48 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` on the worktree's `mashedmod/build/mashed_re.exe`, private root
`D:/MashedWT/D5-KNOBS-E5-REST_root`, each batch run as `mashed_lock.py run` (D2: `run --solo`) around `test-slot`.
13 tracks: `verify/rsky_20261016/run13.py run`, root `D:/MashedWT/D5-KNOBS-E5-REST_root13`, under
`mashed_lock.py run --solo`, then scored with `--cite verify/rres_20261016/cites_rres.json`. Driver: E4's `gates.sh`
with `STEP` changed. Refs from `re/orchestrator/merge_train.json` at `f50fa777b`.

| Gate | This exe (`mashed_re.exe` sha256 `e5b7f049`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3 arms at 3/3 (`vsum_b.txt` sha256 `6b0c8fea`, byte-identical to E3's and E4's) | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1917.10 / 1917.10 / 1919.62, slip1500 0.1930 x3, all `in`, H1 398/400. motion_diag 1640 / 1641 / 1643 lines (min 1600), no re-run needed | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` | 39 rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, file sha256 `2431afb1` | `2431afb1` | same |

Every gate ran on the first attempt with no piz collision (G-CTRL + B 19:55-20:06, D2 20:06-20:11, 13 tracks
20:11-20:22). Committed copies are in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-E5-REST_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9
- `arms/D2_DEFH_2.motion_diag.log` e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258
- `arms/D2_DEFH_3.motion_diag.log` 8f7417e9dba2437061061778f8304cd4ae05f02ebd9930015bd1b8fac9b46ce3
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 1cafaa03a5905a9c9ad2ddf8775ad9dfc9d6a890c5b1ad7697147d49fb4a1570
- `score_d2.txt` 3c58fd28e85e943027c79ea8ddd5562a8883de7ebfbcc4e7da436307c34c9214
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` 814398384853e94fe232a09438125b79cc0d73a997c5065112c7175367fa3a76
- `lint.txt` 07661a25b4c6dae1a1e5d47126587e614e09db5c921a168710354053e5378adf
- `gates_run.log` 1eee4748275403f6a678da417d6b01956733b7aca4508de5bb98d554b4e4abdd
- `gates.sh` ed616feff0de639f05f3e3812ebdf70b8225bec8041ccced47e7b43cf7bbe188
- `mashed_re.exe` e5b7f049bf9e8f9a7a9daae9aafb417061e06f5096a81cce536d42f496730b0d
- `mashed_re_dev.asi` f6154e3951bf0d1d6032827897ac5662a8abd19aed0b3677e1ed5ee4fe9610ac
- `tracks/` (13 track folders, 115 MB)

## 5. For the lead

- No `.ps1` / `.bat` edit needed, so there is no proposed diff.
- **.asi.** No .asi default moved. The edited TUs in `asi_sources.rsp` are `Ai/AiStandalone.cpp` (the TGT3 edit is
  inside `#ifdef MASHED_STANDALONE`, the SUBSTATE edit is a comment) and `Race/SubStateCallees.cpp` (`SubState_Enabled`
  is `false` in the .asi, as its unseeded value). No hook body's C-level moves, and no tracker row was touched.
- **SUBSTATE / INTRO_MOVIES decisions**: sections 1.1 and 1.2. Neither is kept as a documented opt-out.
- **kFaith merge.** This branch edits the same `kFaith` region as any other knob step in flight. The resolution is the
  union of the deletions.
- **Residue (not a knob read, left in place).** Comments that describe the old knob-OFF state still name
  `MASHED_SUBSTATE` (TrackRenderer.cpp phase-3 / camera notes, `Race/CameraApply.h:34`, `Race/CameraEntries.cpp:270`,
  `Race/SubStateCallees.cpp:26/35`, `exe_main.cpp:1817`) and `MASHED_GRID_DIR` (`Race/RoundGrid.cpp:137`,
  `Vehicle/VehiclePhysicsRun.cpp:1616`). The countdown stand-in in `TrackRenderer::Update` (`if (countdown_ > 0.f)`
  after the sub-state block: the countdown decrement, `Ai_AdvanceCountdownClock`, `LaunchRev_PreRaceTick`, the
  countdown-spawn pieces) is now unreachable, because the sub-state block runs on every `countdown_ > 0` frame and returns
  on every path. It was already unreachable under the seed. It is left in place for a follow-up dead-code step, not
  deleted here, so this diff stays a pure opt-out deletion.
- **Refresh.** Before reporting, `orch/integration` was still at `f50fa777b` (this branch's base), so there was nothing
  to merge and no rebuild or gate re-run was needed. The gated exe is the branch head's build.
