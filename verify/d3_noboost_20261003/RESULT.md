# RESULT — D3: what the port-only AI start boost does to criteria (b) and (e)

Pre-registration: `PREREG.md`, committed **unrun** at `2b3e2f47`. Branch
`race/first-frame-parity`. **No code change under `mashedmod/src`, no build, no band edit,
no C-level move, nothing pushed.**

## Verdict in one line

**Removing the seed improves (b) from 13 failing bands to 6 — car 3 passes outright — and
regresses (e) from 6/6 PASS to 6/6 FAIL, by −46 % to −90 %. Neither arm is correct, and the
trade-off is the user's call, stated below and not made here.**

## ONE GATE WAS REPLACED — read this before the numbers

`PREREG.md` registered the witness's base validation as **`record[v] + 0x000 == v`**, the
self-ref slot index the original writes at `0x0046bab8`
(`Util/PromoLoop_round80.cpp:31`). **That check is FALSE in the port and it voided the first
witness run** (`0/1902` samples passed). The cause, measured not guessed: the standalone's
`g_records` is a **port-local mirror** (`VehiclePhysicsRun.cpp:122`) `memset` to 0 by
`VehiclePhysics_Init` (`:240`); the self-ref store belongs to the original's absolute
`DAT_008815a0` array, which the standalone does not route through. Every slot's head reads
`00 00 00 00 01 00 00 00 ...`.

**Replacement, and it is strictly stronger than what it replaced** — three independent legs,
all reported:

1. **`g_startBoosted[0..3]` contains only 0/1** (it is `bool[16]`). → `True` in both arms.
2. **The witness's `+0x9e4` agrees with the same run's `MASHED_AI_STEPDUMP` `rec_9e4`
   column**, an independently produced channel, so base + stride + offset are all validated
   together. Arm A maxima: witness `4476.997 / 4476.417 / 4476.888`, CSV
   `4476.997 / 4476.439 / 4476.952`. Arm B: witness `4479.722 / 4475.924 / 4477.165`, CSV
   identical to all printed digits.
3. **The arm A vs arm B contrast on `g_startBoosted` is itself the live/inert control** — a
   wrong base cannot read `[0,1,1,1]` in one arm and `[0,0,0,0]` in the other.

`selfref_samples = 0` is reported in both JSONs as a disclosed zero, not hidden.

## G-KNOB — the knob took, and the seed is the only thing it moves

`MASHED_NO_START_BOOST` has **exactly one read site in the whole port**:
`VehiclePhysicsRun.cpp:703` (`grep` over `mashedmod/src` returns 3 hits — that `getenv`, its
own comment at `:702`, and a comment in `LaunchRevCharge.cpp:258`). So the knob's entire
effect is the five lines at `:703-707`.

`re/tools/sa_boostwatch.py`, `--hz 100`, `ReadProcessMemory` only, no injection, no Frida.
`g_records` `0x00c09b88`, `g_startBoosted` `0x00c16ff8` (linker map, ASLR off —
`/BASE:0x10000 /DYNAMICBASE:NO`, `build.bat:218`).

| slot | arm A (`aw.json`, 3572 samples) | arm B (`bw.json`, 3546 samples) |
|---|---|---|
| **0** (player) | `g_startBoosted` only ever **0**; `+0xbf8` only **0**; `+0xbf4` max **0**; `+0x9e4` max **0.638** | **identical**: `0` / `0` / `0` / **0.638** |
| **1** | `g_startBoosted` 0→**1**; `+0xbf8` observed **1** on 11 samples; `+0xbf4` **1100, 700, 500, 300, 100** | `g_startBoosted` **0** all run; `+0xbf8` **0** all run; `+0xbf4` max **0** |
| **2** | same: 0→**1**, `+0xbf8` **1** x11, `+0xbf4` **1100…100** | same as slot 1: never seeded |
| **3** | same: 0→**1**, `+0xbf8` **1** x11, `+0xbf4` **1100…100** | same as slot 1: never seeded |

**G-KNOB PASSES all three legs.** The observed `+0xbf4` ladder **1100 → 900 → 700 → 500 →
300 → 100** is a **−200 per frame** decay, which is the documented law
(`ForceIntegratorStubs.cpp:86`), and the first sample after the seed reads **1100**, which is
exactly what `VehiclePhysicsRun.cpp:707`'s own comment predicts (*"1300 pre-A6a value; A6a's
two sites leave the measured 1100"*). The seed is observed in flight, not inferred.

**The witness runs reproduce the scored runs on every printed digit** — `awstep.csv` == `a1`
and `bwstep.csv` == `b1` on all (b) lines, all three cars — so the polling and
`MASHED_PLAYERTRACE` perturbed nothing and the witness transfers to the scored arms.

## G-DET — both arms deterministic

| gate | outcome |
|---|---|
| **G-DET-A** | **PASS.** `a1`/`a2`/`a3` agree to **every printed digit** on all (b) and all (e) lines, all three cars. |
| **G-DET-B** | **PASS.** `b1`/`b2`/`b3` agree to **every printed digit**, same scope. **No spread to report.** |
| **G-BOOT** | **No failed boot.** All six scored runs and both witness runs produced all three screenshots and a full-length CSV. Nothing was retried. |

**Arm A was re-captured, not reused** (the exe at `a6b07bd1…` post-dates
`verify/d3_rebase_20261002/r1..r3` and no SHA was recorded for the exe that produced them).
**Cross-check: fresh arm A reproduces `r1/r2/r3` on every printed digit** — `launch`
1426.4 / 2053.0 / 2055.2, `ft_median_m0` 2550.6 / 2053.0 / 2278.2, window medians
3421.7 / 3346.2 / 3495.9, 13 failing (b) bands. The 2026-10-02 baseline stands.

Band files proven unedited: `git diff --stat re/tools/ai_ctrl_window.py
re/tools/ai_speed_env.py` **empty**, `git status --porcelain` on both **empty**.

## Criterion (b) — all 30 bands, both arms, with the original

`py -3.12 re/tools/ai_ctrl_window.py --check`, n = **220** calls per car, `start=0`,
**median call index 109.5 on every row of every arm and on the original** — the three
populations are the same moment, not merely the same regime.

| car | band | tol | ORIG `o1` | **arm A** (boost ON) | | **arm B** (boost OFF) | |
|---|---|---|---:|---:|:--:|---:|:--:|
| 1 | `c0_distinct` | 13..37 | 13 | 6 | **FAIL** | 7 | **FAIL** |
| 1 | `c1_distinct` | 17..70 | 21 | 103 | **FAIL** | 80 | **FAIL** |
| 1 | `steer_distinct` | 29..96 | 33 | 108 | **FAIL** | 86 | PASS |
| 1 | `c0_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |
| 1 | `c1_median` | 0..0 | 0.0 | 52.5 | **FAIL** | 0.0 | PASS |
| 1 | `abs_steer_median` | 0..23 | 7.0 | 52.5 | **FAIL** | 6.0 | PASS |
| 1 | `accel_distinct` | 2..4 | 3 | 2 | PASS | **1** | **FAIL (new)** |
| 1 | `accel_median` | 25..255 | 255.0 | 255.0 | PASS | 255.0 | PASS |
| 1 | `brake_distinct` | 2..2 | 2 | 2 | PASS | **1** | **FAIL (new)** |
| 1 | `brake_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |
| 2 | `c0_distinct` | 13..37 | 35 | 29 | PASS | 33 | PASS |
| 2 | `c1_distinct` | 17..70 | 50 | 94 | **FAIL** | 71 | **FAIL** |
| 2 | `steer_distinct` | 29..96 | 85 | 122 | **FAIL** | 103 | **FAIL** |
| 2 | `c0_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |
| 2 | `c1_median` | 0..0 | 0.0 | 38.0 | **FAIL** | 0.0 | PASS |
| 2 | `abs_steer_median` | 0..23 | 11.0 | 57.5 | **FAIL** | 21.0 | PASS |
| 2 | `accel_distinct` | 2..4 | 2 | 2 | PASS | 2 | PASS |
| 2 | `accel_median` | 25..255 | 255.0 | 255.0 | PASS | 255.0 | PASS |
| 2 | `brake_distinct` | 2..2 | 2 | 2 | PASS | 2 | PASS |
| 2 | `brake_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |
| 3 | `c0_distinct` | 13..37 | 27 | 20 | PASS | 21 | PASS |
| 3 | `c1_distinct` | 17..70 | 70 | 100 | **FAIL** | 43 | PASS |
| 3 | `steer_distinct` | 29..96 | 96 | 119 | **FAIL** | 63 | PASS |
| 3 | `c0_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |
| 3 | `c1_median` | 0..0 | 0.0 | 49.0 | **FAIL** | 0.0 | PASS |
| 3 | `abs_steer_median` | 0..23 | 23.0 | 49.0 | **FAIL** | 9.0 | PASS |
| 3 | `accel_distinct` | 2..4 | 2 | 2 | PASS | 2 | PASS |
| 3 | `accel_median` | 25..255 | 255.0 | 255.0 | PASS | 255.0 | PASS |
| 3 | `brake_distinct` | 2..2 | 2 | 2 | PASS | 2 | PASS |
| 3 | `brake_median` | 0..0 | 0.0 | 0.0 | PASS | 0.0 | PASS |

**Failing bands: car 1 5 → 4, car 2 4 → 2, car 3 4 → 0. Total 13 → 6.**

By the pre-registered rule **G-READ**, the count drops on all three cars, so **(b) IMPROVES**.
**(b) is still NOT MET** — 2 of 3 cars fail.

**And the aggregate hides a real regression that is stated here rather than netted out:**
car 1 gains **two new failing bands**, `accel_distinct` and `brake_distinct`. In arm B car 1
takes **only** `c4 = 255` and **only** `c5 = 0` across the whole window (`a255 = 1.000`,
`b255 = 0.000`); in arm A it takes `{0, 255}` on both. The original's car 1 takes
`c4 ∈ {0, 64, 255}` with `c4 = 255` on 63.6 % of calls and `c5 ∈ {0, 255}` with `c5 = 255`
on 21.4 %. Arm B's car 1 never lifts and never brakes in the scored window.

### The five steer bands, isolated

| band | car 1 A → B | car 2 A → B | car 3 A → B | tol |
|---|---|---|---|---|
| `c0_distinct` | 6 → **7** | 29 → 33 | 20 → 21 | 13..37 |
| `c1_distinct` | 103 → **80** | 94 → **71** | 100 → **43** | 17..70 |
| `steer_distinct` | 108 → **86** | 122 → **103** | 119 → **63** | 29..96 |
| `c1_median` | 52.5 → **0.0** | 38.0 → **0.0** | 49.0 → **0.0** | 0..0 |
| `abs_steer_median` | 52.5 → **6.0** | 57.5 → **21.0** | 49.0 → **9.0** | 0..23 |

`c1_median` and `abs_steer_median` collapse to band on **all three cars**. That is the
expected sign of `m = err * speed * 0.0030034` (`_DAT_005cd0e8`, `0x00416656`) with a lower
`speed`, and it is the single largest effect in this measurement.

## Criterion (e) — six gated values, both arms

`py -3.12 re/tools/ai_speed_env.py --check`. `regime0 = 1`, `flag = [0]`, `k_own = 220` on
every car in **both** arms; no car needed re-taking.

| car | stat | band (ref) | **arm A** | | **arm B** | | delta |
|---|---|---|---:|:--:|---:|:--:|---:|
| 1 | `launch` | 1397.2..1454.2 (1425.7) | **1426.4** (n=220) | PASS | **200.5** | **FAIL −85.9 %** | −1225.9 |
| 1 | `ft_median_m0` | 2500.6..2602.6 (2551.6) | **2550.6** (n=100) | PASS | **1364.3** (n=100) | **FAIL −46.5 %** | −1186.3 |
| 2 | `launch` | 2011.5..2093.6 (2052.5) | **2053.0** (n=220) | PASS | **200.5** | **FAIL −90.2 %** | −1852.5 |
| 2 | `ft_median_m0` | 2011.5..2093.6 (2052.5) | **2053.0** (n=23) | PASS | **200.5** (n=23) | **FAIL −90.2 %** | −1852.5 |
| 3 | `launch` | 2013.9..2096.1 (2055.0) | **2055.2** (n=220) | PASS | **200.5** | **FAIL −90.2 %** | −1854.7 |
| 3 | `ft_median_m0` | 2232.5..2323.7 (2278.1) | **2278.2** (n=39) | PASS | **353.2** (n=39) | **FAIL −84.5 %** | −1925.0 |

By **G-READ**, **(e) REGRESSES: 6 of 6 gated values leave their bands.**

**This is the load-bearing fact of the whole measurement.** The bands are the **original's
own** envelope. The original's `launch` is **1425.7 / 2052.5 / 2055.0**, so the original
**does** apply a large start acceleration. Arm A reproduces it to **0.05 %**. Arm B does not
reproduce it at all (200.5 on every car). **Removing the seed does not remove a port
artefact — it removes the port's only reproduction of a real original behaviour.**

## Window speed, beside the original's

Plain median of `rec_9e4` over the 220-call window; **median call index 109.5 on all three
sides**; n = 220 everywhere.

| car | ORIGINAL `o1` | arm A | arm B | A vs orig | B vs orig |
|---|---:|---:|---:|---:|---:|
| 1 | **2419.5** | 3421.7 | **3169.4** | +41.4 % | **+31.0 %** |
| 2 | **2397.0** | 3346.2 | **2904.7** | +39.6 % | **+21.2 %** |
| 3 | **2617.6** | 3495.9 | **3011.0** | +33.6 % | **+15.0 %** |

Arm A reproduces U-9185's recorded `3421.7 / 3346.2 / 3495.9` exactly. Removing the seed
cuts the mid-window excess roughly in half and **leaves +15..31 %**. So the sustained
over-speed is **not** the seed: the seed accounts for about half of it and something else
carries the rest.

## STEP 3 — decomposition of what remains in arm B

(b) still fails in arm B, so this leg ran. **Descriptive, no fix.**

### 3.1 Counterfactual matrix (`ai_band_sim.py` on `b1`)

Simulator validation gate: **all four bytes exact 220/220 = 1.000 on all three cars** —
PASS, so the counterfactuals are believed. **Caveat carried from the tool:** it scores
against the tighter *pure regime-0 K=220* envelope (`c0_distinct` 13..35,
`abs_steer_median` 7..23), **not** `ai_ctrl_window.py`'s TOLERANCE, so its PASS/FAIL words
are not the official (b) verdict. The official verdict is the table above.

| car | arm | `c0D` | `c1D` | `stD` | `|st|Med` |
|---|---|---:|---:|---:|---:|
| 1 | own speed, own err (= arm B as run) | 7 | 80 | 86 | 6.0 |
| 1 | **ORIG speed**, own err | **12** | **50** | **61** | **12.0** |
| 2 | own speed, own err (= arm B as run) | 33 | 71 | 103 | 21.0 |
| 2 | ORIG speed, own err | 49 | 73 | 121 | 38.0 |
| 2 | own speed, **ORIG err** | **18** | **43** | **62** | **8.5** |
| 3 | own speed, own err (= arm B as run) | 21 | 43 | 63 | 9.0 |

**The two remaining failures have different carriers.**
**Car 1: speed.** Substituting the original's speed closes `c1_distinct` (80 → 50, in band)
and `steer_distinct` (86 → 61, in band) and brings `c0_distinct` from 7 to **12 against a
floor of 13**. **Car 2: error, not speed.** Substituting the original's speed makes car 2
**worse** on every statistic; substituting the original's `err` is what closes it.

### 3.2 Matched position (`ai_posmatch.py`, R = 0.12)

Part A (U-9183's position-matched curvature) **passes on all three cars** in arm B, with
coverage up from 105 / 68 / 79 matched calls to **143 / 143 / 102** — arm B tracks the
original's line better.

U-9185's two candidates, median `|Δ|` in degrees at matched position:

| car | quantity | arm A (`r1`) | arm B (`b1`) |
|---|---|---:|---:|
| 1 | `d_err` | 0.945 | **0.452** |
| 1 | `d_target_dir` | 0.2815 | 0.2322 |
| 1 | **`d_body_heading`** | **0.9796** | **0.6406** |
| 2 | `d_err` | 1.296 | **1.841** |
| 2 | `d_target_dir` | 0.4814 | 0.2197 |
| 2 | **`d_body_heading`** | **0.9085** | **1.6059** |
| 3 | `d_err` | 0.466 | 0.531 |
| 3 | `d_target_dir` | 0.4477 | 0.1735 |
| 3 | **`d_body_heading`** | **0.6506** | **0.4549** |

**The body heading remains the larger median share on all three cars in both arms**, and on
car 2 — the car whose remaining (b) failure the counterfactual attributes to `err` — it
**grows** 0.909 → 1.606 when the seed is removed. U-9185's candidate ranking survives the
knob. Which of its two port-side candidates carries it (the physics' `io.yaw`,
`TrackRenderer.cpp:3327`, versus the `(cos, sin)` reconstruction at `:3729`) is **still
`[UNCERTAIN]`** — this step did not separate them, and the third candidate it was meant to
rule out (the seed) is now ruled out as the *sole* cause but not as a contributor.

Matched-position speed `|Δ|` median **worsens** in arm B — car 1 0.96 → **266.4**, car 2
255.1 → **737.2**, car 3 215.5 → **602.0** — consistent with (e): at the same place on the
track the port is now far too slow, because it no longer launches.

### 3.3 Cross-side at matched call index (window idx 0..219, median call index 109.5 both sides)

Median `|orig − port|` over the window:

| field | car | arm A | arm B | B/A |
|---|---|---:|---:|---:|
| `c1` (the steer byte) | 1 | 16.5 | **10.0** | 0.61 |
| `c1` | 2 | 51.5 | **10.5** | 0.20 |
| `c1` | 3 | 42.0 | **13.0** | 0.31 |
| `rec_9e4` | 1 | 317.3 | **1184** | 3.73 |
| `rec_9e4` | 2 | 627.6 | 408.6 | 0.65 |
| `rec_9e4` | 3 | 593.2 | 713.5 | 1.20 |
| `c0`, `c4`, `c5`, `ai_mode`, `ai_override`, `ai_spline_idx`, `diff_a360`, `substate`, `tgt_7ffc`, `march_n`, `c3`, `c7`, `flag_a368` | 1,2,3 | **0** | **0** | — |

The steer byte moves **toward** the original on all three cars; the speed moves **away** on
cars 1 and 3. Same split as everywhere else in this result.

## Collateral review — run AFTER the verdicts, declared as such

`re/tools/statediff/collateral.py`, scope file `scope.txt` committed with the PREREG.

**Noise floor, stated plainly: it is 0.** The repeats are bit-identical on every column, so
the measured p99 floor is exactly 0 for all 35 fields, and every nonzero difference is
"above floor" by construction. What carries information here is the **magnitude ranking and
the first divergent frame**, not the word "divergent".

**Paired, arm A vs arm B, per car, anchored at `c4 ≥ 1`** (frame 0 = the scored window's
call 0), floors from the A and B repeats:

| car | fields diverging | **OUTSIDE-SCOPE rows that moved** | first frame |
|---|---:|---|---:|
| 1 | 20 of 35 | `seq` (med\|A−B\| 35), `ai_spline_idx` (med 0) | 1290, 2661 |
| 2 | 20 of 35 | `ai_spline_idx` (med\|A−B\| 1), `c7` (med 0) | 557, 1231 |
| 3 | 20 of 35 | `ai_spline_idx` (med\|A−B\| 1), `seq` (med\|A−B\| 35) | **160**, 1289 |

**Within the noise floor on every aligned frame, i.e. did not move at all:** `ai_mode`,
`ai_override`, `ai_type`, `block`, `c3`, `clk_0ff4`, `diff_a360`, `flag_a368`, `frame`,
`spline`, `step_1008`, `substate`, `tgt_7ffc`, `v` on all three cars (plus `c7` on cars 1
and 3, `seq` on car 2).

**Reading the three outside-scope rows:**
- **`ai_spline_idx` on car 3 at frame 160 is the only outside-scope field that moves inside
  the scored window** (0..219). It goes `0 → 1`: the AI selects a different spline. Real,
  and reported.
- `ai_spline_idx` on cars 1 / 2 moves at frames 2661 / 557, **outside** the scored window.
- `seq` is the dump's own row counter, not game state; a 35-row offset is the two runs
  having different lengths.

**Banded, cross-side, port arm B vs the original `o1`, `--speed rec_9e4`:** 17 / 17 / 18 of
32 paired fields exceed their floor in some band. **Outside-scope rows:** `ai_mode` (car 1 —
the known modes-3/7 residue D3-R1), `ai_override` (car 2), `ai_spline_idx` (cars 2 and 3).
**Caveat, and it is why this table is not leaned on:** the tool flags `!!` on most bands
because the median **frame** of each band differs between sides (e.g. car 1's 2000-2600 band
is orig frame 44 vs port frame 1428) — the bands compare different moments. The
moment-matched statements are §3.2 (matched position) and §3.3 (matched call index).

## D2 WATCH (D-11071)

**The knob touches none of D-11071's five named triggers.** Its entire effect is
`VehiclePhysicsRun.cpp:703-707`, writing `+0xbf8` and `+0xbf4`. `+0x4a4`, the contact
collector / `FUN_00538c80`, grip-clamp #6, the substep/chunk loop and `ReassertContacts` are
not read or written by it, and **no D2 code was changed in this session.**

**Player slot 0 — the physics is bit-identical between arms.** `MASHED_PLAYERTRACE` on both
witness runs, 4198 and 4226 lines: over the entire common prefix, **exactly one field ever
differs**, and it is **not** a physics channel. `pos`, `yaw`, `sp`, `vel`, `dt`, `a144`,
`av`, `c9ec`, `gnd`, `bodyfwd`, `n9c8`, `r0up`, `r0at`, `b14`, `b1c`, `v9e4`, `gate`, `lap`,
`prog`, `rt` are **identical on every line**. **No D2 REOPEN CANDIDATE on player physics.**
(`player speed on Training` is not exercised by this recipe at all: the player is stationary,
`sp = 0.6380385160446167` on every line of both arms.)

**One row IS filed, and it is a confound, not a physics move:**

| trigger | finding | evidence |
|---|---|---|
| none of the five — **filed as a measurement confound** | `race_[0].alive` differs between arms at trace line **223**, `rt = 1.8667 s`: **arm A 0 (player eliminated), arm B 1 (player alive)**. The writer is `race_[victim].alive = false` at `TrackRenderer.cpp:4664`, reached through `RE::SegmentCheck` + `race_cam_.EliminationCheck`, which read the **AI cars'** positions — so the seed changes the elimination outcome. This matters because `round_mode_` is **true** (`MASHED_ROUND=1`, `TrackRenderer.cpp:4026`), `g_aib.alive[0]` is cleared at `:3722`, and the AI tick loop at `AiStandalone.cpp:1708` runs **`for (int v = 0; v < 4; ++v)`** gated on `car_alive(v)` — so `VehicleStep(0)` runs in arm B and does not in arm A. `rt = 1.8667 s` is **inside the scored window** (the window spans `clk_0ff4` 50 → 11000 ms). **Therefore the (b)/(e) differences above are the seed PLUS this elimination divergence, and this step does NOT separate the two shares. `[UNCERTAIN]`.** | `aw_player_trace.log` vs `bw_player_trace.log`, line 223 |

## What this does and does not establish

- **Established:** the seed is live, observed in flight, and is the sole effect of the knob;
  it is **worth ~half** of the port's mid-window over-speed (+41/+40/+34 % → +31/+21/+15 %);
  it is **the entire reason (e) passes** (the original's own launch is 1425.7 / 2052.5 /
  2055.0 and arm B produces 200.5); and it is **the dominant carrier of (b)'s
  `c1_median` / `abs_steer_median` failures**, which collapse to band on all three cars when
  it is removed.
- **Not established** `[UNCERTAIN]`: the split between the seed's direct effect and the
  player-elimination divergence it triggers at 1.87 s; which of U-9185's two port-side
  heading candidates carries the residual; and what carries the **+15..31 %** sustained
  over-speed that survives the knob.

## The decision the user now faces

Three options, none of which this session took:

1. **Keep the seed (status quo).** (e) MET 3/3, (b) 13 failing bands. D3's AI third stays
   blocked on (b).
2. **Remove the seed.** (b) 6 failing bands with car 3 passing outright, (e) fails 6/6 by
   46-90 %. **This trades a MET criterion for a still-NOT-MET one** and removes the port's
   only reproduction of a launch the original demonstrably has.
3. **Keep the seed and attack the sustained over-speed instead.** The +15..31 % that
   survives the knob is a separate carrier, and §3.1 says closing it would move car 1's
   `c1_distinct` 80 → 50 and `steer_distinct` 86 → 61 into band. This is the only option
   that does not trade one criterion against the other, and it is **not costed** here.

**Option 2 is what U-9185's remedy list asked to measure, and the measurement says the
trade-off is real and adverse on (e). Which to take is the user's call.**

## Provenance

```
# six scored runs (only MASHED_NO_START_BOOST differs between arms):
py -3.12 re/tools/sa_capture.py verify/d3_noboost_20261003/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    [MASHED_NO_START_BOOST=1] MASHED_WIN_POS=primary-bl \
    MASHED_TITLE="D3 noboost <tag>" \
    MASHED_AI_STEPDUMP=verify/d3_noboost_20261003/<tag>.csv
  a1 a2 a3   arm A, default (boost ON)
  b1 b2 b3   arm B, + MASHED_NO_START_BOOST=1

# two witness runs (not scored):
py -3.12 re/tools/sa_boostwatch.py verify/d3_noboost_20261003/<aw|bw> 40 --hz 100 \
    --map mashedmod/build/mashed_re.map <same env> MASHED_PLAYERTRACE=1 \
    MASHED_AI_STEPDUMP=verify/d3_noboost_20261003/<aw|bw>step.csv

# original reference, unchanged:
verify/d3_modes37_20261002/o1.msd.aistep.csv
```

`mashed_re.exe` SHA-256 **`a6b07bd10ec86a78870108e7619afd30d708efb74c3664477acf0556d88e99e0`**,
built 2026-10-02 21:33:35, **unchanged across every run in this directory**. **No build was
run** — no `mashedmod/src` file is newer than the exe and `git status --porcelain
mashedmod/src` is empty. Map timestamp `6ac04d5f` = the same build.

Every `mashed_re.exe` was spawned and killed **by PID** by `sa_capture.py` /
`sa_boostwatch.py`. No blanket kill by name. No worktree. `original/MASHED.exe` untouched;
no `unlock_*` patch applied. `log/rules_oracle_rule3.json` carries an uncommitted
modification that is **not this session's** and was **never staged**.

The `.csv` payloads are large and untracked by this repo's convention for AI step dumps; the
distilled evidence is this file plus the committed `*.json` and `coll_*.txt`.
