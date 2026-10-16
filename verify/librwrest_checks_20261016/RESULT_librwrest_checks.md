# RESULT librwrest_checks: L1 (livery) and W1 (wheel / steer / body-lean law) against the original

Branch `work/librwrest_checks`, base `race/first-frame-parity` `e9a425db`. PREREG
`PREREG_librwrest_checks.md` (`94cfb916`, amended in `437bbf55` and in the commit that holds this
file). Model `claude-sonnet-5-5` (account2, no Ghidra MCP, decomp through `re/tools/decomp_pc.py`).
The audio endpoint was back (Philips FTV, Status OK), so the original booted on every run.

**Verdict.** **L1 PASS** (7 original-side arms, 6 with a standalone side) and **W1 PASS** (8 captures,
3 tracks). Both knobs stay default ON. Neither gate failed, so no fix was needed. Coverage limits are
listed in section 4 and are part of the verdict.

Standalone under test: default build, knobs `MASHED_CAR_LIVERY` / `MASHED_CAR_PARTS` unset,
`mashedmod/build/mashed_re.exe` SHA-256 `842EF63C36AD6CCD7500B42D6A1CE2379D01983C20A75B4230465DC186A6A7DD`,
built from commit `437bbf55` sources (no `mashedmod/src` change after the build). Original:
`original/MASHED.exe` as installed (the patched boot recipe).

## 1. Gates

| gate | result |
|---|---|
| **L1-a** original rule, `orig_<arm>/trace.csv.carparts.json`, `L1_report.txt` | PASS 7/7 arms, 28/28 cars: `id % 6 == colour` and the descriptor DFF digit equals it (mode 0xb arm: equals the slot index) |
| **L1-b** standalone DFF equals the original's rec+0x268 DFF | PASS 6/6 arms, 24/24 cars (T0 A0 C0 T1 A1 C1); T2 (mode 0xb) has no standalone side |
| **W1-a** angle law, all 4 wheels | PASS: 12,860 wheel steps over 8 captures, 0 above 1e-4 deg, **12,860 bit-identical** (float32) |
| **W1-b** wheel, hub, part 0x13 frames vs recorded modelling matrices | PASS: 28,935 frames compared, 0 failing, 0 missing; worst `max abs d` 5.0e-10 (wheel and body), 4.98e-10 (hub), tolerance 1e-4 |
| W1 negative control (`negctl.py`, T0) | comparator FAILS on perturbed inputs as it must: 1900 / 2024 angle steps and 4430 / 4554 frames fail (`orig_T0/W1_negctl_report.txt`) |
| L1 negative control (standalone given colours 0,0,0,0 against T1's 1,2,3,4) | L1-b FAILS 4/4 cars (`L1_negctl_report.txt`) |

L1 per arm (`orig` = rec+0x268 descriptor read by `camera_probe.py --carparts`):

| arm | track | colours the original had | DFFs (original = standalone) |
|---|---|---|---|
| T0 | TRAINING | 0,3,1,2 (natural, not poked) | Advantage0, 3, 1, 2 |
| A0 | Arctic | 0,3,1,2 (natural) | same |
| C0 | City | 0,3,1,2 (natural) | same |
| T1 | TRAINING | 1,2,3,4 (poked at `FUN_0040d110` entry) | Advantage1, 2, 3, 4 |
| A1 | Arctic | 5,4,3,2 (poked) | Advantage5, 4, 3, 2 |
| C1 | City | 3,0,5,1 (poked) | Advantage3, 0, 5, 1 |
| T2 | TRAINING | natural 0,3,1,2, mode word 0x0067e9fc forced to 11 during `FUN_0040d110` | Advantage0, 1, 2, 3 (slot index; ids 6,7,8,9) |

Ids are 6..11 throughout, i.e. `(groupID / 6) * 6 == 6` for the Advantage group (`FUN_0040d110`
`0x0040d110` decomp, `decomp_livery_wheel.txt`).

W1 per capture (`orig_<arm>/W1_report.txt`; `orig_T0a` is the first smoke capture, kept):

| capture | rows | records compared | excluded | angle steps | frames | verdict |
|---|---|---|---|---|---|---|
| T0 | 175 | 506 | 194 | 2024 | 4554 | PASS |
| A0 | 119 | 353 | 123 | 1412 | 3177 | PASS |
| C0 | 161 | 332 | 312 | 1328 | 2988 | PASS |
| T1 | 175 | 505 | 195 | 2020 | 4545 | PASS |
| A1 | 146 | 367 | 217 | 1468 | 3303 | PASS |
| C1 | 179 | 345 | 371 | 1380 | 3105 | PASS |
| T2 | 175 | 508 | 192 | 2032 | 4572 | PASS |
| T0a | 122 | 299 | 189 | 1196 | 2691 | PASS |

A record is one (row, car); a row is one trace row (the race frame), the windows are the 60-row
blocks ending at race start +60, +120 ... +420 that exist before the round ends. Exclusions are all
A5 (the first row of a block, 4 per capture) or A3/A7 (rec+0x298 at row n-1 equals 2). No A4
exclusion occurred (`veh+4 != 1` never coincided with an unchanged angle).

Units: `DAT_007f1008` (recorded per row) equals `ff4(n+1) - ff4(n)` on every row that has a successor
in every one of the 8 captures (bad = 0 in all 8, e.g. T0 174 ok / 0 bad, `W1_report.txt`).

## 2. Predictions

- **Pr-L2 (colour 0 on T0, A0, C0): falsified.** The natural Quick Battle navigation gave colours
  0,3,1,2 on all three tracks (ids 6,9,7,8 in the descriptors). The d1r1 captures' all-RED reference
  is therefore not what this navigation produces. The cause was not investigated (every run, natural
  or poked, logged two `[nav] target depth 3` retries before the race). Consequence: every arm, natural or
  poked, has cars with colour != 0, so the "at least one scene with colour != 0" requirement is met
  by all 7 arms.
- **Pr-W1 (angle law holds to float precision): held**, 12,860 / 12,860 bit-identical.

## 3. What was checked and how

- **Original side.** `re/frida/camera_probe.py --frame --early --carparts --shots-race
  60,120,180,240,300,360,420 --track N --rows 2200` per arm (`run_orig_arms.py`). New in this branch:
  `--colours` / `--livery-mode` write the colour `DAT_007f1a1c[i*4]` and the mode word `0x0067e9fc`
  at `FUN_0040d110`'s entry (mode word restored at its exit), `units` (`DAT_007f1008`) and `al`
  (veh+4) per snapshot. The poke writes inputs only, the descriptor and matrices are read from the
  original's own state afterwards.
- **Standalone side.** `run_stand.py` runs the race demo through a private `MASHED_ROOT` copy of
  `original/` (SHA-256 of `MASHED.exe` equal to the install's), with `MASHED_ROUND_COLOURS` set to the
  colours the original capture recorded, and `MASHED_CARPARTS_LAW=<records>` (new, default OFF,
  one shot, `TrackRenderer::CarPartsLawReplay`). The replay feeds each recorded row (previous row's
  angle, units, spin ints, steer mean, the two body vectors) through `CarPartsSpinStep` (the
  per-wheel arithmetic of `CarParts_FrameTick`, extracted unchanged and now called by it) and
  `BuildCarPoses` on the loaded livery model, and writes the poses. `w1_check.py compare` checks
  them against the recorded atomic-frame, parent-frame and part 0x13 modelling matrices.
- **Amendments** A1..A8 (units source, the port's code, LOD set, rows not updated, first row, shot
  lists, LOD timing) are in the PREREG with their evidence. The material one is A7: a probe row is
  read after `FUN_00420870` returned, so rec+0x298 at row n is the value after that frame's update
  while `FUN_0041f3d0` used the previous row's. Evidence: car 0 in `orig_T0a` at row 1191 records 1
  but its angles equal row 1190's (2 there) with spin 952 and units 50.

## 4. Findings and coverage limits

1. **The original freezes the wheel angles when rec+0x298 is neither 0 nor 1.** `FUN_0041f3d0`
   `0x0041f3d0` returns at the top of its loop (decomp). Measured: 617 of 617 (row, car) pairs with
   rec+0x298 == 2 at row n-1 in T0, A0 and C0 have all four angles unchanged, 542 of them with
   non-zero spin. `FUN_00420870`'s visibility switch (decomp, `0x00420a33` area) shows the wheel
   parts 0..3 only at 0 and 0x21..0x24 only at 1, so no wheel part is shown at 2. **The port's
   `CarParts_FrameTick` has no such gate**: it advances the angles at every LOD. No car returned from
   LOD 2 to LOD 1 inside any captured window, so the phase difference never reached a visible row
   here. It would after any LOD 2 -> 1 transition, by the spin accumulated meanwhile. The LOD state
   is carlod's scope and is not in the standalone, so no fix was made. Follow-up for carlod: gate
   the step on the port's LOD in {0, 1}. The writer of rec+0x298 is not identified in this work.
2. **LOD 0 (wheel set parts 0..3) was never observed.** rec+0x298 at row n-1 is 1 or 2 on all 4972
   (row, car) pairs of the 8 captures, never 0. Every compared frame is therefore the set
   0x21..0x24 (k 4..7). The port builds the set 0..3 with the same code and frame-index logic, but
   no original data covers it. `FUN_0041f770` (decomp, `decomp_lod.txt`) has a `rec+0x298 == 0` arm,
   so the state exists in the original.
3. **The arms are three distinct trajectories plus T0a.** T0, T1 and T2 show identical steer / lean
   maxima (4.197 deg steer, 3.109 deg X lean), as do A0 and A1, C0 and C1: the colour or mode poke
   does not change the race. Input ranges over the compared records: steer up to 14.7 deg
   (Arctic), body lean Z up to 0.79 deg and X up to 3.11 deg, spin ints up to 5449.
4. **Not covered by W1.** The port's own window sum for the steer mean (the replay takes the
   recorded mean, `CarParts_FrameTick`'s 16-float sum is not replayed); the live inputs (the port's
   own spin ints and `units`, `TrackRenderer.cpp` `CarParts_FrameTick(units)` at the race tick) are
   not compared with the original's, which is physics scope; the standalone side of the mode 0xb
   arm (no setting for `0x0067e9fc` exists in the standalone); the LOD 2 behaviour in finding 1.
5. **Original-side race length varies.** The racing phase lasted 556 rows in `orig_T0a` and 119 to 179
   rows in the final arms (an arm's snapshot count, because its windows run contiguously from the race
   start to the end of the round): the round ends at a variable time, so windows past the end do not
   exist. This is why the shot list was changed twice (A6, A8).

## 5. Source changes (default build)

- `TrackRenderer.cpp`: `CarPartsSpinStep` extracted from `CarParts_FrameTick` (same operations in the
  same order, so the live arithmetic is the function W1 exercised). `CarPartsLawReplay` and one call
  in `Render` (env-gated, default OFF, one shot, no effect when `MASHED_CARPARTS_LAW` is unset).
  `TrackRenderer.h`: its declaration.
- `re/frida/camera_probe.py`: `--colours`, `--livery-mode`, `units` and `al` per carparts row, the
  `poke` in the carparts JSON, `mode_word_0067e9fc` in the livery record.
- G-CTRL was **not** re-run: the change touches render-side code only and no sim state reads
  `car_parts_`, but the hashes `c2e4d60d / b50bd39b / ba90ed88` were not measured on this branch.

## 6. Trackers

None touched. No `hooks.csv` row moves: W1 is a law check on recorded state against the port's code
on recorded inputs, not a `diff-original` of a hooked function, and no C-level is claimed.
Candidate DEFERRED rows for the parent (not written, trackers go through `re-classify`): the LOD
gate of finding 1 (re-pickup: carlod's LOD state exists in the standalone), the LOD 0 coverage of
finding 2 (re-pickup: an original capture in which rec+0x298 == 0).

## 7. Reproduce

```
py -3.12 verify\librwrest_checks_20261016\run_orig_arms.py T0 A0 C0 T1 A1 C1 T2   # original, ~1 min per arm
robocopy original <scratch>\gameroot\original /E                                  # private MASHED_ROOT
py -3.12 verify\librwrest_checks_20261016\run_checks.py <scratch>\gameroot T0 A0 C0 T1 A1 C1 T2
py -3.12 verify\librwrest_checks_20261016\negctl.py <scratch>\gameroot T0
```

`orig_<arm>/trace.csv.carparts.json.gz` and `stand_<arm>/law_out.txt.gz` are committed gzipped
(`gunzip` to re-run the compare). Raw traces, backbuffer dumps and the private game root stay local.
`run_checks_all.txt` is the console output of the full run.

## 8. Process

- Only PIDs this session spawned were stopped. The first capture driver was stopped mid-run to change
  the shot list (its own process tree only).
- `mashedmod\build.bat` deployed `mashed_re_dev.asi` into this worktree's own `original\`, not into
  the main install: the main `original\mashed_re_dev.asi` still has its 2026-10-13 timestamp.
- No original-side load crash and no audio AV occurred on any of the 7 final runs, so no re-run was
  needed under the piz-collision rule.
