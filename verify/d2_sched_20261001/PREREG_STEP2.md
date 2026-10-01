# D2 attempt 14, STEP 2A — PRE-REGISTRATION: the release-aligned first-divergence scan

Written and committed **BEFORE any reduction run and before any game launch**, after STEP 1
closed **MATCH** ([`RESULT_STEP1.md`](RESULT_STEP1.md), commit `1e49a050`). HEAD at
registration: `1e49a050`. **Not to be amended** — a failing gate stops the lane and is
reported.

## 1. The target, and why it moves upstream of the first bounce

The session directive's step 2 is *"align both sides on the first bounce (§20.14's ~5-frame
window), compare per FRAME INDEX (not speed band), and name the first diverging term by RVA"*.

STEP 1 moved the target **earlier than the first bounce**. With the input schedule now proven
identical release-aligned, §3b of `RESULT_STEP1.md` shows the two arms leave rest at
incomparable rates — ORIGINAL `+0.1158` horizontal units/frame over `R+1..R+3`, PORT `+13.98`,
a factor of **121** — with the same throttle byte, the same steer byte and the same ramp clock.
That is **~120 frames upstream** of any bounce, so a bounce-aligned scan would start downstream
of a divergence that is already present. **The anchor is therefore the RELEASE, not the first
bounce.** §20.14's bounce window remains the registered fallback: if the scan finds no
divergence in tiers 0-4 inside the launch window, the same rule is re-run bounce-anchored and
that is reported as the result.

## 2. The anchor

> **`R` = the first capture frame at which the vehicle consumed a nonzero steer byte.**
> ORIGINAL: the first frame with `+0xb24 != 0` (A4 `0x00470670`'s reset law
> `+0xb24 = (in0 == 0) ? 0 : +0xb24 + dt`, `0x00470737..0x00470746`,
> `VehicleControl.cpp:118-135`). PORT: the first frame with `in[0] != 0`
> (`VehiclePhysicsRun.cpp:1259`).

Every frame is reported as `d = f - R`. Measured in STEP 1: `R_orig = 886`
(`orig_solo3.msd`), `R_port = 1` (`sc1/sc2/sc3`). `R` for `orig_solo4.msd` is computed by the
same predicate, not assumed.

**Window: `d` in `[0, 130]`.** It covers the whole 120-frame steer ramp on both sides (the
original saturates at `d = 119`, the port at `d = 119`) and 11 frames past it.

## 3. The channels, in strict upstream-to-downstream tiers

Every field below is a **record dword on both sides** — the ORIGINAL's from
`verify/d2_reopen_20260929/orig_solo3.msd`, the PORT's from
`verify/d2_l60_20261001/sc1/motion_diag.log`, whose offsets are cited verbatim from
`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1226-1270`. **Port-only quantities are
excluded by name** and may not enter the rule: `susp` (`g_suspScale`), `wle4`/`wld4`
(`g_a8WheelLe4`/`g_a8WheelLd4`), `steer` (`io.steer`), `bodyH` (`io.yaw`), `velH`, `slip`,
`d[]` (`posDelta`), `reseed`, and the derived sums `ftot[]`.

| tier | name | record offsets | port field |
|---|---|---|---|
| **T0** | INPUT (settled by STEP 1) | `+0x1a8`, `+0xb24` | `in[0]`, `in[1]`, `in[2]` |
| **T1** | drive scalars | `+0x490`(i), `+0x494`(i), `+0x478`, `+0x47c`, `+0x480`, `+0x484`, `+0x488`, `+0x48c`, `+0x498`, `+0x49c`, `+0xb0c` | `gear`, `gtmr`, `gt[0..5]`, `gb498`, `gb49c`, `b0c` |
| **T2** | drive accumulator | `+0xb14`, `+0xb18`, `+0xb1c` | `b14[0..2]` |
| **T3** | wheel state | `+0x1a0`(i), `+0x264`(i), `+0x328`(i), `+0x3ec`(i); `+0x1f8`, `+0x1fc`; `+0x210`, `+0x2d4`, `+0x398`, `+0x45c`; `+0x220`, `+0x228`, `+0x2e4`, `+0x2ec`, `+0x3a8`, `+0x3b0`, `+0x46c`, `+0x474` | `fl[0..3]`, `p15`, `p16`, `w1b[0..3]`, `wax[0..7]` |
| **T4** | wheel forces | `+0x214`, `+0x21c`, `+0x2d8`, `+0x2e0`, `+0x39c`, `+0x3a4`, `+0x460`, `+0x468` | `wf[0..7]` |
| **T5** | ground contact | `+0x9e0` | `gnd` |
| **T6** | body angular velocity | `+0x9bc`, `+0x9c0`, `+0x9c4` | `av[0..2]` |
| **T7** | velocity | `+0x9b0`, `+0x9b4`, `+0x9b8`, `+0x9e4` | (`horiz` is derived from `+0x9b0`/`+0x9b8`; `sp` = `+0x9e4`) |

`+0x9b0`/`+0x9b4`/`+0x9b8` are not logged individually by `motion_diag`, so on the port T7 is
represented by `sp` (`+0x9e4`) and `horiz` (`sqrt(+0x9b0^2 + +0x9b8^2)`,
`VehiclePhysicsRun.cpp:1205`); the ORIGINAL's `horiz` is computed from the same two dwords.

## 4. The tolerance

For field `F` at release-relative frame `d`:

> `tol(F, d) = max( floor99(F), 0.02 * |orig(F, d)|, eps(F) )`
>
> - `floor99(F)` = the **p99 of `|orig_solo3(F, d) - orig_solo4(F, d)|`** over `d` in
>   `[0, 130]`, each capture release-aligned by §2's predicate. This is the ORIGINAL's own
>   same-arm noise floor **on this exact window**, not a floor inherited from another window.
>   The PORT's floor is **exactly 0** (§26.1: 205 of 205 fields bit-identical on 1627 of 1627
>   frames across two boots) and so contributes nothing.
> - `0.02` is the project's standing D2 tolerance (`re/tools/ai_speed_env.py:115`
>   `BAND_PCT = 2.0`; §3a).
> - `eps(F) = 1e-6 * max(1, p95(|orig(F, d)|))` over the window, so a field that is constant
>   zero on the original cannot be declared divergent by a rounding bit.
>
> Integer fields (`+0x490`, `+0x494`, `+0x1a0`, `+0x264`, `+0x328`, `+0x3ec`) are compared
> **exactly**: any difference is a divergence.

## 5. The FIRST-DIVERGENCE rule

> The **first diverging term** is the `(d, F)` with the **smallest `d`** in `[0, 130]` such
> that `|orig(F, d) - port(F, d)| > tol(F, d)`, **and every field in every strictly lower tier
> is inside its tolerance at that same `d`**. Ties within one `d` are broken by the **lowest
> tier**, then by the largest `|orig - port| / tol` ratio.

The report must give, for that term: `d`, the raw frame on each side, the field, its tier, both
values, the ratio, and **the RVA of its writer**, cited from Ghidra or from an existing
RVA-cited note. If the writer cannot be cited, that is stated as `[UNCERTAIN]` and the term is
reported **without** a writer rather than with a guessed one.

## 6. The gates. A gate that fails STOPS the lane; none may be amended.

- **GA — the channel-phase robustness gate.** `RESULT_STEP1.md` §2e records an unresolved
  one-frame phase question between the `.msd` tick snapshot (`0x004c1be0`) and
  `motion_diag`'s write point (`VehiclePhysicsRun.cpp:1210`): at `d = 0` the ORIGINAL shows
  the input consumed but `h` still `0.000000`, while the PORT shows `in[0] = 255` and
  `horiz = 13.38` on the same line. The scan is therefore run **twice**, with the port
  re-indexed at `phi = 0` and at `phi = +1` (`d_port = f - R_port - phi`).
  > **GA passes iff the rule names the SAME tier and the SAME field under both `phi`.**
  > If it does not, the phase is a confounder, the rule's output is not read, and the lane
  > stops until the phase is resolved by direct measurement.
- **GB — the floor is usable.** `orig_solo4.msd` must release-align by §2's predicate and
  yield a finite `floor99` on **at least 90 %** of the fields in §3. Otherwise the floor is
  reported as unusable and the lane stops.
- **GC — the verifier has a passing baseline** (memory `verifier-needs-passing-baseline`).
  The identical rule is run with arm B = **`orig_solo4.msd`** instead of the port.
  > **GC passes iff that control run reports NO divergence in tiers T0-T4 anywhere in
  > `d` in `[0, 130]`.**
  > If the original's own same-arm repeat trips the rule, the rule is too tight, its output on
  > the real arms is meaningless, and the lane stops.
- **GD — the window is populated.** Both arms must have at least 131 frames at `d >= 0`.

## 7. What happens on the verdict

- **A term is named, GA-GD all pass** -> STEP 2B is pre-registered **separately and
  committed before its first run**: the hypothesis the term generates is tested **on the
  running ORIGINAL** before any code is written (session directive; §22.2's pattern). Only if
  that test survives is a fix authored, and then with the **promotion leg** (memory
  `fix-briefs-carry-a-promotion-leg`): the shared-TU body at the RVA plus `run_diff` /
  `run_verify_hook` in the same child, `re-classify` with only what was earned, and the
  dual-copy guard reporting `NEW=0`.
- **No divergence anywhere in T0-T4 in the launch window** -> the same rule is re-run
  **bounce-anchored** per §1's fallback, and that result is reported.
- **Any gate fails** -> reported as a failure, not amended, and the lane stops there.

## 8. Scoring and collateral, unchanged

The §3 bounds are **not renegotiable**: `slip 1500-2000` `0.18855..0.19635`,
`slip 2000-2600` `0.24488..0.25487`, `driving-median` `1904.70..1982.44`. Arm §16.7
(`MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`, `MASHED_STEER_HOLD_AFTER=0`), scored **3
times**, `participants=1` confirmed in `mashed_re.log` (memory
`verify-the-harness-knob-actually-took`), build `NEW=0`. **No fix authored means no re-score is
claimed** — the no-change control is reported as the control it is.

The attempt ends with `re/tools/statediff/collateral.py` over its own arms, with a same-arm
floor, `--scope re/tools/statediff/scope_a6a.txt`, `--mode banded` for anything cross-side, and
§26.10's median-frame-index guard respected: a `!!`-flagged row is not read. Every metric is
reported with `n`, median speed **and** median frame index.

## 9. Process constraints

Muted launches, `MASHED_TITLE` on every run, `MASHED_WIN_POS=primary-bl`, `--poke-ctrl-slots`
on any race capture, never `MASHED_NAV_DEMO`, Frida **entry hooks only**, own PIDs tracked and
only those killed, build only via `mashedmod\build.bat` from PowerShell, trackers only through
`re-classify`, commits as `nanofives` with explicit pathspecs, nothing pushed.
