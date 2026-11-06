# PREREG D4-DC-INC-0046d4a0-REPLAYORDER

Step: L-82 (a) (Mariano 2026-11-02). Move the mode 7 M3 callback replay (`Gameplay::PickupCb_MaybeReplay`, env-gated by
`MASHED_PKCB_REPLAY`) after `Vehicle::VehiclePhysics_Init`, rerun the replay with the knob OFF and ON, then flip
`MASHED_PK_ONEBODY_46D4A0` (one body for `0x0046d4a0` at the pickup-collect call site `0x00458581`). Closes
D4-DC-INC-0046d4a0 and D4-DC-INC-0046d4a0-KNOB. Base: `orch/integration` `bf8abf3d0` + `orch/D4-DC-INC-0046d4a0-KNOB`
`db3b9bb6e` (merge `8e88ae071`). Knob evidence: `verify/d4dc_0046d4a0_knob_20261101/RESULT_D4-DC-INC-0046d4a0-KNOB.md`.

Written before any arm of this step ran. Committed alone.

## 1. What is being tested

The KNOB step found the replay ran at track load (`D3d9Render/TrackRenderer.cpp`, the path-progress block), before
`VehiclePhysics_Init` set `g_vehicleArrayBase` (`Vehicle/VehiclePhysicsRun.cpp`, `g_vehicleArrayBase = g_records`). There
`PtrCompute881ec8` returns 0 (`Vehicle/VehicleRecordPtr.cpp`, `if (!rec) return 0`) and `FUN_00458570` reads
`[0 + 0x30]` (`0x00458586..0x0045858d`). The original's `FUN_0046d4a0` never returns 0 for `idx < 0x10`
(`0x0046d4a4..0x0046d4c8`).

Fix commit `3a3533f01`: the replay call leaves the track-load block and runs once in `TrackRenderer::UpdateCar`, right
after the one-shot `VehiclePhysics_Init` / `VehiclePhysics_SetWorld` block. With `MASHED_PKCB_REPLAY` unset the call
returns at its first line (`getenv` NULL), so the default build computes nothing new.

## 2. Exes

- `E_FIX`: `3a3533f01` (order fix, knob still selectable). Built `MASHED_NO_DEPLOY=1`. `mashed_re.exe` sha256
  `724c00f6304b17ebce8019db1786381cab079e644dc830760d7c4113cebd472b`, kept at `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/exe_fix/`.
- `E_FLIP`: the flip commit (after this PREREG). `0x00458581` calls `PtrCompute881ec8` directly
  (`re/tools/naked_specs/pickups_m3.json`, regenerated `Gameplay/PickupCollect_naked.inc`), the verbatim `PK_46d4a0`
  body, the selector `PK_46d4a0_sel`, `MASHED_PK_ONEBODY_46D4A0` and its `_WIT` witness are deleted, and the allowlist
  key `DUP-IN-TARGET:0046d4a0` is removed. No opt-out knob: an opt-out would need the second body, which is what the
  allowlist row tracks (precedent: D4-DC-4d20-FLIP deleted `MASHED_GRID_ROTATE_RW` with its inline form).

## 3. Arms and predictions

Recipe for the replay arms = `verify/d4dc_0046d4a0_knob_20261101/run_replay.py` (synth set
`verify/d3_mode7_m3_20261010/synth_unicorn.cb.tsv`, `MASHED_PICKUPS_PC=24`, TRAINING, car 1, round rule 4,
`MASHED_DETERMINISTIC=1`, `MASHED_DET_FRAMES=60`), private root `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_root`, under
`mashed_lock.py run --label D4-DC-INC-0046d4a0-REPLAYORDER` inside `test-slot`. Procedural fallback, fixed now: if an
arm self-exits with no replay report because `UpdateCar` was not reached in 60 frames, the arm is rerun once with
`MASHED_DET_FRAMES=600`. That changes no prediction.

| Gate | Exe / env | Prediction (PASS criterion) |
|---|---|---|
| **G-RP-OFF** | `E_FIX`, knob unset, `_WIT` set | `rows=3888 exact=3888 entry_bad=0 calls_bad=0`, arms `bump=60 collect=60 no-op=3522 vanish=246` |
| **G-RP-ON** | `E_FIX`, `MASHED_PK_ONEBODY_46D4A0=1`, `_WIT` set | same as G-RP-OFF, process self-exits 0 (no AV) |
| **G-READS-ON** | witness of G-RP-OFF and G-RP-ON | 60 rows each, every row `one_ret=1` and `one_ptr != 0` (`one_ptr - idx*0xd04` constant across rows) |
| **G-RP-FLIP** (= G-M3-REPLAY-SYNTH) | `E_FLIP`, default env | `rows=3888 exact=3888 entry_bad=0 calls_bad=0`, same arm counts, self-exit 0 |
| **G-DEF** | `E_FLIP`, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | equal to `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` at run time (today G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3) |
| **G-LINT** | `E_FLIP` tree | `scripts/lint_rva_bodies.py` `NEW=0`, finding count one lower than on `E_FIX`'s tree; `archive_lint.py` PASS |

G-RP-FLIP PC_53 (`MASHED_PICKUPS_PC=53`) is run as a diagnostic, prediction 3888 / 3888 (PREREG_M3 section 3), not a
pass gate here.

Expected hashes after the flip: unchanged (the call's result is dead, ST0 popped at `0x004589e2`, and `FUN_00458570`'s
out buffer is a local the caller never reads). If G-DEF misses, the same arms run on `E_FIX` to split the order fix
from the flip, and the step reports RED with both tables. The merge train's gate run on the flip commit is the final
confirmation (the flip is a code-layout change).

## 4. Verdict rules

GREEN only if every bold gate passes. Any replay MISMATCH line, AV, or G-DEF mismatch is RED. A D3D9 CreateDevice
failure (`0x8876086A`, display asleep) is not a result: the arm is rerun once `scripts/diag.py probe-render` passes.
Piz-collision signatures (U-9677) are reruns.
