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
