# RESULT — D2 attempt 18, STEP 1: the per-frame velocity budget at matched `d`

Executes `PREREG_STEP1.md` (committed `5cc22d0c`, unrun). One gate FAILED and is reported
as a failure; its retirement and replacement are registered separately in
`PREREG_STEP1B.md`, committed unrun **after** this result and **before** that gate's
replacement was scored.

Instrument: `re/tools/statediff/a18_budget.py`. No game run — a reduction of two committed
captures. No `mashedmod/src` change, so **`NEW = 0` by construction** for this step.

`d` = frames from release, `L = 0`, **R = 890** on `orig_sl1.msd` (the capture's own
`+0xbf8 != 0` marker at `0x0046d7a2`, confirmed by the tool: *first `+0xbf8 != 0` at frame
890*), **R = 1** on `p1/motion_diag.log`.

`linTerm = 1.6666668613120733e-05 = 50.000004 * 0.0010000000474974513 * (1/3000)`, imported
from `a10_gain.py` rather than restated.

---

## 1 The gates, in order

| gate | what it asked | result | verdict |
|---|---|---|---|
| **KA-M** | `Rf(+0x54)` exactly one distinct value `== 0.0010000000474974513` on the ORIGINAL | **1** distinct value, `0.0010000000474974513` | **PASS** |
| **KA-R** | ORIGINAL `s_mid/s_post` over `d = 0..400`, `s_post > 1`: `>= 99 %` in `[0.9990,1.0010]` AND median in `[0.9999,1.0001]` | n = 396, median **1.000008427**, p25 **0.999958439**, p75 **1.000046037**, in range **385/396 = 97.22 %**, `>1.02` = **11** | **FAIL** (fraction leg; median leg passes) |
| **CO** | coverage counted | ORIG 1442 steps, drops `{no_prev 1, zero_prev 891, nonfinite 0}`; PORT 1625 steps, drops `{no_prev 1, zero_prev 0, nonfinite 0}`. The 891 ORIG drops are the pre-release at-rest frames (`s_post <= 1e-6`), i.e. `d < 0`, outside every scored window. | **PASS** |
| **EV** | `d = 222..250`, both arms: `n >= 25`, `+0x9e0 == 4.0` every frame, `&#124;ctrl_xz&#124; > 0` every frame | **nO 29 / nP 29**, gnd4 **29/29** and **29/29**, `ctrl == 0` **0** and **0** | **PASS** |
| **SS** | substep accounting reported, live leg blocking if `T_post` is named | reported in §3; `T_post` IS named, so the live leg is **BLOCKING** | **reported** |

### 1.1 Why KA-R failed, measured not asserted

The 11 out-of-range frames are **exactly** the 11 frames with ratio `> 1.02`, i.e. the
original's own contact-fixup frames. §23.2's published prior for this quantity is
"**23 frames > 1.02** out of n = 1447" = **1.59 %** — so a **99 %** in-range bar is
**unsatisfiable against the gate's own prior**, which itself scores 98.41 %. The bar was
mis-set when the gate was written; the quantity it was meant to check reproduced to three
more digits than it needed (median 1.000008 vs §23.2's 0.999998, p25 0.999958 vs 0.999955,
p75 1.000046 vs 1.000044).

KA-R is **retired, not loosened**. Its replacement KA-R2 asks a different question on a
different population (the complement of a counted fixup population) and is registered in
`PREREG_STEP1B.md` §2, committed before being scored. **This is the gate replaced this
session, and this is the reason.**

Because KA-R failed, the decision rule **did not execute as registered**. §2 reports what
the rule would have named and §4 reports the term naming as **pending KA-R2**.

---

## 2 The budget — three windows, matched `d`, call order by RVA

Thresholds are attempt 10's, reused verbatim (`a10_gain.py:249-250`):
`(a) |mp - mo| >= 0.20 * |median T_W1 on the ORIGINAL|`,
`(b) |mp - mo| >= 0.30 * |median dS_O - median dS_P|`. OUT iff (a) AND (b).

### 2.1 `d = 200..222` — the CONTROL (n = 23 each; **below EV's own 25 bar**, stated)

median speed O **327.2** / P **322.2**. bar(a) **3.6891**, bar(b) **0.6394**.

| term | RVA span | ORIG | PORT | &#124;delta&#124; | (a) | (b) | OUT |
|---|---|---:|---:|---:|---|---|---|
| T_drive | `0x0046862d..0x004686a2` | +26.55782 | +26.21925 | 0.33857 | no | no | |
| T_rest | `0x00470670` + `0x0046ddb0` + `0x0046833a..0x00468625` | -8.58772 | -1.52597 | 7.06175 | PASS | PASS | **OUT** |
| T_post | `0x004687f0..0x0046897b` + `0x00468980` + `0x004709a0`xN | -0.00401 | -6.73326 | 6.72924 | PASS | PASS | **OUT** |
| dS | | +16.52818 | +18.65941 | 2.13123 | | | |
| T_W1 | | +18.44539 | +24.75150 | | | | |

identity `max|resid|` **3.553e-15** — **construction identity, not evidence**.
`SS`: `T_post != 0` on ORIG 23/23 median **-0.00401**, PORT 23/23 median **-6.73326**.
gear ORIG `[0]` PORT `[0]`. median `|ctrl_xz|` ORIG **1.683e6** PORT **1.674e6**.

### 2.2 `d = 222..250` — the DEFECT window (n = 29 each)

median speed O **790.9** / P **633.2**. bar(a) **5.5560**, bar(b) **8.6373**.

| term | RVA span | ORIG | PORT | &#124;delta&#124; | (a) | (b) | OUT |
|---|---|---:|---:|---:|---|---|---|
| T_drive | `0x0046862d..0x004686a2` | **+33.20455** | **+27.08079** | 6.12376 | PASS | no | |
| T_rest | `0x00470670` + `0x0046ddb0` + `0x0046833a..0x00468625` | **-3.95911** | **-2.50577** | 1.45334 | no | no | |
| **T_post** | `0x004687f0..0x0046897b` + `0x00468980` + `0x004709a0`xN | **-0.00464** | **-27.50393** | **27.49929** | **PASS** | **PASS** | **OUT** |
| dS | | +27.74798 | -1.04312 | 28.79110 | | | |
| T_W1 | | +27.78004 | +25.07255 | | | | |

identity `max|resid|` **0.000e+00** — **construction identity, not evidence**.
`SS`: `T_post != 0` on ORIG 29/29 median **-0.00464**, PORT 29/29 median **-27.50393**.
gear ORIG `[0, 1]` PORT `[0, 1]` — the same two gears, as U-9177 recorded.
median `|ctrl_xz|` ORIG **2.385e6** PORT **2.223e6** (U-9177's 2.381e6 / 2.233e6 at n = 28
reproduces at n = 29).

### 2.3 `d = 250..260` — shape only (n = 11 each; below EV's 25 bar)

median speed O **1310.6** / P **385.5**. bar(a) **4.6524**, bar(b) **14.0530**.

| term | ORIG | PORT | &#124;delta&#124; | (a) | (b) | OUT |
|---|---:|---:|---:|---|---|---|
| T_drive | +32.60644 | +19.66704 | 12.93940 | PASS | no | |
| T_rest | -9.34454 | -1.37409 | 7.97045 | PASS | no | |
| **T_post** | -0.02615 | **-41.46950** | 41.44335 | PASS | PASS | **OUT** |
| dS | +23.26387 | -23.57960 | 46.84348 | | | |

identity `max|resid|` **0.000e+00**. The two arms' median speeds are 1310.6 vs 385.5 here,
so this window is reported for **shape only** and nothing is concluded from it.

---

## 3 What the rule would name, and the control's answer

**Walking the terms in call order `T_drive -> T_rest -> T_post` over `d = 222..250`:**

- `T_drive` is OUT on **(a) only** — not OUT.
- `T_rest` is OUT on **neither** — not OUT.
- **`T_post` is OUT on both (a) and (b)**, `|delta| = 27.49929` against bars 5.5560 and
  8.6373.

So the first term OUT OF TOLERANCE with every upstream term IN tolerance is **`T_post`**,
and it is the **only** term OUT in the defect window. Reported as **pending KA-R2** because
KA-R failed and the rule did not execute as registered.

**The control condition registered in `PREREG_STEP1.md` §6 is NOT met.** `T_post` is OUT
OF TOLERANCE at `d = 200..222` as well (ORIG **-0.00401**, PORT **-6.73326**,
`|delta| = 6.72924` against bars 3.6891 / 0.6394). Per the registered wording, `T_post` is
therefore reported as a **standing divergence**, not as something that switches on at
`d = 222`.

### 3.1 Why the net gain nonetheless flips at `d = 222` — arithmetic, not a new term

| window | median speed P | PORT T_W1 | PORT T_post | PORT dS | ORIG dS |
|---|---:|---:|---:|---:|---:|
| `d` 200-222 | 322.2 | +24.75 | **-6.73** | **+18.66** | +16.53 |
| `d` 222-250 | 633.2 | +25.07 | **-27.50** | **-1.04** | +27.75 |
| `d` 250-260 | 385.5 | +18.27 | **-41.47** | **-23.58** | +23.26 |

The port's `T_W1` is nearly flat across the three windows (+24.75 / +25.07 / +18.27) and is
within **9.7 %** of the original's in the defect window (+25.07 vs +27.78). Its `T_post`
sink **triples** (-6.73 -> -27.50 -> -41.47) while the original's stays at **-0.004 to
-0.026**. `d = 222` is the frame at which `|T_post|` crosses `T_W1` and the net gain goes
negative. **U-9177's "switch-on at `d = 222`" is a crossing point of a term that is already
divergent at `d = 200`, not the onset of a new term.** That is a correction to U-9177's
framing and it is registered as such.

As a fraction of the port's own median speed, `T_post` is **-2.09 %** (d 200-222),
**-4.34 %** (d 222-250), **-10.76 %** (d 250-260) per frame. The original's is
**-0.0012 %**, **-0.0006 %**, **-0.0020 %**.

### 3.2 `T_drive`, reported because it is OUT on (a)

ORIG **+33.20455** vs PORT **+27.08079**, ratio **0.8155**, while median `|ctrl_xz|` is in
ratio **0.932** (2.223e6 / 2.385e6). The remaining **12 %** is the `ctrl . u` direction
term, `u` being the previous frame's velocity unit. It fails leg (b) by a wide margin
(6.12 against a bar of 8.64), so under the registered rule it is **not** the diverging
term; it is reported here so the number is on the record and not discovered later.

`T_rest` — which §24.4/§25.1 identified as the port's A5 Phase-4 drag share with
`local_70` at 0.850000024 against the original's 1.0 (U-9171) — is **IN tolerance on both
legs** in the defect window (-3.959 vs -2.506). Its sign makes the port **faster**, exactly
as §25.1 recorded, and at this `d` it is 1.45 against bars of 5.56 / 8.64. **U-9171 is not
the carrier at `d = 222..250`.**

---

## 4 The diverging term, and why STEP 1 does not fix it

**Named: `T_post`** — the velocity change between the `+0x9e4` store at `0x004686cc` and
the render-tick snapshot. Pending KA-R2.

`T_post` is a **LUMP**: grip-clamp #6 `0x004687f0..0x0046897b`, A6b `0x00468980`, and the
substep loop `0x004709a0` run `N` times. Gate SS therefore makes the split **blocking
before any fix**, which is registered, not decided after the fact. A6b is statically
excluded (§26.4) and the substeps are excluded by prior measurement on both sides
(§25.3, §26) — but those are inherited findings, and `PREREG_STEP1B.md` §4 registers an
**independent** test of the attribution rather than relying on them.

No fix is authored in STEP 1. No knob, no clamp, no fitted constant, no source change.

---

## 5 Artefacts

```
verify/d2_budget_20261002/
  PREREG_STEP1.md    PREREG_STEP1B.md    RESULT_STEP1.md
  budget.csv         per-frame terms, both sides, d 180..280
re/tools/statediff/a18_budget.py        the reducer (imports a10_gain's linTerm + thresholds)
```

Inputs, both already committed and provenance'd, neither re-captured:
`verify/d2_b0c_20261002/orig_sl1.msd` (git `d9db5e80`) and
`verify/d2_b0c_20261002/p1/motion_diag.log` (§16.7 arm, participants = 1).
