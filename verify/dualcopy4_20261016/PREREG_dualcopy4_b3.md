# PREREG dualcopy4 group (b), block 3: the RaceCamera pairs (Vec3Mag 0x004c3ac0, Vec3Norm 0x004c39b0, RotateAboutAxis 0x004c4d20)

Committed BEFORE the first camera run of this lane. Read against `PREREG_dualcopy4_b.md` and `verify/d1r2_20261016/PREREG_d1r2.md`.

## 1. Facts (read in the tree, 9b0248128 + this branch)

`Race/RaceCamera.cpp` (exe only) carries three local helpers in an anonymous namespace, each annotated with the RVA it stands in for:

| helper | what it computes | the shared body | difference |
|---|---|---|---|
| `Vec3Mag` (0x004c3ac0, hooks.csv C4, file `Math/Vec3.cpp`, exe_file `Race/RaceCamera.cpp`) | `std::sqrt(x*x + y*y + z*z)` | `Math/Vec3.cpp:Vec3Magnitude` (compiled into the exe; no LUT in the exe, so its fallback is `sq > 0 ? std::sqrt(sq) : 0` with `sq` a float local) | same expression; the sum is stored to a float local in the shared body, left in the x87 register (or not) in the camera: that is the only possible bit difference |
| `Vec3Norm` (0x004c39b0, C2) | `m = Vec3Mag(src)`, `dst = src / m` (division); m <= 0 copies src | `Math/RwV3dNormalize.cpp:RwV3dNormalize` (exact-sqrt fallback in the exe, `scale = 1/m; out = scale*in`, and on `m <= 0x005d757c` calls the original's FUN_004d7ff0 / FUN_004d8480 BY ADDRESS) and `Math/RwLutTables.cpp:LutNormalize` (the real LUT, no error report, used by Race/PathNodes.cpp) | division vs multiply by the reciprocal, exact vs LUT sqrt. `RwV3dNormalize` is unsafe in the exe on a zero vector (calls into zeroed memory), so it is NOT a candidate |
| `RotateAboutAxis` (0x004c4d20, C2) | builds a 3x3 Rodrigues matrix with `std::cos/sin` and applies it | `Math/RwMatrixRotate.cpp:RwMatrixRotate` + `RwMatrixRotateInner` + the device transform FUN_004c3df0 | the shared path needs a RwMatrix, the x87 FSIN/FCOS and a vector transform; `RwMatrixRotate` is C2, so swapping to it replaces a camera that is 1e-4-accurate against the original (d1r2 G-POSE) with an unproven function |

d1r2's pose scores are 1e-4 (position median 1.1e-4, `verify/d1r2_20261016/scores.txt`), three orders above any last-bit change in these helpers. So the replay can
confirm "nothing moved or nothing worsened", it cannot rank the arithmetic variants.

## 2. Arms (one exe per arm, built from this branch; baseline = `images/base.exe`)

| arm | change |
|---|---|
| C0 | base.exe |
| C1 | `Vec3Mag` becomes a call to `Vec3Magnitude`; helper deleted; `Vec3Norm` and `RotateAboutAxis` unchanged |
| C2 (experiment, reported, lands only if the gates below hold) | additionally `Vec3Norm` calls `Math::LutNormalize` |

`RotateAboutAxis` is NOT changed in this lane: stated skip (needs the RwMatrix and an unproven C2 callee; the pose score cannot validate it).

## 3. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| G-POSE-C1 | `cam_dump.csv` and `librw_dump.csv` sha8 of C1 equal C0's (`1e7d2274` / `b33ca1b4` at 88a98a2f, measured again on base.exe), and `score_d1r2.py pose` unchanged | IDENTICAL, because the expressions are the same. If NOT identical, the first differing rows are named and the arm is reported as a numeric change, not a pure consolidation |
| N0-cam | two replay runs of the same exe give identical dumps | identical (dualcopy3 measured that) |
| G-POSE-C2 | pose scores of C2 equal or better than C0 at the printed precision (position median 1.1e-4 p95 1.0e-3 max 2.6e-3, angles as scores.txt) | equal at the printed precision (last-bit changes under a 1e-4 error floor); `cam_dump.csv` differs in the low bits |
| G-CTRL (exe) | `2b4d47bb / 356821c8 / f7a155e1` for C1 and C2 | UNCHANGED: the camera does not feed the sim in the G-CTRL recipe |
| Rows | C1 lands: 0x004c3ac0 `exe_file` -> `Math/Vec3.cpp` (== file). C2 lands only if G-POSE-C2 holds AND the maintainers' note "do not flip the global paths silently" is respected: `LutNormalize` is used for the CAMERA only (precedent: Race/PathNodes.cpp, VehicleInit A3), `RwV3dNormalize` stays on its fallback | |

## 4. Decision rule

C1 lands if G-POSE-C1 is identical or the differing rows are named and the pose scores are unchanged. C2 lands only on equal-or-better pose scores AND unchanged G-CTRL;
otherwise it is reported as measured and reverted. Anything that moves the default camera numerically is listed under "default build behaviour: yes" in the report.
