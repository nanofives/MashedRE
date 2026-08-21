# RVA-tunnel audit of `mashed_re.exe` — 547 tunnels, and why they are silent

Date: 2026-08-21
Trigger: the `RwMatrixRotate` root cause (`D2_REALPHYS_REMEASURE_2026-08-21.md`,
fixed in `9cc41fa8`) — two constants read from MASHED absolute addresses that
read **0** in the standalone, turning every axis-angle rotation into a scale and
freezing vehicle steering for seven weeks. This audit asks: how many more?

## The mechanism — the exe deliberately maps a zero wedge

This is the finding that matters most, because it explains why a defect of this
class survives for weeks instead of crashing on the first frame.

- `exe_main.cpp:5348` `MapMashedDataSection()` `VirtualAlloc`s **every free 64 KB
  granule in 0x00500000–0x009fffff** as zero-filled `PAGE_READWRITE`
  (`kMashedDataBase` / `kMashedDataSize`, `exe_main.cpp:484-485`), called at
  `exe_main.cpp:5880`.
- `Compat/StandaloneRvaThunks.cpp:20` maps **0x00420000–0x0047ffff** and
  **0x005c0000–0x005cffff** as zero-filled `PAGE_EXECUTE_READWRITE`.
- A TLS callback pre-reserves those granules (`exe_main.cpp:272-280`).
- Exactly **8 addresses hold correct values**: two `.rdata` scale constants
  written by hand (`exe_main.cpp:5975-5976`) and six JMP thunks
  (`exe_main.cpp:5978-5983`).

**Consequence: every other MASHED address in those ranges reads 0 with no access
violation.** `0x005cd7a8` (pi/180) and `0x005cc320` (1.0f) — the two the
`RwMatrixRotate` bug read — sit inside the mapped-zero range. The wedge converts
a loud crash into a silent wrong value.

There is no `#ifdef MASHED_STANDALONE` guard on any tunnel: only ONE exe TU
references that macro at all (`Save/GameSaveBuffer.cpp`), and no header defines
or tests it.

## Inventory — 547 runtime tunnels across 84 of 205 exe TUs

Exe TU set established from `mashedmod/build.bat`: 200 `.cpp` on the
`/DMASHED_STANDALONE /Fe"…mashed_re.exe"` line (`build.bat:177-382`) plus 5
pre-compiled isolated objects (`build.bat:378-382`). The tree holds 434 `.cpp`
total, so 229 are `.asi`-only and correctly out of scope.

| Kind | Region | Count | Effect in the exe |
|---|---|---:|---|
| DATA deref | 0x00500000–0x009fffff (zero wedge) | **405** | reads 0 — **silent**, the found bug's shape |
| CALL via fn-ptr | 0x00420000–0x0047ffff (zero wedge) | 55 | call into `00 00…` → AV (except the 6 thunks) |
| CALL via fn-ptr | unmapped | 51 | AV on first call |
| CALL via fn-ptr | 0x00500000–0x009fffff | 28 | AV on first call |
| DATA deref | unmapped | 7 | AV, or stored-but-unused |
| DATA deref | 0x00420000–0x0047ffff | 1 | reads 0 |
| Provenance (comments) | — | 181 | not a defect |

The 405 silent data reads are the dangerous population. An AV is self-reporting;
a zero is not.

## The finding that changes D2's scope

**The largest concentration of the exact bug shape sits in the physics/collision
cluster, which ROADMAP §D2 plans to switch on.**

~80 macros of the form `#define _DAT_005cxxxx (*(const float*)0x005cxxxxu)`,
whose commented true values are `1.0f`, `0.5f`, `-1.0f`, `2.0f`, `0.99f`,
`FLT_MAX`, `FLT_MIN` and assorted epsilons. **Every one evaluates to `0.0f`**
in the exe. Highest density:

```
Collision/RwpSolverCore17.cpp:57-68   (12)   Collision/RwpSolverCore12.cpp:63-73  (11)
Collision/RwpSolverCore18.cpp:64-72   ( 9)   Collision/RwpSolverCore15.cpp:42-50  ( 8)
Collision/RwpSolverCore8.cpp:71-75          Collision/RwpSolverCore10.cpp:47-52
Collision/RwpSolverPartition13.cpp:60-64    Collision/RwpSolverCore21.cpp:89-94
Collision/RwpSolverIntegrate6.cpp:84-87     Collision/RwpSolverLeaves1.cpp:52-55
Collision/RwpSolverMath2.cpp:54-56          Collision/RwpSolverCore{4,9,11,14,16,23}
Collision/RwpVtableKV1.cpp:92               Vehicle/VehicleCouplingBridge.cpp:65
```

They are currently dead because the whole chain is gated on
`MASHED_REAL_PHYSICS` (`Vehicle/VehiclePhysicsRun.cpp:155`, consumed at the sole
entry `D3d9Render/TrackRenderer.cpp:2519-2550`), default OFF.

**D2's stated goal is to invert that flag. Doing so activates ~80 zero-valued
physics constants simultaneously.** The `RwMatrixRotate` fix restored steering,
but it fixed *one* instance of a class that is densest in exactly the code D2
wants to make default. Any D2 plan that treats the flag inversion as a one-line
change is wrong on this evidence.

## `Math/` — the directory the bug lived in is otherwise sound

12 `Math` TUs in the exe. Clean: `RwV3dTransformPointsCPU.cpp` (zero MASHED
addresses anywhere), `RwMatrixScale.cpp`, `RwV3dTransform.cpp`, `FPURound.cpp`.

**`RwSqrt.cpp` and `Vec3.cpp` are the correct model for this whole class.** They
route `0x007d3ff8`/`0x007d3ffc` through `RwLutGuard.h`, which validates the
resolved LUT root and returns `nullptr` in the standalone, falling back to
`std::sqrt` — with the rationale written out at `RwSqrt.cpp:42-63`.
`RwV3dNormalize.cpp` and `RwV2d.cpp` are clean by the same construction (the
guard returns before the tunnel lines are reached).

Two files carry real exposure, both blocked by a **caller-side accident** rather
than an in-file guard:

- `Math/RwMatrixRotateInner.cpp:159-166` — reached from the now-live
  `RwMatrixRotate`, but only on `mode == 1|2`. It reads a device table out of the
  wedge and calls `mul(...)`, which resolves to **nullptr** (the fake device at
  `RwIm2DBridge.cpp:240` is memset to 0 across +4/+8). Safe today only because
  every live caller passes mode 0; the mode-1 callers in
  `Vehicle/AeroStabilize.cpp:78,82,91` sit behind `if (orient)` and the sole call
  site passes `nullptr` with the comment "orient bound at A8"
  (`Vehicle/VehicleControl.cpp:155`). **A8 is the next D2 task — binding `orient`
  would arm a null call.**
- `Math/RwV3dTransformPoints.cpp:46-49` — unguarded deref + call through a
  wedge-read function pointer. Dead only because callers bind
  `RwV3dTransformPointsCPU` instead (`Vehicle/ForceIntegratorStubs.cpp:37-39`).

## Things a `0x00` grep would have missed

- **20 multi-line casts** where the type and literal are on different lines, e.g.
  `Frontend/MenuSpriteDispatch.cpp:82,87,92`, `Frontend/MenuNearLeaves_s6.cpp:150,158`
  (double-indirect through `*(void**)0x007d3ff8`), `Boot/BootLowRvaCluster.cpp:245`.
- **11 `base + index*stride` reads** where only the table base is a literal:
  `Frontend/MenuStateMachine.cpp:133,135,244,246,356,358` (`0x007f1041/2/3`,
  `0x007f1501/2/3`, stride `0x4c` — the per-player input descriptor array from
  U-3558), `Frontend/MenuHelpers.cpp:109`, `Frontend/SlotZeroers_s1.cpp:67,106`,
  `Render/SlotObjectAccessors.cpp:89,147`, `Util/EventTable.cpp:31`.
- **Arrays of addresses**: `Frontend/BatchAA_s3.cpp:109`
  `kActiveSlots[4] = {0x007f1a14, …}`, indexed at runtime.
- **Function pointers initialised from a literal then called via the variable**:
  `HUD/ScenarioWriters_sa2s2.cpp:64,66,71,74`, all four in unmapped regions.

There is **no `g_base + offset` relocation scheme anywhere** — no indirection
layer exists to intercept these centrally.

## [UNCERTAIN] — what this audit did not settle

- **Bulk reachability of ~460 of the 547.** Reachability was resolved precisely
  for `Math/`, the physics cluster (env gate) and 7 sampled Frontend/Util files
  (dead exports, `RH_ScopedInstall` being a no-op in the exe via
  `Stubs/HookSystemNoOp.cpp`). The remainder is classified by *region*, not by
  *reachability*. Settle it with the linker `/MAP` cross-referenced against the
  call graph from `WinMain`, or — cheaper and decisive — **re-run with the wedge
  granules set `PAGE_NOACCESS`, which converts every live tunnel into an
  immediate, self-reporting fault.** That inverts the silence that let this class
  hide, and is the single highest-value follow-up.
- `Math/MatrixOrthoResidual.cpp:127,240` — `__asm mov ecx, 0x005cc320` loads the
  address; whether the following FPU ops dereference it was not confirmed.
- The 5 isolated-object TUs (`LibRw/*`, `Collision/QhullBridge.cpp`) compile
  **without** `/DMASHED_STANDALONE`. Not a defect today (none appeared in the
  scan), but the flag asymmetry means any future `#ifdef MASHED_STANDALONE`
  added there would silently take the `.asi` branch inside the exe.
