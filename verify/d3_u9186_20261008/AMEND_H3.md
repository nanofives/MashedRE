# AMENDMENT to PREREG_H3.md — the stepdump arm, and why `H3-GATEFIRE` is not buildable

Date 2026-10-08. **UNRUN at write time.** Amends `PREREG_H3.md` §2. Nothing here promotes
anything, changes a default, or touches `original/` or the `.asi`.

## 1. Why this amendment exists

`RESULT_H3.md` §4 left three gates unrun and one gate unbuilt:

- `H3-KNOBOFF` / `H3-NOREG-E` / `H3-NOREG-B` — not run because the `MASHED_U9186_GATES` dump is a
  24-column gates schema and `ai_speed_env.py` / `ai_ctrl_window.py` take the 78-column
  `MASHED_AI_STEPDUMP` schema. This amendment adds the missing arm. **§2.**
- `H3-GATEFIRE` — registered as "the one that matters" and not built. It is **not buildable as
  registered**; the blocker is measured in **§3** and the gate is **withdrawn pending a decision**,
  not silently dropped.

## 2. The stepdump arm (`H3step`) — both paths named

Per the standing rule earned three times this session (`G-TOOK`, `H1-CONVERGE`, `H3-WROTE`), each
gate below names the path the change alters and the path the instrument exercises, and they are
required to be the same.

**The path H3 altered in the default build.** H3's commit `1560986e` touched exactly three
exe-linked things: `Race/SpectatorDistances.cpp` (new TU, +217, added at `exe_sources.rsp`),
`D3d9Render/TrackRenderer.cpp` (+42 — the gates-dumper columns plus the per-frame
`RaceComputeDistancesTick` call site), and the `.rsp` line itself. With every knob unset both new
bodies early-return on a `static` `getenv` miss, so the **claim under test is that those 42 lines
and one new TU change nothing on the default path**. This is *not* the near-tautology H1a's
`H1-KNOBOFF` was (`RESULT_H1.md`, `H1-CALLERS = 0`): here the added call site executes every frame
in the default build and only the early-return makes it inert.

**The path the instrument exercises.** `MASHED_AI_STEPDUMP` is emitted from
`TrackRenderer::AiStepDump()` (`TrackRenderer.cpp:4149`), called at `TrackRenderer.cpp:3463` — the
line immediately above the `U9186GateDump()` call H3 added, in the same per-frame AI bridge path.
The instrument therefore runs in the same function, on the same frame tick, as the edit. Same path.

**Arm.** One run of the current HEAD build with `MASHED_AI_STEPDUMP=H3step.csv` and
`MASHED_REFDIST`, `MASHED_U9186_GATES`, `MASHED_SLOTSTATE_SEED`, `MASHED_RACEPCT_BRIDGE` all unset,
under the E1 scenario and `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`, plus one repeat
(`H3step_r2`) for `H3-DET`.

| gate | instrument | threshold |
|---|---|---|
| `H3-KNOBOFF` (primary) | `det_prefix.py H1step.csv H3step.csv` | `R-PREFIX` = max frame + 1, i.e. **identical over the whole overlap**. H1step is the pre-H3 build under the same knobs and the **same stepdump schema**, so the two captures differ by exactly H3's default-path edit. This is the sharp control. |
| `H3-KNOBOFF-LONG` (secondary) | `det_prefix.py E2off_1.csv H3step.csv --common-cols` | same, on the longer baseline. `--common-cols` is expected to drop nothing; if it drops columns, report the list — a schema drift is itself a finding. |
| `H3-NOREG-E` | `ai_speed_env.py --check H3step.csv` | criterion (e): the committed six digits `launch 1426.4 / 2053.0 / 2055.2`, `ft_median_m0 2550.6 / 2053.0 / 2278.2` (`PREREG_CARCAR.md:72`) unchanged. |
| `H3-NOREG-B` | `ai_ctrl_window.py --check H3step.csv` | criterion (b): the same **5 bands** with the same numbers (`PREREG_CONSUMER.md:128`). |
| `H3-DET` (stepdump arm) | `det_prefix.py H3step.csv H3step_r2.csv` | identical. F2 proved the scenario bit-reproducible; a failure here invalidates the other three rather than indicting H3. |
| `G-BANDS-UNEDITED` | `git status --porcelain` on `ai_speed_env.py`, `ai_ctrl_window.py`, `det_prefix.py` | empty. Checked **before** the run; re-checked after. |

**Decision rules.** `H3-DET` fails → nothing else is read. `H3-KNOBOFF` fails → H3's default-path
edit is not inert and must be named row-by-row, not waived; `H3-INERT`'s ON-vs-OFF result does not
cover this, because both of its arms carry the gates probe. `H3-NOREG-*` fail → report the changed
digits/bands; do not re-cut a denominator.

**Registered weakness, stated up front.** All three knob-off gates are *negative* controls. Passing
them says the default build is unchanged; it says nothing about whether the H3 producer is correct,
which is `H3-WROTE`'s job and was already reported as a registered-threshold failure with the
corrected denominator alongside.

## 3. `H3-GATEFIRE` is WITHDRAWN — the blocker, measured

`PREREG_H3.md` §2 registered a default-OFF counter on `FUN_00414a70 == 2` and on
`FUN_004148b0 != 0 && FUN_00416060 != 0`, to be compared against the original's **36 / 64** calls
over the 220-call window (`verify/d3_elim_20261003/RESULT_STEP2.md:106`, `:108`, `:178`). The
registration assumed this was instrumentation. It is not: **none of the three predicates exists on
the standalone's code path**, and the two that have bodies cannot be linked into the exe as they
stand.

| predicate | status in the standalone exe | evidence |
|---|---|---|
| `FUN_00414a70` | **no body anywhere in the port.** Only an absolute-address call-through thunk, in an `.asi`-only TU | `hooks.csv:645` is `C2,mapped` with an empty impl column; the sole reference is `Ai/AiControlStep.cpp:91` `Pred14a70` → `reinterpret_cast<fn_pred_t>(0x00414a70)` |
| `FUN_004148b0` | body exists (`Ai/AiLeaderTimer.cpp:90`) but the TU is `.asi`-only and the body is **saturated with absolute-RVA reads into the original image** | `asi_sources.rsp:230`, absent from `exe_sources.rsp`; body reads `0x005f2dd8` (limit table), `0x005cd0a0/a4/a8`, `0x005cc35c`, `0x0089a4c4`/`0x0089a4c8` per-car timers, and calls `0x0040e470`, `0x00442cc0`, `0x0046d4a0`; carries a naked-asm prologue thunk and `RH_ScopedInstall(…, 0x004148b0)` at `:181` |
| `FUN_00416060` | **two** bodies (`Ai/AiTargeting.cpp:84` live, `Ai/AiLineOfSight.cpp:73` dead-on-install), both `.asi`-only, both reading `0x007f1a9c` / `0x007f9a9c` and calling `0x004c3bf0` | `asi_sources.rsp:224` / `:227`; the duplicate-RVA repoint is recorded in `hooks.csv:654` |

`ControlStep` does not call any of them: `AiStandalone.cpp:844` is a hardcoded `int mode = 0;` with
the targeting chain elided at source level, not a stub that returns 0.

Dropping the `.asi` TUs into `exe_sources.rsp` would not be a link change: the absolute addresses
above are blank-mapped standalone unless something loads them, so the predicates would read zeros
or fault, and `RH_ScopedInstall` would register against an image that is not there. A counter built
that way reports a number with no meaning — the `_abs`-column failure mode `RESULT_H1.md` recorded,
and the exact shape of "schedule-derived counts are not evidence".

### 3.1 Static substrate census — the two branches are NOT equally blocked

Each absolute address the two bodies depend on, checked for whether any **exe-linked** TU
references it. This is static evidence of reachability only; it is **not** a runtime measurement
and no fire-rate claim follows from it.

| address | what it is | exe-linked reference? |
|---|---|---|
| `0x007f1a9c` / `0x007f9a9c` | the `.AI` tile grid + sub-cells, `FUN_00416060`'s whole input | **YES, and it is loaded** — `TrackRenderer.cpp:359` calls `Ai::AiData_LoadInto(raw, len, (void*)0x007f1a9c)` |
| `0x00442cc0` | per-car progress | **YES** — `AiStandalone.cpp`, `TrackRenderer.cpp`, `Race/SpectatorDistances.cpp` |
| `0x0040e470` | car-active query | **YES** — `AiStandalone.cpp`, `TrackRenderer.cpp`, several Frontend TUs |
| `0x0089a4c4` | per-car rank counter (`+ v*0x74`) | **YES** — `AiStandalone.cpp` |
| `0x0089a4c8` | per-car catch-up timer (`+ v*0x74`) | **no** — `.asi` TUs only |
| `0x005f2dd8` | `FUN_004148b0`'s int limit table | **no** — `AiLeaderTimer.cpp` only. Same class as `0x005f2770`: a `.data` table the standalone never loads, so it must be extracted from `MASHED.exe.unpatched` and written in as cited literals, exactly as `H3-CONSTS` did for the four floats |

**So the LOS half of the 64-branch is already live.** More than live: `AiStandalone.cpp:282-288`
(exe-linked) already implements `FUN_00416060`'s exact tile test — `word [0x007f1a9c + cell*2]`,
`0 < s < 0x200`, `byte [0x007f9a9c + ((rx&7) + s*8)*8 + (rz&7)] in {0,3}` — as the Phase-8 wall-march
helper, with a standalone-only range guard documented at `:274`. It is not exposed as a callable
`LineOfSight(a, b)`, but the substrate and the predicate logic are both present in the exe.

**Revised cost, per branch:**

- **64-branch (`FUN_004148b0 != 0 && FUN_00416060 != 0`)** — reachable with bounded work: lift the
  existing tile test into a `LineOfSight(a,b)` shape, port `FUN_004148b0`'s body against standalone
  substrate, extract `0x005f2dd8` from the unpatched image, and decide `0x0089a4c8`. No new
  unknowns. The registered risk at `RESULT_STEP2.md:223` — that `LeaderTimer`'s rank/progress reads
  are `.bss` zeros standalone, making any wiring inert rather than correct — is **narrowed but not
  closed** by the table above: `0x0089a4c4`, `0x00442cc0` and `0x0040e470` have exe-side
  references, `0x0089a4c8` does not. Whether those references make the values *live during a race*
  is unmeasured. **[UNCERTAIN]**
- **36-branch (`FUN_00414a70 == 2`)** — a genuine port from zero: `FUN_00414a70` is `C2,mapped`
  with no body, and the plate names a callee `FUN_00414300` that is also unported.

**`H3-GATEFIRE` is therefore withdrawn from H3 and nothing is claimed for it.** It is a port leg,
not an instrumentation leg, and it needs its own pre-registration and a user decision on scope —
including whether to take the 64-branch alone first, which is the cheaper and better-understood
half, with a runtime substrate census as its first gate.

This withdrawal does **not** change any H3 verdict: `RESULT_H3.md` already stated that nothing is
claimed about whether U-9186's branches fire.

## 4. Non-goals, unchanged from `PREREG_H3.md` §5

Any default-ON change. Wiring the `FUN_00416250` branches to `ctrl`. Any C-level promotion. Any
edit to `original/` or the `.asi` code path. Shipping the `MASHED_RACEMETRIC_ARC` ON arm.
