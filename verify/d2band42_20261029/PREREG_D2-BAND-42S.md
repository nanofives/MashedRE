# PREREG D2-BAND-42S: the D2 band re-measured on the original with a 42 s hold (L-57, run 22)

Date 2026-10-29. Branch `orch/D2-BAND-42S` from `orch/integration` @ f6033d936 (contains 4ee1823e1, the
`MASHED_DRIVE_HOLD_MS` harness knob). Host LIVING-PC. Written and committed alone, before any original or port run of
this step. No source change, no default change, no .ps1 edit, no tracker touched, `score_d2_v2.py` and the 30 s
`band.json` untouched.

## 1. The knob on the original side

`MASHED_DRIVE_HOLD_MS` is **standalone-only**: its one reader is `mashedmod/src/mashed_re/exe_main.cpp:1784`
(`s_driveHoldMs`, the demo's drive hold). MASHED.exe never reads it, and `re/frida/scenario_launch.py` has no reference
to it. The original-side window has its own mechanism: `scenario_launch.py --hold <s>` (`ap.add_argument("--hold")`,
"seconds to hold in the race after spawn"), a wall-clock hold during which the `.msd` capture grows. No .ps1 is involved.

The band's recipe (`verify/d2rebase_20261019/PREREG_d2rebase.md` section 2, `band.json`, the u9763 provenance
`verify/u9763_20261019/orig_solo_r1.msd.provenance.json`) is:

`scenario_launch.py --statediff-out <msd> --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38 --poke-ctrl-slots`

so the 30 s band was built with `--hold 38`, not 30. The port's "30 s -> 42 s" has two readings on the original side:

- **O42 (primary, the band this step delivers):** `--hold 42`, the literal 42 s hold. Everything else in the argv unchanged.
- **O50 (report arm):** `--hold 50`, the same +12 s delta applied to the band recipe's 38.

Both are captured, 8 runs each. Only O42 becomes `band.json` in this folder. O50 is written as `band_o50.json`, report only,
so the lead can pick it without a re-run. Window witness, report only: capture frames (provenance `capture.frames`) and the
reducers' driving-sample `n`, set next to the port arms' motion_diag row counts and `n`.

## 2. Runs

Each original run: `MASHED_ROOT=D:\MashedWT\D2-BAND-42S_root` (robocopy of `original\`, `MASHED.exe.unpatched` sha256
BDCAE093...3C0E = the anchor), cwd = this worktree, inside `test-slot` and
`mashed_lock.py run --solo --label D2-BAND-42S`, alone. Output `D:/MashedWT/D2-BAND-42S_data/orig/o42_r<N>.msd` /
`o50_r<N>.msd`, N = 1..8. Order: o42_r1..r8, then o50_r1..r8. Before every launch `Get-PnpDevice -Class AudioEndpoint
-PresentOnly`; none listed or an AV at `0x005baf48` = stop, BLOCKED 'audio endpoint', no retry. A crash at
`MASHED.exe+0x1509b4` or `+0xe66db` (U-9677) = re-run that run, not a finding. Any other crash or a capture that ends early
is reported and that run is re-run once; a second failure is reported RED.

Port arms on this worktree's exe (`MASHED_NO_DEPLOY=1` build of f6033d936, default, no source change), through
`verify/savelink_20261023/run_arms.py` with `A4_OUT=D:/MashedWT/D2-BAND-42S_data/arms`, `A4_ROOT=D:/MashedWT/D2-BAND-42S_root`,
`A4_D2_WALL=110` (the SUBH wall), each alone under `mashed_lock.py run --solo`:

- `D2_DEFH_1..3`: the default build with `MASHED_DRIVE_HOLD_MS=42000` (new stack `DEFH`, the only edit to run_arms.py).
- `D2_SUBH`: not re-run. The three D3-SUBSTATE-SETTLE-ORDER logs
  (`D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_data/final/D2_SUBH_1..3.motion_diag.log`, exe 58c21e05) are scored as they are.
- `D2_DEF` (30 s): the same step's `final/D2_DEF_1..3.motion_diag.log`, scored for reference.

## 3. Scorer and band rule

Scorer: `verify/d2rebase_20261019/score_d2_v2.py orig` for the originals (whole capture) and `port` for the port logs
(whole log), unchanged. Band rule unchanged from `band.json` / d81a8df6 section 3a, over the 8 runs of an arm, per metric
(slip1500, slip2000, drvmed): mean = arithmetic mean of the 8 values, h = (max - min) / 2, tol = max(3h, 2% of mean),
band = [mean - tol, mean + tol]. H1 is checked as before (>= 200 of 400 post-trough frames >= 100, median >= 900) and
reported per run.

The new band file is `verify/d2band42_20261029/band.json` (same schema as the 30 s one). A byte-identical copy of
`score_d2_v2.py` sits next to it, so `py -3.12 verify/d2band42_20261029/score_d2_v2.py port <log>` scores against the 42 s
band (the scorer reads `band.json` from its own folder). The 30 s files are not edited.

## 4. Predictions (registered before any run)

- P1: all 16 original runs boot, reach the race and write a capture; H1 398/400 median 1333.9 on every run (the 30 s
  band's eight all give that).
- P2: O42 capture frames > the 38 s recipe's 2335-2336; driving n > 1153-1185.
- P3 (no direction registered on the metrics): the O42 band is reported next to the 30 s band, per metric.
- P4: D2_SUBH_1..3 and D2_DEFH_1..3 scored against the O42 band, each metric in or OUT. No outcome registered.

## 5. Verdict rule

GREEN = the O42 band is built from 8 valid runs and the SUBH / DEFH in-or-out is stated. RED = fewer than 8 valid O42
runs after the re-run rule. BLOCKED = audio endpoint. Which window the flip uses (O42 or O50) is the lead's call. This
step does not make it.

## Amendment 1 (2026-10-29 16:20, after o42_r1..r7 ran, only o42_r1 scored)

The hold is wall-clock, so a slow stretch shortens the driving window. o42_r3 captured 2085 frames against 2518-2552 for
r1, r2, r4..r7: its lock line reads "draining the current holder" at boot, and its car first moves at +23 s against +18 s on
the others (`o42_r3.txt`). Rule added before r3 or any later run is scored: **a run is valid when its capture frames are
within 5% of the arm's median capture frames.** An invalid run is re-run once as `r<N>b` and the re-run replaces it in the
band. The invalid run is scored and reported, never banded. A second invalid run for the same N is reported RED. Applies to
O42 and O50 alike. The 30 s band's eight runs all pass this rule (2335-2336 frames on the four u9763 runs).
