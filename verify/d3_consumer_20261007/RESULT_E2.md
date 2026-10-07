# RESULT — leg E2: swap the rule engine onto the monotone race-position metric

Date 2026-10-07. Pre-registration: `PREREG_CONSUMER.md` §4, as amended by `AMEND_E2.md` (A1–A4
registered at `ae906890`, A7 at `ee98e0ab`, both before the run they govern). **RAN**, twice — run 1
failed `E2-WROTE` on a defective witness, A7 fixed the witness, run 2 is scored below. No C-level.
Nothing default-ON. `original/` untouched.

## 0. Verdict first

**The swap reaches the rule engine, reorders the cars on 29.50% of frames, and changes nothing
else — not one other value in the entire capture.** The monotone metric is correct, consumed, and
**inert at the outcome level**.

| gate | verdict | figure |
|---|---|---|
| `G-TOOK` | **PASS** | `step_1008 == 50` on every row of all six runs |
| `E2-WROTE` | **PASS** | 11671/11671 in-scope rows, both arms, 100.0000% |
| `E2-DIFF` (control that can fail) | **PASS** | ordering differs on 318/1078 3-car frames = **29.4991%** vs a 1% bar |
| `E2-EFFECT` | **INERT** | arms produced identical `RULE-EVAL`; see §3 for the stronger form |
| `E2-KNOBOFF` | **PASS** | cross-build identical over all 77 pre-existing columns, 19,418 keys |
| `E2-DET` | **PASS** | 3 repeats per arm, identical over the whole overlap |
| `E2-NOREG-E` | **PASS** | (e) 3/3 cars |
| `E2-NOREG-B` | **PASS (unchanged)** | 5 failing bands on `v2`, identical to baseline — see §5 |
| `E2-LAPAGREE` (A4, can fail) | **NON-ZERO** | `lap != arclaps` on 214/19418 rows = **1.1021%** |

Raw output: `E2_GATES.txt`, `E2_CELL.txt`, `E2_ARMS.txt`, `E2{off,on}_{1,2,3}.{csv,log}`. Run 1 is
preserved as `E2_GATES_r1.txt` and `*_r1.csv/.log`.

## 1. Run 1 failed `E2-WROTE`, and the cause was my witness

Run 1 scored 60.1040% on **all six** captures — an identical figure on both arms, which is the
signature of a scorer defect rather than an implementation one. Measured: all 7,747 non-matching
rows in `E2off_1_r1` are car `v=2`, frames 5751..13497, every one carrying the same `rmetric` value
`1.49606395`. The slot was **frozen**, not wrong: cars 1 and 3 stop being dumped at frames 4590 and
5568, the rule engine stops writing `rule_metric_` after 5750, and car 2 keeps being dumped to
13497. On frames 0..5750 run 1 was already 11671/11671.

Restricting the denominator to "frames ≤ 5750" would have been cutting it after seeing the result.
`AMEND_E2.md` A7 instead added `rtick` — a counter bumped once per rule-engine write — so a row is
in scope exactly when its `rtick` rose since that car's previous dumped row. No frame threshold
appears anywhere in the scorer. Run 2 then passes at 100% with `max|err|` of `8.1e-08` (OFF) and
`1.1e-07` (ON), which is float32 round-trip through `%.9g`.

The in-scope fraction, now measured rather than assumed, is **11671/19418 = 60.10%** — the other
39.90% of dumped rows are frames where the engine was not running at all.

## 2. `E2-DIFF` — the swap has real content

```
E2off_1 vs E2on_1: ordering of cars 1..3 differs on 318/1078 3-car frames (29.4991%)
```

Denominator is frames where all three AI cars are dumped, as registered. All 1,078 such frames
measure in 0..4590, inside the live region, so this is scored entirely on rows where `rmetric` was
being written. Consistent with E1's independent 28.6–38.6% across five runs.

So the metric handed to `UpdateFinishOrder` genuinely differs between arms. The swap is not a no-op
at its input.

## 3. `E2-EFFECT` — INERT, and the evidence is stronger than the gate asked for

Both arms produce byte-identical `RULE-EVAL` output: 5 rounds, `scores=4,7,5,8 / 2,6,6,10 /
1,4,8,11 / 0,2,10,11 / 0,0,12,11`.

The registered gate stops there. A cell-for-cell diff of the two arms says considerably more
(`E2_ARMS.txt`):

```
A = E2off_1.csv   B = E2on_1.csv
shared (frame,seq,v) keys: 19418   max frame: 13497
columns differing anywhere (top 10 by row count):
    rmetric       19418 rows
```

**`rmetric` is the only column that differs, and no other column differs on any row.** Across all
19,418 dumped rows and 79 columns, every car position, control byte, lap counter, contact field and
arc quantity is identical between arms. The swap changes the metric the rule engine consumes and
propagates to **nothing**.

That localises the inertness precisely: `UpdateFinishOrder`'s output is not read by anything that
affects state under `rule=4` on this scenario. The blocker is **not** the progress substrate —
that was solved by the bridge (`275d9b26`) and leg A — and it is not reachability, which E1
established. It is that this consumer's output has no onward effect here.

**[UNCERTAIN]** whether the same holds for other rules. Only `rule=4` was run. `rule=0` and
`rule=10` have different segment/elimination logic and are untested by this leg.

## 4. `E2-LAPAGREE` — the registered formula is internally inconsistent

`lap != arclaps` on **214/19418 rows (1.1021%)**, identically in both arms (the two counters are
dumped regardless of the knob).

`AMEND_E2.md` A4 predicted this: `PREREG_CONSUMER.md` §4's formula pairs `race_[i].laps` with
`arcpct`, which derives from the separate forward-only `race_[i].arclaps`
(`TrackRenderer.cpp:5002`, `5011`). On those 214 rows the ON-arm metric mixes a lap count from one
pointer with a lap fraction from another. The formula was implemented exactly as registered rather
than quietly corrected, and the defect is now measured instead of argued.

This does not change §3's verdict — the arms are identical in every column but `rmetric`, so the
inconsistency has no observable consequence here — but **any future attempt to ship the ON arm must
resolve it first**.

## 5. No-regression gates, and why they were guaranteed

`E2-KNOBOFF` compared the new build's OFF arm against `F2a.csv`, captured from the **previous**
build under identical knobs and scenario, on the 77 columns both share:

```
--common-cols: comparing 77 shared columns; dropped 3: rmetric, arclaps, rtick
R-PREFIX = 13498 (IDENTICAL over the whole overlap)
```

Adding the knob perturbed the default path by exactly nothing. Every scorer computed from those 77
columns is therefore unchanged by construction, which is what `E2-NOREG-E`/`-B` assert:

- `E2-NOREG-E`: (e) **PASS 3/3** — launch 1426.4 / 2053.0 / 2055.2, matching the committed figures.
- `E2-NOREG-B`: (b) PASS on `v1` and `v3`; `v2` fails 5 bands (`c0_distinct=39`, `c1_distinct=75`,
  `steer_distinct=113`, `c1_median=5.5`, `abs_steer_median=44.5`). **This is the pre-existing
  baseline state, not a regression** — `E2-KNOBOFF` proves the inputs are byte-identical to the
  pre-change build. Reported because the gate asks for it, not as a finding of this leg.

`E2-DET`: all three repeats identical within each arm, over the whole overlap.

## 6. What this means for D-11072

The lane's working hypothesis was that the non-monotone `race_[].progress` was starving a consumer
that matters. E1 found the consumer and proved it reachable; leg A built a monotone replacement;
the bridge proved the state is published and read. E2 now closes the loop and the answer is
negative: **feeding the rule engine the monotone metric changes the car ordering it computes and
has zero effect on anything else.**

Consequences:
- The race-position **state** problem is finished. Nothing further is owed on the substrate.
- D3's AI criterion (b) blocker (`D3-R1`) does **not** recede into this consumer. The three
  unported `FUN_00416250` branches (U-9186) remain the named carrier, and E2 removes the rule
  engine as an alternative route to them.
- Shipping the ON arm default is out of scope for this leg by `PREREG_CONSUMER.md` §4, and §4's own
  rule now bites for an additional reason: an inert change with a known formula inconsistency (§4
  above) has no case for shipping.

## 7. Scope limits

Standalone side only. Scripted-capture regime: `MASHED_DETERMINISTIC` suppresses live input and
stays OFF by default. One track (Training), one rule (4), one car selection. Original-side
behaviour was not measured by this leg and nothing here is a C-level claim.
