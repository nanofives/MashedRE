# KICKOFF — velocity-vector motion model (unblocks the A8 orientation fix)

Written 2026-08-25 at the end of the A8 session. Paste the fenced block below as the
first message of a fresh session. Everything it needs is already committed.

---

```
Mashed RE. Task: replace the standalone's heading-plus-scalar motion model with a
velocity-vector one, so the already-ported body-orientation integrator can be wired.

READ FIRST, in this order:
  - re/orchestrator/state.json, item a8_yawrate_speed_inverted (the full narrative;
    read the LAST entry, it supersedes the earlier ones and several are retractions)
  - re/analysis/data/A8_body_heading_law_20260825.md (Q3/Q4 + Follow-ups 1-4)
  - mashedmod/src/mashed_re/Vehicle/BodyOrientationIntegrate.cpp (the ported law,
    currently UNWIRED — read its header comment, it explains why)

WHAT IS ALREADY DONE (do not redo any of this):
FUN_0046e9e0, the original's per-tick body-rotation writer, is fully ported and cited:
the omega steer law, the +0x144 accumulator, the mode-7 branch, and the 0x004c4680
ortho-normalize. Every constant is traced to an address, every branch was verified
against the instruction stream, nothing was invented, and there is no open [UNCERTAIN]
in the law. It compiles and is deliberately not called from anywhere.

THE PROBLEM YOU ARE SOLVING:
The port cannot represent slip, by construction. VehiclePhysicsRun.cpp:559-564 projects
the chain velocity onto the body forward {cos,0,sin(io.yaw)}, :600-612 relaxes a scalar
g_bodySpeed toward that projection with PD gain 20, and TrackRenderer integrates
pos += {cos,0,sin(io.yaw)} * drive_speed * dt. That is only correct while io.yaw equals
the velocity heading, which is exactly what the deleted velocity-chasing alignment law
guaranteed. Give the body an independent orientation and the projection shrinks by
cos(slip), the scalar speed decays, and the car stops accelerating.

MEASURED TWICE, both reverted: wiring the orientation dropped median speed from a 378
baseline to 7.28 (attempt 1) and 5.56 (attempt 2, which also made the integrated matrix
BE xform and used the real ortho-normalize). Attempt 2 refuting attempt 1's hypothesis is
what located the real cause. Do not attempt a third variation of the same wiring.

WHAT THE ORIGINAL DOES: FUN_0046e9e0 integrates position and orientation SEPARATELY in
the same function, with nothing forcing them to agree:
    position:    EBX[0xc..0xe] = EDI[0xc..0xe] + k*dt*ESI[0x26c..0x26e]   (from velocity +0x9b0)
    orientation: EBX[row]      = EDI[row] + (omega x EDI[row])            (from omega)

THE CHANGE, three coupled parts (all three or none — that is the lesson from today):
  1. Position integrates from the velocity VECTOR. Retire io.drive_speed, g_bodySpeed,
     the PD relaxation, and MASHED_CHAINSCALE (which exists only to convert the scalar).
     Check every consumer of drive_speed/car_speed_ before removing them.
  2. Wire BodyOrient_* : seed at spawn, BodyOrient_OmegaFromSteer +
     BodyOrient_IntegrateStep per substep, io.yaw = BodyOrient_Heading.
  3. Delete the velocity-chasing lag and the MASHED_ALIGNRATE knob.

GATES — apply in this order, and stop if the first fails:
  a. SPEED PROFILE FIRST, before looking at any yaw number. Recipe:
     MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
     MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1 MASHED_WIN_POS=left-bl
     (DRIVE_HOLD is mandatory; WS_A8_REALPHYS_2026-07-01.md:15 omits it and caps runs
     at 6.6 s. Telemetry lands in mashed_re.log as PLAY-DEMO lines.)
     Baseline to match: verify/a8_standalone_20260824/play_demo_drivehold.txt,
     median 378.31 / max 3664.81. Run-to-run variation on this recipe is real, so
     "match" means same order, not same digits.
  b. Only then the yaw-vs-speed shape. Ground truth (original): 0.109 / 0.608 / 0.854 /
     1.139 / 1.451 rising. Pre-fix port: 0.0638 / 0.0827 / 0.0785 / 0.0681 / 0.0532
     falling. Report n per band and refuse to conclude from n<=3.

TRAPS THAT COST THIS SESSION FOUR RETRACTIONS — do not repeat them:
  - A DIAG SAMPLE CAP IS A REGIME FILTER. A 60-sample cap covered only the opening
    straight-line phase where steer was 0 on 58 of 60 frames, which manufactured a
    "grip is 7 orders of magnitude short" finding that did not exist. Log the INPUT that
    is supposed to drive the effect (the steer bytes) on the same line as the effect.
  - GREPPING A LITERAL RECORD OFFSET MISSES WRITERS. The port writes fields as dword
    indices off a computed base (local_90[0x17] is +0x1f0 off record+0x194), so
    `grep 0x1f0` proved a writer absent that was present all along.
  - A FIELD READING 0 DOES NOT DISTINGUISH "nobody writes it" from "somebody writes 0".
  - NORMALIZE CRLF (tr -d) BEFORE DIFFING these logs, or a naive diff reports every line
    changed. That false positive fired twice.
  - RUN A SAME-BINARY CONTROL before attributing any delta to a change. This recipe is
    deterministic for ~58 samples and diverges in the tail.

RULES: this is a port, not a tuning exercise. No magic constant invented to make a number
look right, and per the S-DoD rule an env-gated alternative does not count as done — the
old MASHED_ALIGNRATE default of 7.0 is precisely the mistake being undone here. Ghidra
work goes to a claude3 child (write findings to a file, return a compact table). Track
PIDs you spawn and kill only those. Update the a8_yawrate_speed_inverted ledger item via
re/orchestrator/orch.ps1 set.
```

---

## Why this is a separate session

It touches the physics/render boundary and every position-dependent consumer downstream
(camera, AI targeting, lap/gate detection, collision response). It needs its own baseline
gate and its own budget, which is why it was parked rather than forced in at the tail of
the A8 investigation.
