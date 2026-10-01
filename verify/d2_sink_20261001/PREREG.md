# D2 attempt 12 — STEP 1 pre-registration: log the PORT's `G`, `fVar4` and `local_70`

Written and committed **BEFORE** the source change is built or any game is run.
Base HEAD `4e54a209`, branch `race/first-frame-parity`. Never amended.

Follows §24.6 job 1. Inputs that are **not** re-derived: §24.3 (`T_rest` = `T_accum + curv +
resid`, `resid` outside A6a), §24.4 (C1 = A5 `0x0046ddb0` Phase 4; port `l70G` = 0.2342..0.2467
over 8 bands, max/min 1.053; original `G` exactly 0.29025 on 1446/1446).

## 1. What is measured

One **default-OFF** diagnostic line per A5 call, emitted at the end of A5 Phase 4
(`ForceIntegrator.cpp`, after the velocity multiply at `:166-168`), gated on the environment
variable `MASHED_A5GDIAG`. File `a5g_diag.log` in the CWD, append mode, capped at 40000 lines
(§23's 1500-cap regime-filter lesson).

Fields, all read inside A5 from the record `self`:

| field | source | why |
|---|---|---|
| `i` | call counter | ordering only |
| `sEnt` | `\|+0x9b0..0x9b8\|` **at A5 entry**, before Phase 4 writes | lets the log be banded exactly as `a11_drag.py` bands by `s_from` |
| `sp` | `vF(self,0x279)` = `+0x9e4` at A5 entry | C1's own speed input, = `s_mid_prev` |
| `g0 g1 g2` | `vF(0x54) vF(0x55) vF(0x56)` = bytes `0x150/0x154/0x158` | the three factors of `G` |
| `m54` | `vF(self,0x15)` = byte `0x54` | `linTerm = m54*dt*kDt` |
| `dt` | A5's `dt` argument | |
| `base` | `fVar4` as it stands after `:90` | the drive-drag base, half-gate included |
| `l70` | `local_70` after Phase 3 (`:161`) | the quantity §24.4 could only back out |
| `sigma` | `fVar4` after the `:164-165` clamp | the **exact** scalar applied to the velocity |
| `gnd` | `self[0x278]` raw bits | tells whether the `:89` half-gate fired |

This is a read-and-print only. **No value computed by A5 changes**, and the diagnostic block is
entirely inside a `static const bool` env gate, so the default build is byte-identical in
behaviour.

## 2. Output-channel control — run FIRST, and it is a STOP gate

Memory `absent-log-proves-nothing-run-a-control` and `missing-output-channel-fakes-a-red`.

| gate | bar | action if it fails |
|---|---|---|
| **C-ARM** | one armed run (`MASHED_A5GDIAG=1`) produces `a5g_diag.log` with **n >= 500** lines | STOP, report the channel is dead, do not interpret any number |
| **C-CTL** | one unarmed run (variable absent) produces **no** `a5g_diag.log` at all | STOP, report the gate does not gate |
| **C-CNT** | the armed run's line count is within **+/-5%** of the same run's `a6a_dump.log` line count | report the mismatch and the two counts in the RESULT; it does not block, but every per-frame pairing claim must then be dropped |

Both runs use the identical recipe otherwise, same seconds, same track, back to back.

## 3. Known-answer checks against §24.4's back-out

Reference numbers come from re-running `a11_drag.py` **unchanged** on the attempt-11 inputs
(`verify/d2_gain_20261001/p1/`), which this session does not modify.

Banding: the same `BANDS` as `a11_accum.py`, applied to `sEnt`, with the same
all-four-grounded filter (`gnd == 0x40800000` on the row and its predecessor). Only bands with
**n >= 10 on both sides** are scored.

| gate | quantity | bar |
|---|---|---|
| **KA-1** | per band, `median(sigma_logged)` vs `a11_drag.py`'s PORT `median(sigma)` | `abs` diff **<= 1e-3** |
| **KA-2** | per band, `median(l70 * G * half)` vs `a11_drag.py`'s PORT `median(l70G)`, where `half = 0.5` iff `gnd != 0x40800000` | relative diff **<= 0.10** |

`KA-2` is the identity C1 asserts: `l70G == (1-sigma)/(linTerm*s_mid_prev) == local_70 * G * half`.
It is the known-answer check on §24.4's T1 back-out, and it splits the port's `0.2467` into its
two factors for the first time.

## 4. Decision rule

- **R1.** C-ARM or C-CTL fails -> **STOP**. Report the channel failure. No number from this step
  is used anywhere. Step 2 does not start until the channel works.
- **R2.** C-ARM and C-CTL pass, **KA-1 fails** -> the back-out instrument of §24.4 disagrees with
  the directly-logged scalar. **STOP the C1 lane**, record `l70G`/`local_70`/T1/T2/T3 as
  **suspect**, and say so plainly in the RESULT. Step 2 still runs (it does not depend on C1).
- **R3.** KA-1 passes, **KA-2 fails** -> C1 is **not** the whole of the PORT's pre-A6a scalar
  either; §24.4's "T1 confirms C1 is the PORT's entire pre-A6a velocity sink" is **withdrawn to
  the extent of the failing bands**. Record the measured `G` and `local_70` anyway, since they are
  direct reads. Step 2 runs.
- **R4.** KA-1 and KA-2 both pass -> the port's `G` and `local_70` are **measured**, §24.4's
  back-out is validated, and §24.4's `l70G` 0.85 ratio at 1000-1500 is split into a `G` ratio and
  a `local_70` ratio. Report both. Step 2 runs.

**No branch of this rule authorises a source fix.** Step 1 is instrumentation only.

## 5. What this step explicitly does NOT claim

It touches the PORT only. It cannot say anything about the original's sub-500 sink, about U-9170
(speed- vs slip-coupling), or about whether the original's pre-A6a change is a pure scalar. Those
are steps 2 and 3.
