# Next session kickoff

Written 2026-09-28 at the close of the U-D3-DRIVE / physics-drift session.
Branch `race/first-frame-parity`, HEAD `b5923d20` (+ the tracker commit that follows it).
Nothing is pushed.

## Where D3 stands

**D3 is NOT closed, and the reason is one measured gate failure plus two open user
decisions.** Full record: `re/analysis/D3_DRIVE_2026-09-28.md`. ROADMAP §D3 "D3 closure
state 2026-09-28e" is the authoritative summary; do not re-derive either.

| third | state |
|---|---|
| Powerups (c) | **MET** 2026-09-28d. Sweep re-run this session: 11 of 11 decision CLEAN, contact CLEAN on 10 of 11, `g3` DIVERGES unchanged (the known 2-query-of-546 R_FLAME residue). |
| Modes (a)-(d) | MET. Rule 3 oracle spot-checked GREEN this session. |
| AI (a), (c), (d) | MET. |
| AI (b) | **REGRESSED to FAIL on cars 2 and 3** by this session's spawn-settle fix — U-9142. |
| AI (e) | **FAILS on all three cars**, -46 to -90% — U-9140. |

## The three things owed, in the order they should be taken

### 1. U-9140 — the remaining criterion (e) blocker. START WITH THE CADENCE CHECK.

Do **not** start by chasing the 8-9x drive-accumulator gap. It may not be real. The
`[A8-B14CADENCE]` comment at `VehiclePhysicsRun.cpp:849-855` already warns that a
render-tick snapshot of `+0xb14` can be a residue after the whole substep loop, and A4
zeroes it at entry (`VehicleControl.cpp:102`). Settle that first:

```
py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 20 -MASHED_REAL_PHYSICS \
    MASHED_COUPLING_DIAG=1
```

and compare the `ctrl=` consumption-time `+0xb14` (`Integrate2.cpp:461`) against the same
run's render-tick value in `motion_diag.log`. If they differ, every figure derived from the
original's captured `+0xb14` — including §4.4's table — is invalid and the comparison has
to move to a Frida probe at the A6a drive block. Observe the CLAUDE.md hot-path rule
there: hook a callee entry, one function, short run, **not** an Interceptor trace.

What is already nailed down and must not be re-measured: after the spawn settle the gear,
the input bytes (`c4 = 255`), the four wheel contacts, the wheel states `2/2/1/1` and the
gearbox constants `+0x498` = 40000 / `+0x49c` = 4000 all MATCH on both sides. The launch is
still 13.3 / 13.6 / 14.4 / 15.2 per frame against the original's 180 / 190 / 188 / 198.

### 2. U-9142 — a user decision, not an investigation

The spawn settle (commit `83a7b6ea`) makes the port's rest state equal a state read out of
the original, and it costs AI criterion (b) on cars 2 and 3. It ships default-ON with
`MASHED_NO_SPAWN_SETTLE=1` as the A/B revert, both arms measured and deterministic. **Ask
before changing either the default or the (b) bands.** The two facts that bear on it:

- the `accel_distinct` / `brake_distinct` failures are the removal of an artefact — pre-fix
  the port's only non-255 accel call in the 220-call window was the one the spawn transient
  produced, so those bands were being satisfied by the defect;
- the `c1_distinct` / `steer_distinct` failures are 2-8 counts of margin, and `c1` is a
  function of the speed trace that U-9140 is 8-9x wrong about — so U-9140 may move it.

That ordering argument says **do U-9140 first and re-score (b) afterwards** rather than
adjudicate U-9142 cold.

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

## Tools added this session

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
