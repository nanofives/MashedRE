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

Both failures are confirmed against a working reference in the same format:

| Property | Scaffold (default) | Real physics | Ratio |
|---|---|---|---|
| Top speed | caps at **20.11** | saturates **1500.00** | **75x** |
| `steer=+0.50` → yaw | 1.5498 → 4.97 (turns) | **frozen at 1.5498** | no coupling |

The real-physics arm is not "somewhat off" — it is 75x too fast with no heading
response at all. `kSafetyInternal = 1500` (`VehiclePhysicsRun.cpp:480`) is a
safety clamp, so the true unclamped velocity is unbounded, not 1500.

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

## What is NOT established

- **Why** the coupling is lost. This run reproduces the symptom; it does not
  localise the missing term. `MASHED_COUPLING_DIAG=1` is still wired
  (`VehiclePhysicsRun.cpp:597-601`) but emitted no lines to
  `mashed_re.log` in this run — [UNCERTAIN] whether the diag path is reachable
  on the current default code path, which is the obvious next thing to check
  since the 2026-07-01 note quotes its output directly
  (`cv=(31.56, 0.00, 1499.67) … velH=1.5498`).
- Whether the two failures share one cause. Missing forward damping and missing
  steer coupling are consistent with a single lost two-body loop, but that is
  the pre-existing hypothesis, not a finding from this run.
