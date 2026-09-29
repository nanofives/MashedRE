# Next session kickoff

Updated 2026-09-29 at the close of the **player-regression (U-9141 / U-9145)** session.
Branch `race/first-frame-parity`, HEAD is that session's tracker commit. Nothing is pushed.
Superseded kickoff: the 2026-09-28 one (U-9140 / U-9142 are both closed below).

**THE HEADLINE: there is NO player-car physics regression since D2 closed.** The
`-16% / -14% / -64%` the previous kickoff item 3 described was the controlled arm's own
asymmetry, and the D2 reference turns out to be a **one-car race**. On the reference's own
scenario HEAD reproduces `56ad3806` to `-3.1% / -0.1% / +3.3%`. Read
`re/analysis/PLAYER_REGRESSION_2026-09-29.md` and ROADMAP §D2's **second** amendment; do not
re-derive either. **U-9141 and U-9145 are RESOLVED.**

**Three user decisions are in force from 2026-09-29 and are already actioned** — do not re-ask them:
1. **U-9142: KEEP the spawn settle, default-ON**, `MASHED_NO_SPAWN_SETTLE=1` stays as the A/B revert.
   **AI criterion (b) is re-baselined with the settle ON** (ROADMAP §D3), and the (b) bands are NOT moved.
2. **U-9141: the D2 gate recipe has a CONTROLLED arm.** Corrected 2026-09-29b: the arm to use
   is **`MASHED_MEASURE_SOLO=1`** (no opponents — the reference's own scenario), not
   `MASHED_MEASURE_NOOPP=1` (opponents parked, a scenario neither side ran). `--max-lines 1080`
   stays. ROADMAP §D2 carries both amendments; the second supersedes the first's conclusions.
3. **D3 CLOSES BY PORTING BEHAVIOUR MODES 3 AND 7** (`FUN_00414c30` + the world-object query
   `FUN_00484c70`). **D3-R1 is no longer a carried residue** — AI (b) now fails on all three
   cars at correct speed, so there is one open criterion on three cars, not a car-1 residue.

## Where D3 stands

**D3 is NOT closed, and there is now exactly ONE gate failure left: AI criterion (b).**
Full record: `re/analysis/D3_DRIVE_FORCE_2026-09-29.md` and, for the session before it,
`D3_DRIVE_2026-09-28.md`. ROADMAP §D3 "D3 closure state 2026-09-29" is the authoritative
summary; do not re-derive either.

| third | state |
|---|---|
| Powerups (c) | **MET** 2026-09-28d. Sweep re-run 2026-09-29: 11 of 11 decision CLEAN, contact CLEAN on 10 of 11, `g3` DIVERGES unchanged (the known 2-query-of-546 R_FLAME residue). |
| Modes (a)-(d) | MET. Rule 3 oracle GREEN 2026-09-29, and that run *did* produce 2 segment-ends, so the rule-3 tail arm is covered. |
| AI (a), (c), (d) | MET. |
| AI (e) | **MET 2026-09-29** — all six gated values inside 0.05% of the reference against a ±2% band. U-9140 resolved: the cause was A6a's unported START BOOST block. |
| AI (b) | **NOT MET on all three cars, and now measured under matched speed for the first time.** This is the only thing between here and D3 CLOSED. |

### What is CLOSED and must not be re-opened or re-measured

- **U-9140 RESOLVED.** The 8-9x drive-force gap was A6a's `+0xbf8` start-boost block, never
  ported. Ported verbatim (`Integrate2.cpp`, cites `0x00467d3a..0x00467e44`), 5e6/wheel or
  8e6/wheel for the two least-progressed cars, measured on three separate original captures.
- **The `[A8-B14CADENCE]` question is SETTLED and the answer is "they do not differ".** The
  port's render-tick `+0xb14` equals its consumption-time value on 898/899 frames, and the
  original's snapshot is provably a single-pass value (A4 zeroes at entry and calls A6a once;
  plus `linTerm × captured +0xb1c` reproduces the original's own per-frame Δspeed on 9
  consecutive frames). Do **not** re-run a cadence probe.
- **U-9142 ANSWERED by measurement: KEEP the spawn settle.** `MASHED_NO_SPAWN_SETTLE=1` fails
  (e) on all three cars by -2.1% to -6.9%. It is no longer a user decision.
- The force→velocity conversion, the gear law, the gearbox constants, the wheel states and
  the drive-only accumulator law are all confirmed faithful. The launch is not a physics
  question any more.

## What is owed, in the order it should be taken

### 1. AI criterion (b) — the ONLY D3 blocker. Start here.

`re/tools/ai_ctrl_window.py --check <csv>` on a fresh standalone capture:

```
py -3.12 re/tools/sa_capture.py verify/<tag> 8,30,60 MASHED_MUTE=1     MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=left-bl     MASHED_TITLE="D3 AI (b) <what>" MASHED_AI_STEPDUMP=verify/<tag>.csv
py -3.12 re/tools/ai_ctrl_window.py --check verify/<tag>.csv
py -3.12 re/tools/ai_speed_env.py   --check verify/<tag>.csv   # (e) must stay PASS
```

Where it stands on the default build (2026-09-29, `verify/d3_force_20260929/sa_b2.csv`):

| car | failing bands |
|---|---|
| 1 | `c0_distinct` 7 (floor 13), `c1_distinct` 106 (ceil 70), `steer_distinct` 112 (ceil 96), `c1_median` 48.0 (band `[0,0]`), `abs_steer_median` 48.0 (ceil 23) |
| 2 | `c1_distinct` 93, `steer_distinct` 121, `c1_median` 42.0, `abs_steer_median` 58.0 |
| 3 | `c1_distinct` 98, `steer_distinct` 116, `c1_median` 46.5, `abs_steer_median` 46.5, `accel_distinct` 1 (floor 2), `brake_distinct` 1 (needs 2) |

**Read the `c1_median` / `abs_steer_median` rows first — they are the NEW information and the
biggest ones.** The band is `[0,0]` for `c1_median`, i.e. the original's AI issues its steer
on the `c0` byte with `c1` at zero for at least half the window, and the port issues 42-48 on
`c1`. That is a sign/channel asymmetry, not a magnitude tuning problem: `c0` and `c1` are the
mutually exclusive steer pair (`+steer -> input[0]`, `-steer -> input[1]`,
`VehiclePhysicsRun.cpp` WS-A8-STEER block), so the port is steering one way far more than the
original. `c0_distinct` 7 on car 1 has been the standing symptom since 2026-09-26 and is
diagnosed in `D3_AI_RESIDUE_2026-09-27.md` (the `DAT_0089a368` spline-bank roll → curvature →
the `curv>20` multiplier at `0x0041665c`). Check whether the same chain explains the
`c1`-side pile-up before opening a new hypothesis.

Two things that must NOT be used as an excuse:

- These numbers are **worse** than the 2026-09-28 ones, and that is not a regression from the
  boost port: `MASHED_NO_START_BOOST=1` reproduces the 2026-09-28 (b) table exactly. What
  changed is that the cars now go the right speed, so the steer bands are for the first time
  scored on a car whose lookahead/curvature inputs are in the right regime. The older (b)
  numbers were not a measurement of the AI law.
- `accel_distinct` / `brake_distinct` = 1 is the D3-R1 story (unported behaviour modes 3 and 7,
  `FUN_00414c30` / `FUN_00484c70`), i.e. the port genuinely never lifts or brakes. It is a
  real band failure and it is *not* fixable inside the steer chain.

### 2. U-D3-BOOST-ARM — find the writer that arms the boost

The A6a boost FORCE law is transcribed and RVA-cited. The **arming** is a measured seed:
`+0xbf8 = 1`, `+0xbf4 = 1300`, once per AI slot, at its first real post-settle step
(`VehiclePhysicsRun.cpp`, the START BOOST ARM block). `MASHED_NO_START_BOOST=1` reverts.

`py -3.12 re/tools/findoffset.py --writes 0xbf8 0xbf4 0xbf0` puts **every** `.text` access to
those fields inside `FUN_00467650`, so the real arming store uses a base the displacement
sweep cannot see. Next commands, in order:

```
# 1. Frida WRITE watchpoint on &rec[car]+0xbf8, armed during the COUNTDOWN. A6a's two
#    +0xbf8 stores are both inside `bf8 == 1` / `== 2` arms, so while bf8 == 0 nothing in
#    A6a writes it and the first fault IS the arming instruction. Report the faulting EIP,
#    then decomp its containing function.
# 2. If the watchpoint API is unavailable: a Ghidra script walking stores whose base is
#    DAT_008815a0 + k and whose displacement is 0xbf8 - k.
```

Two things the writer would settle: whether 1300 is a constant or a value the countdown
computes from the throttle timing (`+0xbf4` rises +150/frame net through the last five
countdown frames and stops at the green), and therefore whether a human player who does not
jump the lights is boosted — the port currently does not boost slot 0, because no original
capture shows an armed player. Also open, and cheap once the writer is known:
`[UNCERTAIN] U-D3-BOOST-ORDER`, the per-car race-progress float at `0x008a96e8 + car*0x30c`
has no writer in the standalone (`FUN_00408a70` unported), so the 8e6 pair is pinned to the
grid order `{2,3}` instead of tracking race order. No effect on (e).

### 3. U-9141 / U-9145 — **RESOLVED 2026-09-29b.** Everything below this line is HISTORY.

Do **not** run the bisect the old item 3 asks for; it was run, and its premise was wrong.
Authoritative record: `re/analysis/PLAYER_REGRESSION_2026-09-29.md` (plan pre-registered at
`235e964a`, verdict at `2348614e`) and ROADMAP §D2's second amendment. In one paragraph:

- The arm was **asymmetric** — the knob was applied at HEAD and not at `56ad3806`. Applied at
  both ends, `647a5e24` (the *same commit*, both ways) moves `1931.36 → 692.07`, so the
  `-64%` is 0% a commit.
- The D2 reference is a **SOLO race**: `orig_steerR.msd.provenance.json` carries no `--cars`
  and `scenario_launch.py:1739` defaults it to 1; re-run live, `cars=1`, `SegmentCheck`
  1448/1448 with **segment-end 0** and `m1Max = -1`.
- With **`MASHED_MEASURE_SOLO=1`** at both ends: original `0.1913 / 0.2498 / 1940.59`,
  `56ad3806` `0.1374 / 0.2181 / 1760.49` (3/3), HEAD `0.1332 / 0.2179 / 1818.47` (4/5) —
  **HEAD vs `56ad3806` = `-3.1% / -0.1% / +3.3%`. No regression.**
- U-9145's coupling was two channels, **neither a shared physics global**: (A) the harness's
  steer-hold onset was on the real clock while the sim runs on a real-time accumulator —
  **fixed**, it now counts sim steps; (B) the opponents get the PLAYER eliminated at
  `race_time_` 2.1-2.6 s, which freezes `race_[0].gate` into the player's own off-mesh
  re-aim. `g_torqueRingPhase`, `g_suspScratch` and the pickup field are all REFUTED.

**What is newly owed, and where it sits in the order.** Item 1 (AI (b)) is still first, and
the closure path is now decision 3 above — port behaviour modes 3 and 7. Then, in this order:

- **U-9147** — on the matched (solo) arm the port is **~28% short on `slip 1500-2000` at BOTH
  commits** (0.1374 / 0.1332 against 0.1913). Not a regression; a D2-era gap the recipe's
  scenario mismatch concealed. Next command: `MASHED_MEASURE_SOLO=1` + `MASHED_COUPLING_DIAG=1`
  and a per-frame per-wheel lateral-force diff against `orig_steerR.msd`, the way
  `A8_velocity_vector_motion_20260825.md` follow-up 27 does. Re-baselining the ROADMAP §D2 row
  on this arm is a **user decision** — do not do it unasked.
- **U-9146** — does the ORIGINAL also eliminate a stationary player at ~2.5 s with three
  opponents? The reference is solo, so it cannot say. Next command:
  `py -3.12 re/frida/scenario_launch.py --oracle --rule 0 --cars 4 --poke-ctrl-slots --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38`,
  then read `segment-end` / `deadMax` from `log/rules_oracle_rule0.json`. Regardless of the
  answer, `race_[0].alive → race_[0].gate → the off-mesh re-aim` (`TrackRenderer.cpp:2808`)
  has no original counterpart.
- **U-9148** — one HEAD solo run in five lands in a second attractor, so a real-time-keyed
  input survives the sim-clock fix on the HEAD side. Next command: two
  `MASHED_PLAYERTRACE=1 MASHED_MEASURE_SOLO=1` runs until both attractors are sampled, then
  diff the two `player_trace.log` files for the first differing FIELD.

---

**HISTORY (2026-09-29, first pass). Superseded — kept for the audit trail.**

### 3-old. U-9141 — the controlled arm EXISTS and it found something. Redo the bisect on it.

**Read `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` §3 before touching this.** The arm and
its pass rule were pre-registered at `1d0ca916` before any run; the results are at `4938adba`.
Do not re-derive either, and do not move the bounds.

**The arm works.** `MASHED_MEASURE_NOOPP=1` (opponents not updated) plus `--max-lines 1080`
gives a run-to-run spread on `slip 1500-2000` of **exactly 0** on 3/3 runs at both HEAD and
`56ad3806`, where the uncontrolled arm still spreads 0.0088. The knob is proved inert when
unset (a default run on the build containing it reproduces the pre-knob build to every
printed digit). So the instrument question is settled.

**What it found, and it inverts the 2026-09-28 conclusion.** With the opponents absent at
BOTH ends, HEAD is `-16.0% / -14.0% / -64.2%` off `56ad3806`
(slip 1500-2000 0.1609 vs 0.1916, slip 2000-2600 0.2296 vs 0.2669, driving-median 691.0 vs
1932.1). The 2026-09-28 bisect concluded *"no commit in `56ad3806..HEAD` edits the player's
solver, therefore the drift is the instrument"* — **on a controlled instrument that does not
hold.** There is a real player-side difference, and the uncontrolled recipe could not have
seen it: its 0.128..0.177 spread brackets both 0.1609 and 0.1916.

`56ad3806`'s controlled arm reproduces the ROADMAP §D2 row on 2 of 3 gated statistics plus
`av.y` to four decimals (slip 0.00%, slip +0.04%, av.y exact) and misses the driving-median
by +2.39% against a ±2% bound. **D2 is not reopened** — that overshoot is already in the
record at §3.1 of the 2026-09-28 note, so the row's `1887` is itself ~2% low at its own
commit. Re-baselining that figure is a **user decision**; do not do it unasked.

**What is owed, in order.**

1. **Resolve U-9145 first — it may be the whole of U-9141.** The opponents move the PLAYER's
   driving-median 691 → 2538 (3.7x) on the same build and recipe, and `VehicleCarCarContact`
   (`0x00469df0`) has **zero callers** in the port, so no car-car path exists to do it with.
   The coupling is shared mutable state. Per-global A/B on the controlled arm, one temporary
   env-gated diag at a time, removed afterwards; candidates and citations in §3.5 of the note
   (`g_torqueRingPhase` `DAT_007f101c` and A4's steer ring `+0x1ac`/`+0x270` at `0x00470670`
   is the first one to try). **One run per configuration decides**, at a spread of 0.
2. **Then re-bisect `56ad3806..HEAD` on the controlled arm**, one run per commit, over the 17
   `mashedmod/`-touching commits of `D3_DRIVE_2026-09-28.md` §1.2:
   ```
   py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
       MASHED_MEASURE_NOOPP=1 MASHED_TITLE="U-9141 controlled bisect <sha>"
   py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
       --port verify/<tag>/motion_diag.log --max-lines 1080
   ```
   Classification fixed in §3.5: `slip 1500-2000` `>= 0.185` GOOD, `<= 0.170` BAD, between =
   INDETERMINATE and gets a second run. Commits before `09a73dc6` have no opponent loop, so
   the knob is a no-op there.

**Two premises to carry, both already paid for.** The §1.3 one-run-decides rule was refuted on
the UNCONTROLLED arm (0.1840 and 0.1283 from one build) and is sound on this one — do not
re-litigate it in either direction without citing which arm you mean. And one asymmetry
remains in the arm: at `56ad3806` the opponents are moved by the pre-D3 kinematic Option B
model, whereas the knob leaves them PARKED; if U-9145 is real, the HEAD end should use the
Option B treatment instead (§3.4's `NOAIPHYS`, measured 0.1736 / 0.1561 / 0.1774).

## Added 2026-09-29b (the player-regression session)

- **`MASHED_MEASURE_SOLO=1`** (`D3d9Render/TrackRenderer.cpp`, **both** spawn sites — the
  car-load spawn AND `StartRound`'s `ai_cars_.assign`) — MEASUREMENT HARNESS ONLY, default-OFF:
  spawns **no** opponents, so `ai_cars_` stays empty and `UpdateRace`, the `RaceCamera`
  framing, `ParticipantCount()` and the rule engine all see a one-car race. **This is the arm
  the D2 gate should use**, because the original-side reference was captured that way. Verify
  it took: `mashed_re.log` must log `MATCH-SEED … participants=1`. Gating only one of the two
  sites leaves it silently inert — that mistake cost a whole bisect's worth of mislabelled
  runs this session.
- **The steer-hold onset is counted in SIM STEPS, not real seconds** (`exe_main.cpp`,
  `steerHoldApply()`). It used to be decided once per RENDER frame from a real clock while the
  car sim runs on a real-time fixed-timestep accumulator, so the sim step at which the held
  lock began tracked CPU load. Same 4 s threshold. This is what made the default `a8` arm
  deterministic — four consecutive commits now reduce bit-equal.
- **`MASHED_PLAYERTRACE=1`** (`D3d9Render/TrackRenderer.cpp`) — default-OFF per-sim-step
  `%.17g` dump to `./player_trace.log`: world position, `in.dt`,
  `race_[0].gate/laps/progress/alive`, and record floats `+0xb14` / `+0xb1c` / `+0x9e4`, one
  line before and one after `UpdateRace`. **Diff two of these and read the first differing
  FIELD** — that single recipe found both U-9145 channels and refuted four named suspects. It
  is far cheaper than a per-global A/B and it cannot be fooled by a knob that is inert.
- **A `git checkout HEAD -- mashedmod/` restore inside a bisect script will silently delete
  your uncommitted edits.** It ate two of them this session. Commit before probing.
- `a8_run_port.py` at 50 s discarded two boots that stalled in the frontend (`NAV_DEMO
  phase=0 00_challengeselect` in `mashed_re.log`); at **90 s** the exe exits on its own with a
  complete race. Use 90 and re-run once on an empty log.

## Added 2026-09-29 (U-D3-DRIVE-FORCE + the D2 controlled arm)

- `MASHED_MEASURE_NOOPP=1` (`D3d9Render/TrackRenderer.cpp`) — MEASUREMENT HARNESS ONLY, on the
  `MASHED_STEER_HOLD` precedent: sets the per-opponent update loop's bound to 0 and does nothing else.
  Default-OFF and proved inert when unset. **Only legitimate on the D2 controlled arm, where BOTH ends
  of the comparison run it.** Do not use it to make any other number look better.
- `--max-lines N` on `a8_slip_axis.py` and `a8_momentum.py` — truncates the PORT side to the first N
  logged frames, before the regime filter and before the spike median. The ORIGINAL side is
  deliberately never truncated. N = 1080 is the D2 controlled arm's fixed 18.0 s window.
- Frame count == simulated time in the standalone: the chain dt is pinned at `frameMs = 50`, measured as
  a single distinct `linTerm=1.66667e-05` over all 3596 samples of
  `verify/d3_force_20260929/cad1/friction_diag.log`. Frame COUNT varies with machine load (25 vs 30 fps
  gave 1268 vs 1497 frames in the same 50 s wall clock), which is why pinning it is the right control.

- `MASHED_NO_START_BOOST=1` — revert arm for the A6a start boost. Reproduces the 2026-09-28
  criterion (e) and (b) numbers exactly, which is what makes any before/after here legitimate.
- `MASHED_GAMEMODE_STUB=0` — revert arm for `Fi_GameMode()` 6 → 0. Its only live consequence
  is A6a's `+0xbf4` timer site; audited call-site by call-site in `ForceIntegratorStubs.cpp`.
- `Fi_UpdateBoostOrder()` (`ForceIntegratorStubs.cpp`) — ported `FUN_00470c70`
  `0x00470e2e..0x00470f0a`: seeds `DAT_0088e660..66c` with 0,1,2,3 and sorts descending by
  the per-car progress float. Pinned to `{2,3}` in the standalone because that float has no
  writer (see U-D3-BOOST-ORDER above).
- `VehicleControlIntegrate` gained a `car` argument, so A6a's `param_1` is the real car index
  instead of a hardcoded 0. That resolves the `[UNCERTAIN]` that was on `VehicleControl.cpp:187`.
- `re/tools/findoffset.py` is the right tool for "who writes struct field +0xNN" and it was
  what proved the boost arming writer is NOT reachable by a displacement sweep. Read its two
  CAVEATS before citing a hit.
- **`MASHED_AI_STEPDUMP` needs `MASHED_TRACK_VIEW=Training`.** Without it the standalone sits
  in the frontend, never calls `AiStepDump()`, and you get three screenshots and no CSV with
  no error. Cost this session one capture. The full recipe is in
  `verify/d3_force_20260929/PROVENANCE.txt`.
- `a8_run_port.py` moves `motion_diag.log` but **not** `friction_diag.log`. If you run with
  `MASHED_COUPLING_DIAG=1`, delete `friction_diag.log` first and move it yourself afterwards.

## Tools added in the 2026-09-28 session

- `re/tools/ai_speed_env.py` — the criterion (e) scorer. Holds the band as `REFERENCE` /
  `BAND_PCT` / `GATED`. `--check <csv>`, `--envelope`, `--json`.
- `re/tools/statediff/msd_fields.py` — print arbitrary vehicle-record fields per frame out
  of an MSD1 capture. `<msd> 0x490:i 0x494:i 0x498:f ... [--every N] [--first N]
  [--distinct]`. This is what read the original's gearbox and suspension state.
- `MASHED_MOTION_DIAG` now also prints `b0c` / `gb498` / `gb49c`.
- `MASHED_MOTION_DIAG_AI=1` logs the gearbox and launch fields for the **opponent** slots to
  `motion_diag_ai.log` — the widening `D3_SPEED_GAP` §6.3 asked for. Separate file on
  purpose: the `a8` reducers key on `reseed=` … `wax=[…]` and assume slot 0.
- **`MASHED_TITLE` (from `a9da810a`, another session) — use it on every run.** The standalone
  window title is now `Mashed RE | <label> | <state>`, where the label is `MASHED_TITLE` if
  set and otherwise the run's `MASHED_*` env vars. Both harnesses forward bare `KEY=VAL`
  arguments into the child env, so it needs no code change:
  ```
  py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
      MASHED_D3_NOOPP=1 MASHED_TITLE="U-9141 bisect <sha>"
  py -3.12 re/tools/sa_capture.py verify/<tag> 8,65 MASHED_MUTE=1 ... \
      MASHED_TITLE="U-9140 cadence check"
  ```
  This session ran without it and had several near-identical windows open at once while
  bisecting; label them.

## Standing gotchas this session paid for

- The criterion (e) window and the criterion (b) window are the same span
  (`ai_ctrl_window.py`, 220 calls from the first `c4 != 0`), so the two cannot disagree
  about which calls are the race. Keep it that way.
- `flag_a368` (`DAT_0089a368`) is **not binary** on the speed-gap recipe — it takes 0, 1 and
  2. The reference regime is 0 on every window call. A capture with `regime0=0` on a car is
  re-taken, not scored. Two of four originals taken this session landed off-regime.
- The original is **deterministic** on this recipe: 10 regime-0 captures agree to the printed
  0.1 on `launch`, `ft_median_m0` and `ft_median`; only `start_frame` varies (802..891).
  A zero-width envelope is why criterion (e)'s band had to be inherited (2%) rather than
  measured, and that is stated in the note rather than hidden.
- The standalone is deterministic too on the AI capture: `sa_ctl` reproduced the 2026-09-27
  `sa_d1` exactly, which is what makes a one-capture before/after legitimate here.
- Four of this session's `a8` runs produced 38-, 79-, 302- and 361-line logs, i.e. races
  that ended in under a second. Those are truncated boots, not samples; the shortest complete
  race observed is 1023 lines. Discard below ~900 and say so.
