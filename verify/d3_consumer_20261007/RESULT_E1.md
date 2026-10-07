# RESULT — D-11072 leg E1: the race-position metric's LIVE consumer, located and executed

Pre-registration: [`PREREG_CONSUMER.md`](PREREG_CONSUMER.md), committed **UNRUN** at `bcdd3fcb`.

**Verdict: the consumer is located, reachable, and demonstrably executes — and it was never behind
`0x007f0fd0`.** Swapping it to leg A's monotone `arcpct` is **not** inert: car ordering changes on
28.6–38.6 % of frames. **One pre-registered gate failed as written (`E1-EVAL`) and one failed
outright (`E1-DET`)**; both are reported below against the registered text, with the rationale
corrected rather than the threshold restated.

**No C-level moved. No source changed — leg E1 is a zero-code-change measurement.** `original/`
untouched. Build SHA-256 `FD060C72272BB3A34F8E8D227073F71E560A9C614024B0F50D1CC5F6AE5262D4`
(identical to the committed bridge build).

## 1. The runs

Five captures, Training, 240 s, standard knob set (`MASHED_MUTE=1 MASHED_TRACK_VIEW=Training
MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=primary-bl MASHED_AI_STEPDUMP=…`), differing only in
`MASHED_ROUND_RULE`. Drivers `run_e1.ps1` / `run_e1b.ps1`.

| run | `MASHED_ROUND_RULE` | frames | rounds evaluated | note |
|---|---|---:|---:|---|
| `L4` | 4 | 13096 | 5 | |
| `L10` | 10 | 13408 | 5 | |
| `L0` | *(unset → rule 0)* | 13386 | 5 | control |
| `L4b` | 4 | 13792 | 3 | repeat; first attempt exited early at 8 s, re-run |
| `L4c` | 4 | 13790 | 3 | repeat |

## 2. The gates

| gate | threshold | measured | verdict |
|---|---|---|---|
| `E1-RULE` | `MATCH-SEED rule=` equals the requested rule in every run, 5 of 5 | `L4`/`L4b`/`L4c` **rule=4**, `L10` **rule=10**, `L0` **rule=0** — 5 of 5 | **PASS** |
| `E1-LAPS` | max `lap` over the 3 dumped AI cars ≥ 3 on `L4`, ≥ 2 on `L10` | `L4` **8** (car 2), `L10` **6** (car 3); also `L0` 5, `L4c` 4, `L4b` **1** — 4 of 5 runs cross 3.0 | **PASS**, with §3 caveat |
| `E1-EVAL` | `RULE-EVAL` present on `L4`/`L10` **and absent or `r=0`-only on `L0`** | present 5× on `L4`, 5× on `L10`; `L0` round 5 returns **`r=4`**, so the second clause is false | **FAIL as written** — see §4 |
| `E1-DET` | `E1-RULE` + `E1-LAPS` verdicts reproduce on a second `L4` run | `E1-RULE` reproduces; round-level outcomes do **not** — three rule-4 runs give three distinct round sequences (§5) | **FAIL** |

## 3. `E1-LAPS`: the threshold is crossed, but only by the terminal-round survivor

Per-car lap traces segmented at each lap-counter reset (= round restart) show every round except
the last topping out at lap 1–2. The ≥ 3 readings all come from the final round, which does not
end: on `L4` car 2 runs frames 5890–13096 up to lap 8; on `L10` car 3 runs 3764–13408 up to lap 6.
Rounds 1–4 end on elimination at roughly 45 s each, well before any car laps three times.

So the metric threshold is reachable, but in a degenerate way: by the time any car passes 3.0,
it is the only AI car left. Anything that depends on *which* car passes first is, in this
scenario, not a contest.

## 4. `E1-EVAL`: the control discriminated, in the opposite direction to the prediction

The gate was written expecting rule 0 to be the inert arm. It is not. `L0` (rule 0) **concludes**
the match at round 5 with `r=4`; `L4` (rule 4) returns `r=0` on all five rounds and then runs a
sixth round that never ends. The control therefore does discriminate rule-gated behaviour — the
registered direction was simply wrong. Recording this as a fail against the registered text; the
decision it fed (proceed to E2 on `E1-LAPS`) is unchanged.

`L10` additionally shows a rule-specific signal the other arms cannot produce: its `RULE-EVAL`
`timer=` varies per round (11.23 / 13.27 / 11.80 / 21.62 / 5.32) where rules 0 and 4 report a
constant `30.00`.

## 5. `E1-DET`: round-level outcomes do not reproduce — the new blocker

Three runs with **identical** knobs produce three different matches:

| run | round 1 scores | round 2 | round 3 | rounds in 240 s | max lap |
|---|---|---|---|---:|---:|
| `L4` | 4,7,5,8 | 2,6,6,10 | 1,4,8,11 | 5 | 8 |
| `L4b` | 4,7,5,8 | **3,5,6,10** | **2,3,7,12** | 3 | 1 |
| `L4c` | 4,7,5,8 | 2,6,6,10 | **1,4,7,12** | 3 | 4 |

This is **not** a claim that the simulation became nondeterministic. The 60 s captures were never
frame-identical either (`N1/N2/N3` = 2914/2980/2875 frames, `Y1/Y2/Y3` = 2984/2987/2989), and the
project's `G-DET` has always been *scorer-level* reproducibility — criterion (e)'s six digits and
(b)'s band count — not trace identity. What E1 measures is narrower and new: **round-level match
outcomes are wall-clock-sensitive and do not reproduce**, while aggregate statistics over the same
runs are stable (the §6 ordering figure is 37.75 / 38.64 / 38.31 % across the three rule-4 arms).

Consequence, stated before any E2 run: **an E2 gate of the form "the `RULE-EVAL` finish order or
`r` differs between the knob-ON and knob-OFF arms" is unmeasurable in this scenario** — the
between-repeat variation already exceeds any effect such a gate could attribute. E2's outcome gate
as registered in `PREREG_CONSUMER.md` §4 (`E2-EFFECT`) cannot be honoured here without first
making long-run outcomes reproducible.

## 6. Declared collateral: the metric swap is not inert at the ordering level

Computed offline from the captures above (no code change, no extra run): comparing the metric the
rule engine uses today, `lap + racepct/100` (`TrackRenderer.cpp:5132-5134`, non-monotone), against
leg A's `lap + arcpct/100`, over frames where all three AI cars are dumped:

| run | frames with 3 cars | ordering differs | % |
|---|---:|---:|---:|
| `L4` | 1608 | 607 | **37.75** |
| `L4b` | 603 | 233 | **38.64** |
| `L4c` | 603 | 231 | **38.31** |
| `L0` | 2293 | 812 | **35.41** |
| `L10` | 1007 | 288 | **28.60** |

Max per-car difference between the two metrics is **0.0499** (≈ 5 % of one lap), so these are
near-ties being reordered, not gross disagreement. Against the pre-registered `E2-DIFF` bar of
≥ 1 %, the swap is **not inert**.

## 7. What this establishes

- The port's live, rule-gated consumer of the race-position metric is `Race/RuleEngine`'s
  `UpdateFinishOrder` / `SegmentCheck` / `EvaluateResult`, fed at `TrackRenderer.cpp:5132`, gated on
  `TrackRenderer::rule_` and **not** on `0x007f0fd0`. It executes: 5 `RULE-EVAL` evaluations per
  240 s run, with rule-specific outcomes (`L0` concludes, `L4` does not, `L10`'s timer varies).
- It is reachable from the existing Training recipe via `MASHED_ROUND_RULE`, no code change.
- Replacing its metric with leg A's monotone `arcpct` changes car ordering on 28.6–38.6 % of
  frames — the swap has content.
- **New blocker:** round-level match outcomes are not reproducible across repeats, so the
  behavioural half of E2 cannot be gated in this scenario until a reproducible long-run harness
  exists. E2's write-proof, ordering, and (e)/(b)/knob-off no-regression gates remain measurable
  on the deterministic 60 s scenario.
- Also noted, for the separate `0x007f0fd0` mirror sub-lane registered in `PREREG_CONSUMER.md` §5:
  a worker survey of every reader of that global found only **three** that execute in a standalone
  race, all in `AiPreTickRubberBand` (`AiStandalone.cpp:1469/1501/1541`), plus one frontend-only
  reader `ModeCodeLookup` (`Frontend/BatchAA_s4.cpp:64`, reachability `[UNCERTAIN]`). Every other
  reader is `.asi`-only, a dead export (`HudDispatch.cpp`'s `HudIngameDispatch`), or a
  commented-out install (`ScenarioLeaves_sa2.cpp`). Unrelated but worth not tripping over: the
  `// DAT_007f0fd0` annotation on `g_playerCount` (`ForceIntegratorStubs.cpp:22`,
  `ForceIntegrator.h:98`) is a mislabel — that is a different global.
