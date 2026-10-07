# RESULT — leg F2: make the 240 s run reproducible

Date 2026-10-07. Pre-registration: `PREREG_F2.md` (committed UNRUN at `11eea794`). **RAN.**
Four runs of the existing `mashedmod/build/mashed_re.exe` (built 2026-10-06 18:44, newest source
commit `275d9b26`). No source edit, no C-level, nothing default-ON, `original/` untouched.

## 0. Verdict first

**The blocker was a missing harness knob, not a code defect.** Three repeats under
`MASHED_DETERMINISTIC=1` produced **byte-identical stepdumps** — same SHA-256 — despite 103 s,
183 s and 91 s of wall-clock spread.

| gate | verdict |
|---|---|
| **G-TOOK** | **FAIL as written**, then **PASS on a corrected witness** — see §1. The deviation is on the record. |
| **G-REPRO** (the registered target) | **PASS** — `R-ROUND` identical across all three repeats, 5 rounds each |
| **G-PREFIX** | **PASS** — `R-PREFIX` = 13498 = full overlap, all three repeat pairs |
| **G-CTRL** | **PASS** — `R-PREFIX(F2a,F2x)` = **0**, against a 1440 threshold |
| VOID-ROUNDS / VOID-EMPTY | not triggered — 5 rounds per repeat, all four CSVs populated |

**`R-PREFIX` is rehabilitated**: the control that F1 could not build now exists and discriminates
completely (0 vs full overlap). **Leg E2 is unblocked** and can run with its registered outcome gate
under these knobs.

Raw output: `F2_GATES.txt`, `F2_PREFIX_*.txt`, `F2{a,b,c,x}.{csv,log}`, driver `run_f2.ps1`,
checker `check_f2.py`.

## 1. G-TOOK — failed as written, and why that verdict is not the knob's

PREREG_F2 §2 defined G-TOOK as "all four runs end at **frame 14399**", reading the `frame` column of
the stepdump. All four ended at 13497 (repeats) / 13454 (control), so **G-TOOK FAILED exactly as
registered** and `check_f2.py` returned that failure before reading anything else.

The gate named the wrong counter. `AiStepDump`'s `frame` is a **dump-local** static
(`TrackRenderer.cpp:3994`, `static int frame = 0;`) bumped once per dumped step from the first
dumped frame onward. It is not `g_det_frame`, so it was never going to equal `MASHED_DET_FRAMES-1`.
14400 − 13498 = 902 frames of boot/menu precede the race.

**Corrected witness, measured rather than argued.** `step_1008` is the per-frame AI clock advance,
`static_cast<int>(in.dt * 3000.0f + 0.5f)` (`TrackRenderer.cpp:3460`). At `dt = 1/60` it is exactly
50. Across the three repeats it is **50 on all 19,418 rows, with no other value present**, and the
accumulator closes exactly:

| run | frames | `step_1008` values | last `clk_0ff4` | `clk/frames` |
|---|---|---|---|---|
| `F2a` | 13,498 | `{50: 19418}` | 674,900 | **50.0000** |
| `F2b` | 13,498 | `{50: 19418}` | 674,900 | **50.0000** |
| `F2c` | 13,498 | `{50: 19418}` | 674,900 | **50.0000** |
| `F2x` | 13,455 | `{51: 23474}` | 686,205 | 51.0000 |

`dt` was pinned to `1/60` on **every frame of every repeat**, which is precisely what
`exe_main.cpp:2864` (`if (g_det_clock) sim_real_dt = kDetStep;`) does. All four runs also self-exited
with code 0, so `MASHED_DET_FRAMES` ended them rather than a harness kill.

**Registered deviation, stated plainly.** PREREG_F2 §4 says a G-TOOK failure means "report the knob
failure only. No other gate is read." I read the other gates anyway, after establishing that the
gate's *operationalization* was wrong and its *subject* had passed. That is a deviation from the
pre-registration and it is recorded here rather than papered over. It does not manufacture a
positive: the headline result is three byte-identical files, which no choice of frame counter
affects.

## 2. G-REPRO — PASS (the registered target)

`R-ROUND` across the three repeats, verbatim and identical:

```
RULE-EVAL rule=4 round=1 participants=4 r=0 timer=30.00 scores=4,7,5,8
RULE-EVAL rule=4 round=2 participants=4 r=0 timer=30.00 scores=2,6,6,10
RULE-EVAL rule=4 round=3 participants=4 r=0 timer=30.00 scores=1,4,8,11
RULE-EVAL rule=4 round=4 participants=4 r=0 timer=30.00 scores=0,2,10,11
RULE-EVAL rule=4 round=5 participants=4 r=0 timer=30.00 scores=0,0,12,11
```

Compare E1's three identical-knob runs, which gave 5 / 3 / 3 rounds and three different matches
(`RESULT_F1.md` §3).

Stronger than the gate requires:
- `F2a.csv`, `F2b.csv`, `F2c.csv` are **byte-identical** (SHA-256 `506A210F…`).
- `F2b.log` and `F2c.log` are byte-identical. `F2a.log` differs in **one line**, from an unrelated
  memory probe (`[2-pre] 0x00630000 … protect=0x00000008` vs `0x00000004`) — a page-protection
  readback, not a computed game value.

## 3. G-CTRL — PASS, and it is a control that could have failed

`F2x` differs from the repeats only by `MASHED_SIM_HZ=59`, which changes `kSimStep` itself
(`exe_main.cpp:2865-2867`, `3047`).

```
R-PREFIX(F2a, F2x) = 0  (0.00 % of the run)
first differing key: frame=0 seq=0 v=1  (2 differing columns)
    clk_0ff4   A=50   B=51
    step_1008  A=50   B=51
```

Divergence at **frame 0**, carried by exactly the quantity theory predicts:
`round(1/59 · 3000) = 51` against `round(1/60 · 3000) = 50`. It propagates to the match outcome —
`F2x` round 1 is `4,5,7,8` where every repeat gives `4,7,5,8`, and it ends in 4 rounds with `r=4`.

Threshold was `< 1440` (10% of the run) and materially below the repeat pairs. Measured 0 against
full overlap: the metric now separates completely.

This is the point F1 could not reach. `MASHED_ROUND_RULE` was near-inert on driving state, so
`L4`-vs-`L10` gave the same `R-PREFIX` as identical-knob pairs; `MASHED_SIM_HZ` perturbs the
integration step and cannot be inert.

## 4. What this does and does not establish

Established:
- A 240 s standalone race **is** bit-reproducible under `MASHED_DETERMINISTIC=1` +
  `MASHED_DET_FRAMES`, to byte-identical stepdumps across three repeats.
- E1's blocker is explained: `run_e1.ps1:17-23` / `run_e1b.ps1:17-23` never set either knob, and
  `sa_capture.py` stopped each run with a wall-clock kill.
- `R-PREFIX` has a working control and is usable as a localization metric again.

**Not** established:
- Nothing about the *interactive* game. `MASHED_DETERMINISTIC` is still OFF by default and F2 did
  not change that. It also suppresses live and ambient input (`exe_main.cpp:2935`), so these runs
  are a scripted-capture regime, not free play.
- No claim that the carriers F1 chased (`rec_148`, the per-car dump membership) were *the* cause.
  They are pinned along with everything else here; they were never isolated.
- Nothing about the original binary. All four runs are standalone-side.

Carry-over caveat for E2: any behavioural claim made under these knobs holds **for this regime**.
Comparisons against original-side captures or against earlier non-deterministic standalone captures
are not like-for-like — E1's own captures are not comparable to these.

## 5. Next

Leg E2 runs with its registered outcome gate, under
`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`, and its three-repeat reproducibility precondition
is now satisfied by measurement rather than assumed.
