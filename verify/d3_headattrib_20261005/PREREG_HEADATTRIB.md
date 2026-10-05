# PREREG — U-9185 item (b): re-attribute the body-heading share at the physics basis

**Status when committed: UNRUN.** No tool has been written and no number has been computed at the
time this file is committed. The one thing already done is **reading** `ai_posmatch.py` and
reproducing the committed decomposition unchanged (§1), which is what motivated this file.

Parent: **U-9185 item (b)**, reopened 2026-10-05 when U-9188's answer to it was retracted
(`verify/d3_yaww_20261005/RESULT_YAWW.md`). Gate: **D3 criterion (b)**, the phase's sole open
blocker. Results go in `RESULT_HEADATTRIB.md` here.

---

## 1. What is already established, by reading and by re-running the committed tool

The bridge is **exonerated**: measured in-process yesterday, the port's published heading equals the
record's forward row to `0.000000` deg (max 2e-6) on every slot on every stepped frame. So the
heading the AI reads **is** the physics' `io.yaw`, and item (b)'s two candidates collapse to one.
That is why this session goes to the physics basis.

The committed decomposition reproduces. `ai_posmatch.py --part COLL` with the original `o_t1` against
a port capture taken today on the same recipe returns car 3 `d_target_dir` **0.4477**,
`d_body_heading` **0.6506**, `d_err` **0.4661** — U-9185's published car-3 numbers to four decimals.
So the instrument is intact and today's capture is a valid substitute for whatever port capture the
original run used.

### But the "body heading" in that decomposition is not a measurement of the body heading

`ai_posmatch.py:401-422`. Per matched pair it computes `tp`/`to` from the logged positions, takes
`ep`/`eo` from `signed_err()`, and then:

```
411:        hp, ho = tp - (ep % WRAP), to - (eo % WRAP)
413:        dh.append(_wrap180(hp - ho))
```

So `d_body_heading ≡ wrap180( (tp - to) - (ep%360 - eo%360) )`. It is **algebraically determined by
the other two reported quantities**. There is exactly one measured angular series here —
`signed_err` — plus a positional one. The three-way "decomposition" is an **identity**, not three
independent observations, and "the heading is the larger median share on 2 of 3 cars" is a statement
about `|d_target_dir - d_err|` against `|d_target_dir|`.

The tool's own comment (`ai_posmatch.py:376-377`) is what licensed the physics attribution: *"The
heading comes from rec+0x9d4/+0x9dc (FUN_0046d510), i.e. from the ported vehicle physics, not from
the AI."* The **inversion is valid only if `err` is fresh**, because the recovered heading absorbs
any error in `err`.

### And `err`'s source is known to be stale on the original, asymmetrically

`signed_err()` → `classify()` reads `hist_d8` / `hist_dc`, which U-9186 established are the
**steer-history stores** at `0x004165cc` / `0x0041670c`. U-9186's own words: *"Both early returns
precede the mode commit at `0x00416590` and the steer-history stores at `0x004165cc`/`0x0041670c`,
which is why the logged `ai_mode` is stale and `hist_d8`/`hist_dc` are **frozen** on those calls."*
It counts **36** (`FUN_00414a70 == 2`) plus **64** (`FUN_004148b0 && FUN_00416060`) = **100 of 660**
window calls on the original.

**The port cannot have this.** Its mode is pinned to 0 (`AiStandalone.cpp:844`) and neither early
return exists, so `ep` is fresh on every call. That makes the contamination **one-sided** — the exact
shape that manufactures a defect on the side that has it (memories
`cross-side-detector-swamped-on-one-side`, `cross-side-fit-needs-both-sides-checked`).

---

## 2. Hypotheses

- **H-IDENT** — the reported `d_body_heading` is an identity on the other two series, not an
  independent measurement. Decided by arithmetic, not by a run.
- **H-STALE** — the residual is substantially an artifact of one-sided frozen `hist_d8`/`hist_dc` on
  the original. If so, item (b)'s heading share is **not a physics finding** and the carrier is the
  command defect already filed as **U-9186**.
- **H-PHYS** — the residual survives every control, and the port's body heading genuinely diverges
  from the original's at matched position and matched command. Only then is this a physics defect,
  and only then is leg 3 worth running.

**Registered prediction: H-STALE, partially — I expect a material reduction but not a collapse to
zero.** Reasoning: the frozen calls are 100 of 660 = 15 % of the window, which is enough to move a
median but not obviously enough to account for all of a 0.65–0.98 deg share. I am explicitly **not**
predicting which of H-STALE and H-PHYS dominates. If I am wrong in the direction of H-PHYS the
session gets more interesting, not less.

---

## 3. Legs

All of legs 1 and 2 are **offline on already-committed captures, no game run**. New tool
`re/tools/ai_headattrib.py`; `ai_posmatch.py` is **not modified** so its committed results stay
reproducible.

### Leg 1 — is the statistic an identity, and how contaminated is it

1. **Identity check.** For every matched pair recompute `wrap180(d_target_dir - d_err)` and compare
   against the tool's `d_body_heading`. Report max absolute discrepancy.
2. **Freshness detector, empirical and symmetric.** A call is **FROZEN** when its `(hist_d8,
   hist_dc)` pair is bit-identical to the previous logged call for the same car. Applied **identically
   to both sides** — it is not a lookup of the original's early-return columns, so it cannot be
   accused of being tuned to one side. Report the frozen fraction per car per side.
3. **Cross-check the detector** against the original's own `ret14a70` column, which Format D carries:
   report the overlap between FROZEN and `ret14a70 == 2`. This validates the detector without the
   detector depending on it.
4. **Exclusion.** Recompute all three medians on pairs where **neither** side is FROZEN.

### Leg 2 — condition on the steering command

Recompute the three medians on matched pairs that additionally have **identical `(c0, c1)`** on both
sides. At matched position and matched commanded steer, a surviving heading difference cannot be a
command difference. Report n; if n is too small to carry a median, **say so and do not report one**
(the window gives ~104 matched pairs per car before any further filter, so this is a live risk and is
registered as a possible NO-VERDICT rather than treated as a result).

### Leg 3 — direct measurement, only if H-PHYS survives

Leg 3 is **conditional and is not run if legs 1–2 refute H-PHYS.** It removes the derivation
entirely:

- Add `+0x9d4` / `+0x9dc` and the angular velocity `+0x9bc` / `+0x9c0` / `+0x9c4` to the port's
  `AiStepDump` (it already reads `+0x9e4` and `+0xb0c`, so this is five more `RecordF32` calls in an
  existing default-OFF dump).
- Compare against the original's `o_t1.msd` / `o_t2.msd`, which carry the same offsets for **car slot
  1, an AI car**, readable with `re/tools/statediff/msd_fields.py`.
- **Compare the yaw RATE, not the heading.** Heading is an integral, so a matched-position heading
  difference is accumulated history and cannot localise anything (memory
  `a-tolerance-has-no-concept-of-latency`). The rate is the instantaneous quantity.
- Known pairing limitation, stated now: the `.msd` is **one snapshot per render frame** while the
  aistep CSV has multiple AI calls per frame, so the pairing is coarse. Leg 3 must report its pairing
  residual and is **VOID** if that residual is not at least 10x smaller than the difference claimed.

---

## 4. Gates, fixed before running

| gate | PASS | FAIL |
|---|---|---|
| **G-IDENT** | max discrepancy between `d_body_heading` and `wrap180(d_target_dir - d_err)` is below **1e-6** deg → the statistic **is** an identity and must never again be cited as an independent heading measurement | above 1e-6 → `_decompose` does something I have misread; stop and re-read it before any other claim |
| **G-DETECT** | the FROZEN detector covers at least **90 %** of the original's `ret14a70 == 2` calls in the matched population | below 90 % → the detector is weaker than assumed; report its coverage and treat leg 1's exclusion as a lower bound on contamination, not a measurement of it |
| **G-ASYM** | the original's frozen fraction exceeds the port's by at least **5x** on all three cars → the contamination is one-sided as predicted | not one-sided → H-STALE's mechanism is wrong even if the numbers move; say so |
| **G-STALE** | excluding FROZEN pairs drops `d_body_heading` median_abs below **0.25** deg on all three cars (a ≥2.6x reduction from 0.9796 / 0.9085 / 0.6506) → **H-STALE**, item (b)'s heading share is an artifact and the carrier is U-9186 | stays above **0.5** deg on all three → **H-PHYS**, the residual is real and leg 3 runs |
| **G-CMD** | with `(c0, c1)` matched, `d_body_heading` median_abs below **0.25** deg → the residual is downstream of the command | above **0.5** deg with **n ≥ 30** per car → a genuine physics divergence at matched command; this is the strongest result available offline and it promotes leg 3 |

Between the PASS and FAIL thresholds of G-STALE and G-CMD (0.25–0.5 deg) the verdict is
**INCONCLUSIVE** and is reported per car with its n, with **no** conclusion drawn. Registered now so
a middling result cannot be read either way. `n < 30` on any car makes that car **NO-VERDICT** on
G-CMD, not a pass and not a fail.

**What a result here does NOT license.** No C-level moves on any of this: nothing in legs 1–2 reads
or changes a function at an RVA. And **closing item (b)'s heading share does not close criterion
(b)** — the 2026-10-02 counterfactual matrix had **no arm passing (b) on any car**, so even a clean
H-STALE leaves (b) failing and merely moves its attribution onto U-9186.

---

## 5. What could make this file wrong

- **The frozen detector can false-positive** on a genuinely stationary error — a car tracking the
  line perfectly logs the same history twice. G-DETECT is the guard; if coverage is high but the
  frozen fraction is implausibly large, the detector is catching both populations and leg 1's
  exclusion over-corrects. The honest fallback is to report both the inclusive and the exclusive
  medians, which leg 1 does unconditionally.
- **n is small.** ~104 matched pairs per car before filtering, 79 surviving `_decompose` on car 3
  today. Every filter shrinks it further. Every number in the result carries its n, and any median on
  fewer than 30 pairs is reported as NO-VERDICT.
- **Matched position is not matched state.** Two cars at the same point with the same spline index may
  differ in speed, and U-9185 already measures the port **+15..31 %** faster. Position matching does
  not control for that, this leg does not fix it, and a surviving residual therefore has speed as a
  live alternative explanation that leg 3's rate comparison would still have to separate. Stated so
  G-CMD's PASS is not over-read.
