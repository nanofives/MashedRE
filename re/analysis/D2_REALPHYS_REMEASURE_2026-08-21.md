# D2: `MASHED_REAL_PHYSICS` re-measured after B5e — the defect is unchanged

Date: 2026-08-21
Build: `mashed_re.exe` built today from main after the D1 (`0e6e0834`) and Save
(`218b6598`) merges, i.e. **after** the B5e solver-island merge `021a9f38`
(2026-07-20) whose effect on this defect was unrecorded.
Recipe: the one from `WS_A8_REALPHYS_2026-07-01.md:15`, verbatim —
`MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0` (+ `MASHED_COUPLING_DIAG=1`).
Both arms self-exited `rc=0`; no crash, no hang.

## Result: unchanged, to the decimal

`PLAY-DEMO` telemetry, real-physics arm:

```
td=2.89  steer=+0.00  car_yaw=1.5498  speed=   0.00
td=3.16  steer=+0.00  car_yaw=1.5498  speed=  41.81
td=3.42  steer=+0.00  car_yaw=1.5498  speed= 136.19
td=3.69  steer=+0.00  car_yaw=1.5498  speed= 259.62
td=3.95  steer=+0.00  car_yaw=1.5498  speed= 348.45
td=4.22  steer=+0.50  car_yaw=1.5498  speed= 618.27
td=4.49  steer=+0.50  car_yaw=1.5498  speed=1138.05
td=4.75  steer=+0.50  car_yaw=1.5498  speed=1500.00   <- kSafetyInternal clamp
 ... held at 1500.00 for the rest of the run, car_yaw never leaves 1.5498
```

The 2026-07-01 note records the same run as "0→41.81→136 … saturates the
`kSafetyInternal = 1500` clamp … `car_yaw` frozen at 1.5498 through
`steer=+0.50`". **The values are identical, not merely similar** — `41.81`,
`136`, `1.5498`, `1500.00`. The 2026-07-14 re-measure
(`INITD3D9_HANG_AND_REMEASURE_2026-07-14.md` §2) reports the same figures again.

So three measurements across seven weeks agree exactly, and **the B5e
solver-island merge did not affect this defect.** The [UNCERTAIN] left open on
2026-07-20 is now closed, negatively.

The exact reproducibility is itself useful: the defect is fully deterministic,
so it is tractable to debug and any fix will be unambiguous to verify.

## Scaffold control — the same telemetry, working

Identical recipe with `MASHED_REAL_PHYSICS` unset (the shipping default):

```
td=3.16  steer=+0.00  car_yaw=1.5498  speed= 1.72
td=3.95  steer=+0.00  car_yaw=1.5498  speed= 9.87
td=4.22  steer=+0.50  car_yaw=1.6850  speed=12.29
td=4.75  steer=+0.50  car_yaw=2.1061  speed=16.28
td=5.28  steer=+0.50  car_yaw=2.6274  speed=19.21
td=5.55  steer=+0.50  car_yaw=2.9160  speed=20.11
td=6.08  steer=+0.50  car_yaw=3.5026  speed=20.11   <- top speed holds
```

## CORRECTION — the car is NOT 75x too fast. It is slower than the scaffold.

The first version of this note claimed the real-physics arm is "75x too fast",
reading the `speed=` field of `PLAY-DEMO` as the car's velocity. **That is wrong,
and it was committed to this note, to ROADMAP §D2 and to the CHANGELOG before it
was checked.** Corrected here; the ROADMAP text and a correcting CHANGELOG entry
follow.

Deriving actual velocity from the logged positions instead of trusting the field:

```
                reported   |dpos|/dt   ratio
REAL PHYSICS     1500.00      11.86    126x     <- field is NOT the car's speed
                 1500.00      12.31    122x
                 1500.00      11.85    127x
SCAFFOLD           17.87      17.09      1.0x   <- field IS the car's speed
                   20.11      19.76      1.0x
```

So under `MASHED_REAL_PHYSICS` the car actually travels at **~11.9 units/s**,
against the scaffold's **~20**. It is **slower**, not faster. The `speed=` field
reports the chain's internal saturated velocity, not the emitted motion, and the
two coincide only on the scaffold path — which is exactly why the field was
misleading.

`coupling_diag.log` confirms it independently and had been on disk the whole
time: every line reads `horiz=1500.00 fwdDot=1500.00 desired=12.000 bs=12.000`.
`bs` is `io.drive_speed`, the value `TrackRenderer` integrates into position
(`VehiclePhysicsRun.cpp:595`), and it is **12**, matching the measured ~11.9.

### Restated defect

| Property | Scaffold | Real physics |
|---|---|---|
| Actual car speed | ~20 units/s | **~11.9 units/s** (bounded, = `desired`) |
| Internal chain velocity | n/a | **saturates `kSafetyInternal = 1500`** |
| `steer=+0.50` → yaw | 1.5498 → 4.97 (turns) | **frozen at 1.5498** |

Two separate facts, previously conflated into one wrong headline:

1. **The internal velocity integrator is unbounded** and pins the safety clamp
   (`VehiclePhysicsRun.cpp:480`). Real, but it does **not** reach the car: the
   emitted `drive_speed` is the `desired` value, so output is bounded.
2. **Steering produces no heading change at all.** This is the defect that makes
   the build undrivable, and it is unaffected by the above.

The observable symptom is therefore "the car drives in a straight line at ~12
units/s and will not turn" — not "the car rockets away at 1500". The severity
claim changes; the blocking conclusion does not, because a car that cannot steer
is still not shippable as the default.

## Consequence for D2 — this is the blocker, not the wedge

ROADMAP §D2 names the statediff residual wedge as what blocks inverting
`MASHED_REAL_PHYSICS`. That wedge is now shown to be three harness issues
(`D2_WEDGE_REMEASURE_2026-08-20.md`), none of them a port defect. **The actual
blocker is this.** A default build on the ported chain would accelerate to a
safety clamp in 1.6 s and drive in a straight line regardless of steering.

Root cause remains as recorded in `COLLISION_GATE_BRIEF_D1_2026-07.md:56` and
`vehicle_coupling.md`: the original's stability comes from a two-body closed loop
(PD gain `_DAT_005ccd6c = 20.0` @ `0x005ccd6c`, 0.06 s lookahead, proxy body
integrated by the vendor solver, readback resetting render = body). The
standalone's single-body reduction loses both the forward damping and the
steer→lateral→heading coupling. Nothing in this measurement contradicts that
attribution, and nothing in it advances the attribution either — it establishes
only that the symptom is unchanged.

Note the sequencing oddity this leaves standing: physics has been **5/5 C4**
(A3/A4/A5/A6a/A6b) since 2026-07-01, and the flag's stated exit condition
("stays gated OFF until A6a/A6b reach C4") was met the same day. C4 on the
individual hooks and a drivable default build are evidently different bars, and
the ledger currently only tracks the first.

## The diag DOES fire, and it localises the defect

Resolved by deleting `coupling_diag.log` (backed up first to
`verify/realphys_20260821/coupling_diag_preexisting.log`) and re-running: it
**reappeared, 24,734 bytes, freshly timestamped**. So the diag block is live on
the current default path and the instrument is available. Both earlier claims
that it "emitted nothing" were reading errors — first the wrong file, then a
stale mtime.

220 diag lines from one run:

```
first: cv=(0.09,0.00,4.44)      horiz=   4.44 desired= 0.037 bs= 0.012 yaw=1.5498 velH=1.5498
last : cv=(31.56,0.00,1499.67)  horiz=1500.00 desired=12.000 bs=12.000 yaw=1.5498 velH=1.5498

distinct yaw   values: 1   (1.5498)
distinct velH  values: 1   (1.5498)
distinct chain vel x : 100      z: 98
distinct desired     : ramps 0.037 -> 12.000
distinct bs          : ramps 0.012 -> 12.000
```

The chain velocity's **components change (100 and 98 distinct values) but its
direction never does**: x/z is 0.0203 at the first sample and 0.0210 at the
last. The vector only ever *scales*. So `velH` — the velocity heading — is
pinned at 1.5498 for the entire run, and `yaw` follows it because the alignment
block (`VehiclePhysicsRun.cpp:582-589`) steers `io.yaw` toward the velocity
direction rather than from the steer input.

**Therefore the defect localises to: steer input produces no lateral component
in the chain's velocity.** The velocity vector grows along a fixed heading and
never rotates.

Two consequences for how the blocker is described:

- The downstream pieces are **not** broken. Speed emission is correct
  (`bs` tracks `desired` exactly, 0.012→12.000), and the yaw-alignment block
  would follow the velocity if the velocity ever turned.
- "The single-body reduction loses forward damping AND turn coupling"
  (`COLLISION_GATE_BRIEF_D1_2026-07.md:56`) is **half right on this evidence**:
  the emitted speed is damped correctly to `desired`; what is missing is only
  the steer→lateral term. The unbounded internal integrator is a separate
  issue that does not reach the car.

That is a much narrower search than "the coupling reduction", and it is
deterministic and instrumented, so a fix is directly measurable: `velH` must
start moving when `steer` is non-zero.

## Steer DOES reach the wheels — the loss is downstream of A4

Extended the live `MASHED_COUPLING_DIAG` block (`VehiclePhysicsRun.cpp:606`) to
also emit `io.steer`, the two descriptor input bytes, and the **front-wheel
steer-angle slots `+0x1a8` / `+0x26c`** that A4 (`FUN_00470670`) writes. Rebuilt
and re-ran the same recipe:

```
steer=0.500  in=(128,0)  steerAng=(8.5,8.5)   yaw=1.5498  velH=1.5498  cv=(7.90,0.00,375.55)
steer=0.500  in=(128,0)  steerAng=(8.5,8.5)   yaw=1.5498  velH=1.5498  cv=(31.56,0.00,1499.67)
```

Everything upstream is **working**:

- `steer=0.500` maps to `in=(128,0)` — the byte channel is correct (0.5 × 255 ≈ 128
  into the +steer slot, the −steer slot zero, mutually exclusive as the original writes).
- `steerAng=(8.5, 8.5)` — **A4 consumed the input and wrote non-zero front-wheel
  steer angles.**

And yet the velocity direction never moves: `cv` x/z is 0.02103 at the first
sample above and 0.02104 at the last. So a non-zero wheel steer angle produces
**no lateral force at all**.

**The defect is therefore downstream of A4** — in A5 (`FUN_0046ddb0` Phase 0,
which is supposed to turn the steer angle into a per-wheel forward-axis rotation
via `FUN_004c4d20`) or in the contact/force solver that should convert a rotated
wheel axis into lateral force. A4 and the input mapping are exonerated.

### Prime suspect, on the evidence

`velH` equals `yaw` **exactly** in every sample (both 1.5498) — the velocity is
always precisely along body-forward, never at a slip angle. That is what you
would see if every wheel's forward axis were body-forward regardless of its
steer angle.

`VehiclePhysicsRun.cpp:418-423` is the place that could cause exactly that:

```cpp
// The body-forward/wheel-axis world transform A5 needs (zeroed +0x928 wheel
// matrix block produced (0,0,0) -> no drive direction -> no motion). Synthesize
// it from the car's yaw each frame (origin position; only the rotation axes are
// read by A5's forward/right-axis transforms).
float xform[16];
BuildYawMatrix(io.yaw, xform);
```

The port **synthesises** the wheel-axis transform from `io.yaw` alone, as a
workaround for a zeroed `+0x928` wheel-matrix block. A yaw-only matrix carries no
per-wheel steer rotation, so if this synthesised transform is what A5's
forward/right-axis reads consume, every wheel points along body-forward and the
`steerAng` values are never expressed. [UNCERTAIN] — this is a hypothesis
consistent with all observations, NOT a confirmed cause. Confirming it means
checking whether `FUN_004c4d20`'s per-wheel rotation is applied on top of
`xform`, or bypassed by it.

Next measurement, cheap and already instrumented: emit a per-wheel forward axis
alongside `steerAng`. If all four wheels share one axis equal to body-forward
while `steerAng` is 8.5, the synthesised transform is confirmed as the loss point.

### That test REFUTED the suspect — and the real signature is sharper

Emitted `Wheel::kSteeredFwd` (+0xb4) for wheel 0 (front, steered) and wheel 3
(rear, unsteered) — `0x220` and `0x46c`:

```
steer=0.500  steerAng=(8.5,8.5)  fwd0=(0.0420,1.9996)  fwd3=(0.0210,0.9998)
```

The prediction was `fwd0 == fwd3`. It is **not**. `fwd0` is **exactly 2x `fwd3`**
(0.0420 = 2 x 0.0210; 1.9996 = 2 x 0.9998), i.e. the same **direction** at double
the **magnitude**. `fwd3` is unit (|v| = 0.9998); `fwd0` is |v| = 2.0.

So `BuildYawMatrix` is **not** the loss point, and the hypothesis above is
withdrawn. The steer angle is not being discarded — it is being expressed as a
**scalar magnitude change instead of a rotation**. A steered wheel ends up with
its forward axis scaled, not turned, which produces exactly the observed
behaviour: more forward force, zero lateral force, no velocity rotation.

**Restated defect, now one line wide:** for a steered wheel, the steer
contribution is applied along the **forward** axis rather than the **right/lateral**
axis. Adding along `right` would rotate the vector (and give the missing lateral
force); adding along `forward` only lengthens it.

[UNCERTAIN] the exact writer. `Wheel::kSteeredFwd` has no accessor in
`VehicleStruct.h` (unlike `wheel_right` / `wheel_steer`) and no grep-visible
writer at `Base(n)+0xb4` — the ported A5 code addresses the record by raw dword
index (`self[0xb4]` there is a *different* field, per-wheel steer torque at byte
`0x2d0`, `PhysicsChainHooks.cpp:917`), so the writer must be found by offset
arithmetic rather than by name. Note also the accumulate idiom already present in
that file (`a5F(self, 0xb4) = fVar4 + f;`, `PhysicsChainHooks.cpp:735`): an exact
2x is equally consistent with the field being **accumulated twice** for steered
wheels rather than assigned once. Both readings predict the same observable and
are distinguished by reading the writer, which is the next step.

## The writer is found; two hypotheses tested, both refuted

**Writer located** — `ForceIntegrator.cpp:49-57` (A5 Phase 0, per-wheel loop
`piVar12 = self + 0x5b + w*0x31`, so `piVar12[0x2d]` is exactly the
`Base(n)+0xb4` field the diag reads):

```cpp
if (vF(piVar12, 0xf) == kZero) {            // steer angle == 0
    piVar12[0x2d] = self[0x275];            // steered fwd = body forward
    piVar12[0x2e] = self[0x276];
    piVar12[0x2f] = self[0x277];
} else {
    unsigned char m[64];
    Rw_MatrixFromAxisAngle(m, kUpAxis, vF(piVar12, 0xf), 0);   // FUN_004c4d20
    Rw_TransformPoints(vFP(piVar12, 0x2d), vFP(self, 0x275), 1, m);  // FUN_004c3df0
}
```

The **structure is correct** — unsteered wheels copy body forward (which is why
`fwd3` is unit), steered wheels rotate body forward about the up axis by the
steer angle. So the defect is inside one of the two helpers, both of which are
bound to real implementations, **not stubs**
(`ForceIntegratorStubs.cpp:37-45` → `Math::RwV3dTransformPointsCPU` and
`RwMatrixRotate`). Note `ContactStubs.cpp` *does* carry stub versions of the same
two names — a second definition pair that is not the one on this path.

**Hypothesis A — accumulation (`+=` instead of `=`).** Refuted by the structure:
line 56 writes through `Rw_TransformPoints(dst, src, ...)` with `dst != src`, and
an accumulating transform would keep growing each frame. The value is pinned at
exactly 2.0 for the whole run.

**Hypothesis B — uninitialised matrix.** `unsigned char m[64]` was read
uninitialised, and `Rw_TransformPoints` is a *point* transform, so it consumes
the matrix's translation row — a garbage translation equal to the source would
give exactly `2*src`. Tested by `memset(m, 0, sizeof m)` before the call,
rebuilt, re-ran: **output identical, `fwd0=(0.0420,1.9996)` unchanged.**
Refuted. (The `memset` is kept as hygiene — reading uninitialised memory is UB
regardless — with the negative result recorded at the call site.)

### What that leaves

With `m` zeroed, `RwMatrixRotate(m, kUpAxis, 8.5, mode 0)` followed by
`RwV3dTransformPointsCPU(dst, src, 1, m)` yields `dst = 2*src` with the direction
exactly preserved. Since a zeroed matrix that `RwMatrixRotate` failed to write
would give `dst = 0`, `RwMatrixRotate` **is** writing something — and whatever it
writes acts as a uniform scale of 2 rather than a rotation of 8.5.

Next step is to read those two functions directly (`Math/RwMatrixRotate.cpp`,
`Math/RwV3dTransformPointsCPU.cpp`) against `FUN_004c4d20` / `FUN_004c3df0`, and
in particular to check the **angle unit** (`ContactConstants.h:46` records
`kAxisAngle90 = 90.0f` as a `FUN_004c4d20` angle argument, so the helper takes
DEGREES; `8.5` is then 8.5°, which should give a visible rotation) and the
**matrix layout** the CPU transform assumes (row- vs column-major, and whether it
reads a 3x4 or 4x4 stride — a layout mismatch is the standard way a rotation
degenerates into a scale).

## ROOT CAUSE FOUND AND FIXED — an RVA tunnel in `RwMatrixRotate`

`Math/RwMatrixRotate.cpp` read its two constants from **MASHED absolute
addresses**:

```cpp
static constexpr std::uintptr_t kDegToRadAddr = 0x005cd7a8u;  // pi/180
static constexpr std::uintptr_t kOneAddr      = 0x005cc320u;  // 1.0f
const float kDegToRad = *reinterpret_cast<const float*>(kDegToRadAddr);
const float kOne      = *reinterpret_cast<const float*>(kOneAddr);
```

Correct in the injected `.asi`, where MASHED's `.rdata` is mapped. In the
**standalone exe both read 0** — measured, not inferred:

```
RWMATROT-CONSTS kDegToRad=0 (expect 0.0174533) kOne=0 (expect 1) @005CD7A8/005CC320
```

The arithmetic then forces the observed symptom exactly:

```
angle_rad     = deg * 0          = 0
s             = sin(0)           = 0
one_minus_cos = kOne - cos(0)    = 0 - 1 = -1
R = I + s*K + (1-c)*K^2 = I - K^2 = diag(2, 1, 2)   (about the up axis)
```

`diag(2,1,2) * bodyForward` = `(2*0.0210, 0, 2*0.9998)` = **`(0.0420, 0, 1.9996)`**
— the measured `fwd0`, to four decimals. **Every axis-angle rotation in the
standalone was silently a scale.** This is the D0.7 RVA-tunnel class of defect,
the same category as the `Save/GameSaveBuffer` work earlier in this session.

### Fix and result

Materialised the same bit patterns as literals (`0x3c8efa35` for pi/180, `1.0f`)
— identical values in both targets, so bit-identical in the `.asi` and merely
correct in the exe. Constants now read `kDegToRad=0.0174533 kOne=1`.

**The car steers.** `car_yaw`, frozen at 1.5498 across three measurements over
seven weeks, now moves:

```
td=4.75  steer=+0.50  car_yaw=1.5123   pos=(-25.0,20.6)
td=5.02  steer=+0.50  car_yaw=1.4984   pos=(-24.9,22.7)
td=5.28  steer=+0.50  car_yaw=1.4862   pos=(-24.7,25.2)
td=5.81  steer=+0.50  car_yaw=2.0915   pos=(-24.8,30.9)
td=6.34  steer=+0.50  car_yaw=2.0671   pos=(-27.5,35.8)
```

and the path curves (x: −25.0 → −24.4 → −26.0 → −29.1).

### No regression on the default path

Default build (flag unset) re-run: scaffold telemetry is **identical to the
pre-fix control** (`car_yaw` 2.3562, 2.6274, 2.9160, 3.2093, 3.5026; speed caps
20.11), boot chrome intact (`B17-SUMMARY chrome=YES thunks=6/6`), race demo
completes `ok=1`. Checked because `RwMatrixRotate` has **29 callers across
vehicle / camera / HUD / font** — the same bug was silently degrading every one
of them in the standalone, so this fix may also explain unrelated standalone
oddities elsewhere.

### What this does NOT claim

- **Not** that the ported physics is now faithful. Steering exists; whether the
  handling matches the original is the A8 telemetry diff, still unrun.
- The internal velocity integrator still saturates `kSafetyInternal = 1500`.
  Untouched by this fix, and still not reaching the car.
- The yaw discontinuity and turn direction are addressed below.

### Follow-up: the rotation is exactly right; the discontinuity is a collision

**Vector level — correct.** Post-fix diag, steering samples:

```
steer=0.500  steerAng=8.5  |fwd0|=1.0000  |fwd3|=1.0000  angle(fwd0-fwd3) = -8.50 deg
steer=0.500  steerAng=8.5  |fwd0|=1.0000  |fwd3|=1.0000  angle(fwd0-fwd3) = -8.51 deg
```

Magnitude is restored to **1.0000** (was 2.0), and the rotation magnitude equals
the steer angle **exactly** (8.50 vs 8.5). The negative sign is the measuring
convention, not a defect: `atan2(z,x)` decreases for a positive rotation about
+Y, so −8.50° in that measure *is* +8.5° about the up axis. The Rodrigues build
and the transform are both behaving correctly.

**The discontinuity is a collision, and the scaffold has one too.** Both arms
show a yaw jump coinciding with a speed drop:

```
real physics  td=5.55 yaw=1.4750 speed=1500.00  ->  td=5.81 yaw=2.0915 speed=1018.83
scaffold      td=6.08 yaw=3.5026 speed=  20.11  ->  td=6.34 yaw=4.7805 speed=  11.14
```

A discrete heading change with simultaneous deceleration is the signature of
hitting track geometry, and it appears in the **working reference** as well. So
it is not evidence of a defect in the restored coupling.

**Turn direction differs between the two arms, and this cannot say which is
right.** Under the same `steer=+0.50`, scaffold yaw *increases* (1.5498 → 3.50)
while real physics *decreases* (1.5498 → 1.4750). One of the two disagrees with
the original's steer-sign convention. [UNCERTAIN] — resolving it requires the
original's telemetry on matched inputs, i.e. the A8 diff, which is exactly the
remaining D2 gate item. The scaffold is a kinematic approximation and is **not**
authoritative on sign, so "it differs from the scaffold" is not evidence against
the ported chain.

## The 1500 saturation is NOT a defect — but the clamp is mis-sized

I flagged "the internal velocity integrator saturates `kSafetyInternal = 1500`"
three times in this note as an open defect. **The code already says otherwise,
and I should have read it before flagging it.** `VehiclePhysicsRun.cpp:472-478`:

> "anti-overflow safety clamp ONLY. The OLD code hard-clamped record +0x9b0 to 45
> here, which destroyed the accel ramp… The recovered law's SOFT top-speed
> asymptote (below) governs the visible top speed now, so this clamp is set high
> — far above where the tanh saturates — purely to stop the ported chain's
> unbounded straight-line ramp (+0x9b0 grows ~77->553+, Integrate2 grip-clamp #6
> limits only LATERAL speed) from overflowing the round-tripped car_vel_."

So the raw chain velocity was never meant to be the car's speed: the coupling law
converts it through a tanh asymptote to `bs = desired`, which is why the car
moves at ~12 while the internal value reads 1500. Intentional, and consistent
with the 83-constant result above (fixing them changed nothing here).

### But the clamp's own premise is measurably false

The comment's justification is that 1500 sits "far above where the tanh
saturates", i.e. high enough never to bind. Measured against the **stock
original** (`drive_stock_a.msd`, 2731 frames, the archived stock arm):

```
max |v| at +0x9b0            = 4341.1
frames with |v| > 1500       = 756  (28% of the run)
vel.z trajectory   49%=+0  69%=+2766  74%=-19  79%=-335  84%=-3186  98%=+1070
sign changes in vel.z        = 10
```

Two differences from the port, both real:

1. **The original routinely exceeds 1500** — in 28% of frames, peaking at 4341.
   So the clamp is not a never-binding safety net; it is actively truncating a
   range the original occupies. The port pins at 1500 where the original swings
   to ±4341.
2. **The original oscillates; the port ramps monotonically.** Ten sign changes in
   the stock `vel.z` against a monotonic climb in the port. That is a deeper
   difference than clamp sizing and is not explained by it.

### Consequence for A8, which is the next D2 task

The A8 gate is a velocity/position diff against original telemetry on matched
inputs. On this evidence **it will fail at `+0x9b0..+0x9b8` by construction**:
the port cannot reproduce values above 1500, and its trajectory shape differs
regardless. Both need addressing before A8 can produce a meaningful verdict —
otherwise the diff reports a known-by-design mismatch and buries whatever real
signal is there.

[UNCERTAIN] whether the monotonic-vs-oscillating difference is the same
single-body reduction already blamed for the steer coupling, or a separate
missing longitudinal term. `Integrate2` grip-clamp #6 limiting only lateral speed
(per the comment above) is the obvious place to look first.

## Integrate2 grip-clamp #6 — lateral-only, confirmed by reading it

`Integrate2.cpp:347-374`. The clamp is gated on all-four-grounded
(`Ri(v,0x9e0) == 0x40800000`, i.e. float 4.0 read as int) and computes:

```cpp
float fwdDot = Rf(v,0x9dc)*Rf(v,0x9b8) + Rf(v,0x9d4)*Rf(v,0x9b0) + Rf(v,0x9d8)*Rf(v,0x9b4);
float fx = Rf(v,0x9b0) - fwdDot*Rf(v,0x9d4);   // velocity MINUS its forward projection
float fy = Rf(v,0x9b4) - fwdDot*Rf(v,0x9d8);   //   = the LATERAL residual
float fz = Rf(v,0x9b8) - fwdDot*Rf(v,0x9dc);
...
Wf(v,0x9b0, Rf(v,0x9b0) - fx*k);               // subtract a fraction of the LATERAL part only
```

`fx/fy/fz` are the velocity with its forward component removed, and only that
residual is scaled out. **The forward component is never reduced anywhere in the
function.** So the `VehiclePhysicsRun.cpp:477` claim — "Integrate2 grip-clamp #6
limits only LATERAL speed" — is exactly right, and there is no longitudinal
damping term to be missing here.

### Which undermines the standing root-cause attribution

The file header marks this a **verbatim port** (`WS-A6-COMPLETE`, "PENDING
diff-original C4"). If the transcription is faithful, then **the original has no
longitudinal damping in this function either** — so the original's bounded,
oscillating velocity must be produced somewhere else entirely, not by a damping
term the port dropped.

That matters because `COLLISION_GATE_BRIEF_D1_2026-07.md:56` attributes **both**
symptoms to one cause: the single-body reduction "loses forward damping AND turn
coupling". **The turn-coupling half is now disproven** — it was the
`RwMatrixRotate` RVA tunnel (`9cc41fa8`), nothing to do with body count. The
damping half rests on the same argument and the same evidence, so it should no
longer be assumed either.

[UNCERTAIN] where the original's longitudinal bound actually comes from.
Candidates not yet examined: a speed-dependent falloff in the drive force (A4
`FUN_00470670`), aerodynamic drag elsewhere in the chain, or the 2-body loop
genuinely doing it. The stock trajectory shape argues mildly against a simple
drag term — it **oscillates with 10 sign changes** rather than settling to a
terminal velocity, which is spring-like rather than damping-like.

## The drive force has NO speed falloff — so the bound must come from friction

`Integrate2.cpp:160-182`, the per-wheel drive-force block. `drive` is built from:

```cpp
float drive = (float)(unsigned)input[4];              // accel byte, 0..255
if (mode == 7) drive = 0.0f;
if (Ri(v, 0x28) != 0) drive = (float)(100 - Ri(v,0x28)) * drive * k0p01;   // damage scale
if (trackId == -0x69e1a6 || trackId == -0xe17f4c) { ... drive = fwd * drive; }
if (k160 < drive) drive = 160.0f;                     // hard cap on the INPUT, not on speed
drive = drive * local_cc;
Wf(v, 0xb14, Rp(p,0x1f) * drive + Rf(v,0xb14));       // along the wheel's steered forward
```

**Nothing reads the current speed to reduce it.** The only velocity-dependent
term (lines 166-174) uses the velocity *direction* — a normalised dot raised to
the 8th power — and applies to two specific track IDs only, and it *raises*
drive toward alignment rather than damping it. The `160` cap limits the accel
input, not the resulting speed.

So a constant accel input yields a constant force, which integrates to a linear
velocity ramp forever. **That fully explains the port's monotonic unbounded
ramp** — and, since this block is part of the same verbatim A6a port, the
original's drive force is speed-independent too.

### Which localises where the original's bound has to be

Line 326 is the whole linear integration:

```cpp
Wf(v, 0x9b0, linTerm * (Rf(v,0xb14) + l_b8) + Rf(v,0x9b0));
```

Two contributions: the control force `+0xb14` (just shown to be
speed-independent) and the accumulator `l_b8` from the **cross-product friction
block #5** / suspension block #4. Since clamp #6 is lateral-only and the drive
force has no falloff, **`l_b8` is the only remaining place a velocity-opposing
term can live.** A friction force that grows with contact-patch slip would
balance the drive force and produce exactly the terminal velocity the stock
capture shows.

**Next measurement, and it is cheap:** extend the `MASHED_COUPLING_DIAG` line
with `l_b8`/`l_b4`/`lin_b0` and the grounded count `+0x9e0`. If the accumulators
are ~0 while the car is grounded, the friction block is not contributing and that
is the defect. Note the 83-constant fix did **not** change the saturation, and
`Integrate2.cpp` was never affected by the RVA-tunnel class anyway — it already
uses `Cf(0x…)` bit-pattern literals marked EXACT — so whatever is wrong with the
accumulators is not a stale-constant problem.

A candidate worth checking in the same pass: the accumulators are fed by wheel
contact data from the collision solver. If contacts are not being produced in the
standalone, friction is structurally zero regardless of the arithmetic.


## RESOLVED — there is no terminal velocity to reproduce; the clamp was the whole defect

Instrumented the friction accumulators (`Integrate2.cpp`, env-gated) and compared
against the archived stock arm. Both of my earlier readings were wrong.

**The accumulators are NOT zero.** They exist and grow with speed:

```
spd=   0.000  ctrl=(16813,0,799823)        accum=(0,0,0)               grounded=0x40800000
spd= 925.034  ctrl=(685257,0,3183196)      accum=(9642,0,-25598)       grounded=0x40800000
spd=1496.917  ctrl=(-1532765,0,3731676)    accum=(-62110,-1564,61390)  grounded=0x40800000
```

The car is grounded throughout and friction is being produced. My first reading —
"friction is ~60x too small to balance drive" — assumed a balance is supposed to
happen. **It isn't.**

**The original has no terminal velocity either.** From `drive_stock_a.msd`
(`+0x9e4`), the stock run decomposes into rounds:

```
zero-speed runs (resets):  frames 0..897, 1366..1379, 1571..1817, 2427..2662
peak |v| per active round: 4275.1 / 4070.4 / 4344.5 / 1970.3
```

It **ramps unbounded within a round, then resets to ~0 at the round boundary**.
The "oscillation with 10 sign changes" I reported earlier was these round resets,
not damping. Stock control-force magnitudes are the same order as ours (max
12.3M vs our 4.0M), so the drive force is not mis-scaled either.

So the unbounded ramp is **faithful**, and the only unfaithful element was
`kSafetyInternal = 1500` truncating a range the original occupies (28% of its
frames exceed 1500; it reaches 4344).

**Fix applied:** raised to `16384.0f` — ~3.8x above the highest observed original
value, preserving the stated anti-overflow purpose while never binding in
practice. Measured after: speed now ramps 1698 → 1254 (collision) → 1560 → 1882
→ 2211 instead of pinning at 1500.00, i.e. the same ramp-and-reset shape as the
original. Default (scaffold) arm unchanged: `car_yaw` 3.2093 / 3.5026 / 4.7908 /
4.9732, speed caps 20.11, `B17-SUMMARY chrome=YES thunks=6/6`.

This closes the saturation thread. It also removes one of the two reasons A8 was
predicted to fail by construction; the steer-sign question remains open.

## What is NOT established

- **Why** the coupling is lost. This run reproduces the symptom; it does not
  localise the missing term.
- **Whether `MASHED_COUPLING_DIAG` fired in this run.** It writes to a
  cwd-relative `coupling_diag.log` (`VehiclePhysicsRun.cpp:606`), NOT into
  `mashed_re.log` where the first version of this note looked — that was a
  second reading error in the same pass. The file does exist at the repo root
  (50,273 bytes) and its content is the authoritative evidence used in the
  correction above. But its mtime is 09:35 while this run was ~17:00, so this
  run appears **not** to have appended. [UNCERTAIN] and NOT resolvable by
  content, because the defect is deterministic: fresh lines would be
  byte-identical to the existing ones. Resolve by deleting the file and
  re-running — if it does not reappear, the diag block is not on the live path,
  which would matter well beyond the diag (it sits immediately after
  `io.drive_speed = bs`, the value that actually drives the car).
- Whether the two failures share one cause. Missing forward damping and missing
  steer coupling are consistent with a single lost two-body loop, but that is
  the pre-existing hypothesis, not a finding from this run.

## A8 BLOCKED — the original-side steer capture is invalid, on two independent counts

Measured 2026-08-24, read-only, no boot spent. Artifact:
`verify/a8_steer_20260823/orig_steerR.msd` (MSD1, `rec_size=0xd04`,
`base_va=0x8815a0`, 2335 frames), the original-side capture taken 2026-08-23 for
the A8 steer-sign question. **It cannot answer that question and must be
re-taken.** Both defects would have survived into a false verdict.

### 1. The chosen field is unobservable by construction, and would have produced a FALSE GREEN

`re/tools/statediff/field_trace.py` reads the steer angle at `+0x1a8` /
`+0x26c`. In this capture both are **exactly 0.0 in all 2335 frames** (0 nonzero,
`max|v|` = 0), while **315 of 832** dword slots in the record do vary — so the
capture is live, the field is not.

The offsets are **not wrong**. `VehiclePhysicsRun.h:35-36` derives them by RVA
(`wheelN steer = +0x16c + N*0xC4 + 0x3c` => `w0=+0x1a8`, `w1=+0x26c`,
`w2=+0x330`, `w3=+0x3f4`). They are unreadable for a mechanical reason:

- **A4 zeroes all four slots at every entry** — `0x004706c1..d3`, ported verbatim
  at `VehicleControl.cpp:86` (`Ib(v,0x3f4)=0; Ib(v,0x330)=0; Ib(v,0x26c)=0;
  Ib(v,0x1a8)=0;`).
- **A5 `FUN_0046ddb0` Phase 0 consumes them inside the same physics step**
  (`VehiclePhysicsRun.h:42-44`): it reads each wheel's angle at `piVar12[0xf]` and
  rotates that wheel's forward axis.

So they are per-frame scratch. Any capture that samples the record at a frame
boundary reads 0 **by construction**, on either side. Had the standalone-side
capture been taken and diffed, it would also have read 0, the diff would have
been bit-identical, and A8 would have reported GREEN on a field that is
structurally always zero — the same false-GREEN class as the iter17 `0x00482900`
incident in the orchestrator ledger, where a zero-argument `arg_type` on a
two-argument function made both sides read identical garbage.

**Corrected citation, filed as a documentation defect:** `field_trace.py`'s
docstring cited `scenario_launch.py:133-149` as the source for these two offsets.
That block documents `+0x928/+0x958/+0x9b0..b8/+0x9c0/+0x9d4/+0x9dc/+0x9e0/
+0x9e4/+0xb20` and contains **neither** `+0x1a8` nor `+0x26c`. The real source is
`VehiclePhysicsRun.h:26-51`. Docstring corrected in place.

### 2. The capture shows no steering at all

Heading deltas are only defined **within one active round** — the stock car ramps
unbounded then resets to ~0 at each round boundary (established above, "there is
no terminal velocity"). The tool's original summary unwrapped the whole
min-speed-filtered series as one, joining across the excluded reset frames and
reporting `velH delta -13.0160 rad`. **That figure is an unwrap artifact, not a
turn.** Fixed: the tool now segments on frame-index gaps and reports per-round
deltas, giving 15 contiguous segments. The two substantial ones:

```
round 0: frames  888..1086  n=199  velH -1.5708 -> -1.5844  delta -0.0136
round 1: frames 1089..1367  n=279  velH +1.5899 -> +1.5732  delta -0.0166
```

**-0.0136 rad is -0.78 degrees over 199 frames at speed 3000-4000.** A held
steer at full lock is documented as ~16.93 deg of wheel angle
(`VehiclePhysicsRun.h:38-39`); this car is going straight. Round 3's
`delta -3.1338` (~-pi) is a heading flip, i.e. a collision/respawn, not a turn,
and rounds 5-14 are 1-7 frame fragments of a car sitting at speed 10-13 with
`yawrate` ~0 for the last 600 frames.

The economical reading is that the `--statediff-steer` injection did not take
effect in this run. It is **not confirmable from the artifact**: the directory
holds the `.msd` and nothing else, with no sidecar recording the flags used.

### What A8 needs instead

1. **Re-take the original capture** with `--statediff-steer +1` and **persist the
   invoking command line next to the `.msd`**, so "was the steer actually
   injected" is answerable from the artifact rather than by inference.
2. **Assert the injection took effect before spending the standalone boot** — the
   cheap check is a non-trivial per-round `velH` delta in the original. A capture
   whose driving rounds are straight is a failed capture, not a datum.
3. **Derive the steer-sign law from persistent record state, never from
   `+0x1a8`**: yaw rate `+0x9c0` (A6a's body output), velocity `+0x9b0..b8`,
   forward axis `+0x9d4/+0x9dc`. These are integrated body state, not per-step
   scratch, so they survive the sample boundary.

`--statediff-steer` (`re/frida/scenario_launch.py`) is sound and unchanged; the
defect is in the field choice, the summary statistic, and the missing provenance.

## A8 UNBLOCKED — the original's steer-sign law, measured

Re-take 2026-08-24: `verify/a8_steer_20260824/orig_steerR.msd` (2335 frames) plus
`orig_steerR.msd.provenance.json`. Stock original (`"hooks": ""`), one boot.

```
py -3.12 re/frida/scenario_launch.py --statediff-out verify/a8_steer_20260824/orig_steerR.msd \
         --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38
```

### RETRACTION — the "unobservable by construction" claim above is WRONG

The section immediately preceding this one concluded that `+0x1a8`/`+0x26c` are
per-frame scratch, unreadable at a frame boundary "by construction", because A4
zeroes all four at entry (`0x004706c1..d3`) and A5 Phase 0 reads them. **That
inference does not survive re-measurement.** Same tool, same offsets, same sample
point:

| capture | steerAng0 nonzero | mean |
|---|---|---|
| `a8_steer_20260823` | **0 / 1428** | +0.0000 |
| `a8_steer_20260824` | **1441 / 1441** | **+33.2253** |

The entry-zeroing is real, but A4 **writes the slot later in the same call** and
the value persists until the next call, so a frame-boundary sample sees it.

**The real and sufficient cause was defect 2 alone, and it was duller than the
mechanism I proposed:** the harness wrote the steer command into descriptor bytes
`[2]`/`[3]` while A4 reads `[0]`/`[1]`, so `if (input[0] != 0)`
(`VehicleControl.cpp:118`) never fired and the slot kept its zeroed value.
`scenario_launch.py:90` had asserted "block[2]/[3]=steer" against the RVA-cited
`[0]`/`[1]` map in `VehiclePhysicsRun.h:67-73` — both agreed `[4]`=accel, and only
steer disagreed. Fixed: `armCook` now writes `[0]`/`[1]`, keeping the legacy
`[2]`/`[3]`/`[0xe]`/`[0xf]` writes so the accel-only baseline of every prior
statediff capture is byte-unchanged. What those legacy bytes are is **[UNCERTAIN]**
— not established here, do not assume they are dead.

**Generalisable trap:** "the field is structurally unobservable" and "nothing was
commanded, so the field is legitimately zero" produce an *identical* all-zero
capture. An all-zero read alone cannot distinguish them. Command a known-nonzero
input and re-measure before theorising. (The false-GREEN warning stands on its own
merits either way: with no steer commanded, both sides of an A8 diff read 0.)

### The measured law

Magnitude check first: mean `steerAng0` **+33.2253 deg**.
`VehiclePhysicsRun.h:38-39` predicts `input * (+0x190 = 34.0) * (1/256) * 0.5` =
**16.93 deg** at full lock, and `255 * 34.0 / 256 = 33.867` is that value *before*
the `* 0.5`. The grip branch recovers the factor: `force = (f + kFilterClamp) *
force * kGripMul` with `kFilterClamp = 6000` and `kGripMul ~= 1/6000`
(`VehicleControl.cpp:125-127`), i.e. `force * (1 + f/6000)`, which doubles it as
the filtered input saturates. So 33.2253 is consistent with the documented scale,
not a contradiction of it.

The single sustained driving round (frames 887..2316, n=1430):

```
steerAng0 mean  +33.2253   (nonzero 1441/1441)
yawrate   mean  +1.000046  sign +
velH      -1.5709 -> -54.8896   delta -53.3187  sign -
fwdH                             delta -50.5689  sign -
```

**LAW: steer = +1  =>  steerAng0 > 0  =>  yawrate > 0  =>  `atan2(vz, vx)`
heading DECREASES.**

Note the sign inversion in that chain, because it is the part A8 has to get right:
the yaw rate is **+1.0** while both heading measures **decrease** by ~53 and ~51
rad. Yaw rate `+0x9c0` (world-Y component of angular velocity) and
`atan2(vz, vx)` therefore run in **opposite** senses — consistent with a
left-handed / Y-up basis, where rotation about `+Y` decreases `atan2(z, x)`.

**Direct consequence for the port:** `TrackRenderer.cpp:2553` integrates `io.yaw`
from the chain's yaw rate while treating `io.yaw` as a velocity heading
(`forward = {cos, 0, sin}`). On this measurement those two conventions differ by a
sign, so integrating `+yawrate * dt` into that heading turns the car the **wrong
way**. That is a concrete, checkable prediction and it is the next thing to test.

Consistency cross-check: 1430 frames of a frame-locked race at ~30 fps is ~47.7 s,
and `1.000046 rad/s * 47.7 s ~= 47.7 rad` against the measured 53.3 rad of heading
change — same order, the residual being instantaneous-rate sampling and the
frame-rate assumption. **[UNCERTAIN]** the exact frame rate of this run was not
recorded; the sign result does not depend on it.

Rounds 1-7 are 1-2 frame fragments of the post-round car and carry no usable
delta. The provenance sidecar is committed beside the capture; the 7.8 MB `.msd`
itself is local-only per `verify/EVIDENCE_MANIFEST.md`.

### A8 addendum — the negative steer arm, and what it does and does not show

Second boot 2026-08-24: `verify/a8_steer_20260824/orig_steerL.msd` (2334 frames),
stock original, `steer = -1` per its committed `.provenance.json`.

| quantity | `steer +1` | `steer -1` | flipped? |
|---|---|---|---|
| `steerAng0` mean | **+33.2253** (1441/1441 nonzero) | **−33.2346** (1416/1416) | **yes**, magnitudes agree to **0.03%** |
| `yawrate` sign | + | − | **yes** |
| `fwdH` round-0 delta (body heading) | −50.5689 | **+1.5234** | **yes** |
| `velH` round-0 delta (velocity heading) | −53.3187 | −1.6354 | **no** |

**What is established.** `[1]` produces the opposite-signed steer angle of equal
magnitude. The 0.03% agreement between `+33.2253` and `−33.2346` is as clean an
antisymmetry as this harness can show, and it behaviourally confirms three
witnesses that were previously code-only: `VehiclePhysicsRun.cpp:404-405`
(`input[1] = (st < 0.f) ? m : 0`), A4's `FCHS` negation in the `input[1]` branch,
and the `[0]`↔`[1]` invert swap at `0x00496717`.

**What is not, and why it is not a contradiction.** The velocity heading did not
flip — both arms are negative. The `−1` car **collided at frame 966**, leaving 80
driving frames in round 0 and then 1263 frames stationary at speed ~0.1. Across
that collision `fwdH` moves **+1.5234** while `velH` moves **−1.6354**: the body
rotated one way while the velocity vector went the other, which is a skid, not a
steer. So `velH` in this arm never describes a clean arc and the arm-to-arm `velH`
comparison is not a valid test. This is a **capture-quality gap, not a knowledge
hole** — the sign convention A8 depends on (`+yawrate` ↔ decreasing
`atan2(vz,vx)`) was established by the `+1` arm, which held a single 1430-frame
round. No new uncertainty was opened; the gap is recorded on U-9043's resolution.

If `velH` arm-symmetry is ever wanted, re-run `-1` on a track or spawn where a
left turn does not immediately meet geometry, and read round 0 before the first
collision.

## A8 standalone arm — THE PREDICTED SIGN INVERSION DOES NOT EXIST

Measured 2026-08-24. Evidence `verify/a8_standalone_20260824/` (log + extracted
`play_demo.txt` + `PROVENANCE.txt`). Recipe, verbatim from
`WS_A8_REALPHYS_2026-07-01.md:15`: `MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1
MASHED_PLAY_DEMO=1 MASHED_GOTO=6 MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0`. Process
self-exited cleanly.

### The prediction, and its refutation on two independent grounds

The section above predicted: *"`TrackRenderer.cpp:2553` integrates `io.yaw` from
the chain yaw rate while treating `io.yaw` as a velocity heading, so those
conventions differ by a sign and `+yawrate*dt` steers the car the wrong way."*
**That prediction is wrong.** Both halves of it fail.

**1. There is no yaw-rate integration to invert.** `VehiclePhysicsRun.cpp:596-603`
does not read the yaw rate at all — it relaxes `io.yaw` toward the chain
velocity's heading, in the *same* `atan2(z,x)` convention:

```c
float velHeading = std::atan2(cvz, cvx);   // forward={cos,0,sin} -> heading=atan2(z,x)
float err = velHeading - io.yaw;
io.yaw += err * frac;
```

`0x9c0` is never read in that file. So the comment at `TrackRenderer.cpp:2553`
("recovered chain-grip heading (+0x9c0)") is itself misleading — the value is a
velocity heading, not a yaw-rate integral. Worth fixing, but it is a comment
defect, not a behavioural one.

**2. The measured heading turns the correct way.** Under `steer=+0.50`:

```
td=4.22  car_yaw 1.5498 -> 1.5406 -> 1.5269 -> 1.5131 -> 1.4993 -> 1.4862 -> 1.4759
```

**Decreasing** — the same direction as the original, whose `velH` runs
−1.5708 → −54.8896 under `steer=+1`. Sign agrees. D2's remaining work is
therefore **not** a sign bug, and nobody should "fix" one.

**A trap I nearly fell into, recorded because it is reusable.** The last sample
of the run reads `car_yaw=2.1000`, which looks like the heading *increased*. It
did not: at `td=5.81` speed drops 1679.87 → 1254.32 and the position jumps, i.e.
the collision already documented in `efad2dc2` ("the yaw jump is a collision, not
a defect"). Reading the final line instead of the trajectory inverts the
conclusion. Always segment on the collision.

### What remains open, stated without inventing a number

The turn **rate** looks much lower than the original's, but the two are **not
measured in comparable terms** and no factor is claimed here:

| | steer | heading change | speed regime |
|---|---|---|---|
| original (`orig_steerR.msd`) | +1.0 | −53.3187 rad over one 1430-frame round | ~3000–4000 (record units) |
| standalone | +0.50 | −0.0739 rad over 1.33 s | ~340–1680 (world units) |

Different steer magnitude, different speed regime, different units and time base.
A like-for-like rate comparison needs the standalone driven at full lock in a
matched speed band, which this demo does not provide.

**Harness limitation found:** the round ends at **t=6.6 s** (`R6 ROUND END
winner=car3`), while `MASHED_PLAY_DEMO`'s steer ramp needs ~24 s to walk
0 → +0.5 → −0.5 → +1.0 → −1.0 (`exe_main.cpp:2640-2643`). So this recipe only
ever exercises the `+0.5` phase; the negative and full-lock phases are
**unreachable** and every prior A8 run on this recipe had the same blind spot. A
longer round (more laps, or a rules config that does not end at 6.6 s) is needed
before the standalone's `−0.5`/`±1.0` response can be measured at all.

### Full steer ramp, all four phases — sign law COMPLETE, and a new asymmetry finding

The run above only ever reached `steer=+0.5`. Cause found: **`MASHED_RACE_DEMO`'s
own nav script** ends the race after round 1 (`NAV_DEMO phase=1 01_action` →
`phase=2 02_back_to_menu` → `phase=3 race-demo done`), not the race rules —
`MASHED_RACE_MODE=laps MASHED_LAPS=5` changed nothing. The missing flag is
**`MASHED_DRIVE_HOLD=1`** ("G2: hold InRace for sustained-drive calibration",
`exe_main.cpp`).

**Recipe defect worth fixing where it is documented:**
`WS_A8_REALPHYS_2026-07-01.md:15` — the recipe this note has been quoting as
canonical — **omits `MASHED_DRIVE_HOLD`**, which is why every A8 run on it caps at
6.6 s and never leaves the `+0.5` phase. `WSPHYS_DRIVEHOLD_2026-07-06.md:15` has
it. Use the latter.

With `MASHED_DRIVE_HOLD=1` all four phases are reached (114 samples, td 0→29.9).
Segmented on steer changes *and* on collisions (speed drop >15%), collision-free
segments only:

| steer | mean yaw rate (rad/s) | sign | segments |
|---|---|---|---|
| **+1.00** | **−0.0834** | − | 4 |
| **+0.50** | **−0.0313** | − | 2 |
| 0.00 | 0.0000 | 0 | 1 |
| **−0.50** | **+0.0074** | + | 1 |
| **−1.00** | **+0.0262** | + | 2 |

**ESTABLISHED — the sign law is complete and correct.** Positive steer decreases
the heading, negative steer increases it, zero steer holds it, and the response is
monotonic in steer magnitude. Every one of the 10 collision-free segments carries
the expected sign. This matches the original on **both** arms (`+1` → heading
decreases; `−1` → `steerAng0` and `fwdH` flip). **The standalone steers the right
way. There is no sign defect in D2.**

**NEW CANDIDATE DEFECT — left/right asymmetry.** The magnitudes are not
symmetric: `+1.00` gives `−0.0834` against `−1.00`'s `+0.0262` (**3.2x**), and
`+0.50` gives `−0.0313` against `−0.50`'s `+0.0074` (**4.2x**). The original is
symmetric to **0.03%** (`steerAng0` `+33.2253` vs `−33.2346`), so this asymmetry
is a standalone-side property, not a faithful reproduction. That is the first
concrete, quantified D2 defect candidate to come out of A8.

**Caveat, stated because it could bias the numbers:** the car collides
repeatedly, so segment lengths differ between arms — the `+1.00` mean rests on
four short segments (3-6 samples each, all ended by collision) while `−0.50`
rests on one clean 16-sample segment ended by a steer change. The asymmetry is
consistent across both magnitudes and all ten segments, but a cleaner run with
fewer collisions should confirm the ratio before it is treated as a measured
constant.

**Still not claimed:** any absolute rate comparison against the original. The
standalone's ~0.03–0.08 rad/s versus the original's ~1.0 rad/s spans different
units, speed regimes and time bases; no factor is asserted here.

Evidence: `verify/a8_standalone_20260824/play_demo_drivehold.txt` (114 samples)
and `mashed_re_drivehold.log`.
