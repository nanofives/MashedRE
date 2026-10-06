# RESULT — U-9196: the car<->car (b) movement is GENUINE, not a shallow coincidence — verdict PARTIAL

Pre-registration: [`PREREG_U9196.md`](PREREG_U9196.md), committed **UNRUN** at `d3bb302d`.
Tool `re/tools/ai_steerdist.py`. No game run, no build, `original/` untouched — offline
analysis of committed `.aistep` CSVs (`verify/d3_elim_20261003/o_t1.msd.aistep.csv` and
`verify/d3_carcar_20261005/L3on1.csv`). **No C-level moved.**

## The answer

**The specific failure U-9196 feared — a low median achieved WITHOUT the original's
correction tail ("smooth-but-not-cornering") — is REFUTED on both cars.** Both car 1 and
car 3 reproduce the original's bimodal shape: a low median with a fat tail of full-rail
corrections. The (b) movement is a real behavioural effect of the contact, not a statistical
artifact of the median.

The registered verdict is **PARTIAL**, and the split is informative:

```
car1  (window n: orig 220 / port 220)
        median   p75   p90   max   distinct   rail>128
  orig       7    10   255   255        33   28/220=0.127
  port       7   127   158   213        83   52/220=0.236
  G-SHAPE-MED  |7-7|=0  <= 8                 -> PASS
  G-SHAPE-P90  port p90 158 >= 90            -> PASS
  G-SHAPE-TAIL port frac 0.236 in [0.042,0.382] -> PASS      -> car1 GENUINE

car3  (window n: orig 220 / port 220)
        median   p75   p90   max   distinct   rail>128
  orig      23    80   182   255        96   33/220=0.150
  port      11    90   255   255        77   44/220=0.200
  G-SHAPE-MED  |11-23|=11 <= 8              -> FAIL
  G-SHAPE-P90  port p90 255 >= 90            -> PASS
  G-SHAPE-TAIL port frac 0.200 in [0.050,0.450] -> PASS      -> car3 SHALLOW

OVERALL: PARTIAL
```

## Reading it honestly — the registered decision stands, its rationale is corrected

The pre-registered rule makes car 3 `SHALLOW` because `G-SHAPE-MED` failed (median 11 vs 23,
11 > 8). **I honour that decision** (memory `pre-register-the-decision-not-the-diagnosis`).
But the gate's name over-reads the cause: `SHALLOW` was defined to catch
"smooth-but-not-cornering", and car 3 is **not** that. Car 3 has the correction tail in full
(`p90 = 255`, `rail_frac = 0.200`, both passing) — it corners for real; it simply steers
**less at the median** than its own original car (11 vs 23). So car 3 is better described as
*a genuine cornering car that under-steers its reference by ~half the median*, not a shallow
median coincidence. The only car whose steering distribution is a true per-car match is
car 1, where the median is exact (7 = 7) and both tail gates pass.

Two caveats visible in the table, reported rather than buried:
- Car 1's `distinct` is 83 vs the original's 33, and its `p75` is 127 vs 10 — the port
  **jitters more** between corrections than the original. It passes every gate and matches
  the median and the tail, but it is noisier in the mid-range.
- Both the port and the original hit the `max` rail (255) and have comparable rail-fractions,
  so neither car is achieving its median by refusing to turn.

## Collateral — position-matched steering (ungated, as registered)

Pairing port-ON and original rows for the same car at matched `own_x`/`own_z` (R = 0.12) and
comparing `abs(c1 - c0)` at that track spot:

| car | matched pairs | median `|port - orig|` | p90 |
|---|---:|---:|---:|
| 1 | 127 | **7.0** | 121.0 |
| 3 | 94 | **10.0** | 255.0 |

At the same place on the track, car 1 steers within a median of 7 steering-units (on a
0..255 scale) of the original, and car 3 within 10. This is weak evidence (the two sides are
on different trajectories, so a 0.12-unit match is coarse), but it points the same way as the
distribution comparison: the port's steering tracks the original's reasonably closely,
closely on car 1, loosely on car 3.

## What this closes, and what it does not

**Closes U-9196.** The question was "genuine per-car match or shallow median coincidence?"
Answer: **genuine behaviour, not a coincidence** — both cars corner with the original's
bimodal low-median-plus-tail shape — with the nuance that it is an **exact** match on car 1
and a **real-but-under-steered** match on car 3. The 2026-10-02 matrix's "no arm passes (b)"
is not contradicted by a fluke: this is a genuine behavioural change from a physics lever that
matrix never had.

**Does not establish a (b) closure.** Registered in advance and held: car 2 still fails (b)
entirely; the match is on one Training recipe; U-9195's participant-count caveat (the ring
publication leg 3 relies on is only proven inert at `g_playerCount` ∈ {4,8,9}) is untouched;
and `0x00469df0` stays **C2** — a call site plus a distribution match is not a behavioural
diff against the original. The keep-or-revert decision on `MASHED_CARCAR_CONTACT` default-ON
is the user's: it is a genuine step toward the original's AI steering on 2 of 3 cars (exact on
one), it regresses no gated statistic, and it costs car 2 one band.
