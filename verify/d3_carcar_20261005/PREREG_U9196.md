# PRE-REGISTRATION — U-9196: is the car<->car (b) movement genuine or a shallow median coincidence?

**COMMITTED UNRUN.** The port ON-arm distribution has **not** been inspected; only the
original (the reference that defines criterion (b)) has. Tool `re/tools/ai_steerdist.py`,
committed in the same commit. Pure offline analysis of committed `.aistep` CSVs — no game
run, no build, no Ghidra, `original/` untouched.

## The question, stated precisely

Leg 3 (`RESULT_LEG3.md`) wired `0x00469df0` and criterion (b) went **13 -> 5** failing bands,
with cars 1 and 3's `abs_steer_median` entering the original's `[0,23]` envelope (52.5 -> 7.0
and 49.0 -> 11.5). The (b) gate checks the **median** and a **distinct-count**; it does not
check that the port's per-frame steering *distribution* matches the original's. U-9196 asks
whether the pass is a genuine per-car behavioural match or a shallow coincidence of the
median statistic.

## The reference (already measured — it is the ground truth, legitimate to inspect)

The original (`o_t1`/`o_t2`/`o_t3.msd.aistep.csv`, `--track 0` = Training, AI slot 1..3,
**deterministic** — identical to the integer across all three captures) steers **bimodally**:
a low median with a fat tail of full-rail corrections. `abs(c1 - c0)` over the 220-call
window:

| car | median | p75 | p90 | max | >128 | distinct |
|---|---:|---:|---:|---:|---:|---:|
| 1 | 7 | 10 | 255 | 255 | 28/220 = 0.127 | 33 |
| 3 | 23 | 80 | 182 | 255 | 33/220 = 0.150 | 96 |

So a genuine match reproduces that *shape* — low median **and** a correction tail. A shallow
pass has the low median but **no** tail (p90 low, rail-fraction ~0): smooth-but-not-cornering.

## The gates, fixed here before the port is measured

Per car (1 and 3 — the two that newly entered the envelope), `abs(c1 - c0)` over the window:

| gate | rule | justification |
|---|---|---|
| `G-SHAPE-MED` | `|port.median - orig.median| <= 8` | the (b) envelope `[0,23]` is 23 wide; 8 is ~a third of it — a loose "same neighbourhood" test, deliberately not tight |
| `G-SHAPE-P90` | `port.p90 >= 90` | the original's p90 is 182 / 255; a real driver makes occasional rail corrections, so a port with p90 < 90 has **no** tail |
| `G-SHAPE-TAIL` | `port.rail_frac ∈ [orig.rail_frac / 3, orig.rail_frac * 3]` | does the port correct at a comparable rate — the fat tail exists and is neither absent nor wildly inflated |

**Per-car verdict:** `GENUINE` iff all three pass; `SHALLOW` otherwise.

**Overall verdict (the U-9196 adjudication):**
- **CONFIRMED** — both cars `GENUINE`. The (b) movement is a real per-car steering match; a (b)
  improvement from the car<->car physics is supported, and the keep/revert decision on
  `MASHED_CARCAR_CONTACT` default-ON goes to the user as a real improvement.
- **KILLED** — both cars `SHALLOW`. The (b) pass is a median coincidence; the knob should not
  be read as a (b) win, and `RESULT_LEG3.md`'s observation is downgraded accordingly.
- **PARTIAL** — one of each. Reported car-by-car; no blanket (b) claim either way.

## What this does NOT do, registered so it is not claimed later

- It does **not** move any C-level. `0x00469df0` stays C2 regardless of the verdict.
- A `CONFIRMED` verdict is **not** a (b) closure. It says two cars' steering *distributions*
  match the original on three shape statistics on one Training recipe; (b) is a 30-band
  criterion across more tracks and the full field, car 2 still fails, and U-9195's
  participant-count caveat is untouched. It makes the leg-3 observation defensible, not final.
- The original is deterministic (0 run-to-run spread), so there is no spread-based tolerance;
  the thresholds above are fixed absolute/relative values justified against the reference, and
  the raw port-vs-orig table is printed in the RESULT whatever the verdict, so a reader can
  see the shapes rather than trust the pass/fail.

## Inputs

`--orig verify/d3_elim_20261003/o_t1.msd.aistep.csv`
`--port verify/d3_carcar_20261005/L3on1.csv`
`--cars 1,3`

Both are Training, AI slots. `o_t1` is the deterministic original; `L3on1` is the leg-3 ON
arm (one of three identical runs). A secondary position-matched signed-steer comparison
(`ai_posmatch`-style, R = 0.12) may be reported as collateral but is **not** gated — the
two sides are on different trajectories, so position-matching steering is a weaker instrument
than the distribution comparison and is supporting evidence only.
