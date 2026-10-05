# RESULT — U-9185 item (b): the body-heading share, re-attributed

**RAN 2026-10-05.** Pre-registration `PREREG_HEADATTRIB.md`, committed unrun at `225ee06f` and
amended unrun at `d5da030e` (leg 2b registered before its filter executed). Legs 1, 2 and 2b are
**offline on committed captures — no game run, no build, no injection.** Leg 3 is **promoted and not
run**.

**Headline, and it splits the cars.** U-9185 reports the matched-position body-heading share as one
phenomenon, "the larger median share on 2 of 3 cars" (0.9796 / 0.9085 / 0.6506 deg). It is **not one
phenomenon**. On **car 1** the heading divergence is **real and survives every available control** —
0.8918 deg at matched position, matched commanded steer and matched speed (n = 31). On **cars 2 and
3** it **collapses to 0.1064 and 0.208** under the same controls, i.e. it was substantially a command
artifact — but both fall below the pre-registered n floor and are reported **NO-VERDICT**, not
artifact.

**My registered prediction (H-STALE) was WRONG.** Removing the staleness contamination made the
residual slightly *worse*, not better.

---

## 1. The statistic U-9185 relied on is an algebraic identity

**G-IDENT — PASS, decisively.** Max residual between the tool's `d_body_heading` and
`wrap180(d_target_dir - d_err)` is **5.684e-14 / 4.974e-14 / 5.684e-14** deg on cars 1/2/3, against a
threshold of 1e-6.

So `ai_posmatch.py:411`'s three-way split is **one measured angular series (`signed_err`) plus a
positional one, and an identity**. `d_body_heading` is **not an independent measurement of the body
heading** and must never again be cited as one. U-9185's reading — that the heading term is a
"vehicle-physics output, not an AI one" because it derives from `rec+0x9d4`/`+0x9dc`
(`ai_posmatch.py:376-377`) — is only valid to the extent that `signed_err` is fresh, because the
inverted heading absorbs any error in `err`.

That does **not** make the underlying residual fake. It makes the *attribution* unproven, which is
what legs 1–2b then test directly.

---

## 2. Staleness: real, but not the carrier

| gate | car 1 | car 2 | car 3 |
|---|---|---|---|
| matched pairs | 105 | 68 | 104 |
| FROZEN, original | **0** | **0** | **26** (25 %) |
| FROZEN, port | 0 | 0 | 0 |
| asymmetry | n/a | n/a | **infinite** |
| `d_body_heading`, all pairs | 0.9796 | 0.9085 | 0.6506 |
| `d_body_heading`, FROZEN excluded | **0.9796** | **0.9085** | **0.7269** |

- **G-DETECT — PASS where testable.** On car 3, **20 of 20** matched pairs with `ret14a70 == 2` are
  FROZEN by the symmetric detector: coverage **1.000**. The detector is validated against the
  original's own column without depending on it. On cars 1 and 2 the column is present and **no
  matched pair has `ret14a70 == 2`**, consistent with frozen = 0 — nothing to validate, not a failure.
- **G-ASYM — FAILS as a general claim.** The one-sided contamination I predicted exists only on
  **car 3**. Cars 1 and 2 have **zero** frozen calls on either side in the matched population, so the
  mechanism is simply absent there.
- **G-STALE — FAIL → H-PHYS.** Excluding frozen pairs leaves 0.9796 / 0.9085 unchanged (nothing to
  exclude) and moves car 3 **up** to 0.7269. All three stay above the 0.5 deg FAIL threshold.

**H-STALE is refuted.** The 15 %-of-window contamination U-9186 documents is real and lands almost
entirely outside the matched population, where it cannot explain the residual.

---

## 3. Conditioning on the command, then on speed

`d_body_heading` median_abs, degrees:

| filter | car 1 | car 2 | car 3 |
|---|---|---|---|
| matched position only | 0.9796 (n 105) | 0.9085 (n 68) | 0.6506 (n 79) |
| + identical `(c0, c1)` | **0.9417 (n 36)** | 0.1067 (n 18) | 0.2328 (n 22) |
| + speed within 1 % | 0.790 (n 28) | 0.1064 (n 17) | 0.208 (n 18) |
| + speed within **5 %** | **0.8918 (n 31)** ← chosen | 0.1064 (n 17) | 0.208 (n 18) |
| + speed within 10 % | 0.9764 (n 35) | 0.1064 (n 17) | 0.2325 (n 21) |
| **verdict** | **REAL** | NO-VERDICT (n < 30) | NO-VERDICT (n < 30) |

- **G-CMD — car 1 PASS (REAL).** n = 36 ≥ 30 and 0.9417 > 0.5. Cars 2 and 3 are **NO-VERDICT by the
  pre-registered n floor**, not artifact — though their values (0.1067, 0.2328) sit below the ARTIFACT
  threshold and their `d_err` collapses with them (1.2964 → 0.0468 and 0.4661 → 0.0878), which is what
  a command artifact looks like.
- **G-SPEED — car 1 PASS (REAL).** At the tightest tolerance reaching n ≥ 30 (5 %, n = 31) the residual
  is **0.8918 deg**. It is **stable across the whole ladder** (0.790 / 0.8918 / 0.9764 at 1/5/10 %) and
  does **not** collapse when speed is matched. Cars 2 and 3 reach n = 17 and 18–21 at every tolerance
  and are NO-VERDICT.

**So on car 1 the heading divergence survives matched position, matched commanded steer and matched
speed.** It is not a staleness artifact, not a command artifact and not a speed artifact. This is the
strongest statement the committed captures can support, and it is exactly what G-SPEED's PASS clause
was written to license: **leg 3 is promoted.**

---

## 4. Controls, and two defects found in my own work

**The original is deterministic over the scored window — measured, not assumed.** Car 1's first 220
logged calls are **identical on 0 of 220 rows** across `o_t1`, `o_t2`, `o_e1` and
`d3_ai_20260927b/p1` — three different capture sessions — on all nine columns this tool reads
(`own_x`, `own_z`, `ai_spline_idx`, `hist_d8`, `hist_dc`, `c0`, `c1`, `look_x`, `look_z`). This
control was run because five different `--orig` files produced **digit-identical** output, which
could equally have meant a dead `--orig`. It did not: the determinism is real. (`o_inputs` is Format
B with no `own_x` and yields n = 0 — consistent, not a failure.)

**Defect in my own tool, found and fixed mid-session.** The first version conflated "capture has no
`ret14a70` column" with "column present, no qualifying row", and printed *"no ret14a70 column in this
capture — not cross-validated"* for cars 1 and 2, whose captures **do** carry it. That would have
told a reader the detector was unvalidated when the truth is there was nothing to validate. Fixed;
both states are now reported distinctly. The numbers were never affected.

**The delegated capture inventory was wrong on one point:** it reported that `o_t3` does not exist.
It does (`verify/d3_elim_20261003/o_t3.msd.aistep.csv`, 1,123,656 bytes). Recorded so the inventory
is not re-used uncorrected.

**New tool `re/tools/ai_headattrib.py`.** `ai_posmatch.py` is deliberately **unmodified** so its
committed results stay reproducible; every shared primitive (`window`, `signed_err`, `_ang_of`,
`_wrap180`, `_f`, `R_MATCH`, `WRAP`) is imported from it and the pairing is byte-for-byte
`collateral()`'s, so the two tools cannot drift.

---

## 5. Where this leaves item (b), and what is owed

- **Item (b)'s heading share is re-attributed, and the attribution differs per car.** Car 1: a real
  divergence at the physics basis. Cars 2 and 3: consistent with a command artifact already covered
  by **U-9186**, but underpowered and formally NO-VERDICT. U-9185's framing of one phenomenon across
  "2 of 3 cars" does not hold.
- **No C-level moves.** Nothing here reads or changes a function at an RVA.
- **This does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing
  (b) on any car**, so even a fully explained heading share leaves (b) failing.
- **Leg 3 is promoted and is the next step.** It removes the derivation entirely and, crucially,
  compares the **yaw RATE** rather than the heading: a body heading is an **integral** of past yaw
  rate, so car 1's surviving 0.89 deg may be **accumulated before** the matched instant rather than
  generated at it. Matched position, command and speed still do not match **history**. Registered
  limit from `PREREG` §4b, repeated here so the PASS is not over-read: **G-SPEED's PASS promotes leg 3
  and concludes nothing about where in the physics the divergence is made.**
- **Leg 3's shape, already scoped:** add `+0x9d4` / `+0x9dc` and `+0x9bc` / `+0x9c0` / `+0x9c4` to the
  port's existing default-OFF `AiStepDump` (five more `RecordF32` calls beside the `+0x9e4` and
  `+0xb0c` it already reads), compare against `o_t1.msd` / `o_t2.msd` which carry the same offsets for
  **car slot 1, an AI car**, via `re/tools/statediff/msd_fields.py`. **Known pairing limit:** the
  `.msd` is one snapshot per render frame while the aistep CSV has several AI calls per frame, so leg
  3 must report its pairing residual and is **VOID** if that residual is not at least 10x smaller than
  the difference it claims.
- **Underpowered cars.** Raising n on cars 2 and 3 needs a longer window or a looser match radius;
  both change the registered instrument and would need their own pre-registration. Do **not** relax
  either to reach a verdict on the numbers already seen.
