# PRE-REGISTRATION (UNRUN) — U-9186 leg H1a: rebind the gate-chain readers

Date 2026-10-08. **UNRUN at commit time.** Scope: `re/analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md` §2.
No C-level. `original/` untouched. The `.asi` path is not touched at all.

## 0. The change

Under `MASHED_STANDALONE` only, resolve the per-vehicle record base through
`Vehicle::g_vehicleArrayBase` instead of the hardcoded `0x008815a0`, following
`LaunchRevCharge.cpp:77-83` verbatim in shape. Two functions in `Vehicle/VehicleState.cpp`:

| RVA | function | field | absolute | = record + |
|---|---|---|---|---|
| `0x0046c7b0` | `VehicleSlotGetter` | alive | `0x008815a4` | `+0x004` |
| `0x0046cbb0` | `VehicleCarStateRead` | state, secondary | `0x00881f90`, `0x00881f94` | `+0x9F0`, `+0x9F4` |

Offsets are differences from `0x008815a0`, not from the field constants the file currently names —
the existing comment block (`VehicleState.cpp:13-16`) states offsets relative to `0x008815a4`, so
`+0x00` there is `+0x04` here. Recorded because mixing the two conventions is the obvious way to
get this wrong.

The `#else` branch keeps the absolute, so the `.asi` is byte-identical and its Frida evidence
(which covers the absolute form) still applies.

## 1. Deliberately NOT in this leg, and why

- **`0x0046d4a0`** (`PtrCompute881ec8`) — the scope's third reader. It is **`.asi`-only**
  (`PromoLoop_round58.cpp`, empty `exe_file`), so rebinding it means authoring a second body at the
  same RVA. That is a duplicate-RVA hazard (memory `duplicate-rva-implementations-drift`) and needs
  a `hooks.csv` `exe_file` entry through `re-classify`. Registered as **H1b**, not done here.
- **`0x0046c770`, `0x0046dbe0`, `0x0046d700`** — three more getters in the same TU that index the
  same record (`+0x10`, `+0x08`, `+0x9C8/9CC/9D0`). They have the identical defect. They are
  excluded because each one currently returns pad-zero to its own consumers, so rebinding each is a
  **separate potential behaviour change** that deserves its own measurement. Leaving a TU
  half-converted is itself a trap, so this exclusion is recorded in the source, not just here.
- **`0x0046c6d0`** (`VehicleEntitySlotRead`) — **must NOT be rebound.** `0x008820b0` is a separate
  entity table (`VehicleState.cpp:16`); `0x008820b0 - 0x008815a0 = 0x1110`, past the `0xd04` stride,
  so it is not a field of this record.

## 2. Gates

Checkpoint instrument is the already-built default-OFF probe `MASHED_U9186_GATES`
(`verify/d3_u9186_20261008/`), run with the same three arms and the F2 determinism knobs.

| gate | threshold |
|---|---|
| `H1-CONVERGE` | after the rebind, `veh_type_abs == veh_type_rec` and `state0_abs == state0_rec` on **100%** of rows. Pre-rebind they disagree on 100% (`0` vs `1`), so this gate cannot pass by accident |
| `H1-GATE2` | `veh_type_abs == 1` on **100%** of rows (it is `0` today) |
| `H1-ALL` | with `MASHED_SLOTSTATE_SEED=1`, the full chain read at the **absolute** addresses passes on **75%** of rows — matching what `_rec` already measures |
| `H1-KNOBOFF` (**can fail**) | the AI stepdump of a default build (no seed, no bridge) is **cell-for-cell identical** to `verify/d3_consumer_20261007/E2off_1.csv` on all shared columns. H1 is predicted behaviour-neutral because nothing consumes the rebound readers on the default path; if this fails, something does, and that is the finding |
| `H1-NOREG-E` / `H1-NOREG-B` | criterion (e) six digits and criterion (b) bands unchanged on the default build |
| `H1-ASI` | `mashed_re_dev.asi` builds and the `#else` branch is textually the pre-change expression |

## 3. Decision rules

- All gates pass → H1a lands. H1b (`0x0046d4a0` exe body) is registered next, then H2.
- `H1-CONVERGE` fails → the rebind is wrong; **nothing downstream is read** and no other gate's
  result is interpreted.
- `H1-KNOBOFF` fails → H1 is NOT behaviour-neutral. Do not treat that as a regression by default:
  identify the consumer, report what changed, and decide separately. The scope
  (`VEHICLE_TABLE_BIND_SCOPE` §6) already records behaviour-neutrality as a *prediction*, not a
  requirement.
- `H1-NOREG-E`/`-B` fail while `H1-KNOBOFF` passes → contradictory; re-run both before concluding.

## 4. Non-goals

H3 / leg B (`FUN_00442a60`) — still under the `DEFERRED.md:15` prohibition. Any default-ON change.
Any `hooks.csv` mutation outside `re-classify`. Any edit to the `.asi` code path.
