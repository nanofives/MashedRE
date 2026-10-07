# RESULT — leg F1: localize the long-run nondeterminism

Date 2026-10-07. Pre-registration: `verify/d3_determinism_20261007/PREREG_F1.md` (committed UNRUN
at `1f75fe57`). **RAN.** Diagnosis only. No source shipped, no C-level moved, `original/` untouched,
nothing default-ON.

## 0. Verdict first

**`F1-CTRL` FAILED.** The registered control — `R-PREFIX` on the pair that is *supposed* to differ
(`L4` vs `L10`, different `MASHED_ROUND_RULE`) — came out **at or above** the identical-knob pairs:

| pair | knobs | registered `R-PREFIX` | shared-key-only `R-PREFIX` (post-hoc) |
|---|---|---|---|
| `L4` vs `L4b` | identical | 1306 | 2306 |
| `L4` vs `L4c` | identical | 1306 | 2306 |
| `L4b` vs `L4c` | identical | **1341** | **1341** |
| `L4` vs `L10` | **differ** (rule 4 vs 10) | 1306 | 2306 |

PREREG §2 states the failure condition exactly: *"If `R-PREFIX` for the identical-knob pairs is not
materially larger than for this pair, the metric is not measuring what it claims and F1 reports
that instead of a localization."* It is not larger — it is equal or smaller. Per PREREG §3,
**`R-PREFIX` is reported as uninformative and F2 is NOT designed on it.**

`R-ROUND` (the property F2 must actually hit) was measured independently and is **not** affected by
this failure. See §3.

Raw output: `F1_PREFIX_*.txt`, `F1_PREFIX_sharedonly_*.txt`, `F1_ROUND.txt` in this directory.

## 1. Why the control failed — the control pair was invalid by construction

Measured directly: over the 7,411 `(frame,seq,v)` keys `L4c` and `L10` share, **38 rows differ, the
first at frame 3550.** Two runs with *different* `MASHED_ROUND_RULE` are therefore closer to each
other than two runs with *identical* knobs (`L4b` vs `L4c`: first difference at frame 1341, 4,488
differing rows).

That is consistent with what `R-ROUND` shows (§3): rule 4 vs rule 10 changes only *when a round is
scored* (`timer=30.00` vs `11.23/13.27/11.80/21.62/5.32`), not how the cars move. The knob cannot
produce an early driving-state difference, so `L4`-vs-`L10` was never a "should differ early" pair
for this metric. The control did its job: it caught a metric/control mismatch before F2 was built
on it.

**[UNCERTAIN]** Whether a valid early-divergence control exists for `R-PREFIX` at all. None of the
five committed captures varies a knob that is known to perturb driving state on frame 1. Resolving
this needs a new capture (a deliberately perturbed run), which F1 is not scoped to take.

## 2. Second defect found in the metric: `R-PREFIX` on `L4` pairs is a sampling artefact

The registered definition counts a key present in only one run as a disagreement. For all three
`L4` pairs the first disagreement at frame 1306 is exactly that — `<row missing>`, not a value
difference. Coverage then collapses:

| pair | shared keys, frame < 1341 | shared keys, frame < 2306 | total shared |
|---|---|---|---|
| `L4` vs `L4b` | 2,713 | **2,713** | 4,600 |
| `L4` vs `L4c` | 2,713 | **2,713** | 4,597 |
| `L4b` vs `L4c` | 2,816 | 4,881 | 7,474 |

**`L4` shares zero rows with `L4b`/`L4c` anywhere between frame 1306 and 2306.** The post-hoc
shared-key-only figure of 2306 for those pairs is therefore not a measurement of agreement — it is
the next frame at which a comparable row exists. Only the `L4b`/`L4c` number (1341) rests on
continuous coverage.

Per-run dump membership itself is not stable (`AiStepDump` rows per car `v`):

| run | frames | rows | `v=1` | `v=2` | `v=3` |
|---|---|---|---|---|---|
| `L4` | 13,097 | 21,074 | 2,451 | 11,890 | 6,733 |
| `L4b` | 13,793 | 17,969 | 1,446 | 2,730 | 13,793 |
| `L4c` | 13,791 | 18,670 | 1,446 | 3,433 | 13,791 |
| `L10` | 13,409 | 19,617 | 1,850 | 4,720 | 13,047 |

`L4` dumps car 2 4.4x more often than `L4b` and car 3 half as often. Which car is dumped on a given
frame is itself a run-to-run variable, so any future metric over these captures must state its
coverage.

## 3. `F1-ROUND` — `R-ROUND` measured (this result stands)

Verbatim `RULE-EVAL` lines, `F1_ROUND.txt`:

```
L4   r1 scores=4,7,5,8   r2 =2,6,6,10   r3 =1,4,8,11   r4 =0,3,10,11   r5 =0,2,12,11
L4b  r1 scores=4,7,5,8   r2 =3,5,6,10   r3 =2,3,7,12
L4c  r1 scores=4,7,5,8   r2 =2,6,6,10   r3 =1,4,7,12
L10  r1 scores=4,7,5,8   r2 =2,6,6,10   r3 =1,4,8,11   r4 =0,2,10,11   r5 =0,0,11,13
L0   r1 scores=4,7,5,8   r2 =2,6,6,10   r3 =1,4,8,11   r4 =0,3,10,11   r5 =0,2,11,13
```

`L0` (`rule=0`) is included although PREREG §2 named only `L4`/`L4b`/`L4c`/`L10`; it is in the same
committed capture set and it sharpens §1.

- **Round 1 is identical across all four runs**, including the different-knob run: `4,7,5,8`,
  `participants=4`, `r=0`.
- **`R-ROUND` first diverges at round 2**, and only between identical-knob runs (`L4b` gives
  `3,5,6,10` where `L4`/`L4c` give `2,6,6,10`).
- Round count differs: 5 / 3 / 3 / 5 / 5. All `rule=4` and `rule=0` runs carry `timer=30.00` on
  every round, so the round *length* is fixed and the differing round count comes from the match
  ending early.
- **`L0` (rule 0) matches `L4` (rule 4) on rounds 1-4 exactly** and first differs at round 5
  (`0,2,11,13` vs `0,2,12,11`). A third distinct value of the knob therefore also fails to perturb
  the early match, which is the §1 finding restated at round granularity: the rule knob is close to
  inert on driving state, so no pairing against it is a usable early-divergence control.

This reproduces E1 §5's blocker and keeps F2's registered target intact: **`R-ROUND` identical
across 3 repeats of a 240 s run.**

## 4. `F1-RATE` — the frame rate itself is not reproducible

| run | frames over 240 s | mean fps |
|---|---|---|
| `L4` | 13,097 | 54.6 |
| `L4b` | 13,793 | 57.5 |
| `L4c` | 13,791 | 57.5 |
| `L10` | 13,409 | 55.9 |

Spread 13,097..13,793 = **5.3%** across identical-knob runs. This confirms the figure PREREG §2
recorded as already known and is reported as a measurement, not as a cause.

## 5. `F1-FIRSTCOL` — reported, but NOT promoted to "the carrier"

For the one pair with continuous coverage (`L4b` vs `L4c`), the first differing row is a **single
column**:

```
frame=1341 seq=2818 v=3
    rec_148   A=0.406007588   B=0.00600760477
```

`rec_148` is `+0x148` on the vehicle physics record, typed f32 and glossed **angular velocity y** in
`verify/d3_arm_20261005/PREREG_ARM.md:74`. The two values differ by **exactly 0.4** to 7 digits
(`0.406007588 - 0.400000000 = 0.006007588` vs `0.00600760477`), i.e. a discrete offset, not
accumulated float drift. `rec_148` also leads that pair's whole-run differing-row count (4,488 rows,
ahead of `h4x`/`h4z`/`h5x`… at 4,438).

PREREG §3 would read a single-column `F1-FIRSTCOL` as naming the carrier. **That inference is
withheld here** because the metric producing it is the one `F1-CTRL` just invalidated, and because
the result rests on one of three pairs — the other two have no comparable row at that frame (§2).
It is recorded as a lead for a correctly-controlled follow-up, not as a localization.

## 6. What F1 delivers, and what it does not

Delivered:
- `R-ROUND` is measured and well-behaved as a gate: round 1 reproduces across all four runs, round 2
  does not. F2's registered target is unchanged and still usable.
- Two independent defects in `R-PREFIX` as specified: an invalid control pair (§1) and
  coverage-dominated results on two of three pairs (§2).
- `F1-RATE`: 5.3% frame-count spread across identical-knob runs.

Not delivered (and not claimable):
- Any localization of the divergence. PREREG §3's decision rules all branch on `R-PREFIX`, and
  `R-PREFIX` failed its control.
- Any statement that the frame timebase, a PRNG, or the round machinery is the cause.

## 7. What F2 needs before it can be registered

1. A localization metric with a control that **can** fail. Candidate: re-run the captures with a
   deliberately perturbed knob that is known to change driving state on frame 1, so an early-
   divergence control exists.
2. A dump whose per-frame membership is fixed, so coverage stops confounding the comparison (§2).
3. The static source survey of the timestep plumbing, determinism knobs, PRNG seeding and
   wall-clock reads — running on the worker account at the time of writing. Per PREREG §2 that
   survey is **evidence for F2's design, not a substitute for F1's measurement**, and it does not
   repair the control failure.

**Leg E2 remains blocked.** Its outcome gate needs `R-ROUND` reproducible, and F1 did not make it so.
