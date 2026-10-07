# RESULT — U-9186 leg G1: are `FUN_00442a60`'s gates live standalone?

> ## CORRECTION 2026-10-08 (run 2) — the gate verdict stands, the CAUSE in §3 was WRONG
>
> §3 below concluded "the standalone does not own this substrate". **It does.** Run 1's probe read
> only the ORIGINAL's absolute addresses, and under `MASHED_STANDALONE` the vehicle record array is
> rebound to `Vehicle::g_vehicleArrayBase`, not `0x008815a0` (`LaunchRevCharge.cpp:42,62,81`;
> stride witness `imul eax,eax,0xd04` at `0x0046d78e`).
>
> Run 2 logs both addressings side by side. On the port's own record, through
> `VehiclePhysics_Record*`:
>
> | | `_abs` (what the ported consumers read) | `_rec` (the port's live record) |
> |---|---|---|
> | GATE2 `veh_type == 1` | **0%** | **100.0000%** |
> | GATE3 `state0 == 0` | 100% | 100% |
> | `G1-POS` `rec_x` distinct | 1 — **FAIL** | **22,031** — **PASS** |
> | `G1-ALL`, `G1base` | 0 | 0 (slot-state still null) |
> | **`G1-ALL`, `G1seed` / `G1both`** | 0 | **40,494/53,992 = 75.0000%** |
>
> **With the slot-state seed on, the full gate chain passes on 75% of rows — exactly the three AI
> cars — when read through the port's record.** Every input `FUN_00442a60` needs is live.
>
> **What this does and does not overturn.** The gate *verdict* is unchanged and §0's "`G1-ALL` = 0"
> is still true **of the shipped code**, because the ported consumers read the same absolutes the
> run-1 probe did — `Vehicle/VehicleState.cpp:18,39` hardcodes `kVehicleBase_8815a4 = 0x008815a4`
> with no `MASHED_STANDALONE` rebinding. What is overturned is the **cause and the remedy**: the
> blocker is a missing **binding**, not missing **state**, so the fix is the rebinding pattern
> `LaunchRevCharge.cpp:40-42` and `Save/GameSaveBuffer.cpp` already use — not substrate synthesis.
>
> Corroborating, independent of this probe: the per-slot init that writes `alive = 1`
> (`PhysicsChainHooks.cpp:2988`) and the `t` selector (`:3016`) is **`.asi`-only**, so the
> standalone never runs it.
>
> Caveat on one figure: "cars at one frame occupy 2 distinct (x,z) of 4" is sampled at **frame 0**,
> the starting grid. The substantive control figure is the 22,031 distinct `rec_x` over the run;
> an independent mid-run sample (`E2off_1.csv`, frame 2576) shows all three AI cars at three
> distinct positions.
>
> Run 1's captures and gate output are preserved as `*_r1`. Scope: `re/analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md`.


Date 2026-10-08. Pre-registration: `PREREG_G1.md`, committed at `ef420bfc` before the runs.
**RAN.** Measurement only — no port written. No C-level. Nothing default-ON. `original/` untouched.

## 0. Verdict first

**`G1-POS` — the registered control — FAILED on all three arms.** Per `PREREG_G1.md` §4 this leg
therefore reports the measurement as **uninformative about `FUN_00442a60` specifically** and names
the dead source instead:

> the per-vehicle record at `0x00881ec8 + v*0xd04 + t*0x40` reads **one single (x,z) value for all
> four cars on every one of 53,992 rows**, in every arm.

`G1-ALL` is **0 on all three arms**: no row exists on which a ported `FUN_00442a60` would write a
non-zero distance. But the control failure means that is a statement about the **vehicle-table
substrate**, not about `FUN_00442a60`'s own portability.

| gate | `G1base` | `G1seed` | `G1both` |
|---|---|---|---|
| `G1-PTR` `p5f2770` | `0x00000000` | `0x005f2728` | `0x005f2728` |
| `G1-GATE1` `slot_state != 0` | 0% (unevaluable, null ptr) | **75.0000%** | **75.0000%** |
| `G1-GATE2` `veh_type == 1` | **0%** | **0%** | **0%** |
| `G1-GATE3` `state0 == 0` | 100% | 100% | 100% |
| `G1-ALL` | **0** | **0** | **0** |
| `G1-OUT` `refdist` | `{0}` | `{0}` | `{0}` |
| `G1-POS` | **FAIL** | **FAIL** | **FAIL** |

53,992 rows per arm (13,498 frames x 4 cars), all under `MASHED_DETERMINISTIC=1
MASHED_DET_FRAMES=14400`. Raw: `G1_GATES.txt`, `G1{base,seed,both}.gates.csv`.

## 1. This re-confirms in-game what the tracker already recorded statically

**The static blockage was not a new finding of this leg.** `UNCERTAINTIES.md:61` (U-9186) recorded
it on 2026-10-06, before G1 was conceived:

> UPDATE same day: that prerequisite is ITSELF blocked (`verify/d3_overspeed_20261006/RESULT_U9186_PRODUCER.md`)
> — `FUN_00442a60`'s reference-car selection reads the `0x005f2770 -> 0x005f2728` slot-state table …
> and `FUN_00408ad0`'s per-car progress `0x008a96ec` (writer `FUN_00408610` unported -> 0), so
> porting it produces zeros and `RefDist` stays 0.

`DEFERRED.md:15` (D-11072) carries the standing instruction **"DO NOT start leg B"** — leg B being
the `FUN_00442a60` port — and `RACE_POSITION_RECON_SCOPE_2026-10-06.md` §4 adds "Risk: high. … Do
not start Leg B first."

What G1 adds over that record is **measurement rather than inference**, plus three things the prior
work did not establish:

1. **`veh_type` was never measured.** It is `0` on 100% of rows in **every** arm, including with the
   slot-state seed on. It is the hard gate and it is seed-independent.
2. **The seed+bridge combination was untested.** It changes nothing: `G1-ALL` stays 0.
3. **`G1-GATE3`'s 100% "pass" is vacuous.** `state0 == 0` holds only because the table is blank —
   a blank table passes an `== 0` test for the wrong reason (memory
   `all-zero-reads-prove-nothing-alone`). Any future gate chain over this substrate must not count
   it as a pass.

## 2. The probe reads live memory — an unregistered positive control

`G1-POS` failing could in principle mean the probe's address arithmetic is wrong rather than the
state being dead. The `G1both` arm settles it without a new run:

```
G1base / G1seed : racepct_ec distinct values: 1       {0: 53992}
G1both          : racepct_ec distinct values: 17924   {6.16010709e-08: 2628, 0: 2423, ...}
```

With `MASHED_RACEPCT_BRIDGE=1` the probe's read of `0x008a96ec + v*0x30c` returns **17,924 distinct
live values**. The same probe, in the same run, reads one constant from
`0x00881ec8 + v*0xd04 + t*0x40`. So the arithmetic and the mapping both work, and the zeros at the
vehicle tables are genuine dead state.

This control was **not** pre-registered — it is an argument available from the registered arms, and
it is recorded as such rather than as a planned gate.

## 3. What is actually blocking U-9186

Three gates, three different states:

| gate | address | standalone status |
|---|---|---|
| slot-state (`FUN_0040e370`) | `*(u32*)0x005f2770` -> `+0x34+v*4` | **solvable today** — `MASHED_SLOTSTATE_SEED` makes it live; 3 of 4 cars read state `2`, matching `CarSlotStateSet(v,2)` for alive AI cars (`AiStandalone.cpp:1699-1701`), player slot 0 stays `0` |
| vehicle type (`FUN_0046c7b0`) | `0x008815a4 + v*0xd04` | **dead, no knob** — `0` on 100% of rows in all arms |
| position (`FUN_0046d4a0`) | `0x00881ec8 + v*0xd04 + t*0x40`, `+0x30`/`+0x38` | **dead, no knob** — one constant for all cars, all frames |

So U-9186's blocker is **the per-vehicle record substrate `0x008815a4` / `0x00881f90` /
`0x00881ec8`**, which the standalone does not own. That is a larger object than any one function:
`FUN_00442a60`, `FUN_0040e180`, `FUN_0046c7b0`, `FUN_0046cbb0` and `FUN_0046d4a0` all read it, and
all five are inert without it.

**[UNCERTAIN]** whether the standalone could synthesize this substrate from `race_[]` + `ai_cars_`
the way the `0x008a96ec` bridge was synthesized from `arcprog`. Nothing in G1 measures that, and
`RACE_POSITION_RECON_SCOPE` §4 already flags the equivalent reconstruction as "Risk: high".

## 4. Decision taken

`PREREG_G1.md` §4, third rule, applies on the measured result:

> `G1-ALL` == 0 on every arm → `FUN_00442a60` is unportable-to-effect today and U-9186's blocker is
> **the vehicle-table substrate**, not any single function. Report that and do NOT write the port.

**No port was written.** The leg's output is the refusal plus the measurement behind it, which is
also how `RESULT_U9186_PRODUCER.md` closed on 2026-10-06 — G1 reaches the same decision from
in-game data and from the two gates that record never tested.

## 5. Scope limits

Standalone side only, one scenario (Training, rule 4, `MASHED_CAR=1 MASHED_ROUND=1`), scripted
deterministic capture. Nothing here measures the **original**, where all three gates are live by
construction. No claim is made about whether the substrate *should* be synthesized — only that it
is absent today.
