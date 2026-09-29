# Next session kickoff

Written 2026-09-29 at the close of the U-D3-DRIVE-FORCE session.
Branch `race/first-frame-parity`, HEAD is this session's tracker commit. Nothing is pushed.
Superseded kickoff: the 2026-09-28 one (U-9140 / U-9142 are both closed below).

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

### 3. U-9141 — the `a8` gate recipe needs a controlled arm

ROADMAP §D2's table cannot be reproduced at HEAD, and the bisect showed why: **no commit in
`56ad3806..HEAD` edits the player's solver.** The table moved because three RNG-driven
opponents now share the player's world (`09a73dc6`) and the race got longer (`4ff428ad`),
both of which are the port becoming more like the original. Today's build of `56ad3806`
still reproduces the D2 table 4 runs of 4, so this is not drift in the drive laws.

So do not try to "restore the D2 table" by changing physics. What is owed:

```
# 1. give the reducers a --max-lines arm so both ends reduce over the same frame count
#    (a8_slip_axis.py, a8_momentum.py); race length alone repopulates the speed bands
# 2. then, if still needed, bisect commits #5..#16 of D3_DRIVE section 1.2 using
#    MASHED_D3_NOOPP=1 -- the only deterministic instrument found (3/3 to four decimals)
```

**Note the refuted premise before planning any bisect:** one run per commit does NOT
decide. The same build at `09a73dc6` produced slip 0.1840 and 0.1283. Require three runs,
and treat a single run that reaches the 0.1283 attractor as BAD.

## Added 2026-09-29 (U-D3-DRIVE-FORCE)

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
