# KICKOFF — the A8 slip deficit (next session)

Paste the block below as the opening prompt. Everything it references is committed on
branch `a8-velocity-vector-motion` (three commits, fast-forward from `main`).

---

Mashed RE. Task: find why the standalone's SLIP ANGLE is short of the original's, when
every other measured quantity in the vehicle chain now matches.

READ FIRST, in this order:
  - re/orchestrator/state.json, item a8_yawrate_speed_inverted. Read the LAST entry
    only. There are ~20 earlier ones and several are retractions of each other.
  - re/analysis/data/A8_velocity_vector_motion_20260825.md, follow-ups SEVENTEEN
    through TWENTY-ONE. Skip one to sixteen unless you need the history: they contain
    five conclusions that were later withdrawn, and the withdrawals are marked in
    place.
  - the "Traps" section below. It is not boilerplate. Six distinct failure modes
    actually fired in the session that produced this state.

WHAT IS ALREADY DONE — do not redo any of it. All cited, all committed:
  - velocity-vector motion model; BodyOrient_* (FUN_0046e9e0) wired. Position and
    orientation integrate separately, as the original does.
  - time base corrected: the dispatcher budget unit is 1/3000 s, a fixed 50 per frame,
    not dt*1000.
  - A4 cadence corrected: A4 (and so A5/A6a/A6b) runs ONCE per frame at dispatcher
    step 3, outside the substep loop.
  - off-mesh recovery relocates the car instead of compounding a 0.6 speed damping.
  - all seven FUN_004a2c48 (_ftol2) sites ported. They were stubbed to 0.
  - suspDtTerm comes from the course constant DAT_00803324 = 1560.0, not the budget.
  - six decimal literals whose hex was right and whose gloss was wrong are bit-exact.

THE PROBLEM. At MATCHED internal speed, port vs original:
    slip 1000-1500   0.0323 vs 0.0815   (2.5x short)
    slip 1500-2000   0.1321 vs 0.1925   (1.5x short)
    slip 2000-2600   0.1925 vs 0.2498   (1.3x short)
while, in the same bands:
    gate (a) driving-median   ~1745 vs 1760      (within 1%)
    gate (b) yaw rate         2.07 / 2.55 / 2.55 vs 2.31 / 2.66 / 2.65
    per-wheel force |fTot|    within 4-22% above speed 500
    force DIRECTION           lateral fraction within 2%
    grip chain l_60           within 10-19%, and le4 saturates at its 1024 cap on
                              BOTH sides
    record fidelity p[0x1a]   1088.31 vs 1086.83
The shortfalls do not track. Neither the force nor the grip path explains the slip.

ELIMINATED BY MEASUREMENT — do not re-open without new evidence:
  grip/clamp chain, force magnitude, force direction, the constants, the gearbox,
  the per-gear drive table, the A4 call count, the gated velocity drag (+0x1f0 is an
  RGBA surface key), +0xb0c, +0x498/+0x49c, the +0xb14 accumulation cadence, A6b
  (returns immediately when grounded), A5 (never writes the velocity triple).

THE LAST UNTRIED INPUT: p[-9..-7] (wheel base -0x24/-0x20/-0x1c), the wheel mount
offsets that feed le0/ldc/ld8 and hence le4/ld4. These are record fields, so they can
be read straight out of verify/a8_steer_20260824/orig_steerR.msd and compared with a
runtime log, WITHOUT a Frida hook. Note the technique that made the last three
comparisons cheap: check whether a "local" is a pure function of record fields before
reaching for an in-function hook. le4 and ld4 both were.

GATES — run the recipe, then apply in this order:
  MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
  MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1 MASHED_WIN_POS=left-bl
  plus MASHED_MOTION_DIAG=1 (per-frame slip/yaw/gear/force) and/or
  MASHED_A6_DIAG=1 (grip chain) and/or MASHED_COUPLING_DIAG=1 (friction accum).
  a. CHECK THE SAMPLE COUNT FIRST. A healthy run gives ~1100 motion lines. This
     recipe intermittently stalls in the frontend nav and yields 0 or ~400. A short
     run has a different speed distribution and its bands are NOT comparable — one
     such run reported gate (a) 2204 and meant nothing. Allow 95s and retry.
  b. gate (a) speed profile: driving-only median, target ~1760. Run-to-run spread on
     this metric is about +-30, so do not attribute anything smaller.
  c. gate (b) yaw rate by speed band, reseeds EXCLUDED (the diag prints `reseed=`;
     a reseed is a heading discontinuity, not an integration step, and including
     them inflates bands by 2x).

TRAPS — all six of these fired in the previous session:
  1. A DEFECT FOUND UNDER A BROKEN REGIME MAY NOT EXIST. Three findings dissolved
     once the surrounding state was fixed. RE-MEASURE ANY PRIOR FINDING BEFORE
     BUILDING ON IT.
  2. A MODEL CAN OUTLIVE ITS VALIDITY. The equilibrium-slip model
     `slip = (w*dt)/k` fit at ratio ~1.0, then stopped fitting (0.28-1.35) after the
     port changed, while still producing confident-looking numbers. Re-validate on
     the build of the day, or do not use it. It is currently INVALID.
  3. VERIFICATION CAN BE CIRCULAR BY CONSTRUCTION. "g_suspScale matches the original
     to 0.02%" was our computed value against a recomputation of our own formula. If
     a quantity is not in the capture, it has not been measured.
  4. A CONSTANT'S DECIMAL GLOSS MAY BE WRONG WHERE ITS HEX IS RIGHT. Six were.
     Build from the bit pattern (`Cf(0x...)`), and verify against the PE, not against
     a project note.
  5. DO NOT PUT YOUR OWN INFERENCE INTO A SUBAGENT BRIEF AS FACT. This happened three
     times and produced three wrong answers that had to be unwound. Tag every claim
     MEASURED / DECODED / INFERRED, and phrase anything inferred as a hypothesis to
     test.
  6. CONTROL FOR REGIME BEFORE COMPARING. Compare at matched speed AND matched steer.
     An uncontrolled comparison inflated one gap from 3.3x to 4.5x, and a full-lock
     original capture was once mistaken for evidence that a field was not the world
     position.

RULES: this is a port, not a tuning exercise. No constant invented to make a number
look right, and per the S-DoD rule an env-gated alternative does not count as done.
Ghidra work goes to a claude3 child — tag evidence classes in the brief, and tell it to
write its deliverable file EARLY, because two children hit their account session limit
mid-task and one left nothing behind. Track PIDs you spawn and kill only those. Update
the a8_yawrate_speed_inverted ledger item via re/orchestrator/orch.ps1 set.

FINALLY: it is legitimate to conclude that this gap needs a mechanism nobody has
identified yet, and to stop. Four candidate causes are already eliminated. Do not close
it by inventing one.
