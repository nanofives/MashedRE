# PRE-REGISTRATION — D2 attempt 17, STEP 2B: a second, different known-answer check

Written and committed **before it is run**. This is **not an amendment of gate KA**.
KA's verdict stands as recorded and is reported as a FAIL on both routes:

| route | instrument | best result | threshold | verdict |
|---|---|---|---:|---|
| snapshot | `orig_bp1.msd` | lam -1, **0.948789** | 0.99 | **FAIL** |
| snapshot | `orig_bp2.msd` | lam -1, **0.940083** | 0.99 | **FAIL** |
| snapshot | `orig_solo3.msd` | lam -1, **0.945329** | 0.99 | **FAIL** |
| live | `orig_sl1.msd.slideprobe.csv`, A4-entry cross-call | **0.971698** (2266/2332) | 0.99 | **FAIL** |

KA is retired here. Nothing it rejected is re-admitted by this file.

## 1 Why a different question is legitimate

KA scored `rel = |pred - stored| / max(|stored|, 1e-3)`. The quantity under test is

```
+0xb0c = (1.0 - |dot| / speed) * speed        i.e. algebraically  speed - |dot|
```

a **difference of two near-equal float32 numbers**. KA's own worst miss is the proof:
`seq = 938`, `speed = 757.258`, `pred = 0.00055225714`, `stored = 0.00054163329`. The
absolute disagreement is `1.06e-5` on operands of magnitude `757`. One float32 ulp at
`757` is `2^-24 * 757 = 4.51e-5`, so that "miss" is **0.24 of a single ulp of the
operands** — it is the subtraction's own cancellation, not a law mismatch.

A relative-to-the-result tolerance cannot distinguish "the law is wrong" from "the
result has no significant figures left". So KA asked a question that this quantity
cannot answer, which is why it failed on **both** routes including the live one. That is
a fact about `+0xb0c`, and it is reported as such.

**Disclosure:** the `seq = 938` numbers above were produced by the failed KA run and were
visible before this file was written. The budget in §2 is set from the arithmetic, not
from them, and §3 reports the raw distribution so the threshold cannot hide anything.

## 2 GATE KA2 — scale-aware known-answer check. NOT AMENDABLE.

On the **live** `--slide-probe` rows only (the snapshot route stays void):

```
pred_i   = 0.0 if speed_i == 0.0 else (1.0 - |dot_i| / speed_i) * speed_i
stored_i = row (i+1)'s b0c_entry          (consecutive seq only)
ulp_i    = 2^-24 * max(speed_i, 1.0)      one float32 ulp of the writer's own operands
hit      iff |pred_i - stored_i| <= 4 * ulp_i
```

`4` ulps is the budget for: the original evaluating in x87 80-bit and rounding once at
the `fstp` store, this tool recomputing in float64, and the `fdiv`/`fsubr`/`fmul` chain's
three roundings. It is a standard conservative figure, not a fitted one.

**KA2 PASSES** iff the hit fraction is `>= 0.99`. **KA2 FAILS** otherwise, and then
STEP 2 produces **no DR verdict this attempt**, STEP 3 is not entered, and the attempt
reports exactly that.

## 3 Reported unconditionally, pass or fail

- `max_i |pred_i - stored_i| / ulp_i` over all scored rows, and its `seq`, `speed`, `d`.
- The same ratio's median and its 99th percentile.
- `n`, median speed and `d` for every one of them.
- A split by regime: rows with `speed == 0.0` (the `0x0047072c` branch) counted
  separately from rows with `speed > 0`.

## 4 What KA2 does NOT license

KA2 confirming the law at the writer's phase says **nothing** about whether `+0xb0c`
diverges cross-side, by how much, or why. Gates DR and CB of `PREREG_STEP2.md` are
unchanged, still binding, and are what decide that. KA2 only decides whether the live
probe's rows may be used as input to them.

A further consequence is recorded in advance, so it cannot be claimed as a discovery
after the fact: if KA2 passes, then `+0xb0c`'s **precision** at speed `s` is about
`2^-24 * s` in absolute terms, so a `+0xb0c` reading of magnitude `b` carries a relative
precision of roughly `6e-8 * s / b`. At `s = 757, b = 5.5e-4` that is **~8 %**; at
`s = 85, b = 44` it is **1.2e-7**. Any cross-side `+0xb0c` comparison must therefore be
read against that bound at the `d` where it is made, and gate DR's comparison of the
**inputs** rather than of `+0xb0c` itself is what sidesteps it.
