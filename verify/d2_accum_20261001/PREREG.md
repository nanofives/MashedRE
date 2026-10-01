# D2 attempt 11 — PRE-REGISTRATION: measure the ORIGINAL's force accumulator

Written and committed **BEFORE any run of `a11_accum.py` on either side**. Branch
`race/first-frame-parity`, base HEAD `09324f08`. **This file is never amended.** If a threshold
fails, the failure is reported as written.

## 0. The lane, and why it is the one left

§23.4 of `re/analysis/D2_REOPEN_2026-09-29.md` closes ten routes and leaves exactly one unmeasured
term in the free-flight per-frame velocity budget: the **ORIGINAL's `T_rest`**. It reaches
**−42.586/frame** right after the bounce and decays **6x at constant speed** (−34.508 at `s_from`
190.6 → −5.475 at 193.5), against the port's **−0.43/frame** in the same band. `T_drive`,
`linTerm`, a hidden vertical force, grip-clamp #6's code, the fixup impulse, the substep chain,
`+0x9e4`'s write order, the `l_60`/`ld4` lane, the contact cadence and the gain framing are all
closed. `T_rest` is the only home left for the difference.

`T_rest` is carried by exactly one quantity: A6a's force accumulator `accum`
(`l_b8` / `l_b4` / `lin_b0`), the second operand of W1. It is an A6a **local**, so the `.msd`
cannot carry it — but every one of its **inputs** is a record field the snapshot already has.

## 1. The estimator, registered by content

`re/tools/statediff/a11_accum.py`, committed with this file. It replays, in float32 (the widths
the original uses — every stack slot in the range is `fstp dword`, U-A6A-FLOAT10 resolved):

| what | original RVAs (read from `original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`) | port |
|---|---|---|
| cross-product friction block #5, per wheel × 4 | `0x0046833a .. 0x00468544` | `Integrate2.cpp:489-512` |
| `m78` + `frac` + the blend | `0x004685b2 .. 0x00468625` | `:543-565` |
| W1, the only consumer of `accum` | `0x0046862d .. 0x004686a2` | `:630-632` |

Instruction-level anchors inside block #5: gate `0x0046833e` `call 0x4c3ac0` + `fcomp [0x5cd03c]`
(1e-4); `inv = 1/wm` `0x00468357` `fld [0x5cc320]` / `0x0046835d` `fdiv [edi-0x2c]`; `off` reads
`0x00468360` / `0x00468369` / `0x00468372`; `F` reads `0x00468388` / `0x00468391` / `0x004683bf` /
`0x004683ca`; `a*wm` `0x0046849e` / `0x004684a9` / `0x004684b4`; `kNormAccum` `0x004684c0`
`fmul [0x5cea04]` = 50.24 (`0x4248f5c3`); `l_d0` accumulate `0x004684cf` / `0x004684d3`; `m16` gate
`0x004684dc`; `r` `0x004684ec` `fdivr [esp+0xc]`; `l_78/74/70` `0x004684f0 .. 0x00468530`; stride
`0x0046853b` `add edi,0xc4` off base `self+0x1a4`. In the blend: `m78` `0x004685b2` / `0x004685b7`;
`l_d0 - m78` `0x004685bc`; `== 0.0` test `0x004685c3` / `0x004685ce`; `frac` `0x004685d0`
`fdiv [esp+0x20]`; blend `0x004685d4 .. 0x00468621`. In W1: `linTerm` `0x0046865b`
`fmul [esi+0x54]` / `0x0046865e` `fmul [0x5cc948]` (1/3000); `speed` `0x004686a4`; `+0x9e4` store
`0x004686cc`.

### Where every input is read — NO hook, NO probe, NO source change on either side

The estimator consumes **only record fields at the render-tick snapshot phase**, the channel both
sides already have (`.msd` on the original, `MASHED_A6ADUMP`'s `s.*` fields on the port). There is
therefore no instrumentation to register: no entry hook, no by-pointer callee argument, and in
particular no mid-function probe. Per wheel `w`, with `b = 0x1a4 + w*0xc4`:

```
wm   b-0x2c                       off  b-0x24 / b-0x20 / b-0x1c
F    b+0x70 / b+0x74 / b+0x78
```

plus `+0x9b0..0x9b8` (vel), `+0x9e4` (`s_mid`), `+0x9e0` (grounded), `+0x54`, and
`+0xb14` / `+0xb18` / `+0xb1c` (ctrl).

### Frame alignment

- **Port**: `friction_diag.log` line `N` ↔ `a6a_dump.log` `f=N`, both 1-based, same A6a call.
  Registered as a **hypothesis to be tested** by KA1-a below, not assumed.
- **Original**: the `.msd` is keyed by `frame_idx`; steps are consecutive captured records.
- **Cross-side**: no frame pairing. Comparison is **speed-banded** on the step's from-speed
  `s_from = |+0x9b0..b8|(f-1)`, bands `70-100 / 100-150 / 150-260 / 260-500 / 500-1000 /
  1000-1500 / 1500-2000` (plus `40-70`, reported but not comparable, original `n=2`), and
  **input-matched** by both captures being on the §16.7 arm (`MASHED_MEASURE_SOLO=1`,
  `MASHED_TRACK_SEL=12`, `MASHED_STEER_HOLD_AFTER=0`, drive held).
- Bounce alignment is **not** used as a selector; §23.2 showed a window spanning the bounce mixes
  two regimes. The per-frame banded form is the reportable one. The post-bounce frames are
  reported separately as a table, not as a statistic.

## 2. Safety thresholds (STOP on failure — reported as written, never amended)

### S1 — KA1, the known-answer check on the PORT. Fail ⇒ STOP, no original-side run.

Truth: `friction_diag.log`'s verbatim `accum / cMag / fMag / ld0 / m78 / frac`
(`Integrate2.cpp:614-628`). Scope: grounded frames (`+0x9e0 == 0x40800000`) with `fMag > 1.0`
(excludes the stationary pre-race line).

- **KA1-a (join uniqueness).** Median relative error of `fMag` must be **< 1e-5 at offset 0**, and
  **> 1e-2 at every other offset in −4..+4**.
- **KA1-b.** Median relative error of `cMag`, `fMag`, `ld0`, `m78` each **≤ 1e-4**, p95 **≤ 1e-3**.
- **KA1-c.** Median absolute error of `frac` **≤ 1e-4** (`frac ∈ [0,1]`, so absolute).
- **KA1-d.** Median relative error of `|accum|` **≤ 1e-3**, p95 **≤ 1e-2**; median cosine between
  estimated and true `accum` **≥ 0.9999**.

Rationale for the bars: the estimator is a structural transcription, so the only error sources are
float32 rounding and the x87 80-bit transient (≤ 1 ULP) — expected ~1e-7. The failure these bars
actually test is a **phase** error (the snapshot `F` not being what block #5 read), which is O(1),
not 1e-4. Three orders of margin is deliberate.

### S2 — KA2-control, the second sampler, on the PORT with the TRUE accum. 

`pred_s_mid(f) = |v_post(f-1) + linTerm*(ctrl(f) + accum(f))|` against the recorded `+0x9e4(f)`.
Statistic: **median** `|pred − s_mid| / s_prev` over grounded steps with `s_prev > 1e-3`.
Detector-free by construction — the fixup touches ≤ 13% of port frames (§23.2), so the median is
immune.

- Bar: **median ≤ 1e-3** using `friction_diag.log`'s verbatim `accum`.
- **Fail ⇒ KA2 is void as an instrument on both sides.** Then there is no witness that the
  estimator transfers to the original's snapshot phase, so **STOP: no input may be named**, report
  the lane as closed-without-a-named-input, and recommend the entry-hook fallback (§23.4's
  `RwV3dLength`-argument technique on A6a's cross-product callee `0x4c3ac0`).

### S3 — KA2-est and KA2-orig.

- **KA2-est (port, estimated accum)**: median `|pred − s_mid| / s_prev` **≤ 1e-3**.
- **KA2-orig (original, estimated accum)**: median **≤ 1e-3**.
- **Fail of either ⇒ STOP, no input named.** KA2-orig failing is the specific case
  memory `cross-side-fit-needs-both-sides-checked` warns about: the estimator validated on one
  side and not transferring to the other.

### S4 — coverage.

≥ **200** scored steps on each side overall, and ≥ **30** on BOTH sides in at least one band. Any
band with `n < 30` on either side is reported **UNSCORABLE** and may not carry the verdict.

### S5 — the channel can see a difference, and the estimate explains the target quantity.

The estimator's first-order prediction `pred = A + frac*C` (definitions in §3) must reproduce the
**independently measured** `T_rest = (s_mid − s_prev) − linTerm*(ctrl·u)` — a quantity computed
from `+0x9e4` and `+0xb14..1c` only, which the estimator never touches:

- Bar: in every band with `n ≥ 30`, `median(pred) / median(T_rest)` must lie in
  **[0.75, 1.33]** on **both** sides.
- This is also the "can the channel see a difference" control: §23.2 measured `T_rest` at
  **−9.6704 (ORIG)** against **−0.4307 (PORT)** at 150-260, a 22x separation in the very quantity
  S5 pins the estimator to. If the estimator reproduces both, it can see that difference.
- **Fail ⇒ the estimator does not explain `T_rest`; no input may be named from it.** Report
  which side and band failed.

## 3. The decomposition, and the DECISION RULE

With `u = v_post(f-1)/s_prev`, `Sc = (l_b8,l_b4,l_b0)` (normal part),
`Sf = (l_6c,l_68,l_64)` (total), `tan = Sf − Sc` (the tangential/friction part):

```
accum  = Sc + tan*frac                       frac = (l_d0 - m78)/l_d0
T_rest ~= linTerm*(Sc.u)  +  frac * linTerm*(tan.u)   =   A + frac*C
```

Evaluated on the **best-populated band with `n ≥ 30` on BOTH sides**; every other qualifying band
is reported alongside and must agree in direction.

- **R1.** If **exactly one** of `{A, frac*C}` differs cross-side by a factor outside
  **[0.5, 2.0]** while the other is inside **[0.8, 1.25]** → **name that term**, go to R3.
- **R2.** If `frac*C` is the divergent one, split it:
  - `frac` outside [0.5, 2.0] → **name `frac`**, then R4.
  - else `C` outside [0.5, 2.0] → **name the tangential force `tan = Sf − Sc`**, i.e. the
    per-wheel force field `F` at record `wheelbase+0x70..0x78` (original writers
    `0x00467f53..0x00467f70` brake and `0x00468324..0x00468337` suspension) minus its radial part.
- **R3. Materiality.** The named term must account for **≥ 50%** of the measured `T_rest` gap in
  that band. If < 50%, report it as *a* divergence, state explicitly that it does not close the
  gap, and **do not author a fix on it alone**.
- **R4.** If `frac` is named, report `l_d0` and `m78` separately and name whichever carries the
  ratio. `l_d0`'s inputs are `|a*wm|` (block #5, `0x0046849e..0x004684c0`); `m78`'s are the
  torque accumulator `l_78/l_74/l_70` (`0x004684f0..0x00468530`).
- **R5. REFUSAL.** If the named term traces back to grip-clamp #6's code, to `ld4`/`l_60`, to the
  slip angle itself, or to any of the ten routes §23.4 closed → **no fix is authored**; report
  that the lane lands back on a closed route.
- **R6. CLOSES WITHOUT A TERM.** If no term is outside [0.5, 2.0]; or more than one is and no
  split in R2/R4 resolves it; or S4 fails in every band → **the lane closes without a named
  input.** Report what it ruled out, recommend the next lane, author **no** speculative fix and
  **no** deliberately unfaithful knob.

### Step 3 gate, before any code is written

A named input must be traced to its **writer by RVA** and the hypothesis tested against the
**running original** before anything is coded. NO-GUESSING. Specific traps to clear, each by name:
dword-index writers invisible to an offset grep (`offset-grep-misses-dword-index`); capstone
`disasm()` halting at the first bad byte (`capstone-sweep-stops-at-bad-byte`); wrong Ghidra
arities; register-argument contracts the decompiler hides (`decomp-is-silent-about-register-args`,
`caller-saved-register-contract`).

## 4. The fix's target invariant (unchanged from §23.4, never renegotiated)

> `median( +0x9e4 / |+0x9b0..0x9b8| )` over port race frames == **1.000** to **1e-3**.
> ORIGINAL **0.999998** (n=1447). PORT **1.172734** (n=1628).

Any fix goes in **one** body at the RVA, in a TU listed in both `exe_sources.rsp` and
`asi_sources.rsp`, then `run_diff.py` + `run_verify_hook.py` (or `re/CONFIDENCE.md`'s
non-repeatable-function clause), promoted through `re-classify` with only what was earned, with
the dual-copy guard at `NEW=0`.

## 5. The scored arm (bounds NEVER change)

`MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`, §16.7 arm `MASHED_STEER_HOLD_AFTER=0`, 3 runs,
against the unchanged `d81a8df6` bounds:

| metric | PASS interval |
|---|---|
| slip 1500-2000 | 0.18855 .. 0.19635 |
| slip 2000-2600 | 0.24488 .. 0.25487 |
| driving-median | 1904.70 .. 1982.44 |

`MASHED_MEASURE_SOLO` taking effect is confirmed by the game log reporting `participants=1`.

## 6. Data

- **PORT**: `verify/d2_gain_20261001/p1/` — `a6a_dump.log` (1629 frames, `s.*` = the snapshot
  channel), `friction_diag.log` (1629 lines, the truth), `player_trace.log`, `fixup.log`.
  Provenance in that directory. **No new port run is needed for S1/S2/S3-est.**
- **ORIGINAL**: `verify/d2_bounce_20260930/orig_fp2.msd` (2332 frames), the §23 capture. **No new
  original run is needed.** `original/` is not modified.
