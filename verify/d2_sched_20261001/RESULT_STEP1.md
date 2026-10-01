# D2 attempt 14, STEP 1 — RESULT

Pre-registration: [`PREREG_STEP1.md`](PREREG_STEP1.md), committed as `0e35837d` **before** any
reduction run. **Not amended.** Read-only reduction over captures already on disk; **no game was
launched**. Raw output: [`step1_raw.txt`](step1_raw.txt),
[`step1_release_window.txt`](step1_release_window.txt).

---

## 1. THE GATES. G1 passed. **G2 and G3 FAILED AS WRITTEN**, so §5's registered decision rule **did not execute** and **was not amended**.

| gate | bar | measured | verdict |
|---|---|---|---|
| **G1** instrument | `+0xb24` int, 0 on frame 0, `> 0` on the last frame | `0` -> `72350`; nonzero on **1447 of 2333** frames | **PASS** |
| **G2** ramp clock starts at release | `+0xb24 == 0` on every `f < E_orig`, and first `+0xb24 > 0` within **3** of `E_orig` | first `+0xb24 > 0` at **886**, `E_orig` = **891**, lag **5**; 5 pre-`E` nonzero frames (886-890) | **FAIL** |
| **G3** first steering frame is release | first `+0x1a8 != 0` within **3** of `E_orig`, and within **3** of §26.9's 884 | first `+0x1a8 != 0` at **886**; `|886 - 891| = 5`; `\|886 - 884\| = 2` | **FAIL** (first clause) |
| **G4** port release | `E_port` within **3** of 2, on `sc1/sc2/sc3` | `E_port = 1` on all three | PASS, **not reached** |

**Why they failed, and it is an instrument defect, not a finding.** `E` was registered as a
**speed threshold** (`h > 1.0`, non-decreasing). The capture carries a **direct** release
witness and the threshold is a lagging proxy for it: the car accelerates from dead rest and
takes **5 frames** to cross `h = 1.0`. The 3-frame bar was written without checking how long
that crossing takes. Same family as §26.5 (`reference-may-never-enter-the-band`): **a bar on a
derived quantity has to be written against that quantity's measured behaviour, not against the
event it stands in for.**

> **Consequence, honoured: the registered MATCH/MISMATCH rule did not execute.** §2 below is a
> set of **measurements**, not the output of a registered rule, and it is labelled as such.
> §4 registers STEP 1b fresh, and declares plainly that it is written **with knowledge of these
> numbers** and is therefore weaker evidence than a blind registration.

---

## 2. The measurements. Every channel, both sides, with its source.

### 2a. The ORIGINAL's release frame is **886**, and three independent record channels agree on it

`verify/d2_reopen_20260929/orig_solo3.msd`, 2333 frames, read with
`re/tools/statediff/msd_fields.py`:

| f | `h` | `+0x9e4` | `+0xb24` | `+0x1a8` | `+0x190` | `+0xbf4` (int) |
|---:|---:|---:|---:|---:|---:|---:|
| 878-885 | `0.000000` | `0.000000` | `0` | `0.00000` | `34.0000` | **`3000`** |
| **886** | `0.000000` | `0.000000` | **`50`** | **`17.07471`** | `34.0000` | **`2800`** |
| 887 | `0.636886` | `0.636898` | `100` | `17.21582` | `34.0000` | `2600` |
| 888 | `0.752720` | `0.752762` | `150` | `17.35693` | `34.0000` | `2400` |
| 890 | `0.983382` | `0.983382` | `250` | `17.63916` | `34.0000` | `2000` |
| 891 (`E_orig`) | `1.098154` | `1.098205` | `300` | `17.78027` | `34.0000` | `1800` |

1. **`+0xbf4`, the countdown witness** (`scenario_launch.py:2497-2499` names it the documented
   drive anchor). It ramps **up** `0 -> 3000` at `+50`/frame from frame **775**, holds `3000`
   through frame **885**, and at frame **886** begins counting **down** at `-200`/frame. The
   release is a sign change in this one field, at frame 886.
2. **`+0xb24`, the steer-hold ramp clock.** `0` on frames 0-885, `50` at 886, `+50`/frame
   after, and it **never returns to 0** in the remaining 1447 frames (X3). A4's law is
   `+0xb24 = (in0 == 0) ? 0 : +0xb24 + dt` (`0x00470737..0x00470746`,
   `VehicleControl.cpp:118-135`), so a `+0xb24` that is 0 for 886 frames and then monotone is
   a direct statement that **the steer byte the vehicle consumed was 0 until frame 886**.
3. **`+0x1a8`, the steer angle**, first nonzero at 886 at exactly `17.07471`.

> **`+0x190` is `34.0000` on every one of the 2333 frames** — before and after release. So
> `+0x1a8 = in0/256 * +0x190 * 0.5 * ramp` (§26.9) being zero for 886 frames **cannot** be
> blamed on the authority factor. With `+0x190 = 34` and `+0xb24 = 50` (`ramp = 6050/6000`),
> `in0 = 255` gives `255/256 * 34 * 0.5 * 1.0083333 = 17.074707` — the measured value to all
> six printed digits. **The consumed steer byte at frame 886 is 255.**

**[UNCERTAIN]** The record cannot distinguish "A4 `0x00470670` ran during frames 0-885 with
`in0 = 0`" from "A4 did not run during frames 0-885": both leave `+0xb24` and `+0x1a8` at their
init 0. The operative fact — **the vehicle consumed zero steer input before frame 886 and a
full 255 at frame 886** — holds either way. Resolving it would need an entry-count hook on
`0x00470670` and is not needed here.

### 2b. The ORIGINAL's commanded input is pinned from well BEFORE the release — the pre-roll is the game's countdown, not a gap in the harness

Cited, not measured (the descriptor bytes are not in the record): `armCook()` attaches
`onLeave` of `0x00496530` and on every call writes `+4 = 0xff` (accel), `+0 = 0xff` (steer
right), `+1 = 0` into block `0x007f1038` (`re/frida/scenario_launch.py:137-155`); `E.drive(1,
+1)` sets it (`:1853`); under `--statediff-drive-late` both run at phase 3
(`:2494-2502`). `steer_schedule` is `""` in the provenance, so there is **no** time-varying
input.

The launch log dates it: `orig_solo3.launch.txt` prints `cook injector armed` and
`drive-late: full accel, steer=+1 -> 1` at `*** RACE RUNNING (phase 3) ***`, then the hold loop
reports `vel=[0,0,0]` at `+4s`, `+9s` and `+13s` and `vel=[603.2, -0.0, 1239.8]` at `+18s`.
**The cook held full accel and full right lock for roughly 17 real seconds while the car did not
move.** 886 frames over that window is ~52 fps, consistent.

### 2c. The ORIGINAL enters its release at DEAD REST. **The orchestrator's step-1 inference is REFUTED.**

> Over frames **0-885**: `max h = 0.000000`, `max +0x9e4 = 0.000000`, **0 frames** with
> `h > 0.01`, **0 frames** with `+0x9e4 > 0.01`.

The inference under test was: *"with a 120-frame ramp, the original's steering would begin
around frame ~880 (already fast), while the port's begins around frame ~1 (still slow)."*
**The original's speed at its first steering frame is exactly `0.000000`.** It is not fast; it
is stationary, and it is stationary to the last bit for all 886 preceding frames. The two arms
begin steering from the same state.

### 2d. The PORT, measured on all three of attempt 13's scored runs

`verify/d2_l60_20261001/sc{1,2,3}/motion_diag.log`;
`in=(input[0], input[1], input[4], input[5])` (`VehiclePhysicsRun.cpp:1212`, `:1259-1260`) —
the **same four descriptor bytes** the original's cook writes.

| | sc1 | sc2 | sc3 |
|---|---|---|---|
| frames | 1627 | 1627 | 1626 |
| frame 0 | `in=(0,0,0,0)` `sp=216.67` `horiz=0.00` | identical | identical |
| frame 1 | `in=(255,0,255,0)` `sp=13.97` `horiz=13.38` | identical | identical |
| first nonzero `in[0]` (steer) | **1** | **1** | **1** |
| first nonzero `in[2]` (accel) | **1** | **1** | **1** |
| `in[0] != 255` on any `f >= 1` | **0 frames** | **0** | **0** |
| `in[1] != 0` on any `f >= 1` | **0 frames** | **0** | **0** |
| `in[2] != 255` on any `f >= 1` | **0 frames** | **0** | **0** |

The port's commanded schedule, cited: `exe_main.cpp:3016-3022` forces `di.accel = 1.f` and
`steerHoldAfterStep = round(MASHED_STEER_HOLD_AFTER * simHz) = 0`; `exe_main.cpp:3031` then
applies `di.steer = steerHoldVal` from sim step 0. **Measured to match the citation: full accel
and full right lock, held, with zero exceptions over 4880 frames across three runs.**

### 2e. The per-channel comparison table

Release `R` = the first frame the vehicle consumed a nonzero steer byte. `R_orig = 886`
(`+0xb24`/`+0x1a8`/`+0xbf4` all agree), `R_port = 1` (`in[0]`).

| channel | ORIGINAL (raw `f`, `f-R`) | PORT (raw `f`, `f-R`) | agree? |
|---|---|---|---|
| steer byte `in[0]` first nonzero | **886**, `+0` (cited harness; confirmed by the `17.074707` identity) | **1**, `+0` | **yes** |
| steer byte `in[0]` held after | constant `0xff` (cited, `:147-148`) | `255` on 4880/4880 frames | **yes** |
| opposite steer `in[1]` | constant `0` (cited, `:149`) | `0` on 4880/4880 | **yes** |
| throttle `in[4]` first nonzero | **886**, `+0` (cited, `:142`) | **1**, `+0` | **yes** |
| throttle held after | constant `0xff` (cited) | `255` on 4880/4880 | **yes** |
| `+0xb24` ramp clock first tick | **886**, `+0`, value `50`, `+50`/frame | first `+0x1a8` at frame 2 implies the same clock (not logged on the port, §2b of the prereg) | **yes** |
| `+0x1a8` first nonzero | **886**, `+0`, **`17.07471`** | **2**, `+1`, **`17.07471`** (§26.9, inherited) | **yes**, to 7 digits |
| `+0x1a8` step/frame | **`+0.141113`** | **`+0.141113`** (§26.9) | **yes** |
| `+0x1a8` saturation | frame **1005** = the **120th** steering frame, `33.86719` | frame **121** = the **120th**, `33.86719` | **yes** |
| steer **sign** | `+` (right; `gSteer > 0` -> byte `+0`) | `+` (`in[0]`, `in[1] = 0`) | **yes** |
| horizontal speed at `R` | **`0.000000`** (and `0.000000` on all 886 preceding frames) | `0.00` at frame 0; `13.38` at frame 1 | see §3 |
| time-varying schedule | none (`steer_schedule = ""`) | none (`steerHoldAfterStep = 0`, constant after) | **yes** |

> **Every input channel agrees, aligned on release.** The sole raw-frame difference is the
> ORIGINAL's **885-frame pre-roll** (menu, track load, spawn, countdown) that the PORT does not
> have — and across that pre-roll the original's car is at **exactly zero** speed, so it
> contributes no state the port is missing on the velocity side.

The `+0x1a8` first-nonzero frames are `R+0` (original, from the `.msd`) against `R+1` (port,
from the `MASHED_A6ADUMP` channel). **[UNCERTAIN]** whether that one frame is a real offset or
the phase difference between two different log channels — `.msd` samples the record at the
statediff tick `0x004c1be0`, `snap.steer` is written at `VehiclePhysicsRun.cpp:1304`. It is
flagged here and is a registered gate of STEP 2 (§4), not assumed either way.

---

## 3. The CONTEXT rows (pre-registered §7 as CONTEXT; they may not move this attempt's verdict)

- **X1.** Over frames `0..890` the original's record is **not** frozen: **221 of 833** dwords
  change, and **0 of 890** frames are bit-identical to frame 0. So the pre-roll is not a
  stopped simulation; something is integrating. With `h` and `+0x9e4` exactly `0` throughout,
  none of it is the car's velocity.
- **X2.** The frame before release. ORIGINAL `f=885`: `h = 0.000000`, `+0x9e4 = 0.000000`,
  `+0x1a8 = 0`, `+0xb24 = 0`. PORT `f=0`: `horiz = 0.00`, `sp = 216.67`, `in = (0,0,0,0)` — a
  purely **vertical** spawn drop of 216.67, which is gone by frame 1 (`sp = 13.97`,
  `horiz = 13.38`, so the vertical residue is 4.0). **The port begins its run mid-drop; the
  original's 885-frame pre-roll has already landed and settled it.** Horizontally both are at
  zero.
- **X3.** The original's `+0xb24` **never** returns to 0 after release (0 of 1447 frames). The
  cook's `in0` was therefore never overwritten between calls of `0x00496530`.

## 3b. ONE EXPLORATORY row, recorded and NOT acted on

The release-window table shows the two arms leave rest at **completely different rates**:

| | `R+1` | `R+2` | `R+3` | gain/frame over `R+1..R+3` |
|---|---:|---:|---:|---:|
| ORIGINAL `h` | `0.636886` | `0.752720` | `0.868224` | **`+0.1158`** |
| PORT `horiz` | `13.38` | `26.98` | `41.33` | **`+13.98`** |

Same throttle byte, same steer byte, same ramp clock, and the ramp saturates on the 120th
steering frame on both — so the per-frame `dt` the clock sees is the same. **[UNCERTAIN]** the
`R+0`/`R+1` channel-phase question in §2e is not yet settled, and one frame of phase would
change which rows pair. **Nothing is built on this row here.** It is the registered target of
STEP 2 (§4) and is pre-registered there before it is measured further, per §26.10's second
lesson.

---

## 4. STEP 1b — registered fresh, and its weakness is declared

**Declared openly: this is written AFTER seeing §2's numbers.** Its alignment definition was
chosen because §1's failed one was wrong, and a rule written against data already in hand is
weaker evidence than a blind registration. It is recorded as **CONFIRMATORY BOOKKEEPING**, not
as an independent test. The independent content of this step is §2's measurements themselves,
which are raw channel readings and not rule outputs.

> **Alignment.** `R` = the first capture frame at which the vehicle consumed a nonzero steer
> byte, witnessed **in the record**: ORIGINAL = first frame with `+0xb24 != 0`; PORT = first
> frame with `in[0] != 0`. All frames are reported as `f` and `f - R`.
>
> **MATCH** iff, for both arms: the steer byte is nonzero on every `f >= R` and zero on every
> `f < R`; the throttle byte likewise; the opposite-steer byte is zero throughout; the steer
> sign is the same; `+0x1a8`'s first value, per-frame step and saturation count agree to within
> **0.5 %**; and horizontal speed at `R` is `<= 1.0` on both.

**Evaluated against §2:** steer held `R`-onward both sides (orig cited + the `17.074707`
identity; port 4880/4880) **pass**; throttle likewise **pass**; `in[1] = 0` throughout
**pass**; sign `+` both **pass**; `17.07471` vs `17.07471` (0.000 %), `+0.141113` vs
`+0.141113` (0.000 %), 120th steering frame vs 120th (0.000 %) **pass**; `h` at `R` is
`0.000000` (orig) and `0.00` (port) **pass**.

# >> VERDICT: **MATCH.** The two arms share the same input schedule.

Physics work is **not** blocked. There is **no** proposal for the user and **no** diagnostic
run to report: the port arm already reproduces the reference's input schedule, so the
registered §6 MISMATCH branch does not open and §16.7's arm definition needs no change.

---

## 5. What this closes, and what it opens

**CLOSED.** The orchestrator's step-1 inference is **REFUTED by measurement**: the original is
at exactly `0.000000` speed on its first steering frame and on all 886 frames before it. The
arms do not differ in their test input. Attempt 13 §26.9's regime count (`ORIGINAL 1329 of
1329` at speed `>= 100` after saturation against the `PORT 5 of 1507`) is **release-aligned by
construction** — each side's saturation frame is its own — so it **stands**, and the collapse
it names is a real cross-side difference, not an artefact of when each arm was commanded.

**OPENED.** §3b: at the same release-relative frame, with the same bytes, the original leaves
rest at `+0.116` horizontal units/frame and the port at `+13.98` — **121x** — subject to the
unsettled `R+0`/`R+1` channel phase. That is STEP 2's target, and STEP 2 is pre-registered
separately before its first run.

## 6. The instrument lesson this step paid for

**A gate written on a proxy must be sized against the proxy's own measured lag.** `E` was
`h > 1.0` standing in for "control enabled"; the car needs **5 frames** from rest to cross
`1.0`, and the bar was **3**. The capture already carried three direct witnesses of the release
(`+0xbf4`'s sign change, `+0xb24`'s first tick, `+0x1a8`'s first nonzero) and the registration
used none of them. **Before writing a threshold on a derived quantity, check whether the capture
carries the event itself.**
