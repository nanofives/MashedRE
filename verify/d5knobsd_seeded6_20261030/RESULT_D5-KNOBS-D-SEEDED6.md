# RESULT D5-KNOBS-D-SEEDED6 (2026-10-30)

Verdict: **GREEN**. All 6 seeded knobs deleted (read sites and `kFaith` entries), with the 6 names ORed into them
(`POS_SEED`, `POS_WHEELS`, `POS_TRANS`, `POS_ORDER`, `POS_CONTACTS`, `RW_WHEELPOS`) and the dead `RING_SLOT0`. The exe
builds, rva-lint reports 0 NEW, and G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is
3/3 in the 42 s band. The 13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS). No knob moved
a hash, so the rule keeps none of them. The gate runner was the only live setter and is repointed (section 2).

Step: delete the opt-out plumbing of the 6 knobs `Core/FaithDefaults.cpp` seeded ON (TRIAGE-2 finding 1 and 2,
`verify/d5knobsd2_20261030/RESULT_D5-KNOBS-D-TRIAGE-2.md`). Branch `orch/D5-KNOBS-D-SEEDED6` from `orch/integration` @
`9824c43af`, change commit `eb352ac8f`. No default flipped. Each knob was seeded ON before any read, so the default
build ran its ON branch. Every site now keeps that branch only, and the default build runs the same code before and after.

## 1. Audit, one line per knob (read site at `9824c43af`)

`S` = `mashedmod/src/mashed_re/`. Seed = `S/Core/FaithDefaults.cpp` `kFaith` line. Opt-outs before this step:
`MASHED_NO_<name>=1`, `MASHED_NO_FAITH=1`, or `MASHED_<name>` set to a non-`1` value. All read sites are in exe-only TUs
(`exe_sources.rsp` only) or inside `#ifdef MASHED_STANDALONE` (STEER_ACOS_CRT), so the .asi behaviour is unchanged.
Live-setter check = `git grep -E 'MASHED_(NO_)?<name>\b'` over `scripts/ re/tools/ re/frida/ re/orchestrator/
verify/savelink_20261023/` and every `*.ps1` / `*.bat`, minus `*.md` and `roadmap.json`. Closed copies = the same grep over
`verify/*.{py,sh,ps1,bat}` outside `verify/savelink_20261023`. The `MASHED_NO_<name>` form: 0 files for every name.

| Knob | Read site | Seed | Live setter (gate runner `run_arms.py` stacks, line at `9824c43af`) | Closed copies |
|---|---|---|---|---|
| POS_MODEL | `S/Vehicle/PosModelKnobs.h:31` (`PosModelAll`) | :29 | RG/TGTRG no, CCB :35, B :56, RGB :78, FNPM :314, PM :315 | 96 |
| POS_SEED / WHEELS / TRANS / ORDER / CONTACTS | `PosModelKnobs.h:32-36`, each `PosModelAll() \|\| ...` | none (ON through POS_MODEL) | none | 5 / 5 / 6 / 4 / 4 |
| RW_WHEELPOS | `S/Collision/WheelContactSolver.cpp:174`, `\|\| PosModelWheels()` | none (ON through POS_MODEL) | none | 1 |
| ROUND_GRID_ORDER | `S/Race/RoundGrid.cpp:232` (`RoundGrid_OrderEnabled`) | :30 | RG :29, TGTRG :30, CCB :36 | 94 |
| PATHPROG_LATE | `S/Race/PathProgStep.cpp:257` (`PathProg_Late`) | :31 | SPL :92, TGTPL :94 | 83 |
| SPAWN_BASIS_RW | `S/Vehicle/BodyOrientationIntegrate.cpp:136` (`SpawnBasisRw`) | :31 | SB / SPLSB :95, SPLSBKN / SBKN :101 | 83 |
| RING_DBUF | `S/Vehicle/VehiclePhysicsRun.cpp:543` (`RingDbuf`) | :32 | DB :114, XR1 :131 | 78 |
| STEER_ACOS_CRT | `S/Ai/AiStandalone.cpp:272` (`s_acosCrt`, under `#ifdef MASHED_STANDALONE`) | :34 | A64 :177 | 63 |
| RING_SLOT0 (not in the title) | `S/Vehicle/VehiclePhysicsRun.cpp:504` (`RingSlot0Enabled`) | none (default ON) | none | 0 |

`MASHED_RING_SLOT0`: its only use (`VehiclePhysicsRun.cpp:581`, the copy of the pose into the +0x9a8 half in
`SyncContactRingMatrix`) sat after `if (RingDbuf()) return;`, so it was unreachable under the seeded RING_DBUF. Deleting
RING_DBUF leaves that copy dead code, so it is removed with `RingSlot0Enabled` (inventory row
`verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md:143`, class b).

Kept (separate names, probes / diagnostics, default OFF): `MASHED_RING_DBUF_DISCARD` (`VehiclePhysicsRun.cpp`, now
`RingDbufDiscard` reads only its own name), `MASHED_POS_SEED_MIN`, `MASHED_POS_SEED_KEEP` (part-1 diagnostics, through
`PosModelEnvOn`, which stays in `PosModelKnobs.h`), `MASHED_ROUND_GRID_ORDER_REPLAY` / `_REPLAY_OUT` (offline replay).

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26`, `D2_DEFH_1..3` set stack `DEF` (empty) and `DEFH` (`MASHED_DRIVE_HOLD_MS` only), and
`base_env` sets none of these names. `run13.py` sets none of them. So every gate saw the seeded values only.

`MASHED_NO_FAITH` (stacks `NF` :323, `OLDNF` :400, report-only): it no longer turns these 6 off, as for E1's 10.

## 2. Change (commit `eb352ac8f`, 14 files, 149 insertions, 227 deletions)

- `Core/FaithDefaults.cpp`: the 6 `kFaith` entries removed, with a note naming them.
- `Vehicle/PosModelKnobs.h`: `PosModelAll` / `Seed` / `Wheels` / `Trans` / `Order` / `Contacts` deleted, header note added.
  `PosModelEnvOn` stays. Includes dropped from `TrackRenderer.cpp` and `WheelContactSolver.cpp`.
- `Vehicle/VehiclePhysicsRun.cpp/.h` (fix B parts 1-5): the part-1 seed block, part-4 `g_fiGameModeHold = 5`, the
  part-4 release-tick `VehiclePhysicsStateReset` and its motion_diag marker, part-3 `centre[1] = io.pos[1]`, the solver's
  corrected translation, the settle position keep, adapter IN / OUT, and the substep `io.pos[1]` advance run
  unconditionally. Part 2's ring-row publish tests `bodyUp` only (and `s_triBack && bodyUp`). Part 5: the A4-to-substep
  `ReassertContacts` call is gone. `StepCtx::posTrans` deleted.
- `VehiclePhysicsRun.cpp` (RING_DBUF): `RingDbuf` deleted. `SyncContactRingMatrix` writes the dst half only (the
  unreachable +0x9a8 copy and `RingSlot0Enabled` removed), the committed-substep selector swap / discard and
  `RingFrameEnd` run unconditionally.
- `Collision/WheelContactSolver.cpp`: the dispatch writes `g_wheelContactPos` directly (the `rwWheelPos` scratch and
  `s_rwWheelPos` are gone), and the UP_TRINORMAL block reads `g_wheelContactPos`.
- `D3d9Render/TrackRenderer.cpp`: the GO-frame settle (both the sub-state host lambda and the countdown branch) tests
  `VehiclePhysics_Enabled()` only and keeps the settled position. The player and opponent position updates take `io.pos`
  (the `GroundHeight` call stays for its `ok` flag). PATHPROG: the end-of-UpdateCar tick runs when `gates_` is non-empty,
  and the UpdateRace call is gone.
- `Race/PathProgStep.cpp/.h`: `PathProg_Late` deleted.
- `Race/RoundGrid.cpp/.h`: `RoundGrid_OrderEnabled` deleted, `RoundGrid_SlotOrder` tests `RoundGrid_Enabled()`.
- `Vehicle/BodyOrientationIntegrate.cpp`: `SpawnBasisRw` deleted. `BodyOrient_NoteExactBasis` always records and
  `BodyOrient_Init` always looks up the exact basis.
- `Ai/AiStandalone.cpp`: the standalone always takes the CRT acos form. The .asi keeps `std::acos(d) * kSteerScale`
  (the `#ifdef` now has an `#else`).
- Stale comments: every comment that called one of these knobs default OFF or opt-in now says it was deleted
  (`ForceIntegratorStubs.cpp:123`, `VehiclePhysicsRun.h:105`, `:130`, `RoundGrid.h:35`, `PathProgStep.h:14`, and the
  sites above).
- `verify/savelink_20261023/run_arms.py` (the only live setter): the deleted names are stripped from every stack, with a
  header comment naming this RESULT. B, SB, DB, A64 and PM are empty (= DEF). FNPM = FAITH. RGB, SPL, TGTPL, SPLSB,
  SPLSBKN and SBKN equal the stack they extended. DBX = `MASHED_RING_DBUF_DISCARD` only. No stack tag is removed, so every
  derived stack still resolves. `ast.parse` OK. After the change the live-setter grep returns 0 lines for all 13 names.

The 100-odd frozen runner copies under `verify/` (column "Closed copies") are left unedited, as in B1 / B2 / B3 / E1.

Built exe check (exact NUL-terminated strings in `mashed_re.exe`): `MASHED_<name>` for POS_MODEL, POS_SEED, POS_WHEELS,
POS_TRANS, POS_ORDER, POS_CONTACTS, RW_WHEELPOS, ROUND_GRID_ORDER, PATHPROG_LATE, SPAWN_BASIS_RW, RING_DBUF,
STEER_ACOS_CRT, RING_SLOT0 occur 0 times, and the bare `kFaith` names 0 times. Controls: `MASHED_RING_DBUF_DISCARD`,
`MASHED_POS_SEED_MIN`, `MASHED_POS_SEED_KEEP`, `MASHED_ROUND_GRID` 1 each, bare `ROUND_GRID` / `UP_TRINORMAL` 1 each.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmoduild.bat"` = `=== Build OK ===` (shared `.asi` not deployed).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` (this branch's copy), private root `D:/MashedWT/D5-KNOBS-D-SEEDED6_root`,
under `mashed_lock.py run` (D2: `run --solo`) and `test-slot`. 13 tracks: `verify/rsky_20261016/run13.py run`, root
`D:/MashedWT/D5-KNOBS-D-SEEDED6_root13`, under `run --solo`, scored with `--cite verify/rres_20261016/cites_rres.json`.
Refs from `re/orchestrator/merge_train.json`.

| Gate | This exe (`mashed_re.exe` sha256 `393e0c77`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3/3 arms 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1919.62 / 1919.62 / 1917.10, slip1500 0.1933 / 0.1933 / 0.1930, all `in`, H1 398/400; motion_diag 1638 / 1638 / 1640 lines (min 1600, no re-run) | 3/3 in band (B3: drvmed 1917.10 / 1919.62 / 1919.62, H1 398/400) | same (3/3) |
| 13 tracks, `run13.py score` stdout | 39 `^\d\d_` rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, stdout sha256 `2431afb1` | `2431afb1` | same |

All gate arms, the three D2 arms and the 13 tracks ran on the first attempt (no re-run, no piz collision). The arms first waited
in the lock queue behind another session's solo window (D5-FLIP-RFLAME-SORT). Note: the `scores_diff.txt`
the scorer writes into its `--out` folder differs from the reference file by one trailing blank line only (sha256
`52c658bc`). The reference and the merge train's `tracks_score.txt` are the scorer's stdout, which is byte-identical.

Committed copies in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt` (the scorer's stdout).

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-D-SEEDED6_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` fe38081b72afd4ea1ad2c44aa451677598a7c7368bffedb4b6099b13c7042d84
- `arms/D2_DEFH_2.motion_diag.log` fe38081b72afd4ea1ad2c44aa451677598a7c7368bffedb4b6099b13c7042d84
- `arms/D2_DEFH_3.motion_diag.log` dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9
- `score_d2.txt` 7871bbaa5428ac3add1e558270bcfced181e70a8535b402413eaa2d437556c5a
- `vsum_b.txt` b5680ca978f24aeb6f59d299d0d8c30cf8b9659ea1644ba4524dc586c3833c40
- `e_check.txt` f93899b5081afef01dfebe7efb14382659588e8e3e6cdc4b51c4f6f8c0d4a08a
- `score13.log` (= committed `tracks_score.txt`) 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `t13/scores_diff.txt` 52c658bc05ee708b09851413bd258c09e0bd813d4d619187621c072b2e22946c
- `t13/gates_diff.txt` c35d0262ae508004c14c3ed1979e5ece7da2fc8ed84dd5bff637f04a33f1a536
- `build.log` c68d4fadaeab15db1055de83373eb5870571655f2ee5d1e7beff048009398580
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` c42b09c3d2a09b1051fe74af45ecb8282eab281b9ada5bf54ea70e7d9ef2fae2
- `d2_run.log` 7d0bccbcb852f753be1be71806d5e4acfe6d6def2b83942b0b0155c93ecc8291
- `run13.log` 4f8f6d03f5a1d1f6cdada3f91676f9d816e3b750215af4a79b9ff40a356f8de8
- exe `mashedmod/build/mashed_re.exe` 393e0c77d4fe8e1a2462beebdfcbb335544d83d97263015fc9377ee2f9a56c53
- .asi `mashedmod/build/mashed_re_dev.asi` 54b110a9ee66c7a759767833c07974be1b0cea566c6ed946a0f18e8f05e7c7ef (built, not deployed)

## 5. For the lead

- No `.ps1` / `.bat` edit needed: the closed `.ps1` runners that set `MASHED_ROUND_GRID_ORDER`
  (`verify/d3_gatefire_20261008/run_h4.ps1:33-34`, `verify/fe_residuals_20261016/run_h4_wt.ps1:33-34`) are frozen evidence,
  left unedited as in B3.
- No knob kept by the hash rule.
- One name beyond the title: `MASHED_RING_SLOT0` (dead under the seeded RING_DBUF, section 1).
- The inventory rows for these 13 names can be marked deleted by whoever owns the inventory text.
