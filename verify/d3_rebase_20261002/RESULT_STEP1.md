# STEP 1 RESULT — D3 re-baselined on the post-attempt-20 build

Pre-registration: `PREREG_STEP1.md`, committed unrun at `867de577`. **No gate was
replaced, no band was moved, no decision rule was changed.**

## Verdict in one line

**(e) still PASSES 3/3. (b) still FAILS 3/3. Attempt 20's admission test moved the AI
(b)/(e) window statistics by EXACTLY ZERO, and a positive control proves the knob was
live.** Powerups and modes guards are both unchanged.

## Gate outcomes

| gate | outcome |
|---|---|
| **G1-DET** | **PASS.** `r1` / `r2` / `r3` agree to **every printed digit** on all (e) and all (b) lines, all three cars. |
| **G1-BOOT** | `r3`'s first boot exited early after `t30`; the retry exited early after `t08`. **Both are reported as failures of the boot, not hidden.** They do **not** affect the measurement: the scorer's window is the 220 AI-step calls from the first `c4 != 0`, every run reached it, and G1-DET's exact digit-for-digit agreement across all three is the proof. |
| **G1-E** | **(e) PASS on all three cars**, both gated stats, `regime0=1` and `flag=[0]` on every car. No car needed re-taking. |
| **G1-B** | **(b) FAIL on all three cars.** 13 of 30 bands fail (was 15 on 2026-09-29). |
| **G1-DELTA** | computed below, labelled as the 30-commit delta. |
| **G1-ATTRIB** | **`p1` == `r1` on every printed digit**, both criteria, all three cars → attempt 20's admission test moved **0** of these numbers. Positive control below. |

Band files proven unedited: `git diff --stat re/tools/ai_ctrl_window.py
re/tools/ai_speed_env.py` is **empty**, and `git status --porcelain` on both is empty.

## (e) — criterion PASSES 3/3 on HEAD

`py -3.12 re/tools/ai_speed_env.py --check verify/d3_rebase_20261002/r{1,2,3}.csv`

| car | stat | band | HEAD (`r1`=`r2`=`r3`) | n | verdict |
|---|---|---|---:|---:|---|
| 1 | `launch` | 1397.2..1454.2 (ref 1425.7) | **1426.4** | 220 | PASS |
| 1 | `ft_median_m0` | 2500.6..2602.6 (ref 2551.6) | **2550.6** | 100 | PASS |
| 2 | `launch` | 2011.5..2093.6 (ref 2052.5) | **2053.0** | 220 | PASS |
| 2 | `ft_median_m0` | 2011.5..2093.6 (ref 2052.5) | **2053.0** | 23 | PASS |
| 3 | `launch` | 2013.9..2096.1 (ref 2055.0) | **2055.2** | 220 | PASS |
| 3 | `ft_median_m0` | 2232.5..2323.7 (ref 2278.1) | **2278.2** | 39 | PASS |

`regime0=1`, `flag=[0]`, `k_own=220` on all three cars in all three runs.

## (b) — criterion FAILS 3/3 on HEAD

`py -3.12 re/tools/ai_ctrl_window.py --check verify/d3_rebase_20261002/r{1,2,3}.csv`,
n = 220 calls per car, `start=0`.

| car | failing band | HEAD | band |
|---|---|---:|---|
| 1 | `c0_distinct` | 6 | 13..37 |
| 1 | `c1_distinct` | 103 | 17..70 |
| 1 | `steer_distinct` | 108 | 29..96 |
| 1 | `c1_median` | 52.5 | 0..0 |
| 1 | `abs_steer_median` | 52.5 | 0..23 |
| 2 | `c1_distinct` | 94 | 17..70 |
| 2 | `steer_distinct` | 122 | 29..96 |
| 2 | `c1_median` | 38.0 | 0..0 |
| 2 | `abs_steer_median` | 57.5 | 0..23 |
| 3 | `c1_distinct` | 100 | 17..70 |
| 3 | `steer_distinct` | 119 | 29..96 |
| 3 | `c1_median` | 49.0 | 0..0 |
| 3 | `abs_steer_median` | 49.0 | 0..23 |

Median window speed, for context next to the (b) numbers: car 1 `ft_median` **3420.3**
(n=215), car 2 **3334.4** (n=210), car 3 **3496.9** (n=215). Window = calls 0..219 in
every arm, so the median **call index** is 109.5 on every row above and on every row of
the 2026-09-29 comparison — the two populations are the same moment, not merely the same
regime.

## G1-DELTA — the 30-commit, 3-day delta (NOT "what attempt 20 did")

HEAD minus `verify/d3_force_20260929/e_b_check.txt`'s `sa_b2` arm. The range
`d165e6b4..HEAD` is **30 commits touching `mashedmod/src`**, spanning D2 attempts 12-20
**and** unrelated render work (car brightness, car grey chassis, Arctic sea tile, pickups).

**(e), gated — essentially zero:**

| car | stat | 2026-09-29 | HEAD | delta |
|---|---|---:|---:|---:|
| 1 | `launch` | 1426.4 | 1426.4 | **0.0** |
| 1 | `ft_median_m0` | 2550.7 | 2550.6 | **-0.1** |
| 2 | `launch` | 2053.0 | 2053.0 | **0.0** |
| 2 | `ft_median_m0` | 2053.0 | 2053.0 | **0.0** |
| 3 | `launch` | 2055.2 | 2055.2 | **0.0** |
| 3 | `ft_median_m0` | 2278.3 | 2278.2 | **-0.1** |

**(e), reported-not-gated `ft_median` — moved, and only on this stat:** car 1 3422.9 →
3420.3 (**-2.6**), car 2 3425.6 → **3334.4** (**-91.2**), car 3 3525.1 → 3496.9
(**-28.2**). This is the **full-window** median, which §D3 states is reported and not
gated precisely because past `k` the original has run through modes 3 and 7 the port
cannot have.

**(b) — small, mixed, and one car improved:**

| car | metric | 2026-09-29 | HEAD | delta |
|---|---|---:|---:|---:|
| 1 | `c0_distinct` | 7 | 6 | -1 |
| 1 | `c1_distinct` | 106 | 103 | -3 |
| 1 | `steer_distinct` | 112 | 108 | -4 |
| 1 | `c1_median` | 48.0 | 52.5 | +4.5 |
| 1 | `abs_steer_median` | 48.0 | 52.5 | +4.5 |
| 2 | `c1_distinct` | 93 | 94 | +1 |
| 2 | `steer_distinct` | 121 | 122 | +1 |
| 2 | `c1_median` | 42.0 | 38.0 | -4.0 |
| 2 | `abs_steer_median` | 58.0 | 57.5 | -0.5 |
| 3 | `c1_distinct` | 98 | 100 | +2 |
| 3 | `steer_distinct` | 116 | 119 | +3 |
| 3 | `c1_median` | 46.5 | 49.0 | +2.5 |
| 3 | `abs_steer_median` | 46.5 | 49.0 | +2.5 |
| 3 | `accel_distinct` | 1 | **2** | +1 → **now PASSES** |
| 3 | `brake_distinct` | 1 | **2** | +1 → **now PASSES** |

Failing bands in total: **15 → 13**. Car 3 went from 6 failing bands to 4; cars 1 and 2
are unchanged in count. **Nothing here is attributed to attempt 20** — see G1-ATTRIB.

## G1-ATTRIB — attempt 20's admission test moved the (b)/(e) window by exactly 0

`p1` = HEAD + `MASHED_D2_BATCHMODE=plane`, the diagnostic pre-fix arm
(`ContactProducer.cpp:106`). **`p1` reproduces `r1` on every printed digit of both
scorers, all three cars.**

**The positive control, because an identical score could just mean an inert knob**
(memory `verify-the-harness-knob-actually-took`, `absent-log-proves-nothing-run-a-control`).
Full-CSV row-by-row diff of `r1.csv` against `p1.csv`:

- **first differing row: index 1365, car `v=1`, AI-step `frame=455`.**
  Every column of every row before it is identical, on all three cars
  (455 rows per car precede it).
- columns that differ, and on how many rows: `own_x` **2824**, `own_z` **2824**,
  `rec_9e4` **2823**, `rec_b0c` **2823**, `look_x`/`look_z` **2815**, `curv` **2811**,
  `hist_d8` **2046**, **`c1` 1765**, `look_idx` 1603, `look_best` 1586, `hist_dc` 1029.
- first divergent values: `own_x` 13.4910517 vs 13.4912863, `own_z` 23.3614616 vs
  23.3618603, `hist_d8` 356.486389 vs 356.46994.

So the knob **is** live for the AI cars and it moves their position, speed (`rec_9e4`),
curvature and **steer byte (`c1`)** on thousands of rows. It simply does not move
**any** of them inside the scored window: the window is calls 0..219 and the first
divergence is at call **455**, 235 calls later.

**Structural corroboration:** `SolveWheelContacts` is called at
`VehiclePhysicsRun.cpp:1028` inside the per-car substep loop with no slot gate, and it
calls `ProduceTerrainBatch` at `WheelContactSolver.cpp:148`. Every car reaches the
changed code; the AI cars' own data confirms it.

**The substep change is NOT separable here.** `015537a2` landed the integer substep loop
(`VehiclePhysicsRun.cpp:918-922`, `:1076`) with **no revert knob**, so this step cannot
split its share from the other 29 commits. That is a stated limit, not a measurement.

## D2 REOPEN CANDIDATE (informational — no D2 code changed)

| trigger | finding | evidence |
|---|---|---|
| **the contact collector** (`ProduceTerrainBatch` / `MASHED_D2_BATCHMODE`) | Attempt 20's spatial admission test **does** change AI-car trajectories: 2823 rows of `rec_9e4` and 1765 rows of `c1` differ between `local` and `plane`, first at AI-step call 455. This is the **first measurement of attempt 20 on AI slots** — attempt 20 itself ran `participants=1` throughout and recorded the magnitude as `[UNCERTAIN]` (`re/NEXT_SESSION.md` item 7 at `015537a2`). **It is not a defect**: it is the expected effect of the fix, it moves nothing in D3's scored window, and D3's (e) still passes to 0.05%. Filed so D-11071's re-pickup condition is honoured. | `verify/d3_rebase_20261002/r1.csv` vs `p1.csv` |

**No D2 code was changed in this session.**

## Guards — both unchanged

**Powerups** (`pwsh -NoProfile -File re/tools/pu_replay/sweep.ps1`): **11/11 decision
CLEAN**, contact **CLEAN on 10 of 11**, `g3` **DIVERGES** — exactly the known R_FLAME
2-of-546 residue. Criterion (c) unchanged and still MET.

**Modes** (`py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0
--poke-ctrl-slots --oracle --rule 3 --hold 50`): **ORACLE VERDICT GREEN.**
`SegmentCheck 0x00410d10` 2447/2447 MISMATCH=0 with **2 segment-ends**;
`EvaluateResult 0x00410510` 2/2 MISMATCH=0; `FinishOrder 0x004177b0` 3389/3389
MISMATCH=0. Criteria (a)-(d) unchanged and still MET.

## Standing D3 position after STEP 1

| third | criterion | verdict |
|---|---|---|
| Modes | (a)(b)(c)(d) | **MET** (re-confirmed GREEN today) |
| Powerups | (a)(b)(c) | **MET** (re-confirmed 11/11 today; R_FLAME residue) |
| AI | (a) (c) (d) | MET |
| AI | **(e)** speed | **MET 3/3** (re-confirmed today) |
| AI | **(b)** steering | **NOT MET, 3/3 cars, 13 bands** — the sole D3 blocker, unchanged |

## Provenance

```
# standalone, all four arms (only the knob differs):
py -3.12 re/tools/sa_capture.py verify/d3_rebase_20261002/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="D3 rebase <tag>" \
    MASHED_AI_STEPDUMP=verify/d3_rebase_20261002/<tag>.csv
  r1 r2 r3  default build at HEAD (867de577), three repeats
  p1        + MASHED_D2_BATCHMODE=plane
```

Build: `mashedmod\build.bat` from PowerShell, clean, both targets, `[asi] all 422
objects up to date`. Every `mashed_re.exe` was spawned and killed **by PID** by
`sa_capture.py`; the one `MASHED.exe` by `scenario_launch.py`. No blanket kill by name.
No worktree. `original/MASHED.exe` untouched; no `unlock_*` patch applied.

The `.csv` payloads are large and untracked by the repo's convention for this kind of
dump; the distilled evidence is this file.
