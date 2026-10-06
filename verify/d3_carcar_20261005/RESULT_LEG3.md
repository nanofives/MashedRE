# RESULT — LEG 3, the car<->car pair loop: 8 of 8 gates PASS, and the contact moves (b) from 13 failing bands to 5

Pre-registration: [`PREREG_CARCAR.md`](PREREG_CARCAR.md) §4, committed **UNRUN** at `3404612d`.
Leg 1: [`RESULT_LEG1.md`](RESULT_LEG1.md); leg 2: [`RESULT_LEG2.md`](RESULT_LEG2.md).

**No C-level moved** — `0x00469df0` is still C2, and a call site is not a behavioural diff
(registered in advance). **`original/` untouched.** Source footprint: **73 insertions / 0
deletions in one file**, `Vehicle/VehiclePhysicsRun.cpp`. Built exe SHA-256
`9C09212FD227F6906C546DE9117EF57E4464A4F8C6F0AC5E639B6A5DD853CFDC`. Default-ON with the
revert `MASHED_CARCAR_CONTACT=0`; one `getenv` read site.

## 1. The change

The original's car<->car inner loop (`FUN_004709a0`, `j = i+1..15`, calling `0x00469df0` at
`0x00470bcd`) is reproduced inside `VehiclePhysics_StepCar`, immediately before the `break;`
that ends the substep retry — the single insertion point leg 1 identified. The call is
`VehicleCarCarContact(rec(j), r, pass)` — **the OTHER car first** (`in_EAX` = other,
`param_1`/ECX = self), per the ABI read from `0x00470bc5`/`0x00470bcb`. The five gates
(`j >= participantCount`, active flag, `(radA+radB)*0.75` proximity on the `[+0x9a8]`
centroids, game-mode ∈ {6,7,10,0xb}) are reproduced in order with their RVAs cited inline.
`FUN_00467300` (the 1-player collision-win event) is omitted and its RVA cited — it is gated
`DAT_007f0fd0==1` and inert on any race recipe.

## 2. The eight gates

| gate | threshold | measured | verdict |
|---|---|---|---|
| `G-CALLED` | >= 1 entry **and** >= 1 non-zero return | **358 entries, 24 non-zero returns** over a 4-car race (witness log `carcar_called.log`) | **PASS** |
| `G-PAIR` | `j > i` on 100 % of calls, `recJ != recI`, both non-null | **358 of 358**; `(i,j)` histogram `{(0,2):63, (0,3):34, (1,2):149, (1,3):57, (2,3):55}` | **PASS** |
| `G-NOOPP` (control) | **exactly 0** entries | **0** (no log file written) under `MASHED_MEASURE_NOOPP=1` | **PASS** |
| `G-NOREG-E` | `launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0` 2550.6 / 2053.0 / 2278.2, every digit | **identical** on 3 of 3 ON runs, `n`=100/23/39, `flag=[0]`, `regime0=1` | **PASS** |
| `G-NOREG-B` | **<= 13** failing bands (baseline 13, 5/4/4) | **5** (car 1 **0**, car 2 **5**, car 3 **0**) — fewer, not more | **PASS** |
| `G-BANDS-UNEDITED` | both empty | `git diff --stat` and `git status --porcelain` on both scorers empty, before first run and after last | **PASS** |
| `G-KNOBOFF` | committed build's digits | `MASHED_CARCAR_CONTACT=0` reproduces every (e) digit and the **13** bands (5/4/4) and the full-window `ft_median` 3420.3/3334.4/3496.9 | **PASS** |
| `G-DET` | 3 ON repeats identical | identical on the gated (e) stats **and** the per-car band counts `{1:0, 2:5, 3:0}` on all three | **PASS** |

**`G-NOOPP` is the control that could fail, and the mechanism is worth stating** because it
is not quite the one the pre-registration imagined. The pre-registration reasoned "no
opponent stepped → no pair in range". What actually happens: under `MASHED_MEASURE_NOOPP=1`
the AI cars are never physics-stepped, so `SyncContactRingMatrix` (leg 2) never runs for
them and their `[+0x9a8]` ring slot stays at the memset zero `(0,0,0)` — the proximity test
against a self car that has driven away from the origin therefore fails on every pair, for
every frame. Zero entries, for the reason the control was built to confirm: the counter is
wired to real geometry, not to the mere existence of the loop.

## 3. The surprise: the contact moves (b), deterministically, and it is NOT a stuck-car artifact

`G-NOREG-B` only required "no worse than 13". The ON arm is **5**, and all three ON runs
agree to the band. The movement is entirely in the steering distribution, and it brings two
cars **into** the original's envelope rather than past it:

| car | `abs_steer_median` OFF | ON | original envelope `[0,23]` |
|---|---:|---:|---|
| 1 | 52.5 | **7.0** | OFF out, **ON in** |
| 2 | 57.5 | 44.5 | out both |
| 3 | 49.0 | **11.5** | OFF out, **ON in** |

**It is not the degenerate "car got stuck against another car" pass.** Window speed on the
ON arm: car 1 median **3057.7** (vs 3421.7 OFF), car 3 **3254.3** (vs 3495.9), with
near-stopped (< 200) frames **2 / 220** and **1 / 220** — identical to the OFF arm. The cars
are racing at ~89–93 % of their OFF speed while steering far less, i.e. holding a cleaner
line, which is exactly the direction the original differs from the port. The full-window
`ft_median` also drops (car 1 3420 → 2847) and `wmax` rises (3815 → 3948), consistent with
cars being jostled by contact — real behavioural work, not a freeze.

### Why this is reported as an OBSERVATION and NOT a (b) result

Registered in advance (`PREREG_CARCAR.md` §4, §5): leg 3 does not claim to move (b), and
"do not assume any of this closes (b)." It holds here for reasons that are mine to flag, not
to paper over:

1. **The mechanism is not understood.** The contact applies a velocity + angular impulse;
   why that reduces the AI's *steering command* (which reads the car's heading) by ~7x on
   two cars and not the third is not established. A plausible story — the port over-steers
   because its cars drift off-line with nothing to bump them straight — is a story, not a
   measurement.
2. **The summary median landing in the envelope is not the distribution matching it.** The
   (b) gate checks medians and distinct-counts; it does not check that the port's per-frame
   steering *series* matches the original's. The honest test is a cross-side comparison
   against `verify/d3_elim_20261003/o_t*.msd.aistep.csv`, which this leg did not run.
3. **The 2026-10-02 counterfactual matrix found no arm passing (b)** — but it tested AI
   *command* interventions (mode, curvature, speed, error), never the car<->car *physics*
   contact, which was not wired then. So this is a new lever, not a contradiction; and a new
   lever that moves (b) this far deserves suspicion before celebration.
4. **Car 2 got slightly worse** (4 → 5 bands), so even taken at face value this is not a
   clean win across the field.

Filed as **U-9196** with the cross-side validation as step 1. Whether to keep the knob
default-ON given a possible (b) improvement is a user decision, deferred to that row.

## 4. What leg 3 establishes

The car<->car contact `0x00469df0` now has a faithful call site — the sole original call
site, with its ABI, five gates and retry semantics — live in the default `mashed_re.exe` and
proven to fire (358 entries, 24 real overlaps) on real geometry (0 under `NOOPP`). It is
byte-reversible via `MASHED_CARCAR_CONTACT=0`, costs the two gated (e) statistics nothing on
3 of 3 cars, and regresses no band. The function stays **C2**: it has a call site, not a
behavioural diff against the original. The downstream (b) movement is real, deterministic and
non-degenerate, and is handed to U-9196 for the cross-side check before any (b) claim.
