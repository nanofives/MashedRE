# RESULT — D-11072 leg C liveness + leg A scale measurement

Pre-registration: [`PREREG_LEGC_LEGA.md`](PREREG_LEGC_LEGA.md), committed **UNRUN** at `69e207b9`.
Fidelity decision (Mariano, 2026-10-06): **bridge acceptable, knob-gated, no C-level** — recorded
in the pre-registration §0 and `re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md` §1.

**No C-level moved.** `0x0040e470`, `0x0040e480` and every race-position RVA stay where they
were — a seeded static pointer and a measurement are not behavioural diffs, registered in
advance. **`original/` untouched** (anchor `BDCAE093…` re-verified before and after). Source
footprint: **one default-OFF seed + seven appended columns in one file**,
`D3d9Render/TrackRenderer.cpp`; new read-only tool `re/tools/racepct_scale.py`. Instrument
build SHA-256 `DAC3A0E0FE83AE044FCAF59C59886E37E8EC431F0136482C680663980881731C`; baseline (no
instrument) `E515B3B732873D5AE07F3EDDE5F479AA8AAD202B3704E758FDBA3E2EA27C1E68`. **No bridge
code written, no race-position global seeded.**

## 1. Leg C — the slot-state table seeds, reads live, and is behaviourally INERT (all as pre-registered)

Six runs, Training, 3 unseeded (`U1..U3`) + 3 seeded (`MASHED_SLOTSTATE_SEED=1`, `S1..S3`).

| gate | threshold | measured | verdict |
|---|---|---|---|
| `G-C-SEED` | `ss_base` == `0x005f2728` 100 % seeded, == 0 100 % unseeded | **18328/18328** seeded, **18272/18272** unseeded | **PASS** |
| `G-C-EXPR` | seeded `ss_v` == 2 ≥ 95 % per car; unseeded `ss_v` == -1 100 % | **4338/4338, 5186/5186, 8804/8804** (=1.0000) seeded; **100 %** == -1 unseeded | **PASS** |
| `G-C-TABLE` (**control that can fail**) | unseeded `ss_raw` == 0 100 %; seeded `ss_raw` == 2 ≥ 95 % per car | seeded **1.0000** on all 3 cars; unseeded **0** on all | **PASS** |
| `G-C-NOCRASH` | 3/3 seeded runs reach 60 s, non-empty CSV | 3/3 (6080–6128 rows) | **PASS** |

`G-C-TABLE` is the point of the design and it refutes the null. `ss_raw` reads the slot
**absolutely** at `0x005f2728 + 0x34 + v*4` with no pointer deref, so "0 without the seed, 2
with it, on every one of 18k+ rows" cannot be an artefact of the `ss_v` guard — the null
hypothesis "the seed changes nothing" predicts 0 in both arms and is false. `ss_v` (the literal
`FUN_0040e470(v)` expression) reads **2** on every seeded row for all three cars, i.e. the
exact value the pokes `CarSlotStateSet(v, 2)` write (`AiStandalone.cpp:1699-1701`), and the
guarded **-1** on every unseeded row (the deref at base 0 would AV, `AiStandalone.cpp:1385-1393`).

| no-regression gate | threshold | measured | verdict |
|---|---|---|---|
| `G-C-INERT` | criterion (e) six digits identical seeded↔unseeded; (b) identical bands | (e) **digit-identical** across all 6 runs (`launch` 1426.4/2053.0/2055.2, `ft_median_m0` 2550.6/2053.0/2278.2); (b) **5 bands** (car 2 only), identical digits every run | **PASS** |
| `G-INERT` (instrument) | 0 differing cells, pre-existing columns vs baseline `B1` | **0 of 329,550** (65 cols × 5070 shared `(frame,seq,v)` keys) | **PASS** |
| `G-DET` | (e) + bands identical across 3 repeats each arm | identical, both arms | **PASS** |
| `G-BANDS-UNEDITED` | both scorers' `git diff`/`status` empty | empty before first and after last run | **PASS** |

**`G-C-INERT` is the pre-registered prediction confirmed, not a disappointment.** The seed makes
the table writable and readable and changes **no** behaviour, because the standalone's only
consumer of the concept — `Ai::Host::veh_type` — is the synthesized constant `v == 0 ? 0 : 2`
(`TrackRenderer.cpp:93`), not a table read, and `FUN_0040e470`'s 29 real callers are
original-side RVAs the standalone never enters (`Compat/StandaloneRvaThunks.h:7`). So **leg C's
seed is safe to ship default-ON** (bit-faithful: it restores a static `.data` initializer the
binary itself carries, `decomp_pc 0x005f2770 --datarefs` = `value 005f2728, WRITES: (none)`),
but it buys nothing on its own. The behavioural step is **C2** — re-point `aib_veh_type` at the
table so a car `CarSlotStateSet` never marks reads 0 (death-aware) — which is **not written and
not run here** and is where leg C's payoff actually is.

## 2. Leg A — the port metric is NOT a faithful stand-in for the original's spline race_pct

Original `race_pct` from the already-committed `verify/d3_elim_20261003/o_e{1,2}.msd.alive.csv`
(`pct0..pct3` = `0x008a96ec + v*0x30c`), no new original run. Port metric from `S1` (`racepct`
= `fmod(progress,n)/n*100`, `TrackRenderer.cpp:4616-4619`).

**M1 — the original is a clean 0..100 per-lap monotone spline projection** (what the AI gate
consumers are tuned to): `G-A-RANGE` [0.004, 100.000] PASS; `G-A-WRAP` every negative step is a
>50-magnitude 100→0 wrap (2/2) PASS; `G-A-MONO` 437/437 = 1.0000 per car PASS.

**The pre-registered verdict (`NOT on a common scale by range-mapping`) stands. Its registered
rationale was wrong and the real one is stronger — recorded per the
`pre-register-the-decision-not-the-diagnosis` discipline:**

- **The flat-tread mechanism I registered is degenerate on this data.** The rule was "common iff
  `port flat_frac < 5 × orig flat_frac`". The original's `flat_frac` is **0.0000 exactly**, so
  `5 × 0 = 0` is an impossible threshold and the rule returns NOT-COMMON for any positive port
  value regardless of the truth. That verdict is mechanically correct but carries no
  information; I am not relying on it.
- **My staircase prediction is refuted.** I predicted port `flat_frac ≥ 0.50` (a gate-ordinal
  staircase). Measured: **0.0684 / 0.0288 / 0.0000** on cars 1/2/3 — the port metric is *not* a
  staircase, because on Training the gates are close enough that the 8-unit `frac` blend is
  almost always active.
- **The real obstruction is non-monotonicity, measured as collateral (declared as such, run
  after the verdict):** within a lap, moving samples only, **small backward steps** —

  | | car1 | car2 | car3 |
  |---|---:|---:|---:|
  | original `race_pct` | **0** | **0** | **0** |
  | port `racepct` | **484** | **681** | **1287** |

  The port metric runs backward on ~40 % of its forward-moving steps (car 1: 484 reversals vs
  727 advances), while the original never does. Cause: `frac = 1 - d/8` keys on euclidean
  distance to the gate **center**, and the racing line passes off-center, so `d` falls then
  rises between crossings and `progress` oscillates. The original's spline projection only
  advances along the arc. A metric that oscillates backward cannot be range-mapped into the
  original's `0x008a96ec` consumers (ordering / rank / catch-up / the U-9186 branches) without
  them seeing a car's race position jitter.

**So leg A's deliverable is the FINDING, not a candidate map** (as the pre-registration's second
branch required): the bridge needs an **arc-length-proportional, monotone** port progress metric
(project the car onto the AI spline the port already drives, rather than blending distance to
gate centers) **before** leg A can write `0x008a96ec` or leg B can start. No scale map is
registered and `0x008a96ec`/`0x008a96e8`/`0x008989b0` are **not** seeded.

## 3. What this session establishes

- **Leg C: DONE and INERT by design.** The slot-state table seeds bit-faithfully, the exact
  `FUN_0040e470` expression reads the poked value, and it changes no (e)/(b) behaviour — the
  payoff is the separate, un-started step C2.
- **Leg A: a real obstruction found cheaply, before leg B.** The port's gate-ordinal progress is
  non-monotone (484/681/1287 reversals vs 0) and so is not range-mappable into the original's
  spline `race_pct`. This is exactly the go/no-go the inert-first discipline exists to surface:
  **do not start leg B**; the prerequisite is now a monotone port progress metric, not a
  reference-distance port.
- **No C-level moved, nothing shipped default-ON, `original/` untouched.**
