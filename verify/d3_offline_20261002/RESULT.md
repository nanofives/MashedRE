# SESSION RESULT — D3 OFFLINE: U-9183 and U-9182 (2026-10-02)

Branch `race/first-frame-parity`, nothing pushed. **No game run, no build, no edit under
`mashedmod/src`.** The only code change is one read-only analysis tool,
`re/tools/ai_posmatch.py`. The master Ghidra project was never opened.
`log/rules_oracle_rule3.json` was neither staged nor committed; its pre-existing
modification is left exactly as found.

## Verdict

> **U-9183 CLOSES and U-9182 is ANSWERED. Every pre-registered gate PASSED. The
> recommended next target CHANGES: it is not `FUN_00414c30`, and it is not a port at all
> — it is a one-environment-variable A/B against a port-only start-boost scaffold.**
>
> The AI control function is faithful everywhere it can be measured at matched position.
> `curv` agrees to 0.013-0.030°; `c0`'s arithmetic reproduces the logged byte **586/586**
> on the port. What remains of criterion (b) is a **sub-degree** steering-error residual
> crossing a **hard** discontinuity, plus the scaffold.

## Index

| step | record | commit |
|---|---|---|
| pre-registration, **unrun** | `verify/d3_offline_20261002/PREREG.md` | `31fa2fe3` |
| the run, both parts | `re/tools/ai_posmatch.py`, `partA.json`, `partB.json` | `9a372be5` |
| collateral (descriptive, added after) | `collateral.json`, `bandocc.json` | `574a569c` |
| write-up | `re/analysis/D3_B_OFFLINE_2026-10-02.md` | `077fc43c` |
| trackers + handoff | `UNCERTAINTIES.md`, `CHANGELOG.md`, `re/NEXT_SESSION.md` | `5375af67` |
| this file | `verify/d3_offline_20261002/RESULT.md` | (this commit) |

## Gate replaced — ONE, declared before the run

> `PREREG.md` §A.0. The kickoff's known-answer check for Task A — *recompute `curv` from
> the logged inputs on both sides via the transcription* — **cannot be run offline**.
> `FUN_00443440`'s inputs include the spline point array at `0x801aa0 + idx*0x204`, which
> is runtime memory; **no committed capture in this repo carries it**. It was replaced,
> **before** any statistic was computed, by **KA-A1** (bit-exact `curv` at string-identical
> position) and **KA-A2** (single-valuedness within the matching radius), both required on
> both sides. Nothing else was replaced, no band was moved, and no rule was amended after a
> run.

## TASK A — U-9183, position-matched curvature. **CLOSES.**

**Matching key.** Same car; same `ai_spline_idx`; nearest `(own_x, own_z)` within
**R = 0.12** world units — the formula being the original's own median in-window per-call
displacement (`0.1120 / 0.1112 / 0.1213`), fixed before the test.

This is a *complete* key, not a proxy. `FUN_00443440`'s whole input set under the
`0x004162b0` call site is: the spline point array (count `0x0044348c`, X `0x004434a5`,
Z `0x004434ae`), `ownX`/`ownZ` (`0x004434a8` / `0x004434b2`), `param_3` pushed as the
**literal** `0x41200000` = 10.0f at `0x004162a1`, and `param_5` pushed as the literal `0`
at `0x0041629a`. No vehicle record, no clock, no speed, no history global.

**The look-ahead does NOT scale with speed** (`param_3` is a literal), so matched position
does **not** additionally require matched speed — the kickoff asked this explicitly.

**n and known-answer result.**

| | original | port |
|---|---|---|
| **KA-A1** bit-exact `curv` at identical position | **4,088,708** pairs, **0** violations | **1,596** pairs, **0** violations |
| **KA-A2** single-valued within R (band: median ≤ 2°, p90 ≤ 10°) | median **0.0000**, p90 **0.0000** | median **0.0313**, p90 **0.4692** |

| car | n matched / 220 | median \|Δcurv\| | p90 | curv median port vs orig | speed median port vs orig |
|---|---:|---:|---:|---|---|
| 1 | **105** | **0.0197** | 0.0695 | 4.428 vs 4.465 | 2606.0 vs 2606.8 |
| 2 | **68** | **0.0131** | 0.1705 | 1.349 vs 1.356 | 2618.2 vs 2008.6 (**+30 %**) |
| 3 | **104** | **0.0296** | 0.2117 | 41.981 vs 42.053 | 3290.4 vs 2706.0 (**+22 %**) |

**Verdict: CLOSE.** Pre-registered band `n ≥ 50`, median ≤ 2.0°, p90 ≤ 10.0° on all three
cars — passed by two orders of magnitude on the median. **No input of `FUN_00443440`
diverges at matched position, so there is no diverging input to name.** The declared
confound check holds: cars 2 and 3 match at +30 % / +22 % speed and `curv` still agrees to
0.013° / 0.030°, independently confirming the no-speed-input transcription.

**A second, separable cause of the reported "2-8x".** The **full-window** `curv` medians are
**94.14 / 11.40 / 38.28** (original) vs **52.04 / 52.81 / 50.08** (port) — on car 1 the
*original* is higher. The 2-8x came from the `mode == 0` subset, which on the original
selects its low-curvature calls. It was a **conditioning artefact on top of a position
artefact**, and neither is a curvature defect.

**Consequence:** do not port anything in the curvature chain, and `0x00418560`
(`SelectSpline`) is **not** a (b) lead on this evidence — the original's `spline` argument
equals `0x801aa0 + ai_spline_idx*0x204` on **5463 of 5463** rows, and both sides are
`ai_type == 0` with the same idx mix.

## TASK B — U-9182, the `c0` branch split. **ANSWERED.**

**Branch rule, registered up front.** The original's branch is read off the logged
`hist_d8` / `hist_dc` pair, written at `0x004165cc` / `0x004165f7` (LO band) and
`0x004166db` / `0x0041670c` (HI band). `hist_d8 == 360 ∧ 0 ≤ hist_dc < 180` → LO,
`err := hist_dc`; `hist_dc == 0 ∧ 180 < hist_d8 ≤ 360` → HI, `err := hist_d8`; else AMBIG.
Rows with `ai_override != 0` are excluded from the known-answer checks, because the
override replay (`0x00418790..0x00418844`) can overwrite `c0` after every steer site.

**n and known-answer result — both checks exact, both sides.**

| check | original | port |
|---|---|---|
| **KA-B1** every LO row with `err > 30` has `c0 == 255` (`0x00416860`, gated `0x0041683e`/`0x0041684b`) | **2505 / 2505**, 0 violations | **26 / 26**, 0 violations |
| **KA-B2** recompute `c0 = trunc(min(255, err·speed·0.0030034 ·(curv·0.05 if mode==0 ∧ curv>20)))` | **148 / 151 = 0.9801** | **586 / 586 = 1.0000** |

The three original misses are all `c0_hat = c0 + 1` (46/47, 64/65, 12/13) — truncation-
boundary rounding at `call 0x4a2c48`, the allowance declared up front (≥ 95 %).
**The rule reproduces the logged `c0` on both sides.**

**The split — car 1, 220-call window, per side.**

| label | RVA | orig n | port n | `c0` set contributed |
|---|---|---:|---:|---|
| **MAG** | `0x00416697` | **72** | **24** | orig `{1,2,4..13}` (12) / port `{3,4,5,6,7}` (5) |
| **MAGHI** | `0x004167b1` (magnitude → `c1`) | 62 | **171** | `{0}` both sides |
| **DEAD** | err ≤ 1, no write | 54 | 22 | `{0}` both sides |
| **CTR1** | LO counter-steer → `c1` | 32 | 3 | `{0}` both sides |
| **FF30** | `0x00416860` | **0** | **0** | — |
| **CTRHI** | `0x00416758` | **0** | **0** | — |
| **AMBIG** | — | 0 | 0 | — |

`c0_distinct = 1 + |MAG's non-zero set|`: **original 1+12 = 13, port 1+5 = 6.** That is the
whole of U-9182.

**The branch at fault, and why.** The port **never** takes **FF30** or **CTRHI** — but
neither does the original in this window, so neither is the carrier. The branch the port
**over-runs** is **MAGHI** (`0x004167b1`), 171 vs 62, and correspondingly **under-runs
MAG**, the only branch that emits a non-zero `c0`. Its gate input is the **sign** of `err`
from `SteerAngleErrorFwd` (`FUN_00415e20` @ `0x00416596`): the port sits in
`err ∈ [349.5, 360)` on 78 % of the window. Band occupancy is near-inverted — LO/HI is
**158/62** (original) against **49/171** (port) — **at the same error magnitude** (original
MAG `err_med` **1.335°** vs port MAGHI `360 − 358.45` = **1.55°**).

**Why that happens, quantified.** At matched position the median `|Δ signed err|` is only
**0.945 / 1.296 / 0.466°**; the median signed err is port **−0.128°** vs original
**+1.024°**, a **1.15° bias**. The LO/HI split is a **hard discontinuity at `err = 0/360`**
(`0x004165c0` / `0x004166cf`) and `|err|` is a **1-2° oscillation about it**, so that
sub-degree bias flips the band on **43 % / 24 % / 13 %** of matched calls. On car 1 it
persists at **matched speed** (2606.0 vs 2606.8): port LO 49 / HI 56 against original
LO 92 / HI 13 — so it is **not** purely a position or speed artefact.

**`c0`'s arithmetic in the port is bit-faithful. Do not port the magnitude path.**

## Recommended next port target — and the first move is NOT a port

**Ruled out: `FUN_00414c30` + producer chain.** Its effect on (b) is through `ai_mode`,
already refuted as sufficient by the 2026-10-02 modes-3/7 arms; and `curv` (Task A) and
`c0`'s arithmetic (Task B) are now proved faithful, so that chain cannot reach what is left.

**1. First, a one-environment-variable A/B with no code change.**
`VehiclePhysicsRun.cpp:703-707` applies a **port-only start boost to `slot != 0`, i.e. to
exactly the cars criterion (b) scores** (`+0xbf8 = 1`, `+0xbf4 = 1300`, guarded by
`MASHED_NO_START_BOOST`). The port's window speed is **3421.7 / 3346.2 / 3495.9** against
the original's **2419.5 / 2397.0 / 2617.6** (**+41 / +40 / +34 %**), and
`m = err·speed·0.0030034` (`0x00416656`) makes speed a linear multiplier on the steer byte
while the extra distance travelled pushes the port into higher-curvature track inside the
same 220 calls. Re-capture with `MASHED_NO_START_BOOST=1` and re-score **both (b) and (e)**
— **(e) passes today *with* the boost**, so the seed cannot be removed on (b)'s evidence
alone, and an (e) regression is a trade-off for the user, not a fix.

**2. Only if that fails: the AI cars' body heading**, the larger median share of the
matched-position residual (**0.9796 / 0.9085 / 0.6506°**). Two separable candidates, and the
path in the source is mis-stated — see the correction below.

**3. A question for the user, deliberately not decided here.** Criterion (b)'s
`c0_distinct`, `c1_distinct` and `steer_distinct` count which side of a hard discontinuity a
1-2° oscillation lands on; a faithful port can fail them. Whether to re-specify (b) on a
band-invariant statistic is a ROADMAP decision. **No band was moved.**

## Correction filed, not left to stand

My own first draft mis-cited the heading path. `AiStandalone.cpp:939` comments `own_fwd_xz`
as `FUN_0046d510 -> rec+0x9d4/+0x9dc`, but in the standalone `aib_own_fwd_xz`
(`TrackRenderer.cpp:97-100`) returns `g_aib.fwd[v]`, filled as **`(cos(a.yaw), sin(a.yaw))`**
at `TrackRenderer.cpp:3729`, with `a.yaw` round-tripped through
`Vehicle::VehiclePhysics_StepCar` (`:3315` in, `:3327` out). **No `rec+0x9d4`/`+0x9dc` field
is read.** The substance holds — it is a physics output — but the heading reaches the AI as a
**reconstructed scalar**, not the record's forward basis row, and those are two separable
candidates. Also ruled out: the `yerr * (6.0f*dt)` turn-rate limiter at
`TrackRenderer.cpp:3416` / `:3676` is in the legacy "AI v2" `else` branch, which the ported
path does not take. The correction is carried in the note, the `U-9185` row and the
CHANGELOG entry.

## Collateral — at matched position (descriptive, no pass/fail)

Added **after** Tasks A and B produced their verdicts and declared as such in `574a569c`;
`part_a` / `part_b` are untouched by construction and their committed output is unchanged.
n = 105 / 68 / 104, `R = 0.12`. Medians of `|Δ|`:

| field | car 1 | car 2 | car 3 |
|---|---:|---:|---:|
| `curv` (closed target) | 0.0197 | 0.0131 | 0.0296 |
| `look_x` | 0.0133 | 0.0021 | 0.0037 |
| `look_z` | **0.7678** | 0.0301 | 0.0314 |
| `diff_a360` | **0.0000** | **0.0000** | **0.0000** |
| `rec_9e4` (speed) | 0.96 | 255.1 | 215.5 |
| `rec_b0c` | 0.22 | 0.47 | 3.32 |
| signed err | 0.945 | 1.296 | 0.466 |

Integer fields differing: `c0` 44 / 27 / 26 %, `c1` 25 / 59 / 50 %, `c4` 21 / 9 / 27 %,
`ai_mode` 20 / 24 / 24 % (the known modes-3/7 carrier), `ai_override` 0 / 28 / 0 %,
`substate` / `march_n` / `march_idx0` ≤ 1 %.

**Beyond the two targets, nothing shows a defect-sized divergence at matched position.** The
only unexplained item is the `look_z` tail on car 1 (median 0.7678, p90 5.1128) — below
defect size at the median, but the only matched-position field other than speed with a
non-trivial tail. Filed `[UNCERTAIN]` in `U-9185`.

## D2 WATCH — two informational rows

**Not one of D-11071's five named triggers** (`+0x4a4`, the contact collector, grip-clamp
#6, the substep loop, `ReassertContacts`), and **no D2 code was read or changed.**

1. D-11071 parked with the **player's** driving-median **1.3 % short** on Training. This
   session measures the **AI cars** running **+34..41 % fast** on track 0 / mode 10 — with a
   **port-only start-boost scaffold** in place that the original does not have. Opposite
   sign, different car class, different scenario, so it **neither confirms nor refutes**
   D-11071's carrier. Recorded so that whoever re-opens D2 knows AI-slot speed fidelity has
   now been measured and is not close, and that the scaffold must be reverted first.
2. The body-heading residual is a vehicle-physics output (`a.yaw` out of
   `VehiclePhysics_StepCar`) and is the larger median share of (b)'s remaining
   steering-error gap. `[UNCERTAIN]` until a run separates the physics' yaw, the scalar
   reconstruction and the start-boost scaffold.

## Trackers

Mutated through the `re-classify` skill only, in one transaction (`5375af67`), CRLF
preserved on `UNCERTAINTIES.md`, 7 columns per row, resolved rows left in place with the
Type struck through per that file's own convention.

- **U-9183** `~~semantic~~ **RESOLVED 2026-10-02**`
- **U-9182** `~~semantic~~ **RESOLVED 2026-10-02**`
- **U-9185** NEW — carries the remainder of (b)
- **CHANGELOG.md** — one line, inserted immediately below the **exact-line**
  `<!-- ENTRIES -->` marker; 1037 → 1038 lines, no history rewritten.
- **`hooks.csv` untouched. No function changed C-level.** `0x00443440` stays **C2**: the
  evidence here is position-matched agreement on committed captures, which is **not** the C3
  gate (no build, no `RH_ScopedInstall` install, no Frida diff). Not promoted — this is the
  NO-OVERCLAIMING rule, applied to my own result.

## Still open

- **D3 (b) is still NOT MET** and is the phase's sole blocker; **U-9185** carries it.
  `U-9184` stands as filed.
- `[UNCERTAIN]` the `look_z` tail on car 1.
- D2 parked, D-11071: `U-9180`, `U-9181`.

## Hygiene

No game process was spawned, so no PID was killed and no blanket kill by name occurred. No
worktree was created. Ghidra was read through `re/tools/decomp_pc.py`, which acquires a
read-only pool slot; **the master project was never written.** `original/MASHED.exe` was
untouched and no `unlock_*` patch was applied; disassembly read
`original/MASHED.exe.unpatched`. Every commit used explicit pathspecs, never `-A` or `.`.
Nothing pushed.
