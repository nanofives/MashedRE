# PRE-REGISTRATION — D2 attempt 15, STEP 3: re-score the default build with the writer ported

Written and committed **before** the first measurement run of this step. Nothing below may
be amended after a run. If a registered gate fails, the step STOPS and reports the failure.

Branch `race/first-frame-parity`. The build under test is the default one: `0x0046d780`
and `0x0046d7f0` ported at their own RVAs in `Vehicle/LaunchRevCharge.cpp`, wired from
TrackRenderer's countdown, **and `MASHED_D2_BOOSTHOLD` removed from the code**. Build OK,
`rva-lint allowlisted=122 NEW=0`.

---

## 1 The arm — unchanged from §16.7, stated so it cannot drift

```
py -3.12 re/tools/statediff/a8_run_port.py <outdir> 90 \
      MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 \
      MASHED_TITLE=<tag>
```

`a8_run_port.py`'s recipe supplies `MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1
MASHED_PLAY_DEMO=1 MASHED_GOTO=6 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1
MASHED_WIN_POS=primary-bl MASHED_MOTION_DIAG=1 MASHED_STEER_HOLD=1 MASHED_MUTE=1`;
the three extras above override the track and the steer-hold onset. **No other env var is
set**, in particular no launch-rev knob — `MASHED_NO_LAUNCH_REV` is the A/B control and is
absent from every scored run.

Reference: `verify/d2_reopen_20260929/orig_solo3.msd`, release frame **886**; port release
frame **1**. Reducers, identical to §1's:

```
py -3.12 re/tools/statediff/a8_slip_axis.py --orig <orig>.msd --port <dir>/motion_diag.log --max-lines 1080
py -3.12 re/tools/statediff/a8_momentum.py  --orig <orig>.msd --port <dir>/motion_diag.log --max-lines 1080 \
        --orig-steer-min 33.0 --port-steer-min 0.9
py -3.12 re/tools/statediff/a8_medframe.py  --orig <orig>.msd --port <dir>/motion_diag.log
py -3.12 re/tools/statediff/a8_launch.py    --orig <orig>.msd --port <dir>/motion_diag.log
```

### The two reducers rebuilt this attempt, and how far they are trusted

Attempt 14 computed the launch/recovery table and the median-frame columns in scratchpad
scripts that no longer exist. `a8_launch.py` and `a8_medframe.py` are standing replacements
whose definitions were **pinned by reproducing attempt 14's published numbers on attempt
14's own captures** before being used here:

| | attempt 14 published | `a8_launch.py` reproduces |
|---|---|---|
| ORIG peak / at `d` | 1832.40 / 95 | **1832.40 / 95** |
| ORIG trough / at `d` | 85.45 / 101 | **85.45 / 101** |
| ORIG `>=100`, median, max | 397/400, 1333.9, 2478.3 | 39**8**/400, **1333.9**, **2478.3** |
| ORIG `b14` engages | `d` = 15 | **`d` = 15** |
| knob-ON peak / at `d` | 1835.50 / 95 | **1835.50 / 95** |
| knob-ON trough / at `d` | 83.43 / 106 | **83.43 / 106** |
| knob-ON `>=100`, median, max | 243/400 (60.8 %), 132.8, 737.9 | **243/400 (60.8 %)**, **132.8**, **737.9** |
| knob-ON best lag | L = 0 | **L = 0** |
| knob-OFF best lag | L = 15 | **L = 15** |

`a8_medframe.py` reproduces attempt 14's PORT rows to every printed digit (slip 1500-2000
n=20 median speed 1683.53 median frame 71; driving-median n=54 median 1355.66 median frame
54).

**Two stated differences, neither of which moves a verdict, and neither of which is
adjusted after the fact:**
1. The `>= 100` count on the ORIGINAL is **398** where attempt 14 printed 397 — a
   one-frame window-boundary convention. Both are 99.x % against a 50 % threshold.
2. The lag error norm reads **1.33 %** (knob-OFF) and **0.19 %** (knob-ON) where attempt 14
   printed 1.44 % and 0.15 %. The **argmin is identical on both arms**, and the argmin is
   what gate (a) tests. The error percentage is reported but is **not** a gate here.
3. On the knob-OFF arm only, `a8_launch.py`'s trough lands at `d` = 88 where attempt 14's
   landed at `d` = 151. That arm's verdict is FAIL either way (8.0 % / median 32.3 under
   this definition, 0.0 % / 31.5 under attempt 14's). The definition is fixed as written
   above — **first local minimum after the launch crest** — and applied identically to
   every arm in this step.

---

## 2 The gates

### W — WIRING (a VOID condition, not a verdict)

Run 1 must show, in `motion_diag.log`, the direct witness line
`launchrev release car=0 bf4 <c> -> <c'>  bf8 <s> -> <s'>` with **`bf8` ending at 2**.

- If the line is absent, or `bf8` does not end at 2, the run is **VOID**: the wiring is
  broken, no verdict is read from it, the wiring is fixed and STEP 3 restarts from run 1.
  A VOID is reported, not hidden.
- `c` (the charge at release) is **recorded, not gated**: the original's measured value in
  this arm is 3000, but the standalone's countdown length is its own
  (`TrackRenderer.cpp` `countdown_ = 3.0f`, ~180 frames at 60 fps, against the original's
  112 ticks), so any `c > 1000` selects the same arm. Whatever `c` is, it is reported.

### D-0 — the control arm is still the control arm

One extra run with `MASHED_NO_LAUNCH_REV=1`. Its `motion_diag.log` must be **identical to
attempt 14's `s1`** on every shared line (attempt 14's own D-0 showed `s1` and the knob-OFF
build agree on all 1629 lines). If it is not, the port changed something outside the launch
chain and that must be reported before any scored number is read.

### a — LAUNCH

Both of:
- **a1** best-fit integer lag over `d` = 16..95, n = 80: **L = 0**.
- **a2** the port's `+0xb14` engagement frame: **`d` = 15**, equal to the original's.

PASS iff a1 ∧ a2. Any other result is a FAIL and is reported as measured.

### b — RECOVERY

The 400 frames strictly after the trough, scored exactly as attempt 14 registered H1:

- **H1** iff `>= 50 %` of the 400 frames are at speed `>= 100` **AND** the window median is
  `>= 900`.
- **H2** iff `< 10 %` of the 400 frames are at speed `>= 100`.
- **Otherwise INCONCLUSIVE.** No third branch will be added after the fact.

Reference values on the same reducer: ORIGINAL 99.5 %, median 1333.9.

### c — THE THREE SCORED METRICS

**3 runs**, all three reduced, against the **UNCHANGED** `d81a8df6` bounds:

| metric | bound |
|---|---|
| slip 1500-2000 | **0.18855 .. 0.19635** |
| slip 2000-2600 | **0.24488 .. 0.25487** |
| driving-median | **1904.70 .. 1982.44** |

Every reported value carries **n, median speed and median frame index** (and `d`).
`participants=1` must be confirmed from the run's own log on every run; a run that reports
anything else is VOID.

A metric with **n = 0** is **UNSCORABLE**, which is a FAIL, not a pass and not an omission.

**§26.10's median-frame guard still applies to the reading, not to the pass rule:** the
ORIGINAL populates these metrics at median `d` ≈ 719 / 908 / 797. If the port's median `d`
is an order of magnitude smaller, the two sides are being compared at different moments and
the comparison is reported as such — but the bound is still scored, because the bounds are
what D2 closes against.

### DEFINITION OF DONE for this step

**D2's launch is closed** iff gate **a PASSES** and the three metrics are in bounds on
3 of 3 runs. Gate b is diagnostic of what remains: an H1 result means the recovery followed
the launch; INCONCLUSIVE or H2 means a second defect survives and D2 does **not** close.

---

## 3 If recovery is still short

Pre-committed, so the next move is not chosen after seeing the number:

1. Take **`+0xb0c`** first — step 2A's exploratory row (`d` = 1 divergence, ORIG `~1e-5`
   vs PORT 0.59 through the launch).
2. Name the first diverging term after the trough **frame-aligned, NOT speed-banded**
   (memory `band-on-speed-compares-different-moments`; attempt 14's three instruments all
   returned "no overlap" on banded comparisons, and the knob-ON arm is the first with
   shared state).
3. **Test it on the running original before any fix**, and fix it only if confirmed, with
   the same promotion leg (RVA body + path1 + path2).
4. No speculative fixes. No new unfaithful knobs. Any diagnostic stays default-OFF, is
   labelled, and is removed before the end unless the user decides otherwise.

---

## 4 Collateral

The attempt ends with `re/tools/statediff/collateral.py`:
- `--mode paired` same-side, attempt 14's `s1` (pre-fix default) against this attempt's
  run 1 (post-fix default), with a floor from a same-arm repeat pair (`s2`, and run 2 here).
- a cross-side `--mode banded` pass with the median frame per band printed, reading **no**
  band whose two medians differ by more than 50 % (the tool flags those `!!`).
- every divergent field classified with `--scope re/tools/statediff/scope_a6a.txt`; the
  **outside-scope rows are the reportable output** and are exploratory only — they cannot
  change any verdict above.
