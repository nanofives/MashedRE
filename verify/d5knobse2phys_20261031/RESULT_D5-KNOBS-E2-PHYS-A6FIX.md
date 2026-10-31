# RESULT D5-KNOBS-E2-PHYS-A6FIX (2026-10-31)

Verdict: **GREEN**. All 10 named knobs deleted (read sites, `kFaith` entries, the 3 .asi seed entries). The exe builds,
rva-lint reports 0 NEW, and G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is 3/3 in the 42 s band.
The 13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS). No knob moved a hash, so none stays in.

Step: physics A6a / fixup / terrain batch (E2) of the seeded knobs listed in
`verify/d5knobsinvfix_20261030/RESULT_D5-KNOBS-INVENTORY-FIX.md`, section "Outside class d". Branch
`orch/D5-KNOBS-E2-PHYS-A6FIX` from `702354fc5`, source commit `62b3f2515`. No default flipped. 9 of the 10 knobs were
seeded ON in the standalone by `Core/FaithDefaults.cpp` (`kFaith`), and 3 of them also in the .asi by
`Vehicle/PhysicsChainHooks.cpp` (`A6aAsiDefaults`). The tenth, `FIX_5CEA60`, had a `kFaith` entry that nothing read: its
sites read the opt-out `MASHED_NO_FIX_5CEA60` directly (inventory class x). Every site keeps the seeded path, so the
default build runs the same code before and after.

## 1. Audit, one line per knob (read site at `702354fc5`)

Each read but `FIX_5CEA60` was `e && e[0] == '1'` on `MASHED_<name>` (unset = OFF at the site, ON through the seed).
Opt-out was `MASHED_NO_<name>=1`, `MASHED_NO_FAITH=1`, or `MASHED_<name>` set to anything but `1`. `FIX_5CEA60`'s opt-out
was `MASHED_NO_FIX_5CEA60=1` only (`MASHED_NO_FAITH` never reached it). `S` = `mashedmod/src/mashed_re/`. The script check
is `git grep` over `scripts/`, `re/tools/`, `re/frida/` and `re/orchestrator/`: **0 hits for all 10** in any script (the
only `re/orchestrator/` hits are ledger text). "run_arms" = `verify/savelink_20261023/run_arms.py` line numbers.

| Knob | Read site, use | Seed (exe / .asi) | In .asi build | verify/ files (with MASHED_NO_ form) | run_arms stacks | Gate arms use it |
|---|---|---|---|---|---|---|
| A6A_DOT_ORDER | `S/Vehicle/Integrate2.cpp:236`, uses :442, :706, :901 | FaithDefaults.cpp:59 / PhysicsChainHooks.cpp:66 | yes, seeded ON | 50 (46) | DOT :416, NODOT :417 | no |
| A6A_GEAR_ORDER | `Integrate2.cpp:217`, use :325 | :40 / :66 | yes, seeded ON | 76 (1) | GO :214 (TL3GO, FULLRLGO, FULLGO :216, FAITH :317) | no |
| A6A_SPIN_ORDER | `Integrate2.cpp:223`, use :626 | :44 / :66 | yes, seeded ON | 67 (55) | SO :337 (CLS, CLST, SF, F5), N5 :355 | no |
| FIXUP_F32 | `S/Collision/ContactFixup.cpp:186`, uses :188, :212 | :44 / no | no (exe-only TU) | 66 (55) | F3 :345 (CLSF3, F4, F5), N5 :355 | no |
| FIXUP_SLIDE_ORDER | `ContactFixup.cpp:354`, use :355 | :45 / no | no (exe-only TU) | 66 (55) | FSO :340 (SF, CLSF, F5), N5 :356 | no |
| FIXUP_SUM_ORDER | `ContactFixup.cpp:163`, use :164 | :44 / no | no (exe-only TU) | 66 (55) | F4 :348 (CLSF4, F5), N5 :355 | no |
| FIX_5CEA44 | `S/Collision/CarWorldContacts.cpp:260` (`s_velScale2`) | :41 / no | no (exe-only TU) | 92 (1) | F44 :115, F6044, FAITH :317 | no |
| FIX_5CEA60 | no reader of `MASHED_FIX_5CEA60`. Opt-out `MASHED_NO_FIX_5CEA60` read at `S/Collision/WheelContactSolver.cpp:60` (`kWTorque`) and `S/Vehicle/VehiclePhysicsRun.cpp:364` (`kProbeScale`) | :35 (dead seed) / no | no (exe-only TUs) | 111 (7) | F60 :115 (unread name before and after) | no |
| TER_SPIN_F32 | `CarWorldContacts.cpp:265`, use :266 | :44 / no | no (exe-only TU) | 66 (55) | F3 :345, N5 :355 | no |
| UP_TRINORMAL | `S/Collision/WheelContactSolver.cpp:181` | :35 / no | no (exe-only TU) | 122 (1) | SPLUT / SPLSBUT :102, :124, XR1 :136 | no |

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26` and `D2_DEFH_1..3` set stack `DEF` (empty) and `DEFH` (`MASHED_DRIVE_HOLD_MS`
only). `run13.py` sets none of these names. So the gates only ever saw the seeded values. No committed harness uses any of
the 10 as a gate input, so none is kept.

Stacks that now run the same code as the default (report-only, none is a gate): `N5` (:355, the f5flip opt-out five),
`NODOT` (:417), and `NF` (:328) / `OLDNF` for these 10 knobs, since neither `MASHED_NO_<name>` nor `MASHED_NO_FAITH=1`
turns them off any more. The `DOT`, `GO`, `SO`, `FSO`, `F3`, `F4`, `F5`, `F44`, `F60`, `SPLUT`, `XR1` stacks (and the
`FAITH` family's three names) now set an unread name. The frozen runner copies under `verify/` are left unedited, as in E1.

## 2. Change (commit `62b3f2515`, 8 files, 84 insertions, 148 deletions)

- `Vehicle/Integrate2.cpp` (both targets): `A6aGearOrder`, `A6aSpinOrder` and `A6aDotOrder` are deleted, and each use
  keeps its ON branch: the gear table `fVar4 = (c498*0.5)*fk`, `fVar5 = fk*(c49c*0.5)` (0x0046768b..0x00467739), the spin
  factor `(+0x9e8*k)*f*360` (0x00468077..0x004680ae), and the three dot products in the original's association (drive
  arm 0x00467be9, block #5 0x0046837d, clamp #6 0x00468771).
- `Vehicle/PhysicsChainHooks.cpp`: `A6A_GEAR_ORDER`, `A6A_SPIN_ORDER`, `A6A_DOT_ORDER` removed from `A6aAsiDefaults`, with
  a note. `A6A_DRIVE_NORM_LUT`, `A6B_*`, `A5_DRAFT_LUT` stay (not this batch).
- `Collision/ContactFixup.cpp`: `s_sumOrder`, `s_fxF32`, `s_slideOrder` are deleted. The sum grouping, the float dot / f /
  local_c chain and the sign test's order, and the slide association are the only path (the double `else` branch is
  gone).
- `Collision/CarWorldContacts.cpp`: `s_velScale2` and `s_spinF32` are deleted. The spin step is the float chain
  `((|w|*360)*[edi-0x18])*_DAT_005cea44` only.
- `Collision/ContactConstants.h`: `kTer_VelScale2` = `0.27777779f` (`_DAT_005cea44` 0x3e8e38e4, the value
  `MASHED_FIX_5CEA44` selected). Its only user is the line above, so no other site changes value.
- `Collision/WheelContactSolver.cpp`: `s_upTri` deleted, the tri-normal write of +0x9c8..+0x9d0 runs unconditionally.
  `kWTorque = 0.2778f` (`_DAT_005cea60` 0x3e8e3bcd) with no `getenv`.
- `Vehicle/VehiclePhysicsRun.cpp`: `kProbeScale = 0.2778f` with no `getenv`.
- `Core/FaithDefaults.cpp`: the 10 `kFaith` entries and the `[f5flip]` / `[dotflip]` comments are removed, with a note naming
  them (the `MASHED_COLL_LUT` sentence of the f5flip comment is kept in the note).

Built binary check (Python `bytes.count(b"<name>")`, no terminator, so substrings count too): all 10 names occur 0 times
in `mashed_re.exe` and in `mashed_re_dev.asi`.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` gave `=== Build OK ===`, 0 `error C` / `LNK` lines. The shared
`.asi` was not deployed. rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/D5-KNOBS-E2-PHYS-A6FIX_root`, run under
`mashed_lock.py run` (D2: `run --solo`) and `test-slot`. 13 tracks: `verify/rsky_20261016/run13.py run`, root
`D:/MashedWT/D5-KNOBS-E2-PHYS-A6FIX_root13`, under the lock's solo window, then scored with
`--cite verify/rres_20261016/cites_rres.json`. Refs come from `re/orchestrator/merge_train.json`.

| Gate | This exe (`mashed_re.exe` sha256 `48a7b0b7`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3 arms at 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1921.98 / 1919.62 / 1921.42, slip1500 0.1935 / 0.1930 / 0.1935, all `in`, H1 398/400. motion_diag 1616 / 1642 / 1614 lines (min 1600, arms 1 and 2 re-run twice, see below) | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` | 39 rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, file sha256 `2431afb1` | `2431afb1` | same |

G-CTRL, B and all 13 tracks ran on the first attempt, with no piz collision. D2: the first pass gave motion_diag 1422 / 1552 / 1614 lines, all three already in band (drvmed 1887.76 / 1917.10 / 1921.42). Arms 1 and 2 were under `d2_min_lines` 1600, so they were re-run per HB-D2RERUN: the first re-run gave 1451 / 1502 (that solo window opened while another session's MATCH-S02 run was still being drained), the second 1616 / 1642. The table cites the second re-run with arm 3 from the first pass, the set the merge train would score. The short logs are kept as `*.short1.log` / `*.short2.log`. The lock queued behind other sessions' windows (merge-train 20261031_101859 and 20261031_115012, D5-KNOBS-AI-TICK-FIRST, MATCH-S02) and nothing was broken.
Committed copies are in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-E2-PHYS-A6FIX_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` 3a09ed60556add5e5f48f11a3b161c7753af36c402631fdbed8141bc4aaf64ef
- `arms/D2_DEFH_2.motion_diag.log` e11be3a771fc58879c1f7d61502ba7ddca581a7e7c84650fec84ceaff9ba0c6f
- `arms/D2_DEFH_3.motion_diag.log` 83a1f3544903df6494ea29687410d6417ab8bf643fa803a114ecb1cfcf7bafa6
- `arms/D2_DEFH_1.motion_diag.short1.log` 5c6a4f04e41e024e48c5da9778dc7aac3a54523316a2d7aa2f9db90ab505406b
- `arms/D2_DEFH_2.motion_diag.short1.log` eb136f5ab581479879537ecb31ac6a06fb7829e6a8981267b75ff4dbeddffc33
- `arms/D2_DEFH_1.motion_diag.short2.log` 4573348cc8da52117d4cc5b44775ca0812df2863e33864605a385430c91cbb41
- `arms/D2_DEFH_2.motion_diag.short2.log` ce43a7d2411e8148ba7e5ceaf4a5640ddd1f4382300dfef6e8ffc7f16dd3189b
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 4a4331d7a277e142d83aa066e4b425229f7ad5556a9093689df19f815ea7fd15
- `score_d2.txt` 81ba4e04b717e001e00dbf691464ef401c0b115fbde3391c4cf1a93868514da6
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` b2ab7cbd1fb55d63486c3f2ecc434b469854231998a55207f107e8b777b0a607
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` 18e3ab6a2ae972e8d0f37258b584f5b4ad25827a6e9f75961921d4a0f62e3d53
- `d2_run.log` 94f09112b348abe01abd72061b821fd9c3ef11291a15be036bde55d37dfcd1de
- `d2_rerun.log` dcde72faf22c66a4eb4d3d9a5ab51b8731af08279d9b923fb35341c01d809acb
- `d2_rerun2.log` b96f33467f404d3e21e2d0e104e0fb82cdf65003d226ef7e315a363f0ce021c2
- `run13.log` c5375f1db5fa47039beffd8d2eb3f3c9c069f0f14b7243d54912d8bf55312492
- `mashed_re.exe` 48a7b0b7397635c81c4ea78b64b99121ff809e4b5abc224019146181fb5dff2d
- `mashed_re_dev.asi` 01eafdb80eb1cf91444ee2982470f647766f41e1a6125b37f219e53d1e58325e
- `tracks/` (13 track folders, 109 MB)

## 5. For the lead

- No `.ps1` / `.bat` edit needed, so there is no proposed diff.
- **Merge with E1 / E3..E5 and AI-TICK-FIRST.** `Core/FaithDefaults.cpp` `kFaith` and `Vehicle/PhysicsChainHooks.cpp`
  `ks[]` are edited on shared lines by every knob batch, and neither file is in `conflict_union`. The resolution is the
  union of the deletions: keep every entry that no merged branch deleted. On this branch the remaining `kFaith` lines 35
  (`RW_SSE_POINTS`), 40 (`REG_LATE`), 41 (`A5_DRAFT_LUT`) used to share a line with my names.
- **FIX_5CEA60 was a dead seed.** Its `kFaith` entry set `MASHED_FIX_5CEA60=1`, which no site read. The value 0.2778f came
  from the two sites' own default (`MASHED_NO_FIX_5CEA60` unset). The entry is gone with the opt-out.
- **`kTer_VelScale2` constant changed text, not effective value.** The header held 0.277779f and the only reader replaced
  it with 0.27777779f under the seeded knob. The header now holds the binary's value and the site reads it.
- **.asi.** `A6A_GEAR_ORDER` / `A6A_SPIN_ORDER` / `A6A_DOT_ORDER` keep the .asi default (ON). The only .asi change is that
  `MASHED_NO_<name>=1` no longer selects the old association there. The other 7 knobs are in exe-only TUs. No hook body's
  C-level moves, and no tracker row was touched.
