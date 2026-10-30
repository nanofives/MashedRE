# RESULT D5-KNOBS-E1-PHYS-A4A5 (2026-10-30)

Verdict: **GREEN**. All 10 named knobs deleted (read sites, `kFaith` entries, the .asi seed entries). The exe builds,
rva-lint reports 0 NEW, and G-CTRL and B are unchanged. (b) is 3/3 and (e) is 3/3 with launch unchanged. D2 DEFH is 3/3
in the 42 s band. The 13-track table is identical to the reference (39/39 rows, INSTRUMENT GATES PASS). No knob moved a
hash, so the E1 rule keeps none of them.

Step: physics A4/A5 batch of the seeded knobs listed in `verify/d5knobsinvfix_20261030/RESULT_D5-KNOBS-INVENTORY-FIX.md`,
section "Outside class d". Branch `orch/D5-KNOBS-E1-PHYS-A4A5` from `4b04c3841`, source commit `a6d01b132`. No default
flipped. Each of the 10 knobs was seeded ON in the standalone by `Core/FaithDefaults.cpp` (`kFaith`). 7 of them were also
seeded ON in the .asi by `Vehicle/PhysicsChainHooks.cpp` (`A6aAsiDefaults`). Every site keeps the seeded path, so the
default build runs the same code before and after.

## 1. Audit, one line per knob (read site at `4b04c3841`)

Every read was `e && e[0] == '1'` on `MASHED_<name>` (unset = OFF at the site, ON through the seed). Opt-out was
`MASHED_NO_<name>=1`, `MASHED_NO_FAITH=1`, or `MASHED_<name>` set to anything but `1`. `S` = `mashedmod/src/mashed_re/`.
The script check is `git grep` over `scripts/`, `re/tools/`, `re/frida/` and `re/orchestrator/`: **0 hits for all 10**
(and 0 for `MASHED_NO_FAITH`). Every hit is under `verify/`. "run_arms" = `verify/savelink_20261023/run_arms.py` line numbers.

| Knob | Read site | Seed (exe / .asi) | In .asi build | verify/ files (MASHED_NO_ form) | run_arms stacks | Gate arms use it |
|---|---|---|---|---|---|---|
| A5_RUBBER_LIVE | `S/Vehicle/ForceIntegratorLeaves.cpp:39` (exe-only `#ifdef`) | FaithDefaults.cpp:48 / no | the .asi already read the live words | 50 (45) | RL :381, NR :387 | no |
| A6A_LC0 | `S/Vehicle/Integrate2.cpp:227`, use :647 | :33 / PhysicsChainHooks.cpp:66 | yes, seeded ON | 69 (0) | LC :143 | no |
| A4_DOT_ORDER | `S/Vehicle/VehicleControl.cpp:171`, use :209 | :62 / :73 | yes, seeded ON | 39 (0) | P2 :423 | no |
| A4_ORDER | `S/Vehicle/VehiclePhysicsRun.cpp:939` (`VehiclePhysics_A4OrderEnabled`), callers `S/D3d9Render/TrackRenderer.cpp:5479`, `:5638`, `:5674` | :37 / no | no (exe-only TUs) | 91 (0) | `_A4` :58-59, :195, :196 | no |
| A5_AIR_F32 | `S/Vehicle/ForceIntegrator.cpp:132`, use :460 | :62 / :71 | yes, seeded ON | 39 (0) | P2 :422 | no |
| A5_CONTACT_INT | `ForceIntegrator.cpp:137`, use :392 | :62 / :71 | yes, seeded ON | 39 (0) | P2 :423 | no |
| A5_DOT_ORDER | `ForceIntegrator.cpp:143`, uses :241, :361 | :61 / :71 | yes, seeded ON | 39 (0) | P2 :422 | no |
| A5_FD0_LIVE | `ForceIntegrator.cpp:44`, `ForceIntegratorLeaves.cpp:31` (both exe-only `#ifdef`) | :37 / no | the .asi already read the live word | 59 (0) | FL :304, FAITH :317, FNF :325 | no |
| A5_RING_ORDER | `ForceIntegrator.cpp:380` | :61 / :71 | yes, seeded ON | 61 (0) | FAITH2 :321, FRO :323, TC0RO :365, P2 :422 | no |
| A5_SPIN_FILD | `ForceIntegrator.cpp:142`, use :413 | :62 / :71 | yes, seeded ON | 39 (0) | P2 :423 | no |

Gate arms: `H_DEF`, `B_DEF_K0/K1/K26` and `D2_DEFH_1..3` set stack `DEF` (empty) and `DEFH` (`MASHED_DRIVE_HOLD_MS`
only). `run13.py` sets none of these names. So the gates only ever saw the seeded values.

Stacks that now run the same code as the default (report-only, none is a gate): `NR` (:387), `FNF` (:325, its FD0 half),
`NF` (:328) and `OLDNF` (:405), for these 10 knobs only, since `MASHED_NO_FAITH=1` no longer turns them off. The `RL`, `FL`,
`LC`, `P2`, `FRO`, `TC0RO` and `_A4` stacks now set an unread name. The 101 frozen runner copies under `verify/` are left
unedited, as in B1 / B2 / B3.

## 2. Change (commit `a6d01b132`, 10 files, 61 insertions, 117 deletions)

- `Vehicle/ForceIntegrator.cpp`: `Fd0Word` reads `DAT_007f0fd0` in both targets (the `g_playerCount` arm is gone). The
  helpers `A5AirF32`, `A5ContactInt`, `A5SpinFild` and `A5DotOrder` are deleted, and each use keeps its ON branch: the
  drafting dot and steer projection in the original's association, the integer contact test `asI(pf[-0x17]) == 0`, the
  `FILD` read `(float)self[0xb]`, and the float chain for the not-all-grounded force (the double `else` is gone).
  `s_ringOrder` is gone, and `A5FaceNormal(&sc[0], &sc[6], &sc[3], ...)` is the only call left.
- `Vehicle/ForceIntegratorLeaves.cpp`: `Fd0Word`, `RubberRefDist` and `RubberRefCar` are one definition for both targets
  (the live words, as the .asi already had). `A5RubberLive` is deleted, and the `#ifdef` now covers only the game-mode
  thunks.
- `Vehicle/ForceIntegratorStubs.cpp`: comment only. `g_rubberBand` / `g_rubberRefCar` stay, because
  `forceint_selftest.cpp:60` still writes them.
- `Vehicle/Integrate2.cpp`: `A6aLc0` deleted, and lc0 is `(+0x9d8*s1 + +0x9d4*s0) + +0x9dc*(inv*ld8)` only.
- `Vehicle/VehicleControl.cpp`: `A4DotOrder` deleted, and the slide dot keeps the original's association only.
- `Vehicle/VehiclePhysicsRun.cpp/.h`: `VehiclePhysics_A4OrderEnabled` deleted.
- `D3d9Render/TrackRenderer.cpp`: the A4 pre-phase block (`A4PreBegin` / `A4Pre` / `A4PreEnd`) runs unconditionally. The
  two `MASHED_AI_TICK_FIRST || A4OrderEnabled()` statics are gone: `Race::PathProg_FrameTick()` and the early
  `ai_tick_phase()` (still under `g_aib.loaded && race count > 3`) run unconditionally.
- `Core/FaithDefaults.cpp`: the 10 `kFaith` entries and the `[rbflip]` comment are removed, with a note naming them.
- `Vehicle/PhysicsChainHooks.cpp`: the 7 .asi seed names are removed from `A6aAsiDefaults`, with a note naming them.

Built binary check (Python `find(b"MASHED_<name>\0")`): all 10 names occur 0 times in `mashed_re.exe` and in
`mashed_re_dev.asi`. The one `A4_ORDER` substring left is `MASHED_A4_ORDER_TRACE`, a separate trace knob, kept.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` gave `=== Build OK ===`, 0 `error C` / `LNK` lines. The shared
`.asi` was not deployed. rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/D5-KNOBS-E1-PHYS-A4A5_root`, run under
`mashed_lock.py run` (D2: `run --solo`) and `test-slot`. 13 tracks: `verify/rsky_20261016/run13.py run`, root
`D:/MashedWT/D5-KNOBS-E1-PHYS-A4A5_root13`, under `run --solo`, then scored with `--cite verify/rres_20261016/cites_rres.json`.
Refs come from `re/orchestrator/merge_train.json`.

| Gate | This exe (`mashed_re.exe` sha256 `2ae87d82`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3 arms at 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1917.10 x3, slip1500 0.1933 / 0.1930 / 0.1933, all `in`, H1 398/400. motion_diag 1639 / 1641 / 1639 lines (min 1600, no re-run) | 3/3 in band | same (3/3) |
| 13 tracks, `run13.py score` | 39 rows identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt`, INSTRUMENT GATES: PASS, file sha256 `2431afb1` | `2431afb1` | same |

Every arm and all 13 tracks ran on the first attempt: no re-run, no piz collision. The lock queued behind other
sessions' windows (D5-KNOBS-RUNARMS-STACKS, D5-SHIM-RWBRIDGE-COMMENT, D5-FLIP-MUSIC-NOCDAUDIO) and nothing was broken.
Committed copies are in this folder: `vsum_b.txt`, `e_check.txt`, `score_d2.txt`, `tracks_score.txt`.

E1 rule: no knob moved a G-CTRL or B hash, so none stays in.

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-E1-PHYS-A4A5_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `arms/D2_DEFH_2.motion_diag.log` e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258
- `arms/D2_DEFH_3.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `tracks/` (13 track folders, 108 MB)
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 4916f94f69815ad583a54e87164b572a85e57091af125413a0e3bd5269475d8d
- `score_d2.txt` 8782f4fc5d535948db5658a590993f80ccf3c30b385cdee0d2fb00f07d3fe654
- `tracks_score.txt` 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3
- `build.log` 6520999aa937732d4e3fcb8377ed1c0a23bf28af1dd371146d6911570f8fb143
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` e356c9ad8e39617f383ceeaa1d4cfa2ddcded0c3a52575b5a5f25b91ec767ac7
- `d2_run.log` 0e37db67a7934ea8ecc25ddb66c8bd45e2226f336d804d59e66a9b48b3986963
- `run13.log` 269a870aaf9e957da3462848efebac618dcfb8ae74cfc29236e7ef3eb7cea52f
- exe `mashedmod/build/mashed_re.exe` 2ae87d821c1e3a192d2ac30ccca63707725ae73769cecca114abba413f5d2f6e
- asi `mashedmod/build/mashed_re_dev.asi` 275bdfbf88b360b2ee0abc891e50f1e3f593908ddcef34e78a55459aa92fd10e (built, not deployed)

## 5. For the lead

- No `.ps1` / `.bat` edit needed, so there is no proposed diff.
- **Merge order with E2..E5.** `Core/FaithDefaults.cpp` `kFaith` and `Vehicle/PhysicsChainHooks.cpp` `ks[]` are edited
  by every E-batch on shared lines. For example, line 37 holds `A4_ORDER` (E1), `REG_LATE` (E4), `A6A_GEAR_ORDER` (E2)
  and `A5_FD0_LIVE` (E1). Neither file is in `conflict_union`, so the second E-branch to merge will halt. The resolution
  is the union of the deletions: keep every entry that no merged branch deleted. `A6A_GEAR_ORDER`, `A6A_SPIN_ORDER`,
  `A6A_DRIVE_NORM_LUT`, `A6A_DOT_ORDER`, `A6B_*`, `A5_DRAFT_LUT` and `REG_LATE` are not mine and stay on this branch.
- **Side effect on a class (d) knob.** `MASHED_AI_TICK_FIRST` was read only at the two `TrackRenderer.cpp` sites ORed
  with `A4_ORDER` (one more `ai_tick_ran` site, `:6004`, does not read it). With `A4_ORDER` always on it has no
  reader left. Its `kFaith` entry (FaithDefaults.cpp:30) is now a dead seed, left for its owner (class d, TRIAGE-1 row 8).
  `MASHED_AI_TICK_FIRST=0` could never turn the early tick off under the seeded `A4_ORDER` anyway.
- **.asi.** The 7 .asi-seeded knobs keep the .asi default (ON). The only .asi change is that `MASHED_NO_<name>=1` no
  longer selects the old path there. FD0 / RUBBER were already the live words in the .asi, and A4_ORDER is exe-only.
  No hook body's C-level moves, and no tracker row was touched.
