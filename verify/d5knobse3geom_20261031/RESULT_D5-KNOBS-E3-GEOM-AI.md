# RESULT D5-KNOBS-E3-GEOM-AI (2026-10-31)

Verdict: **GREEN**. All 10 named knobs are deleted: their read sites, their 10 `kFaith` entries, and no .asi seed
entry (none of the 10 had one). Both targets build, rva-lint reports 0 NEW, and none of the 10 names occurs in either
binary. G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is 3/3 in the 42 s band. The
13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS). No knob is a gate input, so none stays in.

Step: geometry / AI batch (E3) of the seeded knobs listed in
`verify/d5knobsinvfix_20261030/RESULT_D5-KNOBS-INVENTORY-FIX.md`, section "Outside class d". Branch
`orch/D5-KNOBS-E3-GEOM-AI` from `f6d48c932` (orch/integration with E2 merged), source commit `724ebc38c`. No default
flipped. All 10 were seeded ON in the standalone by `Core/FaithDefaults.cpp` (`kFaith`, exe-only TU). None was in the
.asi seed list `Vehicle/PhysicsChainHooks.cpp` (`A6aAsiDefaults`), so the .asi ran each one at its site default, and the
change keeps that default for each one (section 2).

**AI_LUT check (the lead asked to verify the B3 removal first).** The read site's own opt-out `MASHED_NO_AI_LUT` was
deleted by D5-KNOBS-RUNARMS-STACKS, as the lead said. Three paths still moved the value off the seeded 7 at `f6d48c932`:
`MASHED_NO_FAITH=1` (no seed, site default 5), `MASHED_AI_LUT=<mask>` (read with `atoi`), and `MASHED_NO_AI_LUT=1`
itself, because the seeder `FaithSeed` still skips a knob whose `MASHED_NO_<name>` is set, so the site then saw no
`MASHED_AI_LUT` and took 5. The opt-out was therefore not fully gone, and the knob stays in this batch.

## 1. Audit, one line per knob (read site at `f6d48c932`)

`S` = `mashedmod/src/mashed_re/`. Seed = the `kFaith` line in `S/Core/FaithDefaults.cpp`. The read forms: `on1` =
`e && e[0] == '1'` on `MASHED_<name>` (unset = OFF at the site, ON through the seed), `nz` = `e && e[0] && e[0] != '0'`,
`no` = the site reads only `MASHED_NO_<name>` (ON unless that is `1`, so the seed is redundant and `MASHED_NO_FAITH`
never reached it). Opt-out was `MASHED_NO_<name>=1`, `MASHED_NO_FAITH=1`, or `MASHED_<name>` set to another value (for
`no` sites only the first). The script check is `git grep -w` over `scripts/`, `re/tools/`, `re/frida/` and
`re/orchestrator/` (`*.md` / `*.json` excluded): **0 hits for all 10** (the only `re/orchestrator/` hit is
`ledger/events.jsonl` text). "verify files" counts files under `verify/` naming the knob, with the `MASHED_NO_` form in
brackets. "run_arms" = `verify/savelink_20261023/run_arms.py` line numbers.

| Knob | Read site (form), uses | Seed | TU in .asi build | .asi before = after | verify files (NO_) | run_arms stacks | Gate arms use it |
|---|---|---|---|---|---|---|---|
| WCS_COMBINE_PC24 | `S/Collision/WheelContactSolver.cpp:433` (on1), the PC_24 scope around `FUN_004c52f0` 0x0046ffff | :45 | no (exe-only TU) | n/a | 65 (47) | TC :361 (LT3, TC0, FR, LT1, TC1..3), NOPC24 :419 | no |
| WCS_TRINORMAL | `WheelContactSolver.cpp:88` (on1), uses :131, :361, :395, :409, :413, :430; `S/Vehicle/VehiclePhysicsRun.cpp:727` (on1, basis carry-back) | :43 | no (exe-only TUs) | n/a | 128 (60) | NT :359 | no |
| ROTATE_LUT | `S/Math/RwMatrixRotate.cpp:53` (on1), use :118 | :36 | **yes** | OFF = OFF | 98 (2) | RL :148 (XW1RL; `RL` is redefined at :199 and :381, so the last one wins) | no |
| ROTATE_PC24 | `RwMatrixRotate.cpp:59` (on1), use :125 | :37 | **yes** | OFF = OFF | 88 (2) | P24 :154 (RLOLCP, XW0RLOLCP, F :155 family) | no |
| RW_SSE_POINTS | `S/Math/RwV3dTransformPointsCPU.cpp:68` (no), use :75 | :35 (redundant) | **yes** | ON = ON | 108 (8) | SSE :133 (UTBQ1SSE, BQ1SSE), XR1 :136 | no |
| AI_LUT | `S/Ai/AiStandalone.cpp:153` (`atoi`, unset = 5), mask bits used :247, :315, :363, :369 | :37 (`"7"`) | yes, but `AiLutMask` returns 0 outside `MASHED_STANDALONE` | 0 = 0 | 130 (96) | L5 :31, CCBL5 / CC5 :43-44, SACL5 / SACLR5 :76-77, CL5 / CLR5 :78-79, EXL7 / L7 :180 | no |
| AI_FWD_ATROW | `S/D3d9Render/TrackRenderer.cpp:257` (nz) | :38 | no (exe-only TU) | n/a | 79 (1) | F2 :184 (L7AF, EXL7AF) | no |
| HEADING_FWD_LIVE | `S/Race/HeadingAssist.cpp:157` (on1), use :172 | :37 | no (exe-only TU) | n/a | 82 (1) | F :155 (RLOLCPF, XW0RLOLCPF) | no |
| FWD_AT | `S/Vehicle/VehiclePhysicsRun.cpp:502` (no), uses :1349, :1569, :1903 | :36 (redundant) | no (exe-only TU) | n/a | 109 (8) | FA :126 (SPLSBKNF60FA, F60FA, :127-128), XR1 :136 | no |
| STEER_SCALE_RDATA | `S/Ai/AiStandalone.cpp:688` (nz, inside `#ifdef MASHED_STANDALONE`) | :38 | yes, but the read is exe-only | 0.0030034f = 0.0030034f | 79 (1) | SC :187 (L7AFS, EXL7AFS) | no |

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26` and `D2_DEFH_1..3` set stack `DEF` (empty) and `DEFH` (`MASHED_DRIVE_HOLD_MS`
only). `verify/rsky_20261016/run13.py` sets none of the 10 names. So the gates only ever saw the seeded values. No
committed harness uses any of the 10 as a gate input, so none is kept as a documented opt-out.

Stacks that now run the same code as the default (report-only, none is a gate): `NT` (:359), `NOPC24` (:419), `NF`
(:328) / `OLDNF` (:405) / `NOSXNF` (:414) for these 10 knobs, and `L5` and the `*5` stacks (they set `MASHED_AI_LUT=5`,
now unread, so they run mask 7). The `TC`, `RL` (:148 form), `P24`, `F`, `SSE`, `FA`, `F2`, `SC`, `XR1` (for its
`RW_SSE_POINTS` / `FWD_AT` names), `EXL7` / `L7` stacks now set an unread name. The frozen runner copies under `verify/`
are left unedited, as in E1 and E2.

## 2. Change (commit `724ebc38c`, 9 files, 80 insertions, 116 deletions)

- `Collision/WheelContactSolver.cpp` (exe-only): `WcsTriNormal()` and the `s_cpc` read are deleted. The 3-wheel alignment
  runs only in the original's form: `local_ac = (int)+0x9e0` (0x0046f6e2 / 0x0046f712 `_ftol`), the tri-normal
  `FUN_0046c5f0` form at 0x0046fd57, the dot grouping `(e0.x*up.x + e0.y*up.y) + e0.z*up.z` (0x0046fdaa..ca), the gate
  `dotp < 0.999f` (`_DAT_005cea88`), the acos as `atan2(sqrt((1+c)(1-c)), c)` (FUN_004a3384 form) times
  `kRadToDeg`, and `FUN_004c52f0(ring, m, 2)` (0x0046ffff) inside an unconditional PC_24 scope. The stub path
  (`Sys_TickCount` local_ac, the reversed dot, `kHiProj` 0.998901f, `Math_Acos`, `Rw_SetRotation`) is gone, and
  `kHiProj` with it (its only reader).
- `Vehicle/VehiclePhysicsRun.cpp` (exe-only): the WCS_TRINORMAL basis carry-back after the solver runs whenever
  `bodyUp` is set. `FwdAt()` is deleted: +0x9d4..+0x9dc is the basis `at` row at frame entry (`WriteForwardAt`,
  0x0046ddc9) and in the record publish (still `cos/sin(yaw)` when `g_bodyBasisOk[slot]` is false, as before), and the
  substep loop writes none (the original writes no +0x9d4 inside it).
- `Math/RwV3dTransformPointsCPU.cpp` (both targets): `RwSsePoints()` deleted, the slot +0xc order of 0x004dc370
  (`(z*m[8+c] + y*m[4+c]) + (x*m[c] + m[12+c])`) is the only body. It was already ON in both targets.
- `Math/RwMatrixRotate.cpp` (both targets): `RotateLut()` / `RotatePc24()` become `constexpr` `kRotateLut` / `kRotatePc24`,
  `true` under `MASHED_STANDALONE` (the seeded value) and `false` in the .asi (it never seeded them). No env read is
  left in either target.
- `Race/HeadingAssist.cpp` (exe-only): `HeadingFwdLive()` deleted, the entry copies rec+0x948..+0x950 into
  +0x9d4..+0x9dc unconditionally (FUN_0046d510's write, 0x0046d53a). The `[UNCERTAIN]` note on the getter's order stays.
- `D3d9Render/TrackRenderer.cpp` (exe-only): `aib_own_fwd_xz` takes the `at` row (+0x948 / +0x950) unconditionally; an
  all-zero row still keeps the snapshot, as before.
- `Ai/AiStandalone.cpp`: `SteerMagScale416250` returns `0.003f` (0x005cd0e8 = 0x3b449ba6) in the standalone and
  `kSteerMagScale` in the .asi, with no env read. `AiLutMask` returns the constant 7 (the seed) unless the look replay's
  in-process `s_aiLutOverride` is set (unchanged: it is a measurement mode driven by `MASHED_AI_LOOK_REPLAY*`, not an
  opt-out of the default); the .asi still returns 0. Four comments that named `MASHED_AI_LUT` now say "AI LUT mask".
- `Race/RaceCamera.cpp`: one comment no longer names the ROTATE knobs.
- `Core/FaithDefaults.cpp`: the 10 `kFaith` entries and the trinflip / cpcflip comments are removed, with a note naming
  them. `REGISTRY` and `PICKUPS_ORIG`, which shared a line with `AI_FWD_ATROW` / `STEER_SCALE_RDATA`, stay.

Built binary check (Python `bytes.count(b"<name>")`, no terminator, so substrings count too): all 10 names occur 0 times
in `mashed_re.exe` (sha256 `17aecd53`) and in `mashed_re_dev.asi` (sha256 `8f7cc7a8`).

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` gave `=== Build OK ===`, 0 `error C` / `LNK` lines. The shared
`.asi` was not deployed. rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` on the worktree's `mashedmod/build/mashed_re.exe` (the path the merge train
gates), private root `D:/MashedWT/D5-KNOBS-E3-GEOM-AI_root`, each batch run as `mashed_lock.py run` (D2: `run --solo`)
around `test-slot`. 13 tracks: `verify/rsky_20261016/run13.py run`, root `D:/MashedWT/D5-KNOBS-E3-GEOM-AI_root13`, under
`mashed_lock.py run --solo`, then scored with `--cite verify/rres_20261016/cites_rres.json`. Driver: `gates.sh` in the
data folder (same steps and checks as `merge_train.ps1` Gate-Default / Gate-Full). Refs come from
`re/orchestrator/merge_train.json`.

| Gate | This exe (`mashed_re.exe` sha256 `17aecd53`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3 arms at 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1917.10 / 1917.10 / 1917.10, slip1500 0.1933 / 0.1930 / 0.1930, all `in`, H1 398/400. motion_diag 1639 / 1641 / 1641 lines (min 1600, arm 1 re-run once, see below) | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` | 39 rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, file sha256 `2431afb1` | `2431afb1` | same |

G-CTRL, B and all 13 tracks ran on the first attempt, with no piz collision. D2: the first pass gave arm 1 956
motion_diag lines (under `d2_min_lines` 1600), so it was re-run once per HB-D2RERUN and gave 1639. The short log is kept
as `D2_DEFH_1.motion_diag.short1.log`. The lock queued behind merge-train `20261031_131735` (both its arm window and its
13-track solo window) and another session's MATCH-S04 run, and nothing was broken.
Committed copies are in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-E3-GEOM-AI_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `arms/D2_DEFH_1.motion_diag.short1.log` 6ebbfbf6eb758f53f1faeb0bd32ea16e3d6a8854a38c3cc18babb2e219ad8f60
- `arms/D2_DEFH_2.motion_diag.log` e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258
- `arms/D2_DEFH_3.motion_diag.log` e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 0ea603242b5d8fad76234cafba3bcae84907ebb37d5ba4499bcdbd5297414512
- `score_d2.txt` f71f7e61bf100dcf28378f1fb98840e28206f69e27144adb4d53364c120adbba
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` 53588c9489c50bf5330814c0dd4730ed959ebca3eb416ce44ebb92b6c0dc903d
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `gates_run.log` ed9eca54239133ea57ab58bdb5d4cf90613a7c389420219b93663cb4995e162e
- `gates.sh` 276b444c696271e0539dd953b1513a0e8ce5ebb33962fb4ab461a5a5451dbab0
- `mashed_re.exe` 17aecd5365ccc31c36600f318a7aa066f9e5edaca63732a00c4e354a05bcf39a
- `mashed_re_dev.asi` 8f7cc7a857bf83809587b47c8e4f969d87ba077a3c32da48f2d80b8ab472626a
- `tracks/` (13 track folders, 110 MB)

## 5. For the lead

- No `.ps1` / `.bat` edit needed, so there is no proposed diff.
- **Merge with E4 / E5 and AI-TICK-FIRST.** `Core/FaithDefaults.cpp` `kFaith` is edited on shared lines by every knob
  batch, and it is not in `conflict_union`. The resolution is the union of the deletions: keep every entry that no merged
  branch deleted. On this branch the line after `AI_TICK_FIRST` / `REASSERT_KEEP_NORMAL` now carries only `REGISTRY` and
  `PICKUPS_ORIG` (they shared a line with `AI_FWD_ATROW` / `STEER_SCALE_RDATA`), and the trinflip / cpcflip block before
  the fflip block is gone. `AI_TICK_FIRST` is still in `kFaith` here (that step owns it). `Vehicle/PhysicsChainHooks.cpp`
  is not touched.
- **AI_LUT was not fully removed by B3** (the AI_LUT check at the top): `MASHED_NO_AI_LUT=1` still opted out through the seeder, and
  `MASHED_NO_FAITH` / `MASHED_AI_LUT=<mask>` too. All three are gone now. The report-only stacks that set
  `MASHED_AI_LUT=5` (`L5`, `CC5`, `CL5`, `SACL5`, and the rest in section 1) now run mask 7.
- **.asi.** No .asi default moved. `RW_SSE_POINTS` was ON there (its site read only the opt-out), and stays ON.
  `ROTATE_LUT` / `ROTATE_PC24` were OFF there (never seeded) and stay OFF as `constexpr false`, `STEER_SCALE_RDATA` keeps
  `kSteerMagScale`, and `AiLutMask` keeps 0. The only .asi change is that `MASHED_ROTATE_LUT=1` / `MASHED_ROTATE_PC24=1`
  no longer turn the LUT normalize / PC_24 scope on inside MASHED.exe, and `MASHED_NO_RW_SSE_POINTS=1` no longer selects
  the x87 order. `git grep` finds no script under `re/frida/` or `scripts/` that sets them. No hook body's C-level
  moves, and no tracker row was touched.
- **Refresh.** After the gates the branch merged `orch/integration` @ `5cf5cdd63` (merge `f1ff8a8a9`, no conflict).
  Upstream had merged MATCH-S02 and MATCH-RUBRIC: 33 `mashedmod/` files (matched sources for slice
  0x004099a0..0x00412100 and 20 port body swaps), none of them a file this branch edits. The refreshed tree builds
  (`=== Build OK ===`, 0 `error C` / `LNK`), rva-lint 0 NEW, exe sha256 `16f37188` (the gated exe was `17aecd53`,
  built before the MATCH-S02 swaps), and the 10 names still occur 0 times in the source reads and in the exe. The
  gates were not re-run on the refreshed exe, and the merge train gates the merged tree. D5-KNOBS-AI-TICK-FIRST had
  not merged at refresh time.
