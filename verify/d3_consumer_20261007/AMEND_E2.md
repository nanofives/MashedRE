# AMENDMENT (UNRUN) — leg E2, `PREREG_CONSUMER.md` §4

Date 2026-10-07. Registered **before** `MASHED_RACEMETRIC_ARC` was implemented and before any E2
arm ran. Amends three things in `PREREG_CONSUMER.md` §4 and adds one gate. Everything not listed
here is unchanged and still binding.

## A1. `E2-KNOBOFF`'s reference baseline — CHANGED

**Was:** "knob OFF reproduces **the committed baseline stepdump** cell-for-cell; denominator =
shared `(frame,seq,v)` keys × pre-existing columns."

**Now:** knob OFF reproduces a **freshly-taken OFF-arm reference** cell-for-cell, captured under
`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` in the same session as the ON arm. Denominator
unchanged.

**Why.** The committed baseline is E1's capture, taken with the determinism knobs unset and stopped
by a wall-clock kill (`run_e1.ps1:17-23`). `RESULT_F2.md` §4 records that such captures are not
comparable to deterministic ones: E1's three identical-knob runs disagreed with *each other* on
4,488 rows, so "reproduces the committed baseline cell-for-cell" is unsatisfiable by any build,
correct or not. A gate no implementation can pass measures nothing.

This is a change of **reference**, not of threshold. The bar stays cell-for-cell identity at 100%.
It is also a *tightening* in practice: the deterministic OFF arm must now match to the byte, where
the old reference could not be matched at all.

**Secondary effect, stated so it is not a surprise.** A1 also forces a schema break — see A2.

## A2. Stepdump gains one appended column `rmetric` — NEW

`E2-WROTE` scores "logged `rc.metric[i]`", but no stepdump column carries it today. One column is
**appended** (never inserted), so every existing column keeps its index and `ai_posmatch.py`,
`ai_headattrib.py`, `ai_yawrate.py`, `ai_armregime.py`, `ai_speed_env.py` and `ai_ctrl_window.py`
are unaffected:

| column | source |
|---|---|
| `rmetric` | `rc.metric[v]` as handed to `RE::UpdateFinishOrder`, captured at `TrackRenderer.cpp:5134` |

Consequence: CSVs from this leg have 78 columns against F2's 77, so `det_prefix.py` reports
`COLUMN MISMATCH` between the two generations. That is correct behaviour and is the mechanical
reason A1 is required rather than merely preferable. **F2's captures are not E2's baseline.**

## A3. `E2-EFFECT` is measurable — CHANGED

**Was:** `re/NEXT_SESSION.md` (2026-10-07, leg E1 handoff) instructed that `E2-EFFECT` "must be
declared unmeasurable, not restated", because round-level outcomes did not reproduce.

**Now:** `E2-EFFECT` is **scored normally**. `RESULT_F2.md` §2 establishes `R-ROUND` reproducible
across three repeats, which is exactly the property that instruction was waiting on. Threshold
unchanged: `RULE-EVAL` finish order / `r` differs between arms, or is stated identical → INERT.

## A4. `E2-LAPAGREE` — NEW observation gate, and it can fail

`PREREG_CONSUMER.md` §4 specifies the swapped metric as:

```
rc.metric[i] = race_[i].laps + race_[i].arcpct * 0.01f
```

This mixes two lap counters. `race_[i].laps` is the gate-based counter; `arcpct` derives from
`race_[i].arcprog`, which is built from the **separate** forward-only `race_[i].arclaps`
(`TrackRenderer.cpp:5002`, `5011`). When the two disagree near the start/finish line the ON-arm
metric can step by a whole lap while the fraction wraps — a discontinuity the registered formula
does not acknowledge.

**The formula is implemented exactly as registered.** Changing it here would be substituting my
judgement for the pre-registration. Instead the hazard is measured:

| gate | threshold |
|---|---|
| `E2-LAPAGREE` | fraction of dumped ON-arm rows where `race_[i].laps != race_[i].arclaps`. Reported as a percentage with its denominator. **> 0% means the registered formula is internally inconsistent** and any `E2-EFFECT` result carries that caveat explicitly |

`E2-LAPAGREE` does not block the other gates. It is reported whatever it shows, including 0%.
`lap_9648` and the new `rmetric` column let it be scored offline from the ON-arm CSV; `arclaps` is
appended to the dump as part of A2's single `rmetric` change only if scoring needs it — if it does,
that is a second appended column and is noted in the RESULT.

## A5. Run matrix — fixed now

Six runs, all under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` with E1's scenario env
(`MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_ROUND_RULE=4`):

| run | arm |
|---|---|
| `E2off_1/2/3` | `MASHED_RACEMETRIC_ARC` unset |
| `E2on_1/2/3` | `MASHED_RACEMETRIC_ARC=1` |

`E2off_1` is the fresh reference A1 names. `E2-DET` is scored within each arm across its three
repeats; G-TOOK's corrected witness (`step_1008 == 50` on every row, `RESULT_F2.md` §1) is checked
on all six before any other gate is read.

## A7. `E2-WROTE`'s denominator — ADDED 2026-10-07 after the first run, BEFORE the re-run

**The first E2 run scored `E2-WROTE` FAIL at 60.1040% on all six captures** (`E2_GATES.txt`, run 1,
preserved). The diagnosis is mechanical and is recorded here before any re-scoring:

- All 7,747 non-matching rows in `E2off_1` are **car `v=2`, frames 5751..13497**, and every one
  carries the same `rmetric` value `1.49606395`. The slot is **frozen**, not wrong.
- Cars 1 and 3 stop being dumped at frames 4590 and 5568; `rule_metric_` stops being written after
  frame 5750, while car 2 keeps being dumped to 13497.
- On frames 0..5750 the gate is **11671/11671 = 100.0000%**.

So the gate was scored over rows where the rule engine was not running, and a slot nothing had
written since frame 5750 was read as a formula error. Same family as
`zero-of-n-needs-a-coverage-check`: the denominator was never established.

**Restricting the denominator to "frames ≤ 5750" would be cutting it after seeing the result.**
Instead the in-scope set is made *measurable*:

| change | detail |
|---|---|
| new appended column `rtick` | `rule_metric_tick_`, a monotone counter bumped once each time the rule-engine block writes `rule_metric_[]` (`TrackRenderer.cpp:5134` region). APPENDED after `arclaps`; all earlier columns keep their index |
| `E2-WROTE` denominator | rows whose `rtick` is **strictly greater** than the `rtick` of the same car's previous dumped row. A frozen slot keeps its `rtick` and is excluded by construction, with no frame threshold anywhere in the scorer |
| reported alongside | the in-scope count and its percentage of all dumped rows, so the exclusion is visible rather than silent |

Threshold is unchanged: **100%** of in-scope rows.

**What this does NOT change.** `E2-DIFF` was already computed only on frames where all three cars
are dumped; measured, all 1,078 such frames lie in 0..4590, entirely inside the live region, so its
29.4991% stands on live data and is not affected by A7. `E2-EFFECT` reads `RULE-EVAL` from the log
and never touches `rmetric`. `E2-LAPAGREE` compares two dumped lap counters and never touches
`rmetric`.

Run 1's captures and `E2_GATES.txt` are kept as `*_r1` so the FAIL and the re-run are both on the
record.

## A6. Unchanged and still binding

`E2-WROTE`, `E2-DIFF` (the control that can fail, ≥ 1% ordering change), `E2-NOREG-E`,
`E2-NOREG-B`, `E2-DET` thresholds; and §4's closing rule that **default-ON shipping is out of scope
for this leg regardless of outcome**.
