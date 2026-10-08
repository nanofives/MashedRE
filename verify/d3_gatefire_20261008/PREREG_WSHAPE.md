# PRE-REGISTRATION — `W-SHAPE`, port side

Date 2026-10-08. Written **before** any run of the new build. Closes the half of `W-SHAPE` that
`RESULT_WIRE2.md` §5 left open and `RESULT_FIRESHAPE.md` §4 closed on the original side only.

## 1. The question

On a port call where branch 2 fires, **what were `ctrl[4]` and `ctrl[5]` at ControlStep entry?**

`AiStandalone.cpp:891` claims, as a deliberate property of the port:

> `ctrl[4]` is NOT written. It keeps its entry value (VehicleStep zeroes it), which is why c4 logs
> as 0 -- the branch never touches it.

Every other column in the stepdump is read at **end of frame**, i.e. the OUTPUT pose. So a firing
row showing `c4=0` is today consistent with **both** "the branch left `ctrl[4]` alone and it was
already 0" and "something zeroed it". The claim is untested on the port.

On the original this is already answered: `c4_in = c5_in = 0` on all 64 firing calls
(`RESULT_FIRESHAPE.md` §4) — which also means the original capture **cannot** separate those two
readings either. The port capture can, because the port's `c5_in` is free to be non-zero.

## 2. The change

Two per-car `int` scratch globals (`g_c4In`, `g_c5In`, `AiStandalone.cpp`), written at the very
top of `ControlStep` before any `ctrl` store on any path, read by `AiStepDump` in the same tick —
identical plumbing and identical same-frame ordering to the existing `g_b2Ret` / `g_b2Los`
(`Ai_Standalone_Tick` at `TrackRenderer.cpp:3511` runs before `AiStepDump` at `:3512`).

Two columns **APPENDED** to the stepdump: `c4_in,c5_in`. Every existing column keeps its position.
`-1` = ControlStep did not run for this car before the dump.

Diagnostic only: reads `ctrl`, writes nothing back, and the globals have exactly one reader (the
env-gated stepdump).

## 3. Schema note — why `W-KNOBOFF`'s byte hash cannot be the control here

The stepdump schema is now **84 columns**; `86b7b2bb` is the SHA-256 of the **80-column**
knob-off capture. That generation break already happened once this session — `CW_port.step.csv`
is 82 columns and hashes `f1daa410` because `b2_ret`/`b2_los` were appended — so a byte hash
against `86b7b2bb` is **not available** and claiming it would be false.

The established substitute is `det_prefix.py --common-cols`, added for leg E2 for exactly this
case (its header: "what lets a new-build capture be checked against an older-schema one on the
PRE-EXISTING columns"). Registered as `WS-KNOBOFF` below.

## 4. Runs

Both `MASHED_DETERMINISTIC=1`, `MASHED_DET_FRAMES=14400`, `MASHED_TRACK_VIEW=Training`,
`MASHED_CAR=1`, `MASHED_MUTE=1`. Driver `run_wshape.ps1`, which spawns and stops only its own PIDs.

| run | knobs |
|---|---|
| `WS_ctl` | **none** (plus `MASHED_ROUND=1`, `MASHED_ROUND_RULE=4` — the `GF0step` recipe exactly) |
| `WS_fire` | `SLOTSTATE_SEED` + `SLOT_PLAYER` + `A364_RESET` + `REFDIST` + `RACEPCT_BRIDGE` + `NO_ELIM` + `WIRE_B2` |

`WS_fire` carries the **full prerequisite stack**. An under-specified arm already produced a false
"the wiring is inert" once this session; the arm is verified to have taken by `WS-ARMED` before any
verdict is read.

## 5. Gates — registered now, verdicts reported whatever they say

| gate | PASS condition |
|---|---|
| `WS-KNOBOFF` | `WS_ctl` is **identical to `GF0step.csv` over every shared key** under `det_prefix.py --common-cols`, and the only dropped columns are `b2_ret`, `b2_los`, `c4_in`, `c5_in`. Proves the entry-snapshot edit changed no computed value. |
| `WS-ARMED` | `WS_fire` contains **≥1 row with `b2_ret == 1`**. Without a firing there is nothing to score and every gate below is VOID, not PASS (memory `absent-log-proves-nothing-run-a-control`). |
| `WS-LIVE` | `c4_in`/`c5_in` are **not uniformly `-1`** and **not uniformly 0** across `WS_fire` as a whole. An instrument that only ever reads one value is indistinguishable from a broken one; this is the coverage check, scored over the whole capture, NOT over the firing calls. |
| `WS-C4` | On every firing call, `c4_in == c4`. This is the test of the `:891` claim: the branch does not write `ctrl[4]`, so entry and exit must agree. |
| `WS-C5OUT` | On every firing call, `c5 == 255`. Re-confirms the output pose on the new build (it held 18/18 on `CW_port`). |
| `WS-C5IN` | **Reported, not thresholded.** The distribution of `c5_in` on firing calls. If it is **always 0** the capture is as blind as the original's and `WS-C4` is weakened to the same degree — that outcome must be stated, not glossed. |

`WS-C5IN` is deliberately un-thresholded: whether the port ever fires on a call that entered with
the brake applied is not something I can predict, and inventing a threshold for it would be
decoration.

## 6. What this cannot establish

- **No C-level.** Entry-value columns are an instrument, not a `diff-original` Frida diff.
- It does not test branch 2's **timing** or **car distribution** — both are downstream of `D-11073`.
- `WS_fire` runs under `MASHED_NO_ELIM`, a measurement control that suppresses a real mechanic.
- Nothing here moves any knob's default. All stay OFF.
