# PRE-REGISTRATION: `H4`, `MASHED_SLOT_PLAYER` decided with `MASHED_WIRE_B2`, in a leg that measures CAR 0

Date 2026-10-08. Written BEFORE any run. Build: fresh `mashedmod\build.bat` at 18:44 on HEAD
`85aa8793` (the 13:21 exe predated `979b017a`'s default-ON seed and was not trusted).

## 0. Why the decision is narrower than "decide them together"

Static facts, all spot-checked in source:

- **Car 0 is never a branch-2 subject.** `VehicleStep` runs `ControlStep` only when
  `(t != 0 && t != 1) || ai_target_enable()==1` (`AiStandalone.cpp:1879-1881`), with
  `aib_veh_type(0) == 0` (`TrackRenderer.cpp:99`) and `aib_ai_target_enable() == 0` (`:109`).
  Car 0 enters branch 2 only as a candidate for `last` (`AiLeaderTimer.cpp:205`).
- **`MASHED_SLOT_PLAYER`'s only non-diagnostic consumers are R2/R3** inside `FUN_004148b0`
  (`AiLeaderTimer.cpp:173`, `:205`). They are reached from the `MASHED_GF1` probe or from
  `MASHED_WIRE_B2` (`AiStandalone.cpp:930`). The other slot-state reader, `LeaderInRange`
  (`:717`), is gated on the synthesized `veh_type` and cannot change. Survey: worker,
  2026-10-08, citations checked.
- **`MASHED_WIRE_B2` is NOT a default-ON candidate in this leg, whatever car 0 shows.** It
  fires only with `MASHED_A364_RESET` + `MASHED_REFDIST` + `MASHED_RACEPCT_BRIDGE` (all
  default-OFF, none decided) and `MASHED_NO_ELIM`, a harness override that disables elimination
  and can never ship (`RESULT_NOELIM.md`). Branch 2's timing also still disagrees with the
  original, because of D-11073. **Registered: `WIRE_B2` stays OFF. That follows from its
  prerequisites, not from this measurement.**

So the live decision is **`MASHED_SLOT_PLAYER` alone**. The C/D arms below are descriptive: they
measure, for the first time, what branch 2 does to car 0.

## 1. Arms

Recipe = `run_h2.ps1` (`MASHED_TRACK_VIEW=Training`, `MASHED_CAR=1`, `MASHED_ROUND=1`,
`MASHED_ROUND_RULE=4`, `MASHED_DETERMINISTIC=1`, `MASHED_DET_FRAMES=14400`, `MASHED_MUTE=1`).
Every arm carries the same three instruments: `MASHED_PLAYERTRACE` (car 0),
`MASHED_U9186_GATES` (pure reader without `MASHED_GF1`, `TrackRenderer.cpp:4210-4270`;
`slot_state` per car incl. v0) and `MASHED_AI_STEPDUMP` (cars 1..3). `MASHED_GF1` is unset in
all arms (it mutates `TimerAt`/`RankAt`).

| arm | knobs on top of the recipe |
|---|---|
| A | none (the default) |
| A2 | none, a repeat of A: the determinism floor |
| B | `MASHED_SLOT_PLAYER` |
| C | `SLOT_PLAYER` + `A364_RESET` + `REFDIST` + `RACEPCT_BRIDGE` + `NO_ELIM` + `WIRE_B2` |
| D | C minus `SLOT_PLAYER` (branch 2 cannot pass `:206`) |

## 2. Gates

| id | registered threshold | can see / cannot see |
|---|---|---|
| H4-FLOOR | A vs A2: `player_trace.log` byte-identical AND stepdump identical under `det_prefix.py --common-cols` | if it fails, every comparison below is VOID |
| H4-ARM | `slot_state` for v0 == 1 on every gates row in B and C, == 0 in A and D; `p5f2770` nonzero in all arms (seed default-ON) | that the knob wrote the cell. Cannot see a write that a later writer undoes between rows |
| H4-C0-INERT | B vs A: `player_trace.log` byte-identical, AND stepdump identical on common columns except `ss_*` | car 0's position, dt, record floats and race state per sim step; cars 1..3 full stepdump |
| H4-FIRE | C: branch 2 fires on >= 1 row (`g_b2Ret`/`g_b2Los` mirrors in the gates dump, or `g_wireFire`). D: 0 firings | that C exercises the thing SLOT_PLAYER enables |
| H4-C0-B2 | C vs D, DESCRIPTIVE: first `player_trace.log` line that differs; its sim step relative to C's first branch-2 firing; field-level magnitude of the car-0 divergence at +60 / +600 steps | branch 2's car-0 effect. Second-order only (contacts, rule engine), by §0 |

**Decision rule (registered):** `MASHED_SLOT_PLAYER` goes default-ON (opt-out
`MASHED_NO_SLOT_PLAYER`, `MASHED_SLOT_PLAYER` kept as a no-op) **iff** H4-FLOOR, H4-ARM and
H4-C0-INERT all PASS, **and** a post-flip run shows the opt-out reproduces A byte-identically
(`player_trace` + stepdump). Rationale if taken: the stored values reproduce the original's
measured `{v0:1, v1-3:2}` (`RESULT_E470.md`, 581/672), and it is then shown inert on all four
cars. Same class as H2's seed.

If H4-C0-INERT fails, it stays OFF and the failing field is named. H4-C0-B2 decides nothing.
