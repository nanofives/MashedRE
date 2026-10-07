# RESULT — U-9186 leg H1a: rebind the gate-chain readers

Date 2026-10-08. Pre-registration: `PREREG_H1.md` (committed at `714a97e1`), amended at §A1 before
the run it governs. **RAN**, twice — run 1's `H1-CONVERGE` instrument was wrong, §A1 fixed it, run 2
is scored below. No C-level. Nothing default-ON. `original/` untouched. `.asi` code path unedited.

## 0. Verdict first

**H1a lands. Every gate passes.** The two gate-chain readers now return the port's live record
values on the standalone, and the default build is byte-for-byte unchanged.

| gate | verdict | figure |
|---|---|---|
| `H1-CONVERGE` | **PASS** | `veh_type_fn == veh_type_rec` and `state0_fn == state0_rec` on **53,992/53,992 (100.0000%)**, all three arms |
| `H1-GATE2` | **PASS** | `veh_type_fn == 1` on **100.0000%** (was `0` at the absolute) |
| `H1-ALL` | **PASS** | with `MASHED_SLOTSTATE_SEED=1`: **40,494/53,992 = 75.0000%** — the three AI cars |
| `H1-KNOBOFF` | **PASS** | `H1step.csv` vs `E2off_1.csv`: **IDENTICAL over the whole overlap**, 19,418 keys |
| `H1-NOREG-E` | **PASS** | (e) 3/3; launch 1426.4 / 2053.0 / 2055.2, unchanged |
| `H1-NOREG-B` | **PASS (unchanged)** | `v1`/`v3` pass, `v2`'s same 5 bands with the same numbers as baseline |
| `H1-ASI` | **PASS** | both targets build; the `#else` branch is the pre-change absolute expression |
| `H1-CALLERS` | **0** | neither rebound function has a standalone caller — see §2 |

**Control held.** The `_abs` columns stayed dead in every arm (`veh_type_abs = {0: 53992}`,
`rec_x_abs` 1 distinct value). The rebind changed the *binding*, not the memory — which is exactly
the claim.

Raw: `H1_KNOBOFF.txt`, `H1{base,seed,both}.gates.csv`, `H1step.csv`. Run 1 preserved as `*_r1`.

## 1. Run 1 failed `H1-CONVERGE`, and the instrument was the cause

Run 1 scored 0/53,992 on all three arms. The probe's `_abs` columns read the absolute addresses as
**raw memory** (`Ai::I32(0x008815a4u + vb)`); H1a rebinds the **functions**. A raw absolute read
sees the blank pad whatever the functions do, so `_abs == _rec` was unreachable by construction.

§A1 replaced the instrument with two columns obtained by **calling** `VehicleSlotGetter(v)` and
`VehicleCarStateRead(v, …)` — the only path H1a alters. The subject of the gate did not change.

**Second occurrence this session** of a gate whose instrument did not touch the thing under test
(`PREREG_F2.md`'s `G-TOOK` read a dump-local counter instead of `g_det_frame`). `PREREG_H1.md` §A2
records the standing remedy: before registering a gate, state which code path the change alters and
which path the instrument exercises, and require them to be the same.

## 2. What H1a does and does not buy

`H1-CALLERS` = **0**. The only callers of `VehicleSlotGetter` are `ScoreMasks_ah3.cpp:108,129`, and
that TU is in `asi_sources.rsp:214` **only**; `VehicleCarStateRead` has no caller outside comments.

So on the standalone target **nothing calls either function today**. H1a is a **precondition**, not
a behaviour change:

- `H1-KNOBOFF`'s byte-identical result is therefore near-tautological and is **weak evidence** on
  its own. It is reported because the gate asked for it, and it does rule out an unnoticed
  side-effect of the edit (e.g. the `nullptr` guard firing on a path that does run), but it is not
  evidence that the rebind *works*.
- The evidence that the rebind works is `H1-CONVERGE` + `H1-GATE2`: called directly, the readers
  return `1` and the live state on 100% of rows, where they returned pad-zero before.

**What is now true that was not:** when H3 ports `FUN_00442a60`, its `FUN_0046c7b0` / `FUN_0046cbb0`
gates will read live values, and with the slot-state seed the full chain passes on the three AI
cars. That is the whole point of the leg.

## 3. Scope actually landed

Two functions in `Vehicle/VehicleState.cpp`, standalone branch only:

| RVA | function | field | absolute | = record + |
|---|---|---|---|---|
| `0x0046c7b0` | `VehicleSlotGetter` | alive | `0x008815a4` | `+0x004` |
| `0x0046cbb0` | `VehicleCarStateRead` | state, secondary | `0x00881f90`, `0x00881f94` | `+0x9F0`, `+0x9F4` |

Via a `VehRecord(idx)` helper shaped after `LaunchRevCharge.cpp:77-83`, with a `nullptr` guard for
the pre-race window before `g_vehicleArrayBase` is allocated (`TrackRenderer.cpp:3094` records a
prior AV from exactly that window).

Deliberately excluded, each with the reason recorded in `PREREG_H1.md` §1 and in the source:
**`0x0046d4a0`** (`.asi`-only, duplicate-RVA hazard — registered as H1b); **`0x0046c770`**,
**`0x0046dbe0`**, **`0x0046d700`** (same defect, but each is a separate potential behaviour change);
**`0x0046c6d0`** (must NOT be rebound — `0x008820b0` is a separate entity table, `0x1110` past the
`0xd04` stride).

## 4. Next

- **H1b** — `0x0046d4a0` exe body. Needs a `hooks.csv` `exe_file` entry through `re-classify`.
- **H2** — the `MASHED_SLOTSTATE_SEED` default-ON question. `H1-ALL` now shows what it buys: 75% of
  rows, the three AI cars. Note it stops being inert once H3 exists, so §5 of the scope must be
  re-run at that point and not before.
- **H3** — `FUN_00442a60`. **Still under the `DEFERRED.md:15` "DO NOT start leg B" prohibition.**
  H1a does not lift it.

## 5. Scope limits

Standalone side only; the `.asi` path is textually unchanged and its Frida evidence still applies.
One scenario (Training, rule 4, `MASHED_CAR=1 MASHED_ROUND=1`), deterministic capture. No C-level:
a rebinding restores the original's own addressing on the standalone target and promotes nothing.
