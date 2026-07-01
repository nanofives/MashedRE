# A6b VehicleAeroStabilizer — NATURAL airborne canonical C4 evidence (2026-07-01)

Closes the last physics-chain C3 gap. A6b = `FUN_00468980` (`VehicleAeroStabilizer`),
RVA `0x00468980..0x00468b34`. Anchored MASHED.exe SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
Lane = `mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp` (verbatim naked-asm body,
installed-hook in-process A/B self-test, `MASHED_PHYS_C4_SELFTEST=1`).

Branch `ws-phys-c4-a6b`.

## Why this run is C4 (vs the 2026-06-23 C3)

A6b's ENTIRE aero body is gated on `+0x9e0 == 0.0f` (fully airborne — all wheels off).
On flat Arctic (Quick-Battle default) no car ever leaves the ground, so only the grounded
short-circuit was reachable. On 2026-06-23 the airborne body was verified 18/18 GREEN but
only under a **forced** upward velocity write (`scenario_launch.py --boost`, which pokes
`record+0x9b4`). A synthetic state write is C3-grade (the record fed to A6b was fabricated),
so A6b was held at C3.

**This run reaches the airborne body NATURALLY** — a car (AI and player) driving off a real
ramp on the **Warzone** track so `+0x9e0` reaches `0.0` under the game's own physics, with
**no `--boost` / no synthetic write**. The record/xform/dt fed to A6b are genuine canonical
game state. The A6b entry trampoline (`A6b_Entry`, `RH_ScopedInstall(A6b_Entry, 0x00468980)`,
PhysicsChainHooks.cpp:3153) is installed **live (inline-JMP)** — the self-test executes only
from inside that trampoline (`A6b_Entry` → `A6b_Hook` → `A6b_SelfTest`), so its firing proves
the inline-JMP at `0x00468980` was active. This is a canonical scenario with the hook
installed → C4-grade (matches how A3/A4/A5 reached C4 on this same lane).

## Track discovery (empirical)

No jump-map existed. Swept the RE'd engine-track indices (`scenario_launch.py` table,
ptr `0x005f2728 -> 0x005f33f8`) via `scenario_launch.py --track T --cars 4 --hold 40
--hooks 0x00468980` (natural, NO `--boost`), checking `original/phys_c4_a6b_selftest.log`
for `air=1` rows:

| Track idx | Name       | Natural airborne (air=1) in a 40s warp |
|-----------|------------|----------------------------------------|
| 0 | Training   | none |
| 1 | Egypt      | none |
| 2 | Neustein   | none |
| 3 | Arctic     | none (known geometrically flat) |
| 4 | Highway    | none |
| 5 | Sands      | none |
| 6 | SuperG     | none |
| 7 | Roundabout | none |
| 8 | Storm      | none |
| 9 | Forest     | none |
| 10 | Dump      | none |
| 11 | **Warzone** | **YES — cars launch off Warzone geometry** |
| 12 | City      | (warp intermittently failed to spawn) |

Warzone (idx 11) is the track whose racing line crosses a jump the cars actually take.

## Result — GREEN across 2 independent runs, 2 car slots, 3 state sub-branches

The airborne aero body has **no PRNG** (pure aero math — record + xform only), so its output
is a deterministic function of (record, xform, dt). The self-test snapshots those two, runs
MINE (verbatim body) on a copy, restores, runs ORIGINAL, and compares (`ndiff` = mismatched
u32s among record `+0x9bc/+0x9c0/+0x9c4` and the 16-float world xform). Bit-identity on
varied real airborne inputs spanning all sub-branches proves the body; the *rarity* is only
about **reaching** airborne (AI/player trajectory is PRNG-varied), not about the math.

| Capture | Harness | Slot | Airborne frames | `state` sub-branches | Verdict |
|---------|---------|------|-----------------|----------------------|---------|
| 1 | Warzone, cars=4, hold=40, no boost | 2 (AI opponent) | 11 | 0, 1, 2 | **11/11 `ndiff=0`** |
| 2 | Warzone, cars=4, hold=40, no boost | 0 (human player) | 1 | 2 | **1/1 `ndiff=0`** |

- **12 natural airborne frames total, 0 RED (all `ndiff=0`)**, reproduced across two
  independent boots and two different car slots (AI opponent + human player).
- Covers airborne `state=0` (the aero auto-level FUN_004c4d20 rotate path), `state=1`, and
  `state=2` (the `state!=0` angular-velocity-zeroing sub-branches at record `+0x9bc/+0x9c0/
  +0x9c4`). Every airborne sub-branch of A6b ran bit-identically.
- Grounded gate also GREEN in every run (16 grounded rows across the two captures, all
  `ndiff=0`), so both A6b branches (grounded short-circuit + airborne aero body) are verified.

Evidence logs (raw self-test output):
- `A6b_airborne_natural_capture1_slot2AI_20260701.txt`  (11 airborne frames, AI slot 2)
- `A6b_airborne_natural_capture2_slot0player_20260701.txt`  (1 airborne frame, player slot 0)

Reproduction command (natural — do NOT pass `--boost`):
```
py -3.12 re/frida/scenario_launch.py --track 11 --cars 4 --hold 40 --hooks 0x00468980
grep 'air=1' original/phys_c4_a6b_selftest.log   # want rows, all ending "ndiff=0 OK"
```
Airborne hit-rate is low (~1 in 8 warps at hold=40) because it depends on an AI/player car
reaching Warzone's jump inside the brief active-racing window before the field settles;
re-run until `air=1` rows appear (the verdict on any captured airborne frame is exact).

## Disposition

A6b `0x00468980` promoted **C3 → C4**. **The physics chain is now FULLY C4:**
A3 `0x0046b540`, A4 `0x00470670`, A5 `0x0046ddb0`, A6a `0x00467650`, A6b `0x00468980` — all C4.
(A6a reached C4 on 2026-07-01 by closing its `[U-A6A-FLOAT10]` suspension-force x87 residual
via 4 verbatim x87 shims, 72/72 GREEN — see CHANGELOG; A6b closes the airborne aero body
here.) Supersedes the 2026-06-18 `PHYSICS_C4_DEBT_DISPOSITION` "A6b accepted faithful-C2"
and the 2026-06-23 boosted-C3.
