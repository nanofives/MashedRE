# RESULT: D-11072 leg P1, the original's static path data, byte-identical

Pre-registration: `PREREG_PATHPROG.md` (`ba552428`, before code). Knob `MASHED_PATHPROG`, default OFF.

## 0. Verdict first

> **G-P1 PASS on both tracks.** With `MASHED_PATHPROG=1`, the standalone writes the course block
> `0x00657498 .. +0x16264` byte-identical to the original's live block, on Training (30 nodes) and
> Arctic (94 nodes): distance table, angle table, every node record (dir, centre, 4 corners), route
> table `0x0066ce58`, safe-start lines, count `0x0066d6d8`, variations, split list `0x0066d6e4`.
> **G-P1-OFF PASS:** knob OFF reproduces the default G-CTRL trio `ae67b13a / c8317ce2 / 31db9384`
> (`P1_CTRL`).

| gate | Training | Arctic |
|---|---|---|
| G-P1-COUNT | 30 = 30 | 94 = 94 |
| G-P1-NODES (0x8e8 / 0x1be8 bytes) | 0 diff dwords | 0 diff dwords |
| G-P1-ROUTE + splits + safe + header | 0 | 0 |
| distance + angle tables (FUN_004783f0) | 0 | 0 |
| G-P1-OFF | `ae67b13a / c8317ce2 / 31db9384` | (one arm) |

## 1. What it took (each step measured, in order)

| run | nodes diff | dist diff | angle diff | change |
|---|---|---|---|---|
| 1 | 439 / 1316 | 25 / 43 | 22 / 69 | first transcription |
| 2 | 10 / 1 | 0 / 0 | 4 / 6 | real RW sqrt LUT + `RwMatrixInvert` exe fix |
| 3 | 0 / 0 | 0 / 0 | 1 / 3 | x87 precision control PC_24 |
| 4 | 0 / 0 | 0 / 0 | 0 / 0 | CRT `acos` keeps the caller's precision bits |

## 2. Three findings that reach beyond P1

1. **The standalone has never used RenderWare's real sqrt LUT.** `RwLutGuard.h` rejects the
   standalone's garbage `*(0x007d3ff8)+*(0x007d3ffc)` chain (no `RwEngineOpen`), so `FastSqrt`,
   `FastInvSqrt`, `RwV3dNormalize` and `Vec3Magnitude` take an exact-`std::sqrt` fallback in
   `mashed_re.exe`. The original's LUT gives e.g. `sqrt(16.011608) = 4.0009765625` where the
   fallback gives `4.0014505`. The tables are now rebuilt in-process (`Math/RwLutTables.cpp`), with
   a generator fitted 4096/4096 on both tables against the live original (`o_rwlut.*.bin`).
   **Only P1 uses them.** Switching the global paths would change the D2 physics, so that is a
   user decision, not made here.
2. **The original runs track load at x87 precision PC_24** (24-bit mantissa), as Direct3D 9 sets
   on device creation. At 53 bits, 11 node dwords and 10 angles came out 1 ULP off; at PC_24 all
   are exact. The CRT `acos` (`0x004a3384`) keeps the caller's PC: its helper `0x004a70a5` does
   `and edx,0x300 / or edx,0x7f / fldcw`. This conflicts with the project rule "ST-resident values
   are `double`" for *this* caller. Whether the race loop also runs at PC_24 is not measured here
   ([UNCERTAIN], needs a CW read on the original during a race).
3. **`Render/RwMatrixInvert.cpp` (0x004c4eb0) returned all zeros in the exe.** It loads its +1.0
   numerator from `ds:[0x005cc320]`, which reads 0 in the standalone's blank-mapped `.rdata`. Exe
   arm now uses `fld1` (the same value). PathNodes is its only exe caller, so nothing else changed
   (G-P1-OFF bit-identical).

## 3. What is ported

`Race/PathNodes.cpp` (x87 TU, exe only): `FUN_0047a0f0` zero-fill + the six LAPDATA handlers
(`0x0047a720 / 750 / 790 / 7d0 / 7f0 / 810`, driven by a flat call-list parser),
`FUN_00478660` per-node body with `FUN_004785e0`, `FUN_004b5030 / FUN_004b4eb0` (colour-key walk,
sector DFS = stream order), `FUN_004b4550`, `FUN_004b46b0`, the cofactor inverse and the slot-`+0xc`
point transform, and `FUN_004783f0` with the route walkers `FUN_00409300 / FUN_00409330`.
Called from `TrackRenderer.cpp` right after the AI BSP parse. No C-level change is claimed: these
are exe-only bodies with a data-level gate, not run_diff evidence.

## 4. Next

Leg P2: `FUN_00408610` per frame (sphere vs AI BSP query + `FUN_00407a90`, `FUN_00407e20`, walkers),
gated by `0x008a96e8` liveness and the grid values 30.00/29.73/29.45/29.18 at frame 0. Trackers
(D-11072 row, the three findings above as U-items) go through `re-classify` at leg end.

## 5. Raw

`o_course_t{0,3}.bin` (original), `p_course_{Training,Arctic}.bin` (port, final build),
`o_rwlut.{sqrt,invsqrt}.bin`, `compare_p1.py`, `run_p1.ps1`,
`../d3_gatefire_20261008/P1_CTRL.{step,gates}.csv`, `listing_4783f0_4785e0.txt`, `decomp_*.txt`.
