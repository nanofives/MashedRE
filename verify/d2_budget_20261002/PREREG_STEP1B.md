# PRE-REGISTER — D2 attempt 18, STEP 1B: gate KA-R RETIRED, and the split of `T_post`

Written and committed **BEFORE** any of it is run. STEP 1's budget HAS been run and is
reported in `RESULT_STEP1.md`; this file registers what follows from it.

---

## 1 Gate KA-R FAILED and is RETIRED, not amended

`PREREG_STEP1.md` §5 registered KA-R as: `s_mid/s_post` over `d = 0..400` with
`s_post > 1.0` must lie in `[0.9990, 1.0010]` on **>= 99 %** of frames, median in
`[0.9999, 1.0001]`.

**Measured on `orig_sl1.msd`:** n = 396, median **1.000008427**, p25 **0.999958439**,
p75 **1.000046037**, in range **385/396 = 97.22 %**, frames `> 1.02` = **11**.
**KA-R FAIL** on the fraction leg. The median leg PASSES.

**Why it failed, measured not asserted.** The 11 out-of-range frames are exactly the 11
frames above 1.02, i.e. the original's own contact-fixup frames. §23.2's published prior is
"**23 frames > 1.02** out of n = 1447" = **1.59 %**. A **99 %** in-range bar is therefore
**unsatisfiable against the gate's own published prior** — the prior itself scores 98.41 %.
The bar was mis-set when the gate was written. The quantity it was meant to check did
reproduce, to three more digits than it needed: median 1.000008 against §23.2's 0.999998,
p25 0.999958 against 0.999955, p75 1.000046 against 1.000044.

KA-R is **reported as a FAILURE in the RESULT** and **retired**. It is not loosened, not
re-thresholded, and its verdict is not reinterpreted. KA-R2 below is a **different
question** with a **different population**, in the same way attempt 17's KA2 was not a
loosened KA.

## 2 Gate KA-R2 — KNOWN ANSWER on the ORIGINAL, fixup frames counted OUT as a named population

The original's clamp #6 is a measured no-op **off its contact frames**. Scored on
`orig_sl1.msd` over `d = 0..400`, `s_post > 1.0`:

1. The frames with `s_mid/s_post > 1.02` are counted and reported as the **fixup
   population**. Its size must be **<= 5 %** of n.
2. On the **complement** of that population, `s_mid/s_post` must lie in
   `[0.9990, 1.0010]` on **>= 99 %** of frames.
3. The median over the complement must lie in `[0.9999, 1.0001]`.

**FAIL => STOP.** Any leg failing stops STEP 1B.

Note the asymmetry and why it is not a double standard: the same three legs are **also
scored on the PORT and reported**, but the PORT's result is **not** a gate, because the
port is the thing under test. The port's published prior (§23.2: median 1.172734, 1324 of
1628 frames above 1.02) is reported next to it.

---

## 3 What STEP 1's budget established, and the one question it leaves

`RESULT_STEP1.md`: over `d = 222..250` (n = 29 both arms, gnd == 4.0 on 29/29 both arms,
`|ctrl_xz| > 0` on 29/29 both arms, median speed O 790.9 / P 633.2), the three terms are

| term | RVA span | ORIG | PORT | &#124;delta&#124; |
|---|---|---:|---:|---:|
| T_drive | `0x0046862d..0x004686a2` | +33.20455 | +27.08079 | 6.12376 |
| T_rest | `0x00470670` + `0x0046ddb0` + `0x0046833a..0x00468625` | -3.95911 | -2.50577 | 1.45334 |
| **T_post** | `0x004687f0..0x0046897b` + `0x00468980` + `0x004709a0` xN | **-0.00464** | **-27.50393** | **27.49929** |

`T_post` is the only term OUT on both (a) and (b). Gate SS therefore makes this split
**blocking**: `T_post` lumps clamp #6 with A6b and the substeps.

A6b is **statically** excluded (§26.4: 132 instructions, writes none of `+0x9b0..0x9b8`).
The substeps are excluded on the port by §26's measurement (velocity-neutral on 4000/4000
substeps, all 160 contacting ones included) and on the original by §25.3's substep-entry
ratio (0.999991..1.000020, coverage 2331/2331). Both are **prior** findings, cited not
re-derived. §4 registers an **independent** test of that attribution rather than inheriting
it.

---

## 4 The split — an EXACT inversion of clamp #6's own arithmetic, offline, both sides

Clamp #6, transcribed byte-exact from `original/MASHED.exe.unpatched` this session
(`re/tools/disasm_va.py 0x468730 0xd0`):

```
0x0046874c  fld   [esp+0x20]           ; the post-W1 speed  (frame -208, section 25.3)
0x00468750  fcomp [0x005d757c]         ; vs 0.0
0x00468758  test  ah, 0x44             ; C2|C3
0x0046875b  jnp   0x00468970           ; TAKEN iff speed == 0.0  -> skip the clamp
0x00468761  cmp   [esi+0x9e0], 0x40800000   ; grounded == 4.0 ?
0x0046876b  jne   0x00468970           ; not all four grounded -> skip the clamp
0x00468771..0x004687d7                 ; lat = vel - dot(fwd,vel)*fwd, into [esp+0x10/14/18]
0x004687db  fmul  [esp+0x20]           ; G = grip * speed        (grip live in ST0 since 0x004686be)
0x004687df  fcom  [0x005ce9fc]         ; vs 32768.0
0x004687ea  jne   0x0046888f           ; -> LOW arm
0x004687f0  fsubr [0x005ce9f8] / fmul [0x005ce9f4]   ; HIGH arm
```

Both gates are **open on both arms in the defect window**: speed is ~790 / ~633 (not 0) and
`+0x9e0 == 4.0` on 29/29 frames each. So the clamp RUNS on both sides and the gate is not
the difference. Stated now, before the split, so it cannot be offered later as the answer.

§26.1's law, carried in: both arms write `vel -= k*lat` with `lat` orthogonal to `fwd`.
Therefore, with `s = |lat|/|v|` **at the clamp's entry** and `a = 1 - k`:

```
R  = |v'| / |v|   = sqrt(1 - s^2 (2k - k^2))          the clamp's magnitude ratio
s' = |lat'|/|v'|  = (1 - k) s / R                     the POST-clamp slip
```

Two measurable quantities, two unknowns, and the system inverts in closed form:

```
a^2 = s'^2 R^2 / (1 + s'^2 R^2 - R^2)        k = 1 - a        s = s' R / a
```

**Both measurables come from the snapshot alone, on both sides:**
`R = s_post / s_mid` (`s_mid = +0x9e4` is stored at `0x004686cc` immediately before the
clamp, `s_post = |+0x9b0..b8|` is the snapshot), and `s'` is built from the snapshot's own
`vel` and `fwd` by the original's own expression at `0x00468771..0x004687d7`.

### 4.1 Conditioning, registered before the run

The inversion is **ill-conditioned as `R -> 1`**. The ORIGINAL's `R` is `1 - 2e-5`, so its
`k` is not recoverable to better than a wide band; the PORT's `R` is `1 - 0.043`, which is
well conditioned. Therefore:

- On the **PORT**, `k` and `s` are reported as measurements.
- On the **ORIGINAL**, `k` is reported **only** as a band with its sensitivity shown, and
  the primary reported quantities are the directly measured `R` and `s'` and the product
  bound `s^2 (2k - k^2) = 1 - R^2`, which is exact and needs no inversion.
  (Memory `a-bound-on-a-product-is-not-a-bound-on-a-factor`: the product bound is reported
  as a bound on the PRODUCT and no claim is made about `k` or `s` separately from it.)
- The port's `sp=%.2f` quantization (`PREREG_STEP1.md` §3) puts a floor of `1.5e-5` on
  `R`. The port's `1 - R = 0.043` is 2800x that floor. The original's is from the
  full-float `.msd`.

### 4.2 Gate INV — the inversion's KNOWN ANSWER, scored on BOTH sides

1. **Round-trip, both sides:** recomputing `R` from the solved `(k, s)` must reproduce the
   measured `R` to `<= 1e-6` relative on **>= 99 %** of scored frames. A solver that does
   not round-trip is not read.
2. **Published prior, PORT:** §26.3 measured the port's `k` independently, from the port's
   own `act.l60` (`G3` passed 1626/1626), as **0.540954** at band 100-150. The inversion's
   port `k`, scored on frames whose median speed lies in 100-150, must agree to within a
   factor **2**. This is the leg that makes the inversion a measurement of clamp #6 rather
   than of an unknown sink: if `T_post` were the substeps and not the clamp, the inversion
   has no reason to reproduce an independently measured clamp coefficient.
3. **Published prior, ORIGINAL:** §26.2/§26.3 established the original takes the **LOW**
   arm (`G = grip*speed = 30 785.1 < 32768` at 100-150) where `k = max((32768-G)*2^-15, 0.1)`
   is floored at **0.1**. The original's inverted `k` band must **contain 0.1** at the same
   band. If it does not, the RESULT says the LOW-arm/floor reading does not reproduce.

**Leg 1 FAIL => STOP.** Legs 2 and 3 are reported as PASS/FAIL and a FAIL does not stop the
step; it **downgrades the attribution of `T_post` to clamp #6 to [UNCERTAIN]** and makes the
live leg (§6) mandatory before any fix.

### 4.3 The discriminator, registered

Scored over `d = 222..250`, matched `d`, n reported:

> **Which input carries it.** If the port's pre-clamp slip `s` is within a factor **1.5**
> of the original's and the port's `k` is more than a factor **2** above the original's,
> the carrier is **`k`**, hence `G = grip * speed` at `0x004687db`, hence `l_60`
> (`0x004686b3`, accumulated at `0x00468220`/`0x0046822b` from `0x004680fb` and
> `0x0046820f`) — the §26 lane. If `s` differs by more than a factor **1.5** and `k` is
> within a factor 2, the carrier is the **slip** and the clamp is innocent. If both move,
> both are reported and neither is named alone.

Because the original's `k` is a band (§4.1), the `k` comparison uses the **band's nearer
edge** to the port's value, i.e. the comparison is made as hard as the data allows to clear
rather than as easy.

---

## 5 U-9173, and what §4 decides about it

§26.4's three-way inconsistency is: (i) the original takes the LOW arm so `k = 0.1`,
(ii) the original's `s = |lat|/|v|` measures **0.729** at band 100-150 (n = 45), and
(iii) the original's `+0x9e4/|v'|` is 0.999991..1.000020 on 2331/2331. (i)+(ii) predict a
**5.5 %** per-frame cut; (iii) measures **0.002 %**.

Registered reading, to be scored not assumed: (ii) is a band-100-150 number and §26.9
declared every original band above 100 speed in that lane **OFF-REGIME** (the original is
at median speed 1847.0 there). §4 measures `s'` at **matched `d`** instead. The prediction
registered here, before the run:

> At matched `d` over `d = 222..250`, the ORIGINAL's `s'` is **below 0.05**, and the
> product bound `1 - R^2` is consistent with `k` on the LOW-arm floor. If so, U-9173's
> inconsistency is a **banding artefact** — two different populations — and that is
> reported as U-9173's resolution. If the original's `s'` at matched `d` is near 0.729,
> U-9173 stands and §4's inversion is reported as not resolving it.

This is a prediction with a number, registered before it is measured, and it is reported
either way.

---

## 6 The live leg, and when it becomes mandatory

Unchanged from `PREREG_STEP1.md` §7, plus: it becomes **mandatory before any fix** if
Gate INV leg 2 or leg 3 FAILS, or if §4.3's discriminator names neither input alone.
If both INV legs pass and §4.3 names one input, the live leg is run as **STEP 2's
confirmation on the running original**, which the brief requires regardless.

## 7 What STEP 1B will NOT do

No `mashedmod/src` change (so `NEW = 0` by construction for STEP 1B), no knob, no clamp, no
fitted constant, no tracker mutation outside `re-classify`, no game run. AI slots 1+
(`VehiclePhysicsRun.cpp:702`) untouched.
