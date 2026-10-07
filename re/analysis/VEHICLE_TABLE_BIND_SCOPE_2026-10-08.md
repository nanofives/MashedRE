# Vehicle-table binding — scope for U-9186's remaining blocker

**WRITTEN 2026-10-08. Scoping only — no code, no build, no C-level, no game run beyond the leg G1
measurement already committed.** Sibling of `RACE_POSITION_RECON_SCOPE_2026-10-06.md` and follows
its shape.

Evidence base:
- `verify/d3_u9186_20261008/RESULT_G1.md` (+ its 2026-10-08 CORRECTION) — the measurement this
  scope rests on, 53,992 rows per arm, three arms, deterministic captures.
- Headless decompilation of `FUN_00442a60`, `FUN_0040e180`, `FUN_0040e370`, `FUN_0046c7b0`,
  `FUN_0046cbb0`, `FUN_0046d4a0`, `FUN_004c3ac0` (slot `Mashed_pool0`, read-only).
- `decomp_pc.py --datarefs` on `0x008815a4` / `0x00881f90` / `0x00881f48` / `0x00881ec8`.
- A worker read-only survey of every port-side consumer, the supply side, and `hooks.csv`.

## 0. The target state and what reads it (payoff)

**This is ONE per-vehicle record, not four tables.** Base `0x008815a0 + v*0xd04`
(stride witness `imul eax,eax,0xd04` at `0x0046d78e`; base independently recorded as `base_va` in
every `.msd` capture provenance, `LaunchRevCharge.cpp:16-17`). The four "tables" are four fields:

| field | absolute | = record + | original accessor | readers |
|---|---|---|---|---|
| **(1)** alive | `0x008815a4 + v*0xd04` | `+0x004` | `FUN_0046c7b0` | `FUN_00442a60` GATE 2, `FUN_0040e180`, `CameraClusterHooks` |
| **(2)** state / spinout | `0x00881f90 + v*0xd04` | `+0x9f0` | `FUN_0046cbb0` (`+0x04` secondary) | `FUN_00442a60` GATE 3, `FUN_0040e180` |
| **(3)** matrix-set selector `t` | `0x00881f48 + v*0xd04` | `+0x9a8` | read inline by `FUN_0046d4a0` | `FUN_0046d4a0` |
| **(4)** body matrix block | `0x00881ec8 + v*0xd04` | `+0x928` | `FUN_0046d4a0` → `rec + t*0x40 + 0x30/0x34/0x38` | world x/y/z for every distance computation |
| (5) slot-state (**separate base**) | `*(0x005f2770)` → `0x005f2728`, `+0x34+v*4` | — | `FUN_0040e370` / `FUN_0040e470` | `FUN_00442a60` GATE 1, `FUN_0040e180`, `LeaderInRange` |

Field (4) with `t = 0` lands at `+0x958/+0x95c/+0x960` — the body-matrix translation row, which the
AI stepdump **already logs** as `rec_958` / `rec_960`, and which memory
`msd-world-position-is-the-0x928-matrix-row` independently names.

**Payoff is larger than the over-speed.** Fields (1)-(4) gate `FUN_00442a60` (the `0x008989b0`
producer), `FUN_0040e180` (most-separated pair), and the whole `CameraClusterHooks` spectator
chain. `0x008989b0` in turn feeds the AI fire gates `FUN_004148b0` / `FUN_00414c30` /
`FUN_00415020` / `FUN_00415200` / `FUN_00415220` and `LeaderInRange` — U-9186 branch 2 is 64 of its
149 diverging calls (`UNCERTAINTIES.md:63`, U-9187).

## 1. The decisive fact — this is a BINDING problem, not a bridge

`RACE_POSITION_RECON_SCOPE` §1 had to confront a substrate mismatch: the port's gate-ordinal
progress is a *different quantity* from the original's spline `race_pct`, so feeding one into the
other is faithful-by-algorithm at best. **That argument does not apply here.** Measured, leg G1
run 2:

| | `_abs` (ported consumers) | `_rec` (port's live record) |
|---|---|---|
| GATE2 `veh_type == 1` | 0% | **100.0000%** |
| GATE3 `state0 == 0` | 100% | 100% |
| `G1-POS` `rec_x` distinct | 1 (FAIL) | **22,031** (PASS) |
| `G1-ALL` with slot-state seed | 0 | **40,494/53,992 = 75.0000%** |

The port already maintains the *same fields* of the *same record layout* at the *same offsets*.
Nothing needs synthesizing, scaling or algorithm-matching. **No fidelity decision is owed here**,
which is the material difference from the race-position lane.

What is actually wrong is that the ported readers hardcode the original's absolute addresses while
the standalone's array lives elsewhere:

```cpp
// Vehicle/VehicleState.cpp:18,39 — exe-reachable body of FUN_0046c7b0, NO standalone rebinding
static constexpr std::uintptr_t kVehicleBase_8815a4 = 0x008815a4u;
return reinterpret_cast<const std::uint32_t*>(kVehicleBase_8815a4)[vehicleIdx * kDWordStride];
```

versus the pattern that already exists two files away:

```cpp
// Vehicle/LaunchRevCharge.cpp:42,62,77-83
//   record array   0x008815a0  -> Vehicle::g_vehicleArrayBase (= g_records)
inline char* LrcVehBase() {
#ifdef MASHED_STANDALONE
    return reinterpret_cast<char*>(mashed_re::Vehicle::g_vehicleArrayBase);
#else
    return reinterpret_cast<char*>(0x008815a0u);
#endif
}
```

**There is a second, independent gap.** The per-slot init that sets field (1) `alive = 1`
(`PhysicsChainHooks.cpp:2988`) and field (3) `t = 0` (`:3016`) is in `asi_sources.rsp` **only**, so
the standalone never runs it. Field (1) measured `1` on the record anyway (G1 `_rec` GATE2 = 100%),
so something else already sets it; field (3)'s standalone value is `kWheelSetSel`, init 1
(`VehicleStruct.h:101`), and the stepdump's `sel_9a8` reads `0` in `E2off_1.csv` — **[UNCERTAIN]**
which writer wins, and it must be pinned before (3) is trusted.

## 2. LEG H1 — rebind the three exe-reachable readers (CHEAP; do first)

**Change:** give `FUN_0046c7b0` (`Vehicle/VehicleState.cpp`), `FUN_0046cbb0` (same TU) and
`FUN_0046d4a0` an exe body that resolves the record base through `g_vehicleArrayBase` under
`MASHED_STANDALONE`, exactly as `LrcVehBase()` does. `0046c7b0` and `0046cbb0` are **C4 impl with
exe bodies**; `0046d4a0` is **C3 impl, `.asi`-only, empty `exe_file`** and needs one added.

**Inert-first checkpoint (gate before any consumer is trusted):** the G1 probe is already built and
default-OFF (`MASHED_U9186_GATES`). After the rebind, CONFIRM the `_abs` and `_rec` columns
**agree** on all four fields for all 4 cars across a race. If they do not converge, the rebind is
wrong and nothing downstream may be read.

**Risk: low.** No behaviour changes until a consumer is wired; the `.asi` path keeps the absolute
and must stay bit-identical (its Frida evidence covers the absolute form).

**Dual-copy hazard:** `0046cbb0` already carries a dual-copy note (live `VehicleCarStateRead` vs
DEAD `CarStatePairGet` in `PromoLoop_round25.cpp`) and `0040e180` was **demoted C4→C2 2026-09-29**
because its exe copy uses `std::sqrt` and disagrees with the asi copy by 7.8%. Edit the `exe_file`
copy, not the `file` copy, and re-check the demotion before relying on `0040e180`.

## 3. LEG H2 — the slot-state seed, default-ON question (CHEAP; already measured)

**Change:** none needed to make it work — `MASHED_SLOTSTATE_SEED` already does it, and G1 measured
the result: pointer `0x005f2728`, 3 of 4 cars read state `2`, player slot `0`. The open item is
whether it ships default-ON. D-11072 leg C already established it is a bit-faithful restoration of
a static `.data` initializer that nothing writes at runtime, so the safety argument exists.

**Inert-first checkpoint:** G1's own `G1-GATE1` on a default build after the flip — CONFIRM
`{0,2,2,2}`.

**Risk: low.** Leg C measured it behaviourally inert on its own; after H1 it stops being inert,
which is the point, so the standing gates in §5 must be re-run at that moment and not before.

## 4. LEG H3 — port `FUN_00442a60` (MEDIUM; build on H1 + H2)

**Change:** the port that leg G1 refused to write. Only after H1+H2 make `G1-ALL` non-zero. The
function itself is 531 bytes and fully decompiled; its one non-leaf callee `FUN_0040e180` has an
exe copy already (see the §2 demotion caveat). It also reads `0x008a96ec` via `FUN_00408ad0`, which
the `MASHED_RACEPCT_BRIDGE` knob already supplies (D-11072 leg A, proven live and consumed).

**Inert-first checkpoint, and the one that matters for U-9186:** a default-OFF gate-fire counter on
the two U-9186 branches. CONFIRM each approaches the original's 36 / 64 calls before any `ctrl`
write is enabled. The committed captures `o_t1`/`o_t2`/`o_t3` carry the exact 149 calls.

**Risk: medium.** `FUN_00442a60` writes `0x008989b0`, which has real consumers; this is the first
leg in the chain that can change behaviour.

**Standing prohibition to clear first:** `DEFERRED.md:15` says **"DO NOT start leg B"** and
`RACE_POSITION_RECON_SCOPE` §4 says "Risk: high. Do not start Leg B first." H3 **is** that leg B.
This scope argues the risk assessment was based on the belief that the substrate had to be
synthesized, which G1 run 2 refutes — but **that is a USER call to re-open, not an analysis one.**
Do not start H3 until the prohibition is lifted explicitly.

## 5. The standing gates for every leg (inherited from the car<->car discipline)

Every leg above inherits, unchanged:

- an inert-first liveness witness, default-OFF, confirmed BEFORE any score is read
  (memory `verify-the-harness-knob-actually-took`);
- `G-NOREG-E` — criterion (e), `ai_speed_env.py --check`, all six digits;
- `G-NOREG-B` — criterion (b), `ai_ctrl_window.py --check`, the same bands;
- `G-BANDS-UNEDITED` — the `ROADMAP.md` §D3 bands are not touched;
- `G-KNOBOFF` — knob OFF reproduces a freshly-taken deterministic reference cell-for-cell
  (`AMEND_E2.md` A1: the reference must be re-taken under the same knobs, not inherited);
- `G-DET` — 3 repeats per arm, under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`;
- the power-up sweep and the modes oracle (rule 3).

Plus one added by this session's experience: **an arms cell-for-cell diff**
(`re/tools/det_prefix.py A.csv B.csv`) before any outcome gate is scored — it is one command and
strictly stronger (memory `reachable-and-correct-is-not-consumed`).

## 6. Recommendation and estimate

Do **H1** next. It is cheap, it is a pattern already in the tree twice, it needs no fidelity
decision, and its checkpoint is a probe that is already built and committed. H2 is a one-line
default flip whose evidence already exists. H3 is the only leg that needs new behaviour work and it
is blocked on a user decision, not on analysis.

**[UNCERTAIN]** — not resolved by this scope:
- which writer sets field (3) `t` standalone, and whether `sel_9a8 == 0` is correct (§1);
- whether `0040e180`'s exe copy's 7.8% `std::sqrt` disagreement matters once it is actually used;
- whether H1 alone changes any behaviour. It should not — nothing consumes the rebound readers yet
  — but that is a prediction, and §5's `G-KNOBOFF` is what tests it.

## 7. Tracker placement

Files under **U-9186** as the named remedy path, and against **D-11072** as the leg-B
precondition that changes its risk assessment. It is explicitly **not** a C-level gate: a rebinding
restores the original's own addressing on the standalone target and promotes nothing by itself.
