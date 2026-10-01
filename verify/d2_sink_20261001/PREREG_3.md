# D2 attempt 12 — STEP 3 pre-registration: split the per-frame budget into PRE-A6a and IN-A6a on BOTH sides, then name the divergent writer by RVA

Written and committed **BEFORE** any reduction or run. Base HEAD `6fc2dfd0`. Never amended.

## 0. Why the target moved

Step 2 (`RESULT_STEP2.md`, R3) withdrew §24.3's ORIGINAL `resid` column. Both halves of the
per-frame speed budget can now be measured **directly on both sides**, with no estimator in the
pre-A6a half:

```
  C - A   ==   (B - A)        +   (C - B)
  total       PRE-A6a             IN-A6a
  A = |snapVel(f-1)|   B = |velocity at A6a ENTRY, frame f|   C = s_mid(f) = +0x9e4 post-W1
```

- ORIGINAL `B`: `--fixup-probe` site 2, entry hook `0x00467650`, join proven bit-exact.
- PORT `B`: `g_a6aFrame.vel` in `a6a_dump.log` (`Integrate2.cpp:235-239`), `act.vel=`.
- `A` and `C`: the render-tick snapshot on both sides.

Step 2 showed `B - A` agrees across sides (0.92x..1.16x over 14x in speed). **So if anything
diverges per frame, it is `C - B`, inside A6a.**

## 1. The five candidate store sites inside A6a, by RVA

Read this session from `MASHED.exe.unpatched` with `re/tools/disasm_va.py` — every `+0x9b0`,
`+0x9b4` or `+0x9b8` store between `0x00467650` and A6a's last `RET` at `0x0046897b`:

| site | RVA(s) | port counterpart |
|---|---|---|
| **V1** | `0x00467aee` / `0x00467afc` / `0x00467b08` | `Integrate2.cpp:215-216` (the `trackId == -1 \|\| -0x373738` scalar `f7`) |
| **V2** | `0x00468692` / `0x0046869e` (+ the `0x9b0` store in the same block) | `Integrate2.cpp:630-632` — W1 |
| **V3** | `0x00468840` / `0x00468854` | clamp region |
| **V4** | `0x004688d4` / `0x004688e8` | clamp region |
| **V5** | `0x0046894e` / `0x00468954` (`MOV ...,EBX`) | zeroing / late guard |

`+0x9e4` is written twice, `0x00467673` (entry, `=\|vel\|`) and `0x004686cc` (post-W1, the `.msd`'s
`s_mid`). **So `C - B` covers V1 and V2 only; V3/V4/V5 are after `+0x9e4` is latched** and cannot
show up in `C - B`. That is a pre-registered consequence, not a later excuse: if the divergence
is in V3/V4/V5 it will appear as a disagreement between `C` and `|snapVel(f)|` instead, which
§23.4's invariant `median(+0x9e4 / |+0x9b0..b8|)` already measures (ORIGINAL 0.999998, PORT
1.172734). **That invariant is therefore the second observable of this step, not an afterthought.**

## 2. Measurements

Banded by `A` with `a11_accum.BANDS`, all-four-grounded on both frames, **n >= 10 on both sides**,
and the per-band median speeds are reported next to every number.

- **S0** `total = C - A`, both sides.
- **S1** `dPre = B - A`, both sides (re-reported from step 2 for the port's new run).
- **S2** `dIn = C - B`, both sides. **The headline.**
- **S3** `residA = C - |entryVel + w|`, `w = linTerm*(ctrl + accum)` with the `a11_accum`
  estimator, both sides.
- **S4** `ratio9e4 = C / |snapVel(f)|`, both sides, per band (§23.4's invariant, band-resolved
  for the first time).

## 3. Gates

| gate | bar | role |
|---|---|---|
| **T-C** known-answer | PORT `median(\|residA\|) <= 0.01` in every scored band, reproducing §24.3's `-0.0000` | if it fails the estimator pipeline is broken in this run -> **STOP**, nothing is used |
| **T-A** | per band, `dIn` ORIG/PORT ratio inside **[0.5, 2.0]** | decides whether anything diverges inside A6a |
| **T-B** | per band, `total` ORIG/PORT ratio inside **[0.5, 2.0]** | decides whether the per-frame budget diverges at all |
| **T-D** | per band, `ratio9e4` ORIG vs PORT, difference **<= 0.01** | decides whether V3/V4/V5 diverge |

## 4. Decision rule

- **R1.** T-C fails -> **STOP**, report, no number used.
- **R2.** T-C passes and **T-A, T-B and T-D all pass in every scored band** -> **the per-frame
  velocity budget AGREES across sides at matched speed, and there is no sink to name.** Say so
  plainly, list what that rules out, and recommend the next lane. **No fix.**
- **R3.** T-A fails in one or more bands -> the divergence is inside A6a between entry and the
  `+0x9e4` latch, i.e. **V1 or V2**. Decompose by S3: if `residA` ORIG/PORT also diverges, it is
  **not** W1/accum/curv and V1 is the candidate; if `residA` agrees and `dIn` diverges, it is
  **V2 / W1** and the divergence is in `ctrl` or `accum`, both already closed by §24.6 routes
  11-12, which would be a contradiction to report rather than a term to name.
- **R4.** T-A passes and T-D fails -> the divergence is **V3/V4/V5**, after the `+0x9e4` latch,
  i.e. the trailing clamp region, and §23.4's 1.172734 is its signature.
- **R5.** T-A passes, T-D passes, T-B fails -> arithmetic contradiction; report it and stop.

### Naming bar, fixed in advance

A writer is **NAMED** only when all three hold:

1. It is an RVA that stores `+0x9b0`, `+0x9b4` or `+0x9b8` inside A6a (one of V1..V5), read from
   `MASHED.exe.unpatched`, with the exact store address cited.
2. Its **gate condition is checked on the running original** — not inferred from the
   decompilation — through an entry-only hook or an existing captured channel, with a count.
3. Its measured per-band magnitude reproduces the divergence to within a **factor of 2** in every
   band where the divergence exceeds the T-A bound.

If any of the three is missing the step reports **"no sink named"** and says which bar failed.

## 5. Hazards this step must actively check, named in advance

`offset-grep-misses-dword-index` (a `[0x17]`-style index hides a `+0x9b0` write — the RVA list
above is from a disassembly sweep of the whole function, not a source grep);
`capstone-sweep-stops-at-bad-byte` (the sweep is bounded by A6a's own `RET` at `0x0046897b` and
the store list is cross-checked against the port's known write sites);
`zero-of-n-needs-a-coverage-check` (any "never fires" claim needs a fired-count from the same
run); `decomp-is-silent-about-register-args` (A6a's record is in **ESI**, confirmed by
disassembly, not by the decompiler's signature); `rate-stats-per-sample-not-totals` (every
statistic here is a per-frame median inside a speed band, never a total).

**No branch of this rule authorises a source fix on its own.** A fix needs the naming bar met
first, and then the promotion leg of `fix-briefs-carry-a-promotion-leg`.
