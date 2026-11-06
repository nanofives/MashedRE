# PREREG D4-DC-INC-SQRT-FOLD (committed alone, before any code change or arm)

Branch `orch/D4-DC-INC-SQRT-FOLD` off `orch/integration` @ `96640c25a`.

Source: `verify/d4dcinc_batch3_20261101/RESULT_D4-DC-INC-batch3.md` §"Residual on 0x004c3ac0 / 4c3b30 / 4c3b90".
Each of the three RVAs keeps two C forms:

| RVA | Installed hook (form 1) | LUT helper (form 2) | Exe behaviour of form 1 |
|---|---|---|---|
| 0x004c3b30 | `Math/RwSqrt.cpp:FastSqrt` | `Math::LutSqrt` (`RwLutSqrt_cd`) | `RwLutGuard::SqrtRoot()` is null in the exe, so `std::sqrt(x)` (0 for x < 0) |
| 0x004c3b90 | `Math/RwSqrt.cpp:FastInvSqrt` | `Math::LutInvSqrt` (`RwLutInvSqrt_cd`) | `1.0f / std::sqrt(x)` (0 for x <= 0) |
| 0x004c3ac0 | `Math/Vec3.cpp:Vec3Magnitude` | `Math::LutLength` (`RwLutLength_cd`) | `std::sqrt(sq)` |

In the dev .asi form 1 reads the live RW table (`RwLutGuard` accepts it), which equals the rebuilt table 4096/4096
(`Math/RwLutTables.cpp` header, fitted against `verify/d3_modes37_20261009/o_rwlut.{sqrt,invsqrt}.bin`).

## Knob

`MASHED_SQRT_FOLD=1`, default OFF, read once (`std::getenv`). Unset: every function takes its pre-change path, byte for
byte the same arithmetic. Set: the three hooks return the LUT helper's result in both targets:
- `FastSqrt(x)` = `Math::LutSqrt(x)`
- `FastInvSqrt(x)` = `Math::LutInvSqrt(x)` (zero input returns the argument, +0, the same bits as the hook's `0.0f`)
- `Vec3Magnitude(v)` = `Math::LutLength(v)` (`(x*x + y*y) + z*z`, the same SSE2 order and rounding as the hook: neither
  `Vec3.cpp` nor `RwLutTables.cpp` is in `mashedmod/x87_tus.txt`)

Diagnostic, knob ON only: `MASHED_SQRT_FOLD_LOG=<path>` appends `<hook> calls=<n> differ=<d>` each time a hook's call
count reaches a power of two, where `differ` counts calls whose folded result bits differ from the pre-change form's.
It writes a file and changes no returned value.

Kept separate from D4-DC-INC-0046d4a0-KNOB (another worker, independent knob).

## Reach, read from the source before any arm (exe C callers of the extern "C" hooks, `exe_sources.rsp`)

- `FastSqrt`: `Collision/RwpSolverIntegrate6.cpp` (`FUN_004c3b30` at line 137), `Collision/RwpSolverMath2.cpp` (lines 351,
  382, 403), `Collision/RwpSolverMath2QuatY.cpp` (line 43), `Race/CameraEntries.cpp` (`FSqrt`, line 490).
  `Collision/CarWorldContacts.cpp:61` calls `Collision::FastSqrt` from `ContactDeps.h` (the `CollLut` knob), not the hook.
- `FastInvSqrt`: no exe C caller. `Math/RwMatrixRotate.cpp:123` calls it only when `kRotateLut` is false, which is the
  .asi (`#ifdef MASHED_STANDALONE` sets it true at line 61).
- `Vec3Magnitude`: `Race/CameraEntries.cpp` (`V3Mag`, line 476), `Race/MostSepPair.cpp:21`, `Race/RaceCamera.cpp:63`.
  `Lane2/L2_00565c20.cpp` is .asi only.

Whether these call sites run on the gate scenarios is not established from the source.

## Arms (standalone `mashed_re.exe` built from this branch with `MASHED_NO_DEPLOY=1`, private root `D:/MashedWT/D4-DC-INC-SQRT-FOLD_root`, output `D:/MashedWT/D4-DC-INC-SQRT-FOLD_data/arms`, one arm per `mashed_lock.py run --label D4-DC-INC-SQRT-FOLD` window, explicit PIDs)

`verify/savelink_20261023/run_arms.py` gets two stacks: `SQF` = `MASHED_SQRT_FOLD=1` + the log, `SQFH` = `SQF` + `DEFH`.

- **G0, knob OFF:** `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **G1, knob ON:** `H_SQF B_SQF_K0 B_SQF_K1 B_SQF_K26`.
- **D, knob ON:** `D2_SQFH_1 D2_SQFH_2 D2_SQFH_3` (solo windows), scored with `verify/d2band42_20261029/score_d2_v2.py`
  as the merge train scores `D2_DEFH_*` (`re/orchestrator/merge_train.json` `full`).

## Predictions (scored in this order)

1. **G0:** G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9` (`merge_train.json` `gctrl.ref` /
   `b.ref` on this base).
2. **G1 log:** `FastInvSqrt` has no line (no exe caller).
3. **G1 hashes and D:** not predicted. The six G1 hashes and the D2 scores are reported. They are the expected values
   for the flip, filed as a roadmap decision.

## Verdict mapping

- RED: 1 fails (the knob-OFF default moved), or 2 fails (an exe caller of `FastInvSqrt` exists that the source read missed).
- Otherwise the arms are reported as measured, and the flip goes to a decision row with the G1 hashes as its expected
  gate values. A D2 arm under 1600 `motion_diag` lines is re-run (wall-clock bound, `d2-arms-are-wall-clock-bound`).
- Piz-collision crashes are re-runs (U-9677).

Raw data: `D:/MashedWT/D4-DC-INC-SQRT-FOLD_data/` (arm files, logs), SHA-256 of each cited file in the RESULT.
