# STEP 2 PRE-REGISTRATION — the AI over-speed with the boost ON. **UNRUN.**

Written and committed **before** any number in §2.1 onward is computed. Branch
`race/first-frame-parity`. The user's decision (ROADMAP §D3, `e0b35ff9`) keeps the start
boost, so **every arm here has the boost ON** and `MASHED_NO_START_BOOST` is never set.

## What is already established and is NOT re-derived here

- The window: car `v`'s calls `[i0, i0+220)`, `i0` = the first call with `c4 != 0`
  (`ai_ctrl_window.py:25`). Median call index **109.5** on both sides.
- Window speed `rec_9e4` medians, n = 220 per car: ORIGINAL **2419.5 / 2397.0 / 2617.6**,
  port arm A **3421.7 / 3346.2 / 3495.9** = **+41.4 / +39.6 / +33.6 %**
  (`verify/d3_noboost_20261003/RESULT.md`).
- `launch` (speed gained over window calls 0..11) matches the original to **0.05 %**:
  1426.4 / 2053.0 / 2055.2 against 1425.7 / 2052.5 / 2055.0. **So the first 11 calls are
  not the gap; the gap is what happens after them.**
- The elimination confound's share of the arm A vs arm B delta is **ZERO** (STEP 1,
  `RESULT_STEP1.md`), so arm A's numbers are single-cause with respect to it.

## The chain this step decomposes, with its RVAs

The commanded bytes come out of `FUN_00416250`'s accel/brake tail,
`0x004167d5..0x0041688b`, transcribed at `AiStandalone.cpp:913-920` with every constant
carrying its `_DAT_` address. In evaluation order:

| rule | port line | RVA | predicate | constants |
|---|---|---|---|---|
| default | `:914` | `0x004167d5` | — | `c4 = 0xff`, `c5 = 0` |
| **R_B0C** | `:917` | `0x004167eb..` | `rate0 - prev0 > 15.0 && rate0 > 10.0` | `_DAT_005cc9b0` = 15, `_DAT_005cc55c` = 10; `prev0` is the persistent `0x008032e0 + v*0x14`, rewritten every call at `0x004167eb` |
| **R_XSPD** | `:918` | `0x00416818..0x0041683a` | `X > 20.0 && speed > 2000.0` | `_DAT_005ccd6c` = 20, `_DAT_005cd0b8` = 2000 |
| **R_ERRLO** | `:919` | — | `30.0 < err < 180.0` | `_DAT_005cc72c` = 30 |
| **R_ERRHI** | `:920` | — | `180.0 < err < 330.0` | `_DAT_005cd0e0` = 330 |

Inputs, all four logged **per call on both sides** by the existing step dumps:

| term | symbol | where it is logged | what produces it |
|---|---|---|---|
| **T0** | `err`, `X` | `hist_d8` / `hist_dc` = `0x008032d8` / `0x008032dc + v*0x14`, written `0x004165cc` / `0x004165f7` (LO) and `0x004166db` / `0x0041670c` (HI) | the AI's steering error, i.e. body heading + target direction (U-9185) |
| **T1** | `speed` | `rec_9e4` = `+0x9e4` | the physics integrator |
| **T2** | `rate0` | `rec_b0c` = `+0xb0c` | A6a's channel, law at `0x00470724` / `0x0047072c`, `max(1500.0 - b0c, 500.0)` at `0x004676de` / `0x00467702` |
| **T3** | rule occupancy | derived from T0..T2 | — |
| **T4** | `c4`, `c5` | logged | — |
| **T5** | `speed` at call k+1 | `rec_9e4` | closes the loop |

`mode` is pinned to 0 in the shipping port (`AiStandalone.cpp:844`), so the mode-5/7/9/2
tails at `:924-950` are not reachable and are **out of scope**; this is stated now so that a
later "but mode 7 sets `c4 = 0x40`" cannot be used after the fact. The 2026-10-02 modes-3/7
arms already refuted the mode route for (b).

## 2.0 — THE KNOWN-ANSWER CHECK, and it runs on the ORIGINAL

`re/tools/ai_band_sim.py:121-127` already reconstructs `c4` / `c5` from exactly these four
logged inputs. **KA-2.0: the reconstruction must reproduce the ORIGINAL's own logged `c4`
and `c5` on ≥ 95 % of its 220 window calls, each of cars 1..3.** The same number is reported
for the port. **If it fails on the ORIGINAL, the model is not a model of the original, every
rule-occupancy claim below is void, and I STOP and report that** instead of reporting
occupancies.

`ai_band_sim.py`'s own validation gate (≥ 95 % on all four bytes) is **not lowered**.

## 2.1 — Rule occupancy at matched call index

Per side, per car, over the 220-call window: the count of calls on which each of
R_B0C / R_XSPD / R_ERRLO / R_ERRHI fires, plus "none". Reported beside the logged
`c4 == 255` and `c5 == 255` fractions.

**The c4 command comparison the kickoff asks for is this table's first row**: the original's
car 1 takes `c4 ∈ {0, 64, 255}` with 255 on **63.6 %**, the port's arm A takes `{0, 255}`
with 255 on **97.7 %** (and `c5 == 255` on **2.3 %** against the original's **21.4 %**).
Those four numbers are already committed in `verify/d3_noboost_20261003/RESULT.md`; what is
new here is **which rule's occupancy carries them**, which is not known yet.

## 2.2 — THE DECISION RULE: the first diverging term

Terms are tested **in the dependency order T0 → T1 → T2 → T3 → T4**, at matched call index
(window index 0..219, median call index 109.5 on both sides).

For each term: `D = median |orig − port|` over the window, and
`Dn = D / IQR_orig`, where `IQR_orig` is the ORIGINAL's own in-window inter-quartile range
of that term. **The first term in the order with `Dn > 0.5` is named the carrier**, provided
every earlier term has `Dn <= 0.5`. **0.5 is registered now, before any of these numbers
exists.** For the boolean terms (T3 occupancy) the statistic is the absolute difference in
occupancy fraction and the threshold is **0.10**.

If **no** term crosses its threshold, that is reported as a null and the step does not
promote a carrier.
If **T0 crosses first**, the carrier is the steering error — i.e. U-9185's body-heading
residual — and the over-speed is **downstream of the AI's own error**, not of a force term.
If **T1 or T2 crosses first with T0 inside**, the carrier is physics, and §2.5 runs.

## 2.3 — matched position

The whole of §2.1 and §2.2 is repeated on position-matched calls only, via
`re/tools/ai_posmatch.py` at **R = 0.12** world units — the original's own median in-window
per-call displacement (U-9183, 0.1120 / 0.1112 / 0.1213), unchanged from the value that
closed U-9183. A conclusion is only reported as the step's answer if **matched index and
matched position agree on which term crosses first.** If they disagree, both are reported and
no carrier is named (memory `band-on-speed-compares-different-moments`).

## 2.4 — The counterfactual that separates command from physics

`ai_band_sim.py` already supports substituting the original's per-call `speed`, `err`,
`mode` and `curv` into the port's reconstruction. **One addition is declared here**, in the
same form and with the same status as the already-declared `curvseq`: **`rateseq`**, the
per-call substitution of the original's `rec_b0c`. It changes **no decision rule** and adds
**no free parameter**.

Arms, all on the port's arm A inputs:

| arm | substitution | what it tests |
|---|---|---|
| (i) | none | KA: must reproduce the port's own logged `c4`/`c5` ≥ 95 % |
| (ii) | ORIGINAL `speed` | is the command divergence downstream of the over-speed? |
| (iii) | ORIGINAL `rec_b0c` | is it downstream of A6a's `+0xb0c` channel? |
| (iv) | ORIGINAL `err` / `X` | is it downstream of the steering error? |

**Registered reading:** the arm that brings the port's `c4 == 255` fraction **within 0.10**
of the original's is the one whose input carries the command divergence. If **no** arm does,
that is a **null** and is reported as a null, with the three near-misses printed, rather than
reinterpreted (memory `three-nulls-mean-run-an-experiment`).

## 2.5 — The force-term budget (CONDITIONAL on §2.2 naming T1 or T2)

Only then. The original side needs **no new code**: `--statediff-out` already dumps every
dword of one car's `0xd04` record per frame (`--statediff-car 1`), so `+0xb14`, `+0xb18`,
`+0xb1c`, `+0x9e4`, `+0xb0c` and `+0xbf4` are available. The port side needs a matching
per-frame AI-slot record dump; that will be added as a **default-off diagnostic**
(`MASHED_AI_RECDUMP`), committed before use, changing no behaviour.

Budget, per call, from the end of the boost window (`+0xbf4` reaches 0, ≈ 6 frames after the
seed — the observed ladder 1100 / 900 / 700 / 500 / 300 / 100) through the scored window:

| term | RVA | what is read |
|---|---|---|
| drive force | `+0xb14` / `+0xb1c`, written by A4 `0x00470670` (`0x004706a9..bb` zero them) | Δ per frame |
| A5 drag | `0x0046ddb0` | its contribution to `+0xb1c` |
| A6a clamps incl. grip-clamp #6 | `0x00467650`, called once at `0x0047094c` | the clamped vs unclamped value |
| contact solver | `0x0046f6c0` | its contribution (attempt 20 changed the contact collector and its AI effect was measured non-zero **after call 455**, i.e. **235 calls past the window** — so it is expected at 0 here, and a non-zero would contradict `verify/d3_rebase_20261002`) |
| gear / rev | `+0xb0c` law `0x00470724` / `0x0047072c` | — |

Same decision rule as §2.2: the first term diverging at matched call index while upstream
agrees.

## Gates

- **G2-KA** — §2.0, on the ORIGINAL. Failure = STOP.
- **G2-DET** — `o_e1` vs `o_e2` and `ea1`/`ea2`/`ea3` must agree to every printed digit on
  both scorers. Any spread is reported, not averaged.
- **G2-AGREE** — §2.3's matched-index and matched-position answers must agree before a
  carrier is named.
- **G2-NOFIT** — **no constant is fitted and no free parameter is introduced**, in any arm,
  at any point. A counterfactual substitutes a *measured* sequence from the other side or it
  does not run.
- **G2-NULL** — a null is reported as a null.

## D2 WATCH (D-11071)

T1 (`+0x9e4`) and T2 (`+0xb0c`) are **shared vehicle-physics outputs that also drive the
player**. If §2.2 names either as the carrier, that is a **D2 REOPEN CANDIDATE** and is
reported as one, with D2's solo arm re-scored (3 runs, unchanged `d81a8df6` bounds) before
any fix is proposed. `0x0046f6c0` and the contact collector are two of D-11071's five named
triggers; any non-zero contribution from them inside the window is reported as a trigger hit.

## Out of scope

- The start boost (kept, by decision).
- Criterion (b)'s steering bands as such — §2.2's T0 arm bears on them but this step does
  not re-litigate U-9185's two heading candidates.
- Any fix. STEP 3 fixes; this step names.
