# D2 attempt 14, STEP 2C — PRE-REGISTRATION: the discriminating DIAGNOSTIC

Written and committed **BEFORE any build and before any run**, after STEP 2B's **GF failed**.
HEAD at registration: `f643b522` + the step-2B result commit. **Not to be amended.**

## 1. Why, and what question this answers

Three instruments in this attempt have now returned *"there is no overlap"*:

| instrument | result |
|---|---|
| §26.9's regime count (attempt 13) | the two sides share **5** frames of common regime |
| STEP 2A's cross-side banded collateral | **0** readable rows; all 7 bands of all 45 fields `!!`-flagged |
| STEP 2B's state-matched response test | **0** readable buckets (`GF` FAILED) |

STEP 2B's occupancy table says why, in the window where both arms carry the **identical
saturated steer angle** (`d` in `[119,300]`, 182 grounded frames each): the ORIGINAL spends
**1 of 182** frames at `cos(fwd,vel) < -0.1` and the PORT **86 of 182**; the ORIGINAL is above
150 speed on **141 of 182** and the PORT on **0 of 182**. **The two arms occupy disjoint
regions of state space after the first bounce**, so no cross-side comparison in that regime can
have power, however it is instrumented.

Two live hypotheses remain, and they have opposite consequences:

> **H1 — the LINE.** The port's non-recovery is downstream of the 15-frame drive-force latency
> (STEP 2A §2c). Because `+0xb14` engages 15 frames early, the port carries
> `15 * 0.141113 = 2.12` degrees **less** steer at matched speed through its whole approach, so
> it enters the wall on a different line and into a basin it cannot leave. If H1 holds, **U-9174
> is on D2's critical path** and the recovery is not a separate defect.
>
> **H2 — a separate RECOVERY defect.** The port would fail to recover even entering the wall on
> the original's line. If H2 holds, **U-9174 is cosmetic for D2** and the next lane is the
> recovery physics itself.

STEP 2B could not separate them, because it has no overlapping state to measure in. **This step
separates them by experiment.**

## 2. The instrument: a labelled, default-OFF, fitted-trigger DIAGNOSTIC

The port already carries the original's `+0xbf8 == 2` arm **byte-faithfully**
(`mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:365-372`, from
`0x00467def`..`0x00467e44`). What is missing is only the **trigger** — and that is exactly what
**U-9174** records as unlocated.

> **`MASHED_D2_BOOSTHOLD=1`** (default OFF, absent from every scored run) performs **one write
> per car per race**, on the first frame that car consumes a nonzero steer byte
> (`input[0] != 0`): `+0xbf8 = 2`, `+0xbf4 = 3000`. Nothing else. The 15-frame hold that
> follows is produced entirely by the **original's own transcribed arm**, not by the knob.

**This is a FITTED TRIGGER and is labelled as one everywhere it is reported.** It is **not** a
fix, it earns **no** C-level, it changes **no** tracker row, and it may not be enabled in any
scored arm. Its only claim is: *if the original's hold is present, does the port behave
differently?*

**Hard requirement:** the default build must be unchanged. `mashedmod\build.bat` must complete
with the dual-copy guard reporting **`NEW=0`** (`scripts/lint_rva_bodies.py:120-122`), and the
knob-OFF arm must reproduce `s1/s2/s3` **bit-identically** (gate D-0 below).

## 3. The gates. A gate that fails STOPS this step; none may be amended.

- **D-0 (the default build is untouched).** With the knob **unset**, one run on the §16.7 arm
  must produce a `motion_diag.log` **bit-identical** to `verify/d2_sched_20261001/s1`'s on all
  shared lines. If it is not, the edit perturbed the default build and the diagnostic is void.
- **D-1 (the knob took)** — memory `verify-the-harness-knob-actually-took`. With the knob set,
  the port's `b14[0]` and `b14[2]` must be **exactly 0 on `d = 0..14`** and **nonzero at
  `d = 15`**, reproducing the ORIGINAL's measured signature (STEP 2A §2c). A positive witness
  the game itself prints, not an inference from the numbers moving. If it fails, the diagnostic
  is **void** and nothing below is read.
- **D-2 (determinism).** Three runs with the knob set must agree on all three scored metrics to
  every printed digit, as the port has done on every arm this session.

## 4. The pre-registered readout, and the DISCRIMINATOR

All release-aligned (`R` = first frame with `in[0] != 0`), and every metric carries `n`,
median speed **and** median frame index.

- **M1 — did the latency close?** Best-fit integer lag `L` of the knob-ON port against
  `orig_solo3` over `d = 16..95`, by STEP 2A's method. **Expected `L = 0`** if the hold is what
  produced the 15.
- **M2 — the launch.** Median `|rel err|` of the knob-ON port against the original at `L = 0`
  over `d = 16..95`, against STEP 2A's lagged **1.44 %**.
- **M3 — THE DISCRIMINATOR.** Peak, trough, and over the **400 frames after the trough**: the
  count and fraction at `+0x9e4 >= 100`, the median and the max. Reference points, both already
  measured: **ORIGINAL 1313 of 1346 (97.5 %), median 1828.5, max 2562.8**; **PORT knob-OFF 0 of
  1488 (0.0 %), median 24.7, max 91.4**.

> **H1 is supported** iff, with the knob on, **>= 50 %** of the 400 post-trough frames are
> `>= 100` **AND** the median is **>= 900** (half the original's 1828.5).
> **H2 is supported** iff **< 10 %** are `>= 100`.
> Anything between is **INCONCLUSIVE** and is reported as such. **No third branch is added
> afterwards.**

- **M4 — the three scored metrics on the knob-ON arm**, reported **labelled DIAGNOSTIC**
  against the §3 bounds, which remain **not renegotiable**. **A diagnostic run can never
  re-close D2**, whatever it measures: its trigger is fitted, so it is not evidence that the
  port is faithful. It exists to tell the user, and the next session, where the defect is.

## 5. What happens on each outcome

- **H1 supported** -> report it; **U-9174 moves onto D2's critical path** and the next lane is
  locating the `+0xbf8 = 2` writer (Ghidra data-xref when the MCP is back, or a hardware
  write-watchpoint, which is **not** an `Interceptor` entry hook and needs the user's sign-off
  first). **No fix is authored in this attempt** — the trigger is still unlocated and a fitted
  one may not ship.
- **H2 supported** -> report it; U-9174 is **not** on D2's critical path, and the next lane is
  the recovery physics with the knob available as a line-matching control that finally gives
  the two arms overlapping state.
- **INCONCLUSIVE** -> reported as such, with the three numbers, and neither hypothesis is
  claimed.

## 6. Process

Muted launches, `MASHED_TITLE` on every run, `MASHED_WIN_POS=primary-bl`, never
`MASHED_NAV_DEMO`, own PIDs tracked and only those killed, build via `mashedmod\build.bat` from
PowerShell, `participants=1` confirmed in `mashed_re.log` on every run, trackers only through
`re-classify`, CRLF preserved, commits as `nanofives` with explicit pathspecs, nothing pushed.
