# PREREG: does x87 PC_24 move the parked D2 metrics? (2026-10-10, user-requested test)

Lead: `verify/d3_modes37_20261009/RESULT_P2.md` §1.1 — the original's race loop runs at x87 PC_24.
Knob `MASHED_X87_PC24` (default OFF, `exe_main.cpp` PumpOnce) forces PC_24 on the game thread every
frame. `MASHED_LOG_CW` logs the live precision bits. No other code change.

## Scope, stated before running
PC_24 only changes x87 arithmetic: the x87 TUs (`RwpSolverBroadphase3/Core14/Core15/Core17`,
`CameraEntries`, `PathNodes`) and naked-asm ports. The SSE2 physics TUs are NOT affected: their
`float` ops already round like PC_24, and their deliberate `double` intermediates stay 53-bit. A null
result here therefore does NOT clear the SSE `double`s; it only clears the x87 slice.

## Arms (D2 §16.7 arm, attempt-20 recipe, `a8_run_port.py`, 90 s; deterministic so 1 run each, a
second run of B to confirm)
- A: default + `MASHED_LOG_CW` (control; must reproduce attempt 20's metrics 0.1914 / 0.2521 / 1879.59)
- B: `MASHED_X87_PC24` + `MASHED_LOG_CW`

## Predictions
- P1: A's log shows PC_53 (`0x10000`) on the game thread (track-load P1 was 1 ULP off without PC_24).
- P2: B's log shows PC_24 (`0x20000`).
- P3: B moves driving-median by less than its 25.11 gap (expected: the x87 slice is the contact
  solver, which attempt 20 exonerated). No number is claimed for the size.
## Decision rule
- If B puts all three metrics inside the unchanged `d81a8df6` bounds: report a **D2 REOPEN CANDIDATE**
  to the user. No default flip in this session.
- Otherwise: report the deltas, and size the SSE-`double` follow-up (count of deliberate `double`
  intermediates in the physics TUs) as the remaining part of the lead.

## Addendum (registered before running, same day): arm C/D, the SSE-`double` slice
A and B were byte-identical over all 1607 common rows, so the x87 slice is inert on this arm. The
remaining part of the lead is the 25 SSE2 physics TUs with 208 `double` mentions. Temporary experiment
build only: those 25 TUs added to `x87_tus.txt`, so their `double` ops run on x87 and round at the
live precision. C = that build at PC_53 (default CW), D = that build + `MASHED_X87_PC24`. Decision rule
unchanged (all three metrics in bounds on D -> D2 REOPEN CANDIDATE, reported, no flip). `x87_tus.txt`
is restored afterwards and the default build re-verified on `ae67b13a / c8317ce2 / 31db9384`.
