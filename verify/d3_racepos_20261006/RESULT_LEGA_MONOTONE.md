# RESULT — D-11072 leg A follow-up: a MONOTONE arc-length port progress metric

Follows the leg-A finding in [`RESULT_LEGC_LEGA.md`](RESULT_LEGC_LEGA.md) §2 (the port's
`race_[].progress` jitters backward mid-lap, 484/669/1273 reversals per car, so it is not
range-mappable into the original's spline `race_pct`). This builds the prerequisite the finding
named: a monotone, arc-length-proportional port progress metric.

**No C-level moved.** **`original/` untouched** (anchor `BDCAE093…` re-verified). The new metric
is a **parallel, inert field** — `race_[].arcprog` is written in `UpdateRace` and read only by
`MASHED_AI_STEPDUMP` and (later) the race-position bridge; it does **not** feed finish
order/elimination (`progress` still does). Confirmed: no reader of `arcprog`/`arcseg`/`arclaps`
exists outside the dump. Final build SHA-256
`B8F1A7815B2FF2A50DE3A5DFDC26C8ED489983C8D9A347EB875CF1BE65B86210`.

## 1. The metric

Per car, each frame, in `TrackRenderer::UpdateRace`'s `step()` lambda
(`D3d9Render/TrackRenderer.cpp`):

- A forward-only segment pointer `arcseg` (+ `arclaps`) tracks which gate-to-gate segment the
  car is on. It advances the instant the car crosses a gate's **perpendicular** (projection
  parameter `t >= 1` on segment `[arcseg, arcseg+1]`), independent of `r.gate` (which lags, being
  driven by nearest-gate-**center**).
- Within the segment, `t` is the clamped along-track projection of the car's XZ onto the segment
  — immune to lateral weave, which is what made the radial `frac` run backward.
- Position is weighted by **physical segment length** from a cumulative table built once per track
  from the gate ring (`gate_cumlen_`, `gate_seglen_`, `total_len_`):
  `arcprog = arclaps*total_len_ + cumlen[arcseg] + t*seglen[arcseg]`, and the dumped per-lap view
  `arcpct = fmod(arcprog, total_len_)/total_len_*100`. Physical weighting (not a flat 1.0 per
  gate) is what removes the step-size change between long and short segments.

Uses only gate centers; no AI-spline load required.

## 2. Acceptance — the blocker is cleared

The leg-A finding set the bar: a faithful stand-in must be **monotone within a lap** like the
original's `race_pct` (0 backward steps). Measured with `re/tools/racepct_scale.py lega --portcol`
(reset/wrap-aware: a negative step landing below 2.0, or exceeding 50, is a lap-wrap or a round
reset, not a defect — the original capture is a single window that never spans one).

| metric | car1 backward-midlap | car2 | car3 |
|---|---:|---:|---:|
| original `race_pct` (reference) | 0 | 0 | 0 |
| port `progress` / `racepct` (old) | **484** | **669** | **1273** |
| port **`arcpct`** (new) | **0** | **0** | **0** |

**0 backward-midlap on all three cars, deterministic across 3 repeats (`ARC3/4/5`)** — the
per-car forward-step counts are stable (car1 1210/1210/1210). The metric is monotone.

**Behaviourally inert, as designed:** criterion (e) is **bit-identical** to the baseline
(`launch` 1426.4, `ft_median_m0` 2550.6 on car 1, all six digits) on `ARC3/4/5`; criterion (b)
is the same **5 bands** (car 2 only). The metric computes a parallel field and changes nothing
the game reads.

## 3. The one residual, characterized and declared

At each gate crossing `arcpct` makes a small **forward** jump (median per-frame step 0.13–0.15,
p99 0.33–0.59, **max 1.9**), because a piecewise-linear gate polyline has corners: at a turn the
projection parameter is discontinuous. The original avoids this only because its Catmull-Rom path
is C1-smooth. The jump is **forward**, so it does not break monotonicity; it is a fidelity ripple
on a metric used for ordering and threshold comparisons, not a correctness defect. Removing it
would require projecting onto the AI spline (`SplineLookahead` Phases 1–4 + a cumulative chord
table) instead of the gate polyline — more code, and gated on the spline being loaded
(`kSplineRaceCnt > 3`). **Not done here**; escalate only if a later leg needs smooth magnitude
fidelity rather than correct ordering.

## 4. Leg A's deliverable — the candidate scale map (REGISTERED, UNRUN)

With a monotone metric, leg A's deliverable is now a registrable map. Both `arcpct` and the
original `race_pct` are **physical-arc-length-as-a-percentage-of-lap, 0..100 per lap** — the same
semantics, computed the same way (arc travelled / total lap length × 100), differing only by the
gate-polyline-vs-spline arc-length approximation (the §3 ripple). So the candidate bridge write
is a **near-identity**, registered here **UNRUN**:

> `*(float*)(0x008a96ec + v*0x30c) = arcpct(v)` for the AI cars, behind a default-OFF knob, with
> the scope-doc §3/§5 inert-first gate: a default-OFF probe confirms `FUN_00408ad0(v)` /
> `FUN_00408a50(v)` return non-zero monotone-in-position values and that the bridged `race_pct`
> **ordering** of the 4 cars matches the original's on a matched capture, before any consumer is
> trusted; then the car<->car no-regression set (G-NOREG-E/-B, G-DET, G-KNOBOFF) plus the modes
> oracle (rule 3 finish order) and the power-up sweep, because leg A's write reaches the
> elimination tiebreak and the fire gates.

**Not written this session.** `0x008a96ec` is not touched. The map above is the next step's
pre-registration seed, not a landed change.

## 5. What this establishes

- The port now has a **monotone, physical-arc-length race-progress metric** (`race_[].arcprog` /
  `arcpct`), 0 backward-midlap on all cars, deterministic, inert on (e)/(b). **The D-11072
  monotonicity blocker is cleared.**
- Leg A's scale map is now registrable (near-identity to the original's `race_pct`), recorded
  UNRUN in §4 with its inert-first gate.
- Remaining before the bridge ships: run the §4 pre-registration (write + liveness probe +
  no-regression/oracle/powerup gates). The §3 corner-ripple is a known, bounded fidelity residual,
  not a blocker for ordering-based consumers.
