# PREREG librwrest_checks: the two original-side checks librwrest could not run (L1, W1)

Branch `work/librwrest_checks`, base `race/first-frame-parity` `e9a425db` (contains the merged
librwrest, `7d9353eb`). Written and committed before the first measuring run. Model
`claude-sonnet-5-5` (account2: no Ghidra MCP, decomp through the read-only
`re/tools/decomp_pc.py`, outputs `decomp_livery_wheel.txt` and `decomp_units.txt` in this
directory). The gates are `verify/librwrest_20261016/PREREG_librwrest.md` section 3, items **L1**
and **W1**, reused verbatim:

> **L1** livery: the standalone's per-car DFF entry (logged at load) equals the original's
> rec+0x268 entry DFF for all 4 cars on the 3 d1r1 tracks.
> **Prediction Pr-L:** every car in the 3 d1r1 captures has colour 0 and `ADVANTAGE0.DFF` (the
> race_hud all-RED reference, Finding 16), so on those frames the livery port changes nothing.
> If Pr-L holds, L1 is also run on one original scenario with a non-zero colour if the scenario
> tooling can set one; otherwise L rests on L1 plus the selection rule's code citation.
>
> **W1** law, on the carparts windows of all 3 tracks: for every row, car, and wheel, the frame
> matrices the port's code computes from that row's inputs (spin, steer, vec3s, previous row's
> angle, units = the ff4 delta) equal the original's recorded modelling matrices, |d| <= 1e-4 per
> element; and the recorded angle equals the port's accumulated angle.

Why this ran now: the PC's audio endpoint is back (`Get-PnpDevice -Class AudioEndpoint`: Philips
FTV (NVIDIA High Definition Audio), Status OK, 2026-10-16), so `MASHED.exe` boots again (memory
`original-boot-av-without-audio-endpoint`). The librwrest result recorded L1 and W1 as NOT RUN.

## 1. Amendments to the verbatim gates (all made before the first run, with the evidence)

- **A1, units.** The gate says "units = the ff4 delta". `FUN_0040fc00` @ `0x0040fc00`
  (`decomp_units.txt`) stores `DAT_007f1008 = unaff_ESI` (the frame's 1/3000 s tick count) before it
  calls `FUN_0040da50`, and adds ESI to the ff4 clock `DAT_007f0ff4` only at its end, and only when
  `DAT_0063ba8c != 7`. A probe row is taken after `FUN_0040da50`, so the row's ff4 is the value
  before this frame's increment and the units the spin step used are `ff4(n+1) - ff4(n)`, not the
  delta to the previous row. The probe now records `DAT_007f1008` directly (`units` per snapshot).
  W1 uses that value. As a cross-check it also reports how many rows have
  `units == ff4(n+1) - ff4(n)`; a disagreement there does not fail W1.
- **A2, "the port's code".** `CarParts_FrameTick`'s per-wheel spin arithmetic is extracted
  unchanged into `CarPartsSpinStep` (`TrackRenderer.cpp`, arithmetic and fold order identical, the
  live path now calls it). A new diagnostic, default OFF, one shot, `MASHED_CARPARTS_LAW=<in>`
  `MASHED_CARPARTS_LAW_OUT=<out>` (`TrackRenderer::CarPartsLawReplay`), feeds the recorded
  per-row inputs through `CarPartsSpinStep` and `BuildCarPoses` on the loaded car model and writes
  the poses. The steer input is the recorded 16-float-window mean (the probe records the mean, not
  the window), so the port's window sum (`CarParts_FrameTick`) is NOT exercised by W1 and is
  reported as not covered.
- **A3, which set is compared.** `FUN_0041f3d0` @ `0x0041f3d0` (decomp) poses rec[w] when
  rec+0x298 == 0, rec[0x21+w] when == 1, and returns for any other value. W1 compares only the set
  rec+0x298 selects. The port poses both sets (librwrest RESULT section 2), so the unselected set is
  out of the gate. A row whose rec+0x298 is neither 0 nor 1 is excluded and counted.
- **A4, rows the original did not update.** `FUN_0040da50` @ `0x0040da50` (decomp) calls
  `FUN_00420870` only when `FUN_0046c7b0(i) == 1` (veh+4) or `FUN_0046c770(i) == 1` (not bound in
  the port, U-9210). The probe now records veh+4 (`al`). A (row, car) is excluded from W1 only if
  `al != 1` AND the recorded angles equal the previous row's; every exclusion is counted and listed.
- **A5, first row.** The first row of each 60-row window has no previous-row angle and is not
  compared (59 rows per window). Windows are consecutive trace rows (checked by row index).

## 2. Pass definitions (fixed now)

- **W1-a angle:** for every compared (row, car, wheel w) `|port angle - recorded angle| <= 1e-4`
  degrees, where port angle = `CarPartsSpinStep(prev recorded angle, units, spin)` and recorded =
  rec+0x220+w*4 at the row. The count of bit-identical float results is reported next to it.
- **W1-b matrices:** for every compared (row, car, wheel w of the selected set) the port's wheel
  frame, hub frame and, for the car, part 0x13's frame equal the recorded atomic-frame modelling
  matrix, the recorded parent-frame modelling matrix and the recorded part 0x13 modelling matrix,
  `max |d| <= 1e-4` over the 12 (right, up, at, pos) elements.
- **W1 verdict:** PASS only if W1-a and W1-b both hold on all three tracks, on every arm. FAIL
  otherwise, and the verdict lists the first failing (row, car, wheel) with both values.
- **L1-a rule (original):** for every car, the recorded descriptor (rec+0x268) satisfies
  `id % 6 == colour` (colour = `DAT_007f1a1c[i*4]`; slot index `i` when the mode word `0x0067e9fc`
  is 0xb at `FUN_0040d110`'s call) and `dff == <name>%d.DFF` with that `id % 6`
  (`FUN_0040d110` @ `0x0040d110` decomp: id = value + (groupID/6)*6; `FUN_0041f330` returns the
  0x84-byte table entry whose id (+0x80) matches).
- **L1-b standalone equals original:** the standalone's logged livery for slot i
  (`[librwrest] livery: slot i ... -> <DFF>` in `mashed_re.log`) equals the original's recorded
  descriptor DFF for slot i, compared case-insensitively, on all 4 cars of each arm.
- **L1 verdict:** PASS only if L1-a and L1-b hold on all arms that have both sides; the mode 0xb
  arm has the original side only (the standalone has no setting for the mode word) and is reported
  as an L1-a result.

## 3. Instruments

- Original side, `re/frida/camera_probe.py --carparts` (librwrest `f5ae95a1`), with these additions
  made before the first run: `--colours a,b,c,d` and `--livery-mode N` write the per-car colour
  `DAT_007f1a1c[i*4]` and the mode word `0x0067e9fc` at `FUN_0040d110`'s entry (mode word restored
  at its exit), so the original's own selection runs on non-zero colours (the prereg's "scenario
  tooling can set one"); `units` and `al` per snapshot; `poke` recorded in the JSON. A poke is an
  input write, the descriptor read is the measurement. Race entry is the d1r1 recipe
  (`--frame --early --carparts --track N`), windows are the 60 rows ending at each `--shots-race`
  row.
- Standalone side: default build of this branch, run through a private `MASHED_ROOT` copy of
  `original\` (CWD-relative `mashed_re.log` stays private, memory `mashed-root-main-shares-append-logs`),
  `MASHED_TRACK_VIEW=<track>`, `MASHED_ROUND_COLOURS=<a,b,c,d>` (the same colours as the original's
  poke), knobs `MASHED_CAR_LIVERY` and `MASHED_CAR_PARTS` unset (default ON), plus the W1 replay
  env vars of A2.
- Tools in this directory: `run_stand.py` (standalone runner), `l1_check.py`, `w1_check.py`.

## 4. Arms

Each original arm runs `camera_probe.py --frame --early --carparts --shots-race 100,400,800
--track N --rows R`; R is chosen so the trace covers raceStart + 800 + 1.

| arm | track (index) | poke | standalone MASHED_ROUND_COLOURS |
|---|---|---|---|
| T0 | TRAINING (0) | none | none |
| A0 | Arctic (3) | none | none |
| C0 | City (12) | none | none |
| T1 | TRAINING | colours 1,2,3,4 | 1,2,3,4 |
| A1 | Arctic | colours 5,4,3,2 | 5,4,3,2 |
| C1 | City | colours 3,0,5,1 | 3,0,5,1 |
| T2 | TRAINING | mode word 11, colours unchanged | not available (original-side L1-a only) |

Prediction Pr-L2 (stated before running): T0, A0 and C0 show colour 0 and `ADVANTAGE0.DFF` on all
4 cars (as d1r1 recorded), and every poked arm shows `id % 6 == colour` with the DFF digit equal to
it. Prediction Pr-W1: the angle law holds to float precision on every wheel; an exception would
come from a row where the original skipped the update (A4) or a units source other than
`DAT_007f1008`.

## 5. Process

- PIDs: every MASHED process is one this session spawned (camera_probe.py kills only its own pid,
  `run_stand.py` only its own). No `original\mashed_re_dev.asi` is deployed or changed.
- A boot/load crash on the original with signature `MASHED.exe+0x1509b4` or `MASHED.exe+0xe66db`,
  or an audio-endpoint AV (EIP `0x005baf48`), is re-run, not reported as a finding.
- Trackers: no tracker row is touched by a measurement. If a hooks.csv row needs to move it goes
  through `re-classify`, and none is expected: neither the livery nor the wheel bodies have a
  Frida diff at function level, and W1 here is a law check on recorded state, not a `diff-original`
  of a hooked function (CONFIDENCE rubric: no C-level promotion is claimed).
- Gate failure: locate the cause, fix if small and cited, otherwise recommend the knob default OFF
  (`MASHED_CAR_LIVERY` for L1, `MASHED_CAR_PARTS` for W1).
- Evidence goes under `verify/librwrest_checks_20261016/`, result in
  `RESULT_librwrest_checks.md`. Large traces stay out of git; the carparts JSON, the replay
  inputs and outputs, and the check reports are committed.
