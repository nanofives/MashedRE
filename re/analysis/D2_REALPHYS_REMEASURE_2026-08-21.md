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
