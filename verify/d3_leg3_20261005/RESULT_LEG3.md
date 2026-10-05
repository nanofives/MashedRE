# RESULT — U-9191 leg 3: U-9191 CONFIRMED independently; generated-vs-accumulated stays OPEN

**RAN 2026-10-05.** Pre-registration `PREREG_LEG3.md`, committed unrun at `52599239`, amended unrun
at `01ada11e` (§5b) and `4429c0ec` (§5c). Both amendments were committed **before** G-DIRECT
produced a number.

**Two outcomes, and they point in opposite directions:**

1. **G-DIRECT PASSES — U-9191 is confirmed by an independent instrument.** The heading residual
   measured **directly** from the record's forward row on both sides is **0.9866 deg (n = 105)**,
   against U-9185's published **0.9796** on the same population. Two completely different
   derivations agree to **0.7 %**. The gate was written to be able to **kill** the parent row; it
   confirms it instead.
2. **The rate legs are ABANDONED — KA-1 failed.** `+0x9c0` is not usable as a per-frame yaw rate, so
   **no cross-side rate number is reported**, and the question leg 3 existed to answer —
   **GENERATED or ACCUMULATED** — **remains open**.

---

## 1. The no-behaviour-change control, first

Seven columns appended to the default-OFF `AiStepDump` (`rec_958`, `rec_960`, `rec_9d4`, `rec_9dc`,
`rec_9bc`, `rec_9c0`, `rec_9c4`), all raw `VehiclePhysics_RecordF32` reads.

- Header prefix **preserved** (35 old columns unmoved), and **exactly** the seven new columns
  appended, so `ai_posmatch.py` and `ai_headattrib.py` are unaffected.
- **6479 of 6479 common rows byte-identical on every pre-existing column** against the committed
  `verify/d3_yaww_20261005/y1.csv`. Adding read-only record reads changed nothing.

---

## 2. KA-1 FAILED, and the failure is a finding

**Correlation 0.068251** against the required 0.95. A single scale `k` does **not** fit
`wrap180(Δ atan2(+0x9dc, +0x9d4)) = k · (+0x9c0)` over the original's own frames.

Measured on `o_t1.msd`, car 1, **3623 frames**:

- **`+0x9bc` and `+0x9c4` are exactly `0.0` on all 3623 frames.** U-9175 measured that for the
  **player** ("no pitch/roll torque on flat ground"); it now also holds for an **AI car**.
- **`+0x9c0` is non-zero on only 317 of 3623 frames**, while the heading moves (`|Δheading| > 0.01`
  deg) on **289 of 3620** frame pairs. The **support matches exactly**: on **0 of those 289** is
  `+0x9c0` zero.
- **But it is not proportional.** `Δheading / +0x9c0` has median **-8.868**, p10 **-1.44e6**, p90
  **+73.2** — six orders of magnitude of scatter, predominantly **sign-inverted**.

So `+0x9c0` co-occurs with turning but is **not a per-frame yaw rate readable from a per-frame
snapshot**. Either it is not `omega.y`, or the `.msd` samples it at a phase where it is zeroed or
only partly accumulated — the record is known to zero per-frame accumulators elsewhere (U-9177 on
`+0xb14`/`+0xb18`/`+0xb1c`). **Which of those is true is not established** and is filed rather than
guessed.

**This bears on the port's physics model.** U-9175 describes the port's `BodyOrientationIntegrate`
(`FUN_0046e9e0`) as driven by `omega.x` / `omega.z` — `at.y += omega.z*at.x - omega.x*at.z` — and
**both are identically zero on the original across every frame measured here.** Recorded as a
question, not a conclusion.

---

## 3. G-DIRECT: the residual is real

| | median_abs, deg | n | derivation |
|---|---|---|---|
| U-9185, published | 0.9796 | 105 | `err` inversion (an **identity** — U-9192) |
| **leg 3, direct** | **0.9866** | **105** | `atan2(+0x9dc, +0x9d4)` read on both sides |
| legs 1–2b, + command + speed matched | 0.8918 | 31 | `err` inversion |

Band **[0.45, 1.80]** deg, fixed before anything ran: **PASS**. Matched pairs **105**, pair distance
median **0.02929**, max **0.11961** — all inside `R = 0.12`.

The direct and inverted derivations agree to **0.7 %** on an identical-size population. **U-9192's
identity finding stands as a methodological correction — `d_body_heading` is still not an independent
measurement — but the number it produced was right.** U-9191's residual is not an artifact of the
inversion.

**Only the median is reported.** Per `PREREG` §5c the pairing's p90 induced error (0.0902 deg) sits
essentially on the registered 10x line (0.0892), so median verdicts are admissible and **tail
statistics are not**. None is given.

---

## 4. Three failed assumptions in my own pre-registration

Stated plainly, because three is a lot for one registration and it is a weakness of the registration,
not a strength of the process:

1. **`+0x9c0` is a usable per-frame yaw rate** — false (KA-1).
2. **The port writes `+0x958`/`+0x960`** — false. Identically `0.0` on all 3201 car-1 rows; the port
   keeps position in `a.pos[]`. I over-extended the memory
   `msd-world-position-is-the-0x928-matrix-row`, which is about the **original's** record.
3. **A per-frame record snapshot can be paired to a per-call dump without a phase term** — false. The
   substitute key had to be measured: phase offset median **0.000000**, p90 **0.123502**, max
   **0.163141**, with 100 of 951 rows exceeding `R` and therefore excluded rather than mispaired.

All three were caught by gates or controls rather than by publishing a wrong number. But a
better-informed registration would have checked the **port's field coverage** before registering a
pairing key, and the next leg should.

**And one gate was mis-scoped by me:** KA-1 was written to stop the whole leg, when G-DIRECT does not
use `+0x9c0` at all. The scope was corrected at `01ada11e` **with G-DIRECT still unrun**, and
G-DIRECT's band was never touched. A reader who rejects that amendment can discard G-DIRECT entirely;
the rate verdict is unaffected, because there is none.

---

## 5. What is owed

- **The generated-vs-accumulated question is OPEN**, and no offline instrument can close it. The next
  attempt needs the original's yaw rate **sampled at a known program point** — an **entry hook**, not
  a per-frame record snapshot. That is a Frida leg, pre-registered on its own, and
  `scenario_launch.py`'s existing `--axis-probe` / `--lat-bracket` shapes are the pattern (both
  currently target the **player** and would need an AI-slot filter).
- **A new row is owed for KA-1's finding** — the original's angular-velocity fields, and what actually
  drives its body orientation if `omega.x`/`omega.z` are identically zero.
- **U-9191 stands, strengthened.** Its residual now has two independent derivations agreeing to
  0.7 %. What it still lacks is a localisation inside the physics.
- **No C-level moves.** Leg 3 added dump columns and an analysis pass; nothing was reimplemented at an
  RVA.
- **No D2 WATCH row.** `PREREG` §7 made a WATCH owed **if and only if** G-RATE returned GENERATED.
  G-RATE did not run, so no D2 code is implicated and no WATCH is filed.
- **This does not close criterion (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b)
  on any car**.
