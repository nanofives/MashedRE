# Race-position reconstruction — scope + pre-registration skeleton

**WRITTEN 2026-10-06. Scoping only — no code, no build, no C-level, no game run.** This is the
plan for the effort the U-9186 chain keeps receding into: reconstructing the standalone's
per-car race-position state so the AI targeting gates (and other race systems) read live
values instead of blank-mapped zeros. It is split into three **legs** of very different cost,
each with its own inert-first checkpoint, so the effort can stop at any leg boundary with a
real result.

Evidence base: `verify/d3_overspeed_20261006/RESULT_U9186_*.md`,
`re/analysis/D3_AI_RESIDUE_2026-09-27.md` §6, `re/analysis/D3_AI_TICK_WIRING_2026-09-14.md`,
`re/analysis/lap_progress_subsystem_2026-06-16.md`, and the 2026-10-06 worker survey.

## 0. The three target states and what reads them (payoff)

All three are original globals in the VirtualAlloc-blank `0x00500000..0x009fffff` pad, so they
read **0** today unless the port writes them.

| state | addr | original writer | readers (payoff beyond U-9186) |
|---|---|---|---|
| **(c) slot-state table** | `0x005f2728` (ptr at `0x005f2770`), per-car `+0x34+v*4` | `FUN_0040e480` from the race allocator / AI tick | `FUN_0040e470` (CarSlotStateGet) → `LeaderInRange` `FUN_00415190`; the participant-count loop `0x0040ff40` |
| **(a) per-car progress** | `0x008a96e8` path_prog / `0x008a96ec` race_pct, `0x30c` stride from `0x008a9620` | `FUN_00408610` (spline-projection tracker) | AI ordering `FUN_00408ad0`/`FUN_00408a50`; points/elimination `FUN_0040eee0` (tiebreak + winner); `FUN_00442a60` reference-car ordering |
| **(b) reference distance** | `0x008989b0[v]` | `FUN_00442a60` (needs `FUN_0040e180` + progress) | AI fire gates `FUN_004148b0`/`00414c30`/`00415020`/`00415200`/`00415220`; `LeaderInRange` |

**Payoff is larger than the over-speed.** (b) live unblocks the whole AI **powerup fire-gate
chain** (MORTAR / DRUM / P_MINE / R_FLAME / SHOTGUN cannot pass their fire gates while
`0x008989b0` is 0 — `D3_AI_RESIDUE_2026-09-27.md:224`) **and** the mode-3/mode-7 producer
`FUN_00414c30`, i.e. the two U-9186 branches. (a) live feeds points/elimination tiebreak. So
this is a race-AI-wide enabler, not a single-defect fix.

## 1. The decisive fidelity fact — this is a BRIDGE, not a C4 port

The port already computes per-car race position in `race_[]`
(`Race/RaceSceneState.h:231-242`): `progress = laps*gateCount + nextGate + frac` where `frac`
is an 8-unit proximity blend to the next gate center (`TrackRenderer.cpp:4860`). The
**original's** `0x008a96ec` is a **spline-projection `race_pct`** written by `FUN_00408610`
(`lap_progress_subsystem_2026-06-16.md:52-56`). **Different substrates**: a gate ordinal vs a
spline percentage. So feeding `race_[].progress` into the original's gate addresses is
**faithful-by-algorithm, not bit-identical** — no C4 follows for the bridged state, and the AI
gates (tuned to the original's `race_pct` scale) may need their thresholds re-derived or the
port metric scale-mapped. **This is the central risk and a USER decision:** is a bridged,
non-bit-identical race-position substrate acceptable in the default build? It is the same
class of decision as `CarDropNonRenderAtomics` ("a measured data-driven equivalent … no
C-level promotion follows"). Recommended answer: yes, gated behind a default-ON knob with the
car<->car discipline (reverts cleanly, no gated-(e) regression) — but it is the user's call and
must be recorded before any leg lands.

## 2. LEG C — the slot-state table (CHEAP; do first)

**Change:** seed `*(u32*)0x005f2770 = 0x005f2728` once at boot (both addresses are in the
VirtualAlloc-blank pad, so the table memory already exists, zeroed). The port's existing
`CarSlotStateSet` (`AiStandalone.cpp:1404`) then stops early-returning, and its already-coded
pokes (`Ai_Standalone_Tick`, `AiStandalone.cpp:1699-1701`, `CarSlotStateSet(v,2)` for alive AI
cars) land in a real 4-DWORD table that `FUN_0040e470` reads. Default-ON with a revert knob.

**Inert-first checkpoint (gate before any consumer is trusted):** a default-OFF probe logs
`FUN_0040e470(v)` for v=0..3 across a race; CONFIRM it reads `{..,2,2,2}` for alive AI cars
after the seed, and `0` (not a crash) without it. If the seed does not make the table readable,
stop — the table hypothesis is wrong.

**Risk:** low. The memory exists; the only change is a self-pointer and (if not already)
flipping the tick gate. Does NOT by itself feed the over-speed branches (those need (b)), but
it re-enables `LeaderInRange`'s last-active-vehicle scan and the participant-count loop. Ship
only if (e) unchanged and (b) not worse, car<->car-style determinism.

## 3. LEG A — per-car progress bridge (MEDIUM)

**Change:** each frame, mirror the port's `race_[v].progress` (and a path_prog analogue) into
`0x008a96ec` / `0x008a96e8 + v*0x30c`, with an explicit scale map from the gate-ordinal metric
to the original's `race_pct` range. Default-ON with a revert knob.

**The scale map is the whole problem and must be pre-registered before the bridge is written.**
Required first: measure the original's `race_pct` range/semantics on a committed capture (does
it run 0..100 per lap? monotonic cumulative?) and the port's `progress` range on the same
scenario, and define the map (and whether the AI gate constants that consume it need
re-deriving). If the two cannot be put on a common scale without changing AI gate thresholds,
that is a finding, not a step to force.

**Inert-first checkpoint:** after the bridge, a default-OFF probe confirms `FUN_00408ad0(v)` /
`FUN_00408a50(v)` return non-zero, monotone-in-position values; and a cross-side check that the
bridged `race_pct` ordering of the 4 cars matches the original's ordering on a matched capture
(ordering, not bit-value — that is what the consumers actually use).

**Risk:** medium. Feeds points/elimination tiebreak (`FUN_0040eee0`) — so wiring it could move
finish-order/elimination behaviour; gate on the modes oracle and the power-up sweep in addition
to (e)/(b).

## 4. LEG B — reference distance (EXPENSIVE; build on A + C)

**Change:** a standalone-appropriate `FUN_00442a60` that writes `0x008989b0[c]` = planar
distance from a reference car to car c. The distance math uses live positions (`own_xz`), so it
is portable; the reference-car **selection** (`FUN_0040e180` max-separation pair + `FUN_00408ad0`
ordering) must be reconstructed from `race_[]` + Leg A's bridged progress rather than the
unloaded `0x005f2728` path, because `FUN_0040e180` as written reads that table and the original
notes call it "a subsystem the standalone does not have" (`D3_AI_RESIDUE_2026-09-27.md:223`).

**Inert-first checkpoint, and the one that matters for U-9186:** after Leg B, a default-OFF
gate-fire counter on the two U-9186 branches (`FUN_00414a70==2`, `FUN_004148b0 && FUN_00416060`)
with `ctrl` unchanged — CONFIRM each fires at a rate approaching the original's 36 / 64 before
any `ctrl` write is enabled. This is the gate the whole chain exists to reach; if the branches
still do not fire with (a)+(c)+(b) live, the over-speed command fix is refuted at a deeper level
and that is the result.

**Risk:** high. Only after this passes does porting the actual U-9186 branch behaviour (and the
powerup fire gates) become the final, separately-gated step.

## 5. The standing gates for every leg (car<->car discipline)

Each leg, before it ships default-ON:
- **inert-first liveness witness** (above) — prove the state goes live and a named consumer's
  reads change, with a control that can fail.
- **G-NOREG-E**: `launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0` 2550.6 / 2053.0 / 2278.2
  unchanged on 3/3 cars.
- **G-NOREG-B**: ≤ 13 failing bands (currently 5 with car<->car ON) — no regression.
- **G-BANDS-UNEDITED**, **G-KNOBOFF** (the revert reproduces the pre-leg digits),
  **G-DET** (3 ON runs identical).
- Guards: power-up sweep 11/11 decision CLEAN, modes oracle rule-3 GREEN — Legs A/B touch
  finish-order and fire gates, so these are not optional for them.

## 6. Recommendation and estimate

- **Leg C** is a few lines + a boot seed + a liveness probe: ~1 focused session, low risk, a
  real (if small) result (participant-count / LeaderInRange reads go live).
- **Leg A** is the pivot: it needs the scale map measured and pre-registered first; ~1–2
  sessions, medium risk (touches finish order). The fidelity USER decision (§1) must be made
  before it lands.
- **Leg B** is the largest and only worth starting after A+C land and the §1 decision is yes;
  its gate-fire checkpoint is the honest go/no-go for the entire over-speed fix.

**Do not start Leg B first.** The cheapest decisive experiment in the whole effort is Leg C's
liveness probe plus Leg A's scale measurement — together they tell you whether the bridge is
viable at all, for ~one session and no behaviour risk, before committing to the expensive Leg B.

## 7. Tracker placement

Filed as a DEFERRED row (race-position reconstruction), re-pickup condition: "when an AI-visible
race-position consumer (powerup fire gates, mode-3/7 producer, or the U-9186 over-speed
branches) is the chosen lane and the §1 fidelity decision has been made." It is explicitly
**not** a C-level gate — no function's status changes by scoping it.
