# PRE-REGISTRATION: D-11073 leg 1b (phases 4 + 5), behind default-OFF `MASHED_SUBSTATE`

Date 2026-10-09. Brief: `re/BRIEF_D11073.md` §2 "Leg 1b" and "Facts found after leg 1a".
Base: `29693063` (leg 1a `c4fb5ea6` + boot-patch guard). Each section below is written and committed
BEFORE its step runs. A later section may amend an earlier one only by an explicit dated addendum.

Listings used: `leg1b_decomp.txt` (`decomp_pc.py` on a read-only pool clone: `0x0040dbd0`,
`0x004103a0`, `0x0046d7f0`, `0x0046d780`, `0x0046c7b0`, `0x004111c0`) and a capstone pass over
`original/MASHED.exe.unpatched` for `0x004103a0` (360 B, `0x004103a0..0x00410507`).

## 1. Step 1: car-record routing of three leg-1a bodies

### 1.1 What changes

`Race/SubStateCallees.cpp` (E+A). Three bodies use the original's absolute record addresses:

| RVA | export | absolute(s) | record offset |
|---|---|---|---|
| `0x0046baa0` | `VehiclePhysicsStateReset` | 66 stores in `0x008815a0..0x008820d0+` and two loops | addr - `0x008815a0` |
| `0x0046c750` | `EntityVelocityCounterGet` | `0x00882194 + v*0xd04` | `+0xbf4` |
| `0x0046c730` | `EntityDamageStateGet` | `0x00882198 + v*0xd04` | `+0xbf8` |

Under `/DMASHED_STANDALONE` (exe only) each absolute becomes `VehRecord(v) + (addr - 0x008815a0)`
through the existing `Vehicle/VehicleRecordBase.h` (the same base `LaunchRevCharge.cpp:77-83`
`LrcVehBase()` and `VehicleRecordPtr.cpp` use). The asi arm keeps every absolute: its compiled
arithmetic does not change. In the exe, a null base (array not allocated yet) returns 0 from all
three with no store. That is a named exe-only guard, not in the original (which has no such state).

### 1.2 Predictions (written before the build)

- **P1.1 G-CTRL:** knob OFF, `run_h4.ps1 L1A_A` recipe on the new build: step `c741a4c5`, gates
  `642af0ac`, ptrace `05984e30`. Reason: the leg-1a census found no exe caller of any of the three
  bodies, and step 1 adds none.
- **P1.2 path1 (asi):** unchanged GREEN. `run_diff.py vehicle_physics_state_reset_obs` 7/7 with the
  same per-slot fingerprints as `L1A_PATH1.txt`; `early_window_leaf_diff` table-seed keys
  `table_882194_get` / `table_882198_get` 10/10.
- **P1.3 path2:** `run_verify_hook.py` E9+rel32 to the export at all three RVAs, call-through PASS for
  `vehicle_physics_state_reset` (4/4, returns 1,1,0,0), `table_882194_get`, `table_882198_get`.
  Full auto-hook set (the boot-patch guard `29693063` fixed the boot AV).
- **P1.4 exe value (the brief's ask):** from the exe, `EntityVelocityCounterGet(v)` returns the same
  int as `LrcCharge(v)` (record + `0xbf4`) and `EntityDamageStateGet(v)` the same as `LrcState(v)`
  (record + `0xbf8`), i.e. what `0x00882194/98 + v*0xd04` hold in the original. Witnessed in step 2's
  live poll (the poller reads the record through the same base and prints both).

### 1.3 What these gates can and cannot see

path1/path2 run in the asi, where the arithmetic is unchanged, so they cannot see the exe arm.
G-CTRL proves the default build does not call the bodies. The exe arm's first behavioural witness
is step 5 (G-EXIT, G-HOLD), where `FUN_004103a0` calls them.
