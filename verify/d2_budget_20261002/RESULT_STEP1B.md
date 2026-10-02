# RESULT — D2 attempt 18, STEP 1B: `T_post` quantified exactly, §23.2's attribution of it to
# grip-clamp #6 REFUTED, and ~90 % of the port's sink is UNIDENTIFIED

Executes `PREREG_STEP1B.md` (committed `d14dcc54`, unrun). Instruments
`re/tools/statediff/a18_clampinv.py` plus three read-only checks on the already-committed
captures and the anchored binary. **No game run, no source change — `NEW = 0` by
construction.** Every number carries `n`, median speed and `d` (`L = 0`, R = 890 orig /
R = 1 port).

**One of this step's own instruments is WITHDRAWN in §3 before anything is built on it.**

---

## 1 The gates

| gate | asked of | result | verdict |
|---|---|---|---|
| **KA-R2** leg 1 | ORIGINAL | fixup population (`s_mid/s_post > 1.02`) **11 / 396 = 2.78 %** (bar <= 5 %) | **PASS** |
| **KA-R2** leg 2 | ORIGINAL | on the complement, in `[0.9990,1.0010]` on **385/385 = 100.00 %** (bar >= 99 %) | **PASS** |
| **KA-R2** leg 3 | ORIGINAL | complement median **1.000006097**, p25 **0.999956816**, p75 **1.000044055** (bar `[0.9999,1.0001]`) | **PASS** |
| **KA-R2** | PORT, reported not gated | fixup pop **193/397 = 48.61 %**; complement in range **41/204 = 20.10 %**; complement median **1.001689226** | FAIL (the port is the thing under test) |
| **INV leg 1** round-trip | both | ORIG solved 738/1439, `<= 1e-6` on **738/738**, worst **2.090e-15**; PORT solved 1276/1611, **1276/1276**, worst **9.414e-15** | **PASS** |
| **INV leg 2** published prior | PORT | inverted `k` at band 100-150 **0.200694** (n = 104, med speed 126.8) vs §26.3's independently measured **0.540954**, ratio **0.3710** (bar: factor 2) | **FAIL** |
| **INV leg 3** published prior | ORIGINAL | `k` band at 100-150 **[0.000023, 0.000171]** (n = 21, med speed 126.5) does not contain the LOW-arm floor **0.1** | **FAIL** |

KA-R2 **PASSES on the original**, so the step proceeded. §23.2's priors reproduce on both
sides (ORIG median 0.999998 / 1.59 % above 1.02 -> here 1.000006 / 2.78 %; PORT median
1.172734 / 81.3 % -> here, over `d = 0..400` only, 48.61 %).

Legs 2 and 3 FAILED. Per `PREREG_STEP1B.md` §4.2 that **downgrades the attribution of
`T_post` to clamp #6 to [UNCERTAIN] and makes the live leg MANDATORY before any fix** —
honoured: no fix is authored here. §4 then shows the failure was not noise: it was the
inversion being invalid.

---

## 2 The exact, assumption-free number

`1 - R^2 = s^2 (2k - k^2)` with `R = s_post / s_mid`. `R` uses only velocity **magnitudes**,
so it needs no forward axis and no inversion.

| window | n each | med speed O / P | **ORIG `1-R^2`** | **PORT `1-R^2`** | factor |
|---|---:|---|---:|---:|---:|
| `d` 200..222 | 23 | 336.4 / 347.4 | 2.3524e-05 | 4.0518e-02 | **1 723** |
| **`d` 222..250** | **29** | **820.5 / 653.0** | **1.2194e-05** | **7.4031e-02** | **6 072** |
| `d` 250..260 | 11 | 1333.9 / 403.9 | 4.0636e-05 | 1.7270e-01 | 4 250 |

Median `1 - R`: ORIG **6.097e-06**, PORT **3.7727e-02**. The port's is **2 515x** its own
`sp=%.2f` quantization floor of 1.5e-5 (`PREREG_STEP1.md` §3); the original's comes from the
full-float `.msd`.

**The original's value is at its measurement floor, not a measured loss.** `R > 1` on
**14 of 29** original frames in the defect window — `|v|` *grew* between the `+0x9e4` store
at `0x004686cc` and the snapshot, which `v -= k*lat` cannot do for `k in [0,2]`. So ORIG
`s^2(2k-k^2) <= ~2.4e-05` is a **bound on the PRODUCT** and no claim is made about either
factor from it alone (memory `a-bound-on-a-product-is-not-a-bound-on-a-factor`).

### 2.1 On the ORIGINAL, `T_post` IS clamp #6 alone — proven from the capture's own fields

Read from `orig_sl1.msd` at matched `d` (n = 29, `d = 222..250`):

| field | value | consequence |
|---|---|---|
| `+0x9f0` | **0** on 29/29 | A4's parked damp at `0x00470948` (port `VehicleControl.cpp:278-282`, gate `== 2`) does **not** fire |
| `+0x9ec` | **0** on 29/29 | `0x00470aef`'s test fails, so `VehicleContactFixup 0x0046ef70` is **never called** |
| `+0x9e0` | **4.0** on 29/29 | **does NOT prove the gate is open — see §2.2** |
| `+0x18c` | **1.0**, one distinct value | `grip == l_60` exactly |
| `+0x2c`, `+0x34` | **0**, one distinct value each | no grip multiplier at `Integrate2.cpp:657-658` |
| `+0x1f0` | `0xffb48080` / `0xffc88080` | matches **none** of the five track-id literals, so no track grip scaling |
| `+0xb20` | **1** on 29/29 | the full-stop block (`== 0` and `speed < 16`) does **not** fire |

A6b `0x00468980` writes no velocity (§26.4, static: 132 instructions, none of `+0x9b0..b8`).
A6a reads `+0x9e0` **twice and writes it zero times** over all 1243 instructions of
`0x00467650..0x0046897b` (capstone scan this session: hits only at `0x00468117` and
`0x00468761`, both `cmp ... 0x40800000`). And between `0x004686ad` and the clamp gate at
`0x0046874c` the only `[esi+...]` store is `0x004686cc mov [esi+0x9e4], edx`.

> **So on the ORIGINAL at `d = 222..250`, `T_post` is grip-clamp #6 and nothing else** — the
> clamp either runs, or is skipped by its own gate and `T_post` is exactly zero.

`R = s_post / s_mid` is a sound ratio for that interval: `+0x9e4` has **exactly two
literal-displacement writers in `.text`** (`0x004686cc`, and `0x0046bc36` in the spawn-init
region) plus A6a's **entry** store `0x00467673 fstp [esi+0x9e4]`, and the folded-base sweep
over 622 511 instructions returns **4 hits, all reads, zero writes**
(`re/tools/fold_sweep.py 0x9e4 0xbf8`, whose known answer on `+0xbf8` **PASSES** —
it recovers attempt 15's `0x0046d7a2 mov [eax+0x882198], 2`). All three writers precede the
clamp, so nothing rewrites `+0x9e4` after it.

> **INSTRUMENT CAVEAT, recorded:** `findoffset.py --writes 0x9e4` reported only the two `mov`
> stores and **missed `0x00467673 fstp dword ptr [esi+0x9e4]`** — a real write. So
> `--writes` is blind to x87 stores as well as to computed bases (memory
> `findoffset-blind-to-computed-bases`). Its `+0x9e0` list below must be read with that
> limitation: it is not a complete writer set.

### 2.2 The snapshot CANNOT decide whether clamp #6's gate is open — A5 zeroes its input

`findoffset.py --writes 0x9e0` returns four literal-displacement stores:

```
0x0046bb8a  mov [ecx+0x9e0], 0x40800000      spawn-init region, = 4.0f
0x0046ddd1  mov [edi+0x9e0], 0               A5 VehicleWheelForceIntegrate 0x0046ddb0
0x0047044b  mov [edi+0x9e0], ebp
0x00470459  mov [edi+0x9e0], 0x3f800000      = 1.0f
```

**A5 `0x0046ddb0` zeroes `+0x9e0` at `0x0046ddd1`, and A5 runs immediately before A6a in the
same frame** (A4 -> A5 -> A6a -> A6b -> substeps). The port does the same:
`ForceIntegrator.cpp:55 self[0x278] = 0;  // grounded count = 0.0` then rebuilds it per
wheel at `:59` (`if (piVar12[0xb] != 0) vF(self,0x278) += 1`). `0x278 * 4 = 0x9e0`.

So the value clamp #6's gate reads at `0x00468761` is the one **A5 rebuilt this frame**, not
the post-substep value the render-tick snapshot carries. **The snapshot's `+0x9e0 == 4.0` on
29/29 frames therefore says nothing about the gate**, and the earlier draft of this section
claimed otherwise. Withdrawn here before anything is built on it.

That makes the gate's state the **live** question, and it is the one quantity that would
explain the original's `T_post` completely:

| | ORIG `s'` | reachable `k` | clamp's demanded `1-R^2` | **MEASURED `1-R^2`** | short by |
|---|---:|---:|---:|---:|---:|
| `d` 200..222 (n=23) | 0.338317 | `>= 0.1249` | `>= 2.68e-02` | **2.3524e-05** | **1 140x** |
| `d` 222..250 (n=29) | 0.044704 | `>= 0.1249` | `>= 3.75e-04` | **1.2194e-05** | **31x** |

(`k >= 0.1249` is structural: §21.9's own measured `ld4 <= 1.11726` over 2164 original
samples and the 1024 cap at `Integrate2.cpp:443` bound `l_60 <= 4 x 1024 x 1.11726 = 4576`,
so `G = l_60*speed <= 3.76e6` at speed 820.5 and the HIGH arm gives
`k = (1e7 - G)*1e-7*0.2 >= 0.1249`; the LOW arm's floor is 0.1.)

The body-heading rotation of §3.1 (ORIG median **1.3986 deg**/frame, max 2.0661) is nowhere
near enough to manufacture the `d = 200..222` figure of **19.8 deg** of snapshot
misalignment after a clamp that had just removed 12.5 % of it.

> **So on the ORIGINAL, grip-clamp #6's velocity stores appear NOT to execute at matched
> `d` — 31x to 1140x below what its own arithmetic demands on its own measured inputs — and
> the candidate producer is its gate `0x00468761`, whose input A5 zeroes at `0x0046ddd1`
> one call earlier.** [UNCERTAIN] until read at A6a's own phase on the running original.
> That is STEP 2.

---

## 3 WITHDRAWN: the snapshot inversion of `(k, s)`. It is invalid on both sides.

`a18_clampinv.py` printed, and §4.3's discriminator consumed, a per-frame inversion
`a^2 = s'^2 R^2 / (1 + s'^2 R^2 - R^2)` giving ORIG `k = 0.011895` / `s_pre = 0.050612` and
PORT `k = 0.302675` / `s_pre = 0.307270`, and it printed
`--> CARRIER: the SLIP s. clamp #6 is innocent.`
**All of that is WITHDRAWN. Three independent reasons, each measured:**

1. **`s'` is not the clamp's post-state.** The inversion requires `s' = |lat'|/|v'|` measured
   with the **same** forward axis the clamp used. The snapshot's axis is not: the body
   orientation is integrated by `0x0046e9e0`, which runs **inside** the substep loop, i.e.
   **after** A6a. Measured per-frame body-heading change at `d = 222..250`:
   **ORIG median 1.3986 deg (max 2.0661), PORT median 1.1130 deg (max 1.2578)** —
   comparable to or larger than the whole misalignment being inverted (ORIG `s' = 0.044451`
   = **2.55 deg**).
2. **It is ill-conditioned on the original and the band proves it.** `1 - R = 6.1e-06` gives
   a `k` band of `0.002179 .. 0.374154` at p10..p90 — a **172x** range — and only
   **15 of 29** frames admit a real solution (the 14 with `R > 1` do not). A band that wide
   contains the port's value by construction, which is the only reason §4.3's `k` leg
   printed "within": its nearer-edge rule returned the port's own number and a ratio of
   exactly 1.0000. **The `k` leg was UNINFORMATIVE, not passed.**
3. **Medians of a nonlinear inversion do not satisfy the relation.** The per-frame round-trip
   passed 100 %, but the reported medians do not: PORT `s_pre = 0.307270` with
   `k = 0.302675` gives `s^2(2k-k^2) = 0.04844` against the measured `1 - R^2 = 0.074031`.

Gate INV legs 2 and 3 failed **because the inversion is invalid**, not because the priors are
wrong. The gate did its job. Nothing in this attempt rests on the inverted numbers.

What **survives** from the tool: `R` (magnitudes only), and `s'` as a **snapshot-phase**
quantity — which is all §5 needs.

---

## 4 §23.2's attribution of the port's sink to grip-clamp #6 is REFUTED, with the port's own
## numbers, through clamp #6's own formulae

The port logs the clamp's inputs per wheel. `l_60 = sum min(le4, 1024) * ld4` over the four
wheels (§21.9/§26.1) is computable from `motion_diag.log`'s own `wle4=[..] wld4=[..]`
(`Integrate2.cpp:434` / `:449`, accumulated `:473`), and `grip == l_60` because `+0x18c` is
1.0. Feeding that through the transcribed arms —
`G >= 32768: k = max(0,(1e7-G)*1e-7)*0.2` ; `G < 32768: k = max((32768-G)*2^-15, 0.1)` —
at matched `d`:

| window | n | PORT `l_60` med | `G = l_60*speed` med | arm | PORT `k` med | `s'` med | **clamp's cost `s'^2(2k-k^2)`** | **MEASURED `1-R^2`** | short by |
|---|---:|---:|---:|---|---:|---:|---:|---:|---:|
| `d` 200..222 | 23 | 400.43 | 139 952 | **HIGH 23/23** | 0.197201 | 0.266725 | 2.5355e-02 | 4.0518e-02 | **1.6x** |
| **`d` 222..250** | **29** | **676.82** | **458 880** | **HIGH 29/29** | **0.190822** | **0.144996** | **7.2038e-03** | **7.4031e-02** | **10.3x** |
| `d` 250..260 | 11 | 392.17 | 171 756 | **HIGH 11/11** | 0.196565 | 0.082892 | 2.4459e-03 | 1.7270e-01 | **70.6x** |

> **In the defect window grip-clamp #6, fed with the PORT's own measured `l_60` and its own
> speed, accounts for 7.20e-03 of a measured 7.40e-02 — one tenth of it.**

And the gap cannot be the forward-axis phase shift of §3.1: closing it would need
`s = sqrt(7.4031e-02 / (2k - k^2)) = 0.4631` at the clamp (**27.6 deg**) against a snapshot
`s'` of **8.34 deg**, i.e. **14.6 deg** of body rotation between the clamp and the snapshot,
against a measured **1.11 deg** per frame. A factor of **13**.

§23.2 concluded "*Only code between the two reads on a non-contact frame = grip-clamp #6
(W2/W3)*" and routed the whole 14.7 %/frame loss there **by elimination**. That elimination
is now contradicted by the clamp's own arithmetic. **The port's `k` is also not the
divergence**: 0.190822 against the original's structurally-bounded range (any `l_60 <= 4576`,
from §21.9's own measured `ld4 <= 1.11726` and the 1024 cap, gives `G <= 3.76e6` and hence
`k >= 0.1249` on the HIGH arm) — the two sides' `k` are within **1.5x**, not 98x. And the
port is on the **HIGH** arm on 29/29 frames, so §26.3's LOW-arm `k = 0.540954` does not
describe it at matched `d` either.

> **NEXT TERM, NAMED AND UNIDENTIFIED: a velocity sink in the PORT between the `+0x9e4`
> store at `0x004686cc` and the render-tick snapshot that carries ~90 % of `T_post`
> (6.68e-02 of 7.40e-02 at `d = 222..250`, n = 29, median speed 653.0) and is NOT
> grip-clamp #6.** [UNCERTAIN] — it is bounded in size and located in the frame, and its
> producer is not identified. On the original that interval contains clamp #6 and provably
> nothing else (§2.1); on the port it contains clamp #6, A6b `0x00468980`, A4's tail parked
> damp `0x00470948` (`VehicleControl.cpp:278-282`, gate `+0x9f0 == 2`), and **3 or 4**
> substep iterations instead of 2 (U-9160, `VehiclePhysicsRun.cpp:915`) each of which can
> reach `VehicleContactFixup`'s three anchored velocity writes (`ContactFixup.cpp:261-265`
> `0x0046f52c`, `:307-309` `0x0046f5ba`/`0x0046f5c0`, `:326-328` `0x0046f5f3`).
> **The port's `+0x9f0` and `+0x9ec` are not in `motion_diag.log`, so which of those fires
> cannot be decided from the committed captures.** That is the port-half diagnostic
> `PREREG_STEP1B.md` §6 registered, and it is the first thing the next attempt should build.

---

## 5 U-9173 RESOLVED — the registered prediction was MET

`PREREG_STEP1B.md` §5 registered, before the run: *the ORIGINAL's `s'` at matched
`d = 222..250` is below 0.05.*

> **Measured: `s' = 0.044451`** (n = 29, median speed 820.5, `d` = 222..250). **MET.**
>
> At band 100-150 the same tool measures the original's `s' = 0.727146` (n = 45, median
> speed 126.2, **median `d` = 133**) — reproducing §26.4's **0.729** (n = 45) to three
> digits.

§26.4's three-way inconsistency was: (i) LOW arm so `k = 0.1`, (ii) `s = 0.729`,
(iii) ratio 0.999991..1.000020; (i)+(ii) predict a 5.5 %/frame cut and (iii) measures
0.002 %. **It is a banding artefact.** Band 100-150 on the original is a **single moment** —
median `d` **133**, the post-trough recovery where the car genuinely slides at 43 deg — while
the 0.002 % ratio is the median over the whole 2331-frame race, almost all of which is
straight driving at `s' ~ 0.04`. The two numbers describe different populations and never had
to agree. Both `s'` figures are the **same snapshot-phase quantity** measured by the same
expression, so the comparison is like-for-like and §3's phase caveat does not touch it. This
is §26.9's own OFF-REGIME finding applied to §26.4's own number — the failure mode memory
`band-on-speed-compares-different-moments` names.

**U-9173 RESOLVED** via `re-classify`, mechanism named, pre-registered prediction met.

---

## 6 One non-faithfulness found en route, named and NOT fixed

`Integrate2.cpp:666` evaluates clamp #6's dot product in a different x87 association order
from the original:

```
ORIGINAL   0x00468771..0x00468793   dot = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z
PORT       Integrate2.cpp:666       dot = (fwd.z*vel.z + fwd.x*vel.x) + fwd.y*vel.y
```

The **same deviation U-9176 records for A4's `+0xb0c` dot at `0x004706db..0x00470701`**, at a
second RVA. §21.5's byte-faithfulness audit covered "both arms, all six constants, the two
floors, the `1 - k`, the early return and the full-stop block" — the lateral construction at
`0x00468771`, which precedes the arm split, was outside that scope. Float-rounding sized;
it cannot be a factor of 10. Not fixed here: claiming it needs the full promotion leg on A6a
`0x00467650`. Recorded against U-9176 with both RVAs and the file:line.

---

## 7 Where this leaves the attempt

- `T_post` is the diverging term (STEP 1), and a **standing** divergence from at least
  `d = 200`, not an onset at `d = 222`.
- Its size is exact: **1.2194e-05 (ORIG) against 7.4031e-02 (PORT)**, factor **6 072**,
  n = 29 each, median speed 820.5 / 653.0, `d = 222..250`.
- On the **original** that interval is clamp #6 and provably nothing else (§2.1) — and the
  clamp's velocity stores appear **not to execute** there, 31x to 1140x below what its own
  arithmetic demands, with its gate `0x00468761` the candidate and A5's `0x0046ddd1` the
  reason the snapshot cannot decide it (§2.2).
- On the **port** clamp #6, through its own measured `l_60` and the transcribed arms,
  accounts for **one tenth** of it (§4). Both sides are on the **HIGH** arm with `k` within
  **1.5x**. **§23.2's attribution is refuted and `k` is not the carrier.**
- The snapshot inversion of `(k, s)` is **withdrawn** (§3), with the measured reason.
- **U-9173 RESOLVED** (§5), by a pre-registered prediction.
- **No fix authored.** `PREREG_STEP1B.md` §4.2's blocking condition is in force, and the
  surviving ~90 % sink has no identified producer.
