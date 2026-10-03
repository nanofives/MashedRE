# STEP 2 RESULT — the modes-3/7 hypothesis, tested on the running original. **G2-SIM is REFUTED by its own pre-registered rule. The port was NOT written.**

Pre-registration: `PREREG_STEP2.md`, committed unrun at `a317d25c`. **No band was
moved. No decision rule was replaced.** One counterfactual **arm** was added after
G2-STEER and is declared as an addition below; it changes no decision rule.

## Verdict

> **Porting behaviour modes 3 and 7 would not close AI criterion (b).** The mechanism is
> real and it is large — it is the single biggest lever found — but with the **original's
> own per-call mode sequence spliced into the port**, (b) still **fails on all three
> cars**, and on **car 2 it gets worse**. Furthermore **no arm of the entire
> counterfactual matrix — mode, curvature, speed, error, or any combination tried —
> passes (b) on any car.** (b) has at least three large independent carriers and the mode
> is one of them, not the one.

This is a finding **against the user's stated closure path** (USER DECISION 2026-09-29:
"D3 closes by porting behaviour modes 3 and 7"). It is reported rather than worked
around, and **no game code was edited in this session.**

## Gate outcomes

| gate | outcome |
|---|---|
| **G2-MODE** | **PASS.** Modes 3 and 7 both occur on today's original. |
| **G2-JOINT** | **FAIL.** The "the original brakes and lifts where the port holds throttle, because of modes 3/7" framing is **partly refuted**: on car 3, **52 of 66** non-full-throttle calls are **mode-0** calls. Reported as a failure, **not re-thresholded.** |
| **G2-STEER** | measured; **supports** the H-LINK mechanism qualitatively, and uncovers two further carriers. |
| **G2-SIM** | **REFUTED** by the rule registered at `a317d25c`. Validation gate **PASSED 220/220 = 1.000 on all three cars**, so the counterfactuals are believable and the refutation is not a tooling artefact. |

## The live capture

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_modes37_20261002/o1.msd \
    --statediff-car 1 --statediff-aistep --hold 60
```
3632 frames, **5463 AI-step calls**, cars [1, 2, 3], `joinMiss=0`. Muted, `MASHED_TITLE`
set, Frida **entry hooks only**, PID tracked and only it killed.

## G2-MODE — PASS. Modes 3 and 7 are live on the current original.

`py -3.12 re/tools/ai_mode_split.py` (new, read-only), window = the 220 calls from the
first `c4 != 0`, identical to `ai_ctrl_window.py`'s.

| car | original `ai_mode` histogram | port (`r1`) |
|---|---|---|
| 1 | `{0: 124, 3: 16, 7: 80}` | `{0: 220}` |
| 2 | `{0: 171, 3: 49}` | `{0: 220}` |
| 3 | `{0: 196, 3: 24}` | `{0: 220}` |

Original window starts at call **752** (the countdown block of `c4 == 0` the §D3
tolerance text describes); the port's at call **0**. Both are "the first 220 racing
calls", and the median call index **inside** the window is 109.5 on both sides, so the two
populations are the same race phase.

Consistent with the 2026-09-27 record (car 1 was `{0:105, 7:115}`); mode 3 now also
appears on car 1. Run-to-run variation, not a contradiction.

## G2-JOINT — FAIL, and it partly refutes the kickoff's framing

Non-full-throttle = `c4 != 255` **or** `c5 != 0`.

| car | original: non-full-throttle | of which `mode == 0` | of which `mode != 0` | port: non-full-throttle |
|---|---:|---:|---:|---:|
| 1 | 96 | **6** | 90 (93.8 %) | 5 |
| 2 | 41 | **0** | 41 (100 %) | 10 |
| 3 | 66 | **52** | 14 (21.2 %) | 5 |

The registered gate was "**every** non-full-throttle call on the original is a
`mode != 0` call". **Car 3 fails it outright** (79 % of its lifting happens in mode 0) and
car 1 fails it narrowly. So "modes 3 and 7 are what make the original brake and lift" is
**true for car 2, mostly true for car 1, and false for car 3.**

What *is* true, and is the part worth keeping: the port lifts **5 / 10 / 5** times in the
window against the original's **96 / 41 / 66**. The port does hold the throttle. The cause
is simply not the mode on car 3. **Note this concerns the accel/brake bands, which
already PASS** — it does not bear on the 13 failing steer bands.

## G2-STEER — the mechanism is visible, and so are two more carriers

| src | car | group | n | med call idx | `abs_steer` med | `c1` distinct | `curv` med | frac `curv > 20` | speed med |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|
| ORIG | 1 | `mode == 0` | 124 | 61.5 | **8.0** | 15 | 6.61 | 0.331 | 2254.8 |
| ORIG | 1 | `mode != 0` | 96 | 147.5 | 0.0 | 8 | 125.82 | 1.000 | 2839.0 |
| ORIG | 2 | `mode == 0` | 171 | 103.0 | **8.0** | 42 | 9.05 | 0.415 | 2524.1 |
| ORIG | 2 | `mode != 0` | 49 | 195.0 | 234.0 | 11 | 91.30 | 0.633 | 2312.4 |
| ORIG | 3 | `mode == 0` | 196 | 100.5 | **23.0** | 65 | 23.64 | 0.505 | 2554.6 |
| ORIG | 3 | `mode != 0` | 24 | 145.5 | 35.0 | 10 | 132.91 | 0.875 | 3995.0 |
| PORT | 1 | `mode == 0` | **220** | 109.5 | **52.5** | 103 | **52.04** | 0.623 | **3421.7** |
| PORT | 2 | `mode == 0` | **220** | 109.5 | **57.5** | 94 | **52.81** | 0.632 | **3346.2** |
| PORT | 3 | `mode == 0` | **220** | 109.5 | **49.0** | 100 | **50.08** | 0.605 | **3495.9** |

Three things fall out, and only the first is about modes 3/7:

1. **The mechanism H-LINK named is visible.** The original's `mode != 0` calls carry
   `curv` medians of **125.8 / 91.3 / 132.9** — far above the multiplier's `k20f = 20.0`
   threshold — and in those modes the `0x0041665c` multiplier is **off** (its `mode == 0`
   conjunct, `AiStandalone.cpp:876`/`:899`). **The original stops amplifying steer by
   curvature exactly where curvature is most extreme.** The port, pinned at
   `int mode = 0` (`AiStandalone.cpp:844`), amplifies hardest precisely there.
2. **The port's curvature is 2-8x the original's**, comparing like with like — `mode == 0`
   group against `mode == 0` group: **52.04 / 52.81 / 50.08** against **6.61 / 9.05 /
   23.64**. `curv` is the multiplier's own other input (`m *= curv * 0.05`), so this is a
   second, independent carrier of the same inflation. Producer:
   `SplineCurvature(spline, ownX, ownZ, 10.0f)` = `FUN_00443440` at `0x004162b0`
   (`AiStandalone.cpp:837`), folded to `[0,180]` at `:838` (`0x004162be..0x004162d5`).
3. **The port is 33-52 % faster inside the window**, again `mode == 0` vs `mode == 0`:
   **3421.7 / 3346.2 / 3495.9** against **2254.8 / 2524.1 / 2554.6**, and
   `m = err * speed * 0.0030034` makes speed a **direct linear multiplier** of steer
   magnitude. Criterion **(e) still passes** because (e) gates `launch` and
   `ft_median_m0` over `[0, k)` with `k = 100 / 23 / 39`; the excess is **after** `k`,
   which is exactly why §D3 left the full-window median reported and not gated.

**Stated confound, because it cuts against reading 2 and 3 as pure defects:** the two
sides traverse the track at different speeds, so at the same within-window index they are
at different track positions, and curvature is a property of position. The comparison is
therefore partly circular. It is reported as a measured difference between the two
windows, **not** as a proven defect in `FUN_00443440`. Resolving it needs a
position-matched comparison, which this step did not run.

## G2-SIM — REFUTED by the registered rule

`py -3.12 re/tools/ai_band_sim.py --orig verify/d3_modes37_20261002/o1.msd.aistep.csv
--sa verify/d3_rebase_20261002/r1.csv --out verify/d3_modes37_20261002/g2sim.txt`

**Validation gate (the tool's own, kept at ≥95 % and not relaxed):** all four ctrl bytes
exact **220/220 = 1.000** on cars 1, 2 and 3. The simulator reproduces the port's observed
bytes perfectly, so the counterfactual arms are believable.

Scored against the tool's `TOL_PURE0` envelope (`ai_band_sim.py:48-55`), which differs
slightly from `ai_ctrl_window.py`'s in two floors (`c1_distinct` 21 vs 17,
`abs_steer_median` 7 vs 0). Neither was edited.

| car | arm | `c0D` | `c1D` | `stD` | `abs_steer` med | verdict |
|---|---|---:|---:|---:|---:|---|
| 1 | baseline (own speed, own err) | 6 | 103 | 108 | 52.5 | FAIL ×5 |
| 1 | **G2-SIM ORIG modeseq** | 6 | **58** | **63** | **15.0** | FAIL ×2 |
| 1 | ADDED ORIG curvseq | 6 | 95 | 100 | 52.5 | FAIL ×5 |
| 1 | ADDED ORIG mode+curv | 6 | **45** | **50** | **15.0** | FAIL ×2 |
| 2 | baseline | 29 | 94 | 122 | 57.5 | FAIL ×4 |
| 2 | **G2-SIM ORIG modeseq** | 29 | **99** | 127 | 47.0 | FAIL ×4 |
| 2 | ADDED ORIG curvseq | 29 | 75 | 103 | **26.5** | FAIL ×4 |
| 2 | ADDED ORIG mode+curv | 29 | 94 | 122 | **26.5** | FAIL ×4 |
| 3 | baseline | 20 | 100 | 119 | 49.0 | FAIL ×4 |
| 3 | **G2-SIM ORIG modeseq** | 20 | 97 | 116 | **24.5** | FAIL ×4 |
| 3 | ADDED ORIG curvseq | 20 | 85 | 104 | 24.0 | FAIL ×4 |
| 3 | ADDED ORIG mode+curv | 20 | 79 | 98 | **20.0** | FAIL ×3 |

**Applying the rule exactly as registered** ("SUPPORTED → `abs_steer_median` **and**
`c1_distinct` both move toward their bands on **all three cars**, and at least one enters
its band on at least one car"):

- `abs_steer_median` moves toward band on all three (52.5→15.0, 57.5→47.0, 49.0→24.5). ✔
- `c1_distinct` moves toward band on cars 1 and 3 (103→58, 100→97) but **AWAY on car 2
  (94 → 99)**. ✘
- Entry: car 1 enters **both** bands. ✔

The "all three cars" conjunct fails on car 2's `c1_distinct`. → **REFUTED**, and per the
registration I **did not write the port**.

**The refutation is not a technicality.** The matrix's own strongest arm — the original's
mode *and* curvature spliced together — still leaves (b) failing on **all three cars** (2,
4 and 3 bands). And **no arm anywhere in the matrix, including `ORIG speed, ORIG err`,
passes (b) on any car.** Splicing every input this instrument can splice does not close
(b).

**Stated limit, as registered:** these arms splice the original's inputs onto the port's
own trajectory. The tool's own banner says it — *"err and speed are COUPLED in the live
loop … Holding one fixed is a BOUND, not a prediction of what a rebuilt binary would
do."* So this bounds the mechanism's contribution; it does not prove a real port could
not do better. What it does establish is that **modes 3/7 alone are demonstrably not
sufficient**, which is the question the gate was built to answer.

## Two residues the matrix exposes and no arm moves

- **`c0_distinct = 6` on car 1** is identical in **every** arm (6 in all seven), against a
  floor of 13. No mode, curvature, speed or error substitution touches it. It is a
  structurally different defect from the rest of (b). `[UNCERTAIN]` — producer not
  identified in this step.
- **`c1_median` ∈ [0,0]** fails in every arm on every car. The original's `c1_median` is 0
  because its steering is one-sided in the window; the port's is 15-49 in every
  counterfactual.

## Scope facts established for whoever does write the port

From `py -3.12 re/tools/decomp_pc.py --datarefs` / `--callers` (read-only pool clone
`Mashed_pool0`; the master project was not written):

- `FUN_00484c70` (`0x00484c70`, 17 bytes) returns `*param_1 = DAT_006e70d8` (count),
  `&DAT_006dccb8` (base). It is **already ported and C3 with a GREEN Frida diff** at
  `mashedmod/src/mashed_re/Util/PromoLoop_round20.cpp:79`, but that TU is **asi-only** —
  `exe_file` is empty in `hooks.csv`, so `mashed_re.exe` has no copy.
- `DAT_006e70d8` is written **only** by `FUN_00484c90` (`0x00484cd4`) and `FUN_00485070`
  (`0x0048508a`); `DAT_006dccb8` by `FUN_00484c90` (`0x00484ca4`), indexed by the
  registrar `FUN_00484cf0` (`0x00484d2d` / `0x00484d35`).
- `FUN_00484c90` ← `FUN_0040cfd0`, `FUN_004111c0`. `FUN_00485070` ← `FUN_0040fc00`.
  **`FUN_00484cf0` ← 13 callers**, most in the already-ported power-up range:
  `0x0045bba0` (the dispatcher), `0x004532f0`, `0x00454350`, `0x004570a0`, `0x00458080`,
  `0x00459000`, `0x0045a530`, plus `0x00419a00`, `0x0041f710`, `0x0044bbc0`,
  `0x0044c490`, `0x00481a30`, `0x00486830`.
- **Therefore porting `FUN_00484c70` alone is inert**: in the standalone both globals are
  `.bss` zeros, the count is 0, the loop in `FUN_00414c30` never runs, and no mode is ever
  set. A real port needs the producer chain. **Seeding the globals would not be a port.**
- `FUN_00414c30` (`0x00414c30..0x00414ef0`, 704 bytes) has **no port at all** — the only
  reference in `mashedmod/src` is the asi-side call-through at `AiControlStep.cpp:92`,
  which jumps into `MASHED.exe`'s own code and cannot work in `mashed_re.exe`.

## D2 WATCH

**No D2 REOPEN CANDIDATE from this step.** Nothing measured here touches `+0x4a4`, the
contact collector, `FUN_00538c80`, grip-clamp #6, the substep/chunk loop,
`ReassertContacts` or player-car speed on Training. **No D2 code was changed.**

## Changes made in this step (none to game code)

| file | change |
|---|---|
| `re/tools/ai_mode_split.py` | **new**, read-only analysis. The G2-MODE / G2-JOINT / G2-STEER reporter. |
| `re/tools/ai_band_sim.py` | `simulate()` gains `modeseq` (**pre-registered**) and `curvseq` (**an ADDITION**, declared here, made after G2-STEER measured the 2-8x curvature gap that the registered arms could not separate from the mode gap). Three counterfactual arms added. **No constant, no band and no decision rule changed**; the four pre-existing arms are untouched by construction (both new parameters default to `None`). |

**Neither `re/tools/ai_ctrl_window.py` nor `re/tools/ai_speed_env.py` was edited** — the
(b) and (e) bands are byte-identical to their state at session start.

## What this means for D3

D3 does **not** close on the modes-3/7 port, and the evidence says it would not close even
if that port were written and perfect. **AI criterion (b) remains the sole blocker**, and
it now has a measured decomposition rather than a single named cause:

| carrier | evidence | status |
|---|---|---|
| **mode pinned 0** (`AiStandalone.cpp:844`) — the `0x0041665c` multiplier never switches off | best single arm on cars 1 and 3: `abs_steer` 52.5→15.0 and 49.0→24.5; `c1D` 103→58 | real, large, **not sufficient** |
| **curvature 2-8x high** — `FUN_00443440` / `0x004162b0`, same comparison group | best single arm on car 2: `abs_steer` 57.5→26.5, `c1D` 94→75 | real, **confounded by position** — needs a position-matched measurement |
| **speed +33-52 % inside the window** — a linear multiplier of steer magnitude | `mode==0` vs `mode==0`: 3421.7/3346.2/3495.9 vs 2254.8/2524.1/2554.6 | real; **(e) still passes**, which bounds it to the post-`k` span |
| **`c0_distinct = 6` on car 1** | unmoved by all seven arms | separate, unexplained `[UNCERTAIN]` |

The cheapest next measurement, and it is a measurement rather than a port: a
**position-matched** comparison of `curv` and `err` (same track distance on both sides,
not same call index), which would turn carrier 2 from confounded into decided.
