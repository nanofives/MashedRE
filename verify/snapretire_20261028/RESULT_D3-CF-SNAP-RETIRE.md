# RESULT D3-CF-SNAP-RETIRE: mismatch count 0 on 17 of 17 arms. OuterSnapshot / OuterRestore retired, every gate unchanged (2026-10-28)

Branch `orch/D3-CF-SNAP-RETIRE` from `orch/integration` `cdbbbfe0b` (HB-FULLCHECK-R17 merged, `run13_reference` =
`verify/hbfullcheck_20261028/tracks_score.txt`). PREREG `PREREG_D3-CF-SNAP-RETIRE.md` committed alone (`c514546a2`), code
`4fe706f55`. Standalone only. No original capture. Every boot under `mashed_lock.py run --label D3-CF-SNAP-RETIRE*` (D2 `--solo`),
private roots `D:/MashedWT/D3-CF-SNAP-RETIRE_root`, `_root13`, `_rootR`.

## 1. Measuring leg (P0): SNAP_POST default ON, before the retirement

Exe `89f91d07` (orch/integration `d92bb3a5e`, detached worktree, `MASHED_NO_DEPLOY=1`. `git diff d92bb3a5e cdbbbfe0b -- mashedmod`
is empty). No knob except `MASHED_SNAP_LOG=1`. Counts from each arm's `mashed_re.log`:

| arm | `[substep] outer` mismatch | `[snap]` restore (`dead` / `spin` / `reloc` / `mismatch`) | `[snap] reloc-post` |
|---|---|---|---|
| G-CTRL `H_SL` | 0 | 0 | 0 |
| B `B_SL_K0` / `K1` / `K26` | 0 / 0 / 0 | 0 / 0 / 0 | 0 / 2 / 0 |
| 13 tracks 01..13 | 0 on every track | 0 on every track | 2, 4, 4, 5, 2, 6, 3, 8, 3, 0, 3, 4, 1 (45) |

B K1's two relocations are frame 331 slot 1 and frame 395 slot 2, the pairs `verify/snap_20261027` logged. Gates on this
exe: G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c / 9fcdbd3a / 53778664` (= `merge_train.json` refs). Its 13-track table
is byte-identical to the reference (SHA-256 `a3c14c41...` both), so `MASHED_SNAP_LOG=1` moves no row.

**P0 holds: mismatch count 0 and restore count 0 on 17 of 17 arms.** The L-43 retire condition is met.

## 2. Code (`4fe706f55`, 3 files, +26 / -62)

- `Vehicle/VehiclePhysicsRun.cpp` / `.h`: `OuterSnap`, `g_outerSnap`, `VehiclePhysics_OuterSnapshot`, `VehiclePhysics_OuterRestore`
  deleted.
- `D3d9Render/TrackRenderer.cpp`: the snapshot loop before `StepCarsOuter`, `outerUndo` (sites `dead` / `spin` / `reloc`) and
  `SnapPost()` deleted. `MASHED_SNAP_POST`, `MASHED_NO_SNAP_POST` no longer read. With the physics the stuck relocation runs
  only after the commit. `!phys` keeps its pre-commit relocation, and `snapKeep` keeps its `phys &&` term, so both paths keep the
  knob-ON skip and relocation order.
- Mismatch net: no restore exists, so the car keeps the outer result (`io = s_outer[v].out`) and the `[substep] outer` line is
  still written, text "kept the outer result". Not reached on any arm here.
- Lost by design: the pre-flip timing (`MASHED_NO_SNAP_POST=1`, B K1 `e85ea6fb`) cannot be reproduced any more, and under
  `MASHED_REAL_PHYSICS=0` (scaffold, A/B only) the `dead` / `spin` / `reloc` restores are gone. No gate runs either path.

## 3. Gates on the retirement exe `c28853f6` (P1, P2, P3)

| gate | reference | retire exe | |
|---|---|---|---|
| G-CTRL `H_DEF` | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | same |
| B `B_DEF_K0 / K1 / K26` | `3a03670c / 9fcdbd3a / 53778664` | `3a03670c / 9fcdbd3a / 53778664` | same |
| D2 x3 (`score_d2_v2.py port`) | `0.1942 / 0.2492 / 1896.74` (R17) | `0.1942 / 0.2492 / 1896.74` x3, 1629 / 1628 / 1634 lines, 0 reruns | same |
| 13 tracks vs `verify/hbfullcheck_20261028/tracks_score.txt` | `a3c14c41...` | `a3c14c41...` (0 differing lines) | same |

- P1 holds (6 hashes). P2 holds (0 rows move). D2 is the full set's third leg (`Vehicle/` is in `full_set_paths`).
- P3 holds: `H_SL` / `B_SL_K*` on the retire exe give the same hashes as `H_DEF` / `B_DEF_K*`, 0 mismatch lines, and B K1's
  `reloc-post` 331/1 and 395/2. On the 13 tracks (run with `MASHED_SNAP_LOG=1`) the sorted `[snap]` / `[substep] outer` line set
  of every track is identical to the measuring leg's (md5 per track equal, 13 of 13), 0 mismatch lines.
- Harness note: retire tracks 2 and 8 first failed inside `mashed_lock.py` (track 2 rc 127 at launch, track 8 a
  `PermissionError` removing `mashed_machine_solo.lock` in `solo_owner`, line 232), before the exe started. Both re-ran
  OK on attempt 1 (`run_retire_fix.sh`). The first measuring launch stalled in the lock queue for 3.5 h and was killed (my
  PIDs only) and relaunched (`run_measure2.log`).

Verdict per PREREG section 4: **GREEN** (P0, P1, P2, P3 hold).

## 4. Raw data (`D:/MashedWT/D3-CF-SNAP-RETIRE_data`, not committed)

| file | SHA-256 |
|---|---|
| `exe_measure.exe` | `89f91d070a25d98cc75c22db21b1978121a6e603ce9211a937c58f59f5c05720` |
| `exe_retire.exe` | `c28853f6b21d9c4be91bfea6ea9ae33de3766891a5d8f5c4049f0b2fe61a66d2` |
| `run_measure.sh` | `7e87b8debcf2163bc0216839df125951ded5338651bfc9ab82996b4c38897fc8` |
| `run_retire.sh` | `c3f8821ab34036f9045d2e797f737a3248403982af3f67a9bac42f46e0bb8ddb` |
| `run_retire_fix.sh` | `553ef7a6efc1091a01e95107f7406df57d4bc25490574f5552f6002ed2e3cef1` |
| `run_measure2.log` | `714512c9d8456b7e57e798c5b1b76aec2ac57dea6161a39ec0f0b1ab9aa8b60b` |
| `run_retire.log` | `868377f7459a57f4e047b6724d81cbc3a7d0b592a7332a8be94d9f97cdbcaba5` |
| `run_retire_fix.log` | `442b78dcba5bfe18cc4f0f840748965078b689ebe35751afbfb6ceb66d10972a` |
| `tracks_score_measure.txt` | `a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0` |
| `tracks_score_retire.txt` | `a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0` |
| `score_d2_retire.txt` | `611b22093e66f5773373c9285e808a97f27c2dbd2b14b846d2c3ce36fee3b666` |
| `arms_measure/RUNLOG.txt` | `57000324f0c8345247170d731f99b55d6ac70613d56f3cd849976edf037bc13c` |
| `arms_retire/RUNLOG.txt` | `e038fa606bbf3f37b53929b4b7b09b15edd0031855822160e14b2764d3d22294` |
| `arms_measure/B_SL_K1.log` | `c42efab5edea30656e4e43e40faa358a87fad7299c2038f54aecbb99672a7f32` |
| `arms_retire/B_SL_K1.log` | `b9b37456c4edbd72ff12f9059a81389ea9d5f93d7873cfd3a36d0fa5444d1ad7` |
| `arms_retire/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms_retire/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `arms_retire/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `arms_retire/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms_retire/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `arms_retire/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

Per-track `mashed_re.log` copies: `tracks_measure/logs/t<N>.mashed_re.log`, `tracks_retire/logs/t<N>.mashed_re.log`.
Re-run (from this worktree): `test-slot bash D:/MashedWT/D3-CF-SNAP-RETIRE_data/run_retire.sh`.
