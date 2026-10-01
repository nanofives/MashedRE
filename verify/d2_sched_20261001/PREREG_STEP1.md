# D2 attempt 14, STEP 1 — PRE-REGISTRATION: do the two arms share the same input schedule?

Written and committed **BEFORE any reduction run and before any game launch**. HEAD `51572cd5`,
branch `race/first-frame-parity`. **Not to be amended** — if a gate fails, the failure is
reported and the lane stops, per the session directive.

## 1. Why this is step 1

Attempt 13 §26.9 measured that A4 `0x00470670`'s steer angle `+0x1a8` ramps in **time**, the
same on both sides (start `17.07471`, `+0.141113`/frame, saturating at `33.86719` on the 120th
steering frame), and that the two arms' **first steering frame** are **884** (ORIGINAL) and
**2** (PORT). It then measured that after saturation the ORIGINAL is at speed `>= 100` on
**1329 of 1329** frames (median 1847.0) and the PORT on **5 of 1507** (median 22.2).

The orchestrator's inference, **UNVERIFIED and to be verified or refuted here**: if the
original's steering begins at frame ~884 when the car is *already fast*, and the port's begins
at frame ~1 when it is *still slow*, then the two arms differ in their **test input**, not in
their physics, and that alone could produce the trap. No physics work may start until this is
settled.

## 2. The channels (cited by RVA / file:line)

### 2a. ORIGINAL arm

Reference capture `verify/d2_reopen_20260929/orig_solo3.msd`, 2333 frames, provenance
`verify/d2_reopen_20260929/orig_solo3.msd.provenance.json`:

```
argv = re/frida/scenario_launch.py --statediff-out <...>/orig_solo3.msd
       --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38
       --poke-ctrl-slots
```

Unspelled defaults, read from the script (memory `read-the-references-own-provenance-argv`):
`--cars` defaults to **1**, `--track` defaults to **0 = Training**
(`re/frida/scenario_launch.py:1773-1777`). Launch log `orig_solo3.launch.txt:2` confirms
`track=0 mode=10 cars=1`.

- **I1/I2 — the commanded input bytes.** NOT in the record, so they are **cited, not
  measured**. The cook injector `armCook()` attaches `onLeave` of `0x00496530`
  (`re/frida/scenario_launch.py:137-155`) and on **every** call writes, into the descriptor
  block `0x007f1038`: `+4 = 0xff` (accel, `:142`), `+0 = 0xff` (steer right, `:147-148`),
  `+1 = 0` (`:149`), plus the legacy bytes `+2/+3/+0x0e/+0x0f` (`:151-154`). `E.drive(1, +1)`
  sets `gAccel=1, gSteer=+1` (`:1853`). Under `--statediff-drive-late` both the arm and the
  drive happen at **phase 3, race running** (`:2494-2502`), i.e. **at or before capture frame
  0**. `--statediff-steer-schedule` is empty in the provenance (`steer_schedule: ""`), so there
  is **no** time-varying input. A4 reads these raw bytes, not the `0x14`/`0x18` floats
  (`:135-136`).
  > **Therefore the ORIGINAL's commanded input is CONSTANT full accel + full right lock for
  > the entire capture, from frame 0.** This is a statement about the harness, and it is the
  > thing the measured first-steering frame of 884 has to be reconciled with.
- **O1 — horizontal speed.** `h = sqrt(F(+0x9b0)^2 + F(+0x9b8)^2)` (memory
  `msd-world-position-is-the-0x928-matrix-row` fixes `+0x9b0..+0x9b8` as the velocity triple;
  the port computes the same quantity as `hs` at
  `mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1205`).
- **O2 — total speed.** `F(+0x9e4)` (`VehiclePhysicsRun.cpp:1226` logs the port's as
  `sp=F(r, off::kSpeed)`).
- **O3 — the steer-hold ramp counter `+0xb24`**, read as **int**. A4 `0x00470670` resets it to
  0 when `input[0] == 0` and otherwise accumulates `dt` through `FUN_004a2c48`
  (`0x00470737`..`0x00470746`, transcribed at
  `mashedmod/src/mashed_re/Vehicle/VehicleControl.cpp:118-135`). It is the clock `+0x1a8`'s
  ramp is a pure function of: `ramp = (min(+0xb24, 6000) + 6000)/6000`
  (`0x0047080c`..`0x0047082f`, §26.9).
- **O4 — the steer angle `+0x1a8`**, read as **float**. A4's two stores `0x00470835` /
  `0x004708fa` (§26.9).

All four are plain record dwords, so all four come out of the **existing** `.msd` with
`re/tools/statediff/msd_fields.py`. **No game launch is needed for the ORIGINAL arm.**

### 2b. PORT arm

Attempt 13's three scored runs `verify/d2_l60_20261001/sc{1,2,3}/motion_diag.log`, 1627 lines
each, arm §16.7 — recipe `re/tools/statediff/a8_run_port.py:16-19`
(`MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6 MASHED_CAR_SEL=0`
`MASHED_DRIVE_HOLD=1 MASHED_STEER_HOLD=1 MASHED_MUTE=1 MASHED_WIN_POS=primary-bl`
`MASHED_MOTION_DIAG=1`) with `MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`,
`MASHED_STEER_HOLD_AFTER=0` overridden on the command line.

- **P1/P2 — the commanded input bytes, MEASURED.** `motion_diag.log`'s
  `in=(%u,%u,%u,%u)` is `(input[0], input[1], input[4], input[5])`
  (`VehiclePhysicsRun.cpp:1212` + `:1259-1260`) — the **same four descriptor bytes** the
  original's cook writes. So P1 = `in[0]` (steer right) and P2 = `in[2]` (= `input[4]`, accel).
- **P3 — horizontal speed** = `horiz=` (`hs`, `VehiclePhysicsRun.cpp:1205`, `:1261`).
- **P4 — total speed** = `sp=` (`F(r, off::kSpeed)` = `+0x9e4`, `:1261`).
- **P5 — the steer angle `+0x1a8`.** NOT in `motion_diag.log` (`steer=` there is `io.steer`,
  the input command — the mismatch §26.9 records). It is in the `MASHED_A6ADUMP` channel as
  `snap.steer = F(r,0x1a8)` (`VehiclePhysicsRun.cpp:1304`). Attempt 13 §26.9 already measured
  it: first steering frame **2**, value **17.07471**, step **+0.141113**, saturation at frame
  **121**. **Inherited, not re-derived**, per the session directive.
- **P6 — `+0xb24`** is not logged on the port. Not measured. The port's law is
  `Ib(v,0xb24) = (input[0]==0) ? 0 : (int)((float)Ib(v,0xb24) + dt)`
  (`VehicleControl.cpp:135`), and P5's measured ramp start/step/saturation is the observable
  that `+0xb24` determines, so P5 stands in for it.

The port's commanded schedule, cited: `exe_main.cpp:3016-3022` sets `di.accel = 1.f` and
`steerHoldAfterStep = round(MASHED_STEER_HOLD_AFTER * simHz)`; `exe_main.cpp:3031` applies
`di.steer = (s_steerHoldStep < steerHoldAfterStep) ? 0.f : steerHoldVal`. With
`MASHED_STEER_HOLD_AFTER=0` that is `steerHoldAfterStep = 0`, so **full accel and full lock
from sim step 0** of the `car_ready_ && !results && !paused` block.

## 3. The definition of each arm's alignment frame

> **E = the CONTROL-ENABLE frame = the first capture frame `f` at which horizontal speed
> `h > 1.0` and `h` is non-decreasing over `f, f+1, f+2`.**

Horizontal, not total: the port's own frame 0 reads `sp=216.67 horiz=0.00`
(`sc1/motion_diag.log:1`), i.e. a purely vertical spawn drop, which a total-speed threshold
would mis-date by construction. The `>1.0` floor is `field_trace.py`'s own default driving
floor (`re/tools/statediff/field_trace.py --min-speed` default 1.0).

Every reported frame index is given **both** as the raw capture frame `f` **and** as
`f' = f - E`, the control-enable-relative frame. **Nothing is aligned on capture start.**

## 4. The gates, in order. A gate that fails STOPS the lane; none may be amended.

- **G1 (instrument).** `msd_fields.py` must read `+0xb24` as an int that is **0 on frame 0**
  and **strictly positive on the last frame** of `orig_solo3.msd`. If `+0xb24` is 0 throughout
  or non-monotone garbage, the offset is wrong and nothing below is measurable.
- **G2 (the original's ramp clock starts at its release).** On the ORIGINAL,
  `+0xb24 == 0` on **every** frame `f < E_orig`, and the first frame with `+0xb24 > 0` is
  within **3** frames of `E_orig`.
- **G3 (the original's first steering frame is its release).** On the ORIGINAL, the first
  frame with `+0x1a8 != 0` is within **3** frames of `E_orig`, and §26.9's measured 884 is
  reproduced to within **3** frames.
- **G4 (the port's release).** On the PORT, `E_port` is within **3** frames of §26.9's
  measured first steering frame of 2, on **all three** of `sc1/sc2/sc3`.

## 5. The decision rule

Evaluated only if G1-G4 all pass.

> **MATCH** iff **all five** hold:
> 1. **Steer input held, both sides.** ORIGINAL: cited constant from frame 0 (§2a I1).
>    PORT: `in[0] == 255` on **every** frame `f >= E_port`, and `in[1] == 0` on every such
>    frame (no left lock, no schedule).
> 2. **Throttle input held, both sides.** ORIGINAL: cited constant from frame 0 (§2a I2).
>    PORT: `in[2] == 255` on **every** frame `f >= E_port`.
> 3. **The ramp clock starts at release on both sides**, i.e. G2 and G3 passed and
>    `E_port - (port first steering frame) == E_orig - (orig first steering frame)` to within
>    **3** frames.
> 4. **The ramp's first value agrees** to within **0.5%**: ORIGINAL `17.07471` at its first
>    steering frame against PORT `17.07471` at its own (both from §26.9, inherited).
> 5. **Both arms enter control-enable from rest**: `h <= 1.0` on the frame before `E` on both
>    sides.
>
> **MISMATCH** otherwise. On a MISMATCH the report must name **exactly which channel differs**
> and **where each arm's value of it comes from** (file:line / RVA), and **physics work stops**.

## 6. What happens on each verdict

- **MATCH** -> step 1 is closed with "the arms share the input schedule, and attempt 13's
  release-aligned collapse count stands". Continue to **STEP 2**, the frame-aligned collapse
  lane, whose own decision rule will be pre-registered separately and committed before its
  first run.
- **MISMATCH** -> **STOP.** Report it, and **propose, do not apply**, the minimal change that
  would make the port arm reproduce the reference's input schedule. The §3 bounds
  (`slip 1500-2000` `0.18855..0.19635`, `slip 2000-2600` `0.24488..0.25487`, `driving-median`
  `1904.70..1982.44`) are **not renegotiable**; the arm's input schedule is part of the arm
  definition (§16.7) and changing it is a **USER decision**. One clearly-labelled
  **DIAGNOSTIC** port run with a matched schedule may be reported (three metrics, with `n`,
  median speed and median frame index) so the user can see the effect. A diagnostic is **not**
  a scored re-close.

## 7. Context measured alongside, pre-registered as CONTEXT and NOT a gate

The ORIGINAL has a pre-roll before `E_orig` that the PORT does not. Three things about it are
measured and reported, and **may not be used to move any verdict in this attempt**:

- **X1.** How many of the 833 record dwords change at all over frames `0 .. E_orig - 1`. A
  bit-frozen pre-roll means the vehicle sim is not integrating; a changing one means it is
  integrating with the input ignored or not yet consumed.
- **X2.** The ORIGINAL's `h`, `+0x9e4` and `+0x1a8` on the frame before `E_orig`, against the
  PORT's on the frame before `E_port`. `sc1/motion_diag.log:1` already reads
  `sp=216.67 horiz=0.00` on the port, i.e. the port enters control-enable **while still
  falling**, which the original's pre-roll would have settled.
- **X3.** Whether the ORIGINAL's `+0xb24` ever resets to 0 after `E_orig` (which would mean the
  cook's `input[0]` was being overwritten between calls of `0x00496530`).

Any X-row that bears on a later step must be **pre-registered before anything is built on it**
(memory `collateral-review-after-every-attempt`; §26.10's second lesson).

## 8. Collateral review

Per memory `collateral-review-after-every-attempt`, this attempt ends with
`re/tools/statediff/collateral.py` over its own arms, with a **same-arm floor** and
`--scope re/tools/statediff/scope_a6a.txt`, `--mode banded` for anything cross-side, and the
`--mode banded` median-frame-index guard from §26.10 **respected**: a flagged (`!!`) row is
not read.

## 9. Process constraints for this attempt

Muted launches (`MASHED_MUTE=1`), `MASHED_TITLE` on every run, `MASHED_WIN_POS=primary-bl`,
`--poke-ctrl-slots` on any race capture, never `MASHED_NAV_DEMO`, Frida **entry hooks only**,
own PIDs tracked and only those killed, build only via `mashedmod\build.bat` from PowerShell,
trackers only through `re-classify`, commits as `nanofives` with explicit pathspecs, nothing
pushed.
