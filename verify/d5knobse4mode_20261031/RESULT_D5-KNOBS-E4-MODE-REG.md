# RESULT D5-KNOBS-E4-MODE-REG (2026-10-31)

Verdict: **GREEN**. All 10 named knobs are deleted: their read sites, their 10 `kFaith` entries, and no .asi seed
entry (none of the 10 had one, and every read site is in an exe-only TU). Both targets build, rva-lint reports 0 NEW,
and none of the 10 names occurs as a bare `MASHED_<name>` / `MASHED_NO_<name>` string in either binary. G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is 3/3 in the 42 s band. The
13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS).
No knob is an input of a gate arm, so none stays in as a documented opt-out.

Step: modes / registry batch (E4) of the seeded knobs outside class d, same pattern as D5-KNOBS-E2-PHYS-A6FIX and
D5-KNOBS-E3-GEOM-AI. Branch `orch/D5-KNOBS-E4-MODE-REG` from `e06b1df60` (orch/integration with E3 merged), source
commit `6990d325a`. No default flipped. All 10 were seeded ON in the standalone by `Core/FaithDefaults.cpp` (`kFaith`,
exe-only TU). None was in the .asi seed list `Vehicle/PhysicsChainHooks.cpp` (`A6aAsiDefaults`).

## 1. Audit, one line per knob (read site at `e06b1df60`)

`S` = `mashedmod/src/mashed_re/`. Seed = the `kFaith` line in `S/Core/FaithDefaults.cpp` at `e06b1df60`. Read forms:
`nz` = `e && e[0] && e[0] != '0'` on `MASHED_<name>`, `on1` = `e && e[0] == '1'`, `set` = `getenv(...) != nullptr`
(any value, `0` included, is ON). No site read a `MASHED_NO_<name>` itself, so the opt-outs were `MASHED_NO_<name>=1`
(the seeder `FaithSeed` skips the knob), `MASHED_NO_FAITH=1`, or `MASHED_<name>=0` / another value (not for `set`).
Script check: `git grep -E "MASHED_(NO_)?<name>\b"` over `scripts/`, `re/tools/`, `re/frida/`, `re/orchestrator/`
(`*.md` / `*.json` / `*.jsonl` excluded): 0 hits for all 10, except `PATHPROG` in `re/tools/dual_copy_allowlist.txt`
(text of the 0x004177b0 row, not a reader). "verify files" = files under `verify/` naming `MASHED_<name>`, with the
`MASHED_NO_<name>` count in brackets. "run_arms" = `verify/savelink_20261023/run_arms.py` line numbers.
`verify/rsky_20261016/run13.py` and `verify/d2band42_20261029/*.py` name none of the 10.

| Knob | Read site (form), uses | Seed | TU in .asi build | verify files (NO_) | run_arms stacks | Gate arms use it |
|---|---|---|---|---|---|---|
| MODE7 | `S/Ai/AiTargetClosest.cpp:460` (nz) in `ModeSeven_Enabled` (AND `PickupCallbacks_Enabled`), use `S/Ai/AiStandalone.cpp:1088` (inside `#ifdef MASHED_STANDALONE` :1035-1112) | :42 | no (exe-only TU; the AiStandalone use is standalone-only) | 86 (1) | M7 :177 (TGTM7, TGTEM7, EM7), M4SET :193 (EXL7AFSM7*, FULLM7T, FULLRL*) | no |
| MODE_LIVE | `S/D3d9Render/TrackRenderer.cpp:233` (nz), uses :237 / :241 (`aib_round_type` / `aib_game_mode_fd0`), :7808 (rule into 0x007f0fd0); `S/exe_main.cpp:2127` (nz, launch mode into 0x0067e9fc), :9790 (nz, `MASHED_ROUND` route: unset `MASHED_GAME_MODE` writes 3) | :47 | no | 9 (47) | NOK :393 | no |
| P3_RESET_PAIR | `S/D3d9Render/TrackRenderer.cpp:5340` (nz, the FUN_0040fc00 top stores 0x0040fc57), `S/Race/RaceCamera.cpp:256` (nz, the reset takes the pair-switch branch, 0x00446520) | :65 | no | 23 (12) | NOSUB / NOP3R :442, SUB :446 (SUBNS, SUBH) | no |
| PATHPROG | `S/Race/PathNodes.cpp:342` (set), `PathProg_Enabled` uses `S/Race/PathProgStep.cpp:225, :233, :244, :260, :311, :349`, `S/Ai/AiTargetClosest.cpp:279`, `S/Ai/AiLeafTargets.cpp:342`, `S/Race/RoundGrid.cpp:49`, `S/D3d9Render/TrackRenderer.cpp:8727` | :34 | no | 208 (1) | PP / TGT / RG / TGTRG :37-39, CCB :44, TPPT :92, plus every stack built on TGT | no |
| PICKUPS_ORIG | `S/Gameplay/PickupTick.cpp:233` (nz, AND `Registry_Enabled`), uses PickupTick :323 / :465 / :472, `S/Gameplay/PickupPoolSpawn.cpp:145` / :154, `S/D3d9Render/PickupIconCube.cpp:64`, `S/Gameplay/PickupGlow.cpp:53` | :41 | PickupPoolSpawn yes, but its uses sit in the `#ifdef MASHED_STANDALONE` branches; the .asi branch always returned 0x0068b198 / 0x0068b9a8 and is unchanged | 99 (1) | M4SET :193 | no |
| REASSERT_KEEP_NORMAL | `S/Vehicle/VehiclePhysicsRun.cpp:429` (on1), use :440 | :36 | no | 111 (1) | KN :108 (SPLKN) | no |
| REGISTRY | `S/Race/WorldRegistry.cpp:164` (nz), uses :171 / :176 / :213, `S/Race/CarRegistrant.cpp:51`, `S/Gameplay/PickupTick.cpp:236` | :41 | no | 101 (1) | M4SET :193 | no |
| REG_CALLBACKS | `S/Gameplay/PickupTick.cpp:314` (nz, AND PICKUPS_ORIG AND REG_CARS), uses `S/Race/WorldRegistry.cpp:79`, `S/D3d9Render/PickupField.cpp:263` / :297, `S/Ai/AiTargetClosest.cpp:463` | :42 | no | 90 (1) | M4SET :193 | no |
| REG_CARS | `S/Race/CarRegistrant.cpp:48` (nz, AND REGISTRY), uses :55, `S/Gameplay/PickupTick.cpp:317` | :42 | no | 93 (1) | M4SET :193 | no |
| REG_LATE | `S/D3d9Render/TrackRenderer.cpp:266` (nz), uses :6261 / :6263 (late site) and :8709 (early site) | :43 | no | 72 (1) | RL :203 (FULLRL, FULLRLR2/3; `RL` is redefined at :385, so the last one wins) | no |

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26` set stack `DEF` (empty) and `D2_DEFH_1..3` set `DEFH` (`MASHED_DRIVE_HOLD_MS`
only), and `run13.py` sets none of the 10. So the gates only ever saw the seeded values, and no committed gate harness
uses any of the 10 as an input.

Report-only stacks that now run the default code for these names: `NOK` (its `MASHED_NO_SLOT_TYPE_LIVE` half still
acts), `NOP3R`, `KN`, `SPLKN`, `RL` / `FULLRL*`, `M7` / `M4SET` and the stacks built on them, `PP`, `TGT` / `RG` /
`TGTRG` / `CCB` / `TPPT` for their `MASHED_PATHPROG` name (their `MASHED_TGT3` / `MASHED_ROUND_GRID` /
`MASHED_AI_COUNTDOWN_CLOCK` names still act), and `NF` / `OLDNF` for these 10. **`NOSUB`** (`MASHED_NO_SUBSTATE=1
MASHED_NO_P3_RESET_PAIR=1`) now turns off only the sub-state machine: the reset pair stays on. Its RaceCamera half is
not behind `SubState_Enabled` (`RaceCamera.cpp:194` forces `force_reset` on the first call), so NOSUB no longer
reproduces the pre-flip `old_ref` of `verify/d3subflip_20261029/expected_refs.json` by construction. That file is the
flip's evidence, not a merge-train input. The frozen runner copies under `verify/` are left unedited, as in E1-E3.

## 2. Change (commit `6990d325a`, 29 files, 130 insertions, 414 deletions)

- `Core/FaithDefaults.cpp`: the 10 entries removed with a note naming them. `ROUND_GRID`, `CC_PC24`,
  `SLOT_TYPE_LIVE` and `SUBSTATE`, which shared a line with a deleted name, stay (E5's set). The SUBSTATE comment now
  names only `MASHED_NO_SUBSTATE=1`.
- **MODE7** (`Ai/AiTargetClosest.cpp/.h`, `Ai/AiStandalone.cpp`): `ModeSeven_Enabled()` deleted; FUN_00414c30 runs at
  LAB_004163b7 (call 0x004163c3) whenever `mode == 0` and `v` is 0..3, as before under the seed.
- **REGISTRY / REG_CARS / PICKUPS_ORIG / REG_CALLBACKS** (`Race/WorldRegistry.cpp/.h`, `Race/CarRegistrant.cpp/.h`,
  `Gameplay/PickupTick.cpp/.h`, `Gameplay/PickupPoolSpawn.cpp`, `Gameplay/PickupGlow.cpp`,
  `D3d9Render/PickupIconCube.cpp/.h`, `D3d9Render/PickupField.cpp/.h`, `D3d9Render/TrackRenderer.cpp`):
  `Registry_Enabled`, `CarsReg_Enabled`, `PickupsOrig_Enabled`, `PickupCallbacks_Enabled` and `IconCube_Enabled` are
  deleted with every guard on them. FUN_00484c90 / FUN_00485070 / FUN_00484cf0, the car registrant FUN_0041f710, the
  pickup tick FUN_00459000 and the callback FUN_00458920 run unconditionally. The standalone pickup pool is the
  original's 0x0068b198 / count 0x0068b9a8 (inside `g_b17_low_arena_pad`), and the private `s_pool` / `s_poolCount`
  are gone. `PickupField::Update` keeps only the M3 branch (mirror pool entry +0x20 into the orb's `active`, now
  `void`), and the field's own collection loop, `PickupField::CollectAt`, the slots 1..3 `CollectAt` loop in
  `TrackRenderer::TickPowerupDispatch` and the trace helper `PickupCb_NoteFieldCollect` (the `PF` rows of
  `MASHED_PKCB_TRACE`, only written on the knob-OFF path) are deleted. The `CNT` trace row keeps its `pfCollects`
  column, now always 0.
- **REG_LATE** (`TrackRenderer.cpp`): `RegLate()` deleted. FUN_0040da50's registrant (0x0040fc75), FUN_0045bba0's
  FUN_00459000 (0x0040fcd0) and FUN_00485070 (0x0040fd07) run only at the late site after the opponents' physics
  (end of UpdateCar); the early UpdateRace site is gone.
- **MODE_LIVE** (`TrackRenderer.cpp`, `exe_main.cpp`): `ModeLiveEnabled()` deleted. `aib_round_type` returns
  `[0x0067e9fc]` (FUN_0042f6a0) and `aib_game_mode_fd0` returns `[0x007f0fd0]`, `SetRaceRule` writes the rule into
  0x007f0fd0, the launch writes the launch mode into 0x0067e9fc, and the `MASHED_ROUND` route writes
  `MASHED_GAME_MODE` (2..10) or 3. The stand-ins 3 / 0 are gone.
- **P3_RESET_PAIR** (`TrackRenderer.cpp`, `Race/RaceCamera.cpp`): the FUN_0040fc00 top stores (DAT_007f1008 = units,
  `SubState_StoreStep` 0x0040fc57) run for sub-states 3..5, and the camera reset takes the pair-switch branch with
  BSS pair (0, 0) when never written (0x00446520). The old skip is gone.
- **REASSERT_KEEP_NORMAL** (`Vehicle/VehiclePhysicsRun.cpp`): `ReassertKeepNormal()` and the three port-only writes of
  ProbeGround's soup normal into the wheel normal are deleted; the re-assert keeps the classifier's normal.
- **PATHPROG** (`Race/PathNodes.cpp/.h`, `Race/PathProgStep.cpp/.h`, `Race/RoundGrid.cpp/.h`, `Ai/AiTargetClosest`,
  `Ai/AiLeafTargets.cpp/.h`, `TrackRenderer.cpp`): `PathProg_Enabled()` deleted. FUN_00407a60 and the FUN_00408610 tick
  write the real rows 0x008a9620 + v*0x30c whenever the course block exists. With it went everything that only ran
  with the knob OFF: arm R's shadow-row tick `PathProg_RacePctTick` and `s_shadow`, the unseeded measurement knob
  `MASHED_PP_PUBLISH` (`PathProg_PublishMask`; every bit acted on arm R only, and bit 0x2's RoundGrid term is
  subsumed), the race_pct bridge (`MASHED_NO_RACEPCT_BRIDGE`, gated on `!PathProg_Enabled()`) and
  `RacePctOrig_Active()`. `TargetClosest_Enabled` (TGT3), `AiLeaf_Enabled` and `RoundGrid_Enabled` keep their own
  knob and lose the PATHPROG term (E5 owns TGT3 / ROUND_GRID).

Built binary check (Python `bytes.count`): no `MASHED_<name>\0` or `MASHED_NO_<name>\0` for the 10 names in
`mashed_re.exe` (sha256 `b73726a2`) or `mashed_re_dev.asi` (`58568d05`). The substrings left in the exe are the
diagnostic knobs `MASHED_MODE7_TRACE`, `MASHED_MODE7_PC`, `MASHED_PATHPROG_{TRACE,DUMP,DBG,GATE_LOG,PC,REPLAY,REPLAY_OUT}`
and `MASHED_REGISTRY_{TRACE,PC,REPLAY,REPLAY_OUT}`, none seeded and none an opt-out of the default.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` gave `=== Build OK ===` for both targets, 0 `error C` /
`LNK` lines. The shared `.asi` was not deployed. rva-lint: `scripts/lint_rva_bodies.py` rc 0,
`48 finding(s): CROSS-TARGET=33, DUP-IN-TARGET=15 | allowlisted=48 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` on the worktree's `mashedmod/build/mashed_re.exe`, private root
`D:/MashedWT/D5-KNOBS-E4-MODE-REG_root`, each batch run as `mashed_lock.py run` (D2: `run --solo`) around `test-slot`.
13 tracks: `verify/rsky_20261016/run13.py run`, root `D:/MashedWT/D5-KNOBS-E4-MODE-REG_root13`, under
`mashed_lock.py run --solo`, then scored with `--cite verify/rres_20261016/cites_rres.json`. Driver: E3's `gates.sh`
with `STEP` changed (same steps and checks as `merge_train.ps1` Gate-Default / Gate-Full). Refs from
`re/orchestrator/merge_train.json` at `e06b1df60`.

| Gate | This exe (`mashed_re.exe` sha256 `b73726a2`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3 arms at 3/3 (`vsum_b.txt` sha256 `6b0c8fea`, the same file as E3) | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1921.98 / 1919.62 / 1917.10, slip1500 0.1935 / 0.1930 / 0.1930, all `in`, H1 398/400. motion_diag 1630 / 1643 / 1640 lines (min 1600; arm 3 gave 204 on the first pass and was re-run once per HB-D2RERUN, short log kept as `D2_DEFH_3.motion_diag.short1.log`) | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` | 39 rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, file sha256 `2431afb1` | `2431afb1` | same |

G-CTRL, B and the 13 tracks ran on the first attempt with no piz collision. Committed copies are in this folder:
`vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-E4-MODE-REG_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` d9122549061105520ea5f117ed9d0690d2ea55aa0b8d122a26875878ada5f66d
- `arms/D2_DEFH_2.motion_diag.log` 8f7417e9dba2437061061778f8304cd4ae05f02ebd9930015bd1b8fac9b46ce3
- `arms/D2_DEFH_3.motion_diag.log` dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9
- `arms/D2_DEFH_3.motion_diag.short1.log` 1727bbce403db02913e3217f675f2be48eca977be821296d96eca3f43e54d5f4
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 788d0abe481b9d3393317bb5e411f537448e43066efd1e19cd784e2736ee8440
- `score_d2.txt` d0fe17874c7da411ed67c113c9f89e72869ecff3efa08e2fc867488397482744
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` c2b70fd77c5d6af1893990f7499510b19eec6ec816005cadc84e288335d62bf2
- `lint.txt` 07661a25b4c6dae1a1e5d47126587e614e09db5c921a168710354053e5378adf
- `gates_run.log` 744426d54bd6ae23f77d0fee6bbbf1b0bdbef4d199a6f71c5b9691bd73425b1e
- `gates.sh` 306feea60a05ed462d05aeb39479b9f27ee2942c29a29e966ec1c7bbcf95c2da
- `mashed_re.exe` b73726a2e9a8e39c505c7f6122e3e0d27941ee1c21508432afdea21af94bb868
- `mashed_re_dev.asi` 58568d05a0ec908717988f57378d11433da90b9119fc0ae22a4e785fc123de75
- `tracks/` (13 track folders, 109 MB)

## 5. For the lead

- No `.ps1` / `.bat` edit needed, so there is no proposed diff.
- **.asi.** No .asi default moved. The two edited TUs in `asi_sources.rsp` are `Ai/AiStandalone.cpp` (the edit is
  inside `#ifdef MASHED_STANDALONE` :1035-1112) and `Gameplay/PickupPoolSpawn.cpp` (only the standalone branches
  changed). No hook body's C-level moves, and no tracker row was touched.
- **For E5 (SUBSTATE).** After this step `MASHED_NO_SUBSTATE=1` alone is the whole SUBSTATE opt-out, and it no longer
  equals the pre-flip default, because the reset pair (P3_RESET_PAIR) stays on. If E5 keeps `MASHED_NO_SUBSTATE` as a
  documented opt-out for the 30 s D2 band, that band is measured with the reset pair ON.
- **kFaith merge with E5.** The `kFaith` lines this branch edits carry E5's names too (`ROUND_GRID`, `SLOT_TYPE_LIVE`,
  `SUBSTATE`, `CC_PC24`). The resolution is the union of the deletions, as in E3.
- **Residue (not a knob read, left in place).** `PickupField::held_` / `held_type_` were set only by the deleted
  collection path, so `TrackRenderer`'s slot-0 `pickups_.held()` block and the `pickup_held*` HUD getters read -1. That
  was already true under the seed. Two stale comments still name `MASHED_RACEPCT_BRIDGE`
  (`Race/PathProgStep.cpp:5`, `Race/SpectatorDistances.cpp:71`), and two name `MASHED_ROUND_GRID + MASHED_PATHPROG`
  (`TrackRenderer.cpp` round grid Stage A blocks); E5 touches those lines.
- **Refresh.** After the gates the branch merged `orch/integration` @ `c3fa6cc42` (merge `d224e8ff2`,
  `git -c merge.autoStash=false`, no conflict). Upstream had merged MATCH-RC-CALLERLESS-FIX, MATCH-S02-RC and MATCH-LIB,
  none of which changes a file under `mashedmod/`. The refreshed tree builds (`=== Build OK ===`, 0 `error C` / `LNK`,
  all 580 .asi objects up to date), exe sha256 `1690ead0`, .asi `0b8e3cc5`. The exe differs from the gated `b73726a2`
  in 4 bytes only, the PE header TimeDateStamp (file offsets 0x120-0x121) and the debug-directory timestamp
  (0x2447c8-0x2447c9), so the gates were not re-run.
