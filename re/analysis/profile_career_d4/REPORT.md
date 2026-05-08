# profile_career_d4 — Analysis Report
Session: HH6 — 2026-05-08  
Pool slot: Mashed_pool1 (read-only)  
Drift-skip: D-6473 (0x00405890) — already C1 via vehicle_damage_d3; D-6461 (0x004c3b30) — resolved by ai_update_d4.  
Parent callee-tree: FUN_00446520 (mode-5 spectator / camera-path handler).

---

## D-6460 — 0x004c1c10 — 101B — C2
`int FUN_004c1c10(int param_1, int param_2)`  
Validates `param_2 ∈ {1, 2}` (0 < param_2 < 3). If valid: sets `*(param_1+0x14) = param_2`; calls `FUN_004c0e50(*(param_1+4))` if non-null; calls `FUN_004c1a70(param_1)`; returns `param_1`.  
If invalid: triggers error via `FUN_004d7ff0(0x80000003, "Invalid_projection_type_specifie..."@0x00617fb8)` + `FUN_004d8480`; returns 0.  
Error string `"Invalid projection type specified"` confirms **RwCameraSetProjection** — param_2=1 (perspective) / 2 (parallel). C2.

---

## D-6462 — 0x00441760 — 116B — C1
`void FUN_00441760(int param_1)`  
Sets `*(param_1+0x24) = DAT_00803344`.  
Calls `FUN_00441700(param_1)` (FOV/aspect setup).  
Gets camera obj: `iVar2 = *(*(param_1+0x84)+4)`.  
Calls `FUN_004c15c0(iVar2)`.  
`iVar1 = iVar2+0x10` (matrix offset).  
Three rotation calls `FUN_004c4d20(iVar1, &DAT_006146fc/@DAT_006146f0/iVar2+0x30, *(param_1+0x38/0x34/0x3c), 2)`.  
Position call `FUN_004c1340(iVar2, param_1+0x40, 2)`.  
Final camera-state apply — called by FUN_00441990, FUN_00442e00, FUN_00446520.

---

## D-6463 — 0x0041ef60 — 20B — C1
`void FUN_0041ef60(int param_1, undefined4 param_2)`  
`*(undefined4*)(&DAT_0063dc78 + param_1 * 0x2AC) = param_2`  
Indexed write to player array at base 0x0063dc78, stride 0x2AC, offset 0. Called 4× per-player in FUN_00446520 entry loop.

---

## D-6464 — 0x00442600 — 316B — C1
`void FUN_00442600(void)`  
Guard `DAT_0089898c != 1`; sets `DAT_0089898c = 1`.  
`FUN_00426c00()` returns track-type. 13-case switch selects camera-path data table from `.rdata` addresses (0x005f7bd0–0x005f9368).  
Calls `FUN_00405540(table_ptr)`.  
Resets index globals `DAT_008964a0 = DAT_0089899c = DAT_00898998 = -1`.  
`DAT_00898994 = FUN_004a2c48()` (count).  
Init loop: `FUN_00441b30(entry@0x008964c0+i*0x36, table_row)` for each entry; records indices of entries with type-float 1.0/2.0/3.0.  
`FUN_00472650(0, 7.0f)`. `DAT_00897fc0 = count << 8`.  
Camera-path initializer for current track.

---

## D-6465 — 0x0046cb30 — 125B — C1
`undefined4 FUN_0046cb30(float *param_1, uint param_2)`  
Returns 0 if `param_2 > 15`.  
Reads `(&DAT_008815b4)[param_2 * 0x341]` flag.  
If flag==0: reads 3-float vec from `(&DAT_00881f50/54/58)[param_2 * 0x341]` into `param_1[0..2]`; returns 1.  
Else: `FUN_0047f1e0(param_1, param_2)` then scales all 3 by `_DAT_005cd8fc`; returns 1 or 0.  
Player 3D offset getter (per-player array stride 0x341).

---

## D-6466 — 0x00441820 — 356B — C1
`void FUN_00441820(int param_1, undefined4 *param_2, float *param_3)`  
Inits local vec `{0, -1.0f, 0}` (0xbf800000 at slot 1).  
`FUN_00409790()` then `FUN_004098a0()` → optional override table at `iVar1 + param_1*0xc`.  
If entry found:  
  - `*param_3 = entry[2]` if entry[2] != -1.0  
  - If entry[0] != -1.0: two `FUN_004c4d20` rotations (axes DAT_006146f0/fc, angles `_DAT_005ccad0 - entry[0]` and `entry[1] + _DAT_005cd09c`); two `FUN_004c3df0` matrix-transforms into `param_2`.  
Fallback: `FUN_00426cc0(param_1)` base orientation; `FUN_00426d00(param_1, 0/3)` control points; computes delta; `FUN_004c4d20` rotate by -25.0f (0xC1C80000); `FUN_004c3df0`.  
Camera path sample — outputs orientation matrix + scalar to caller.

---

## D-6467 — 0x00426bb0 — 5B — C1
`undefined4 FUN_00426bb0(void)`  
`return DAT_0066d6d8;`  
Returns path-count int from global 0x0066d6d8.

---

## D-6468 — 0x00441700 — 82B — C1
`void FUN_00441700(int param_1)`  
`local_8 = (*(param_1+0x6c) * *(param_1+0x58) / *(param_1+0x24)) * DAT_005cc950`  
`fVar1 = *(param_1+0x70) * *(param_1+0x58)`  
`local_4 = DAT_005cc950 * fVar1`; if `DAT_007f1a50 != 0`: `local_4 = fVar1` (no scale).  
`FUN_004c1c80(*(param_1+0x84), &local_8)`  
Camera FOV/aspect ratio computation and apply. Death-match camera setup.

---

## D-6469 — 0x004464c0 — 91B — C1
`void FUN_004464c0(undefined4 param_1)`  
Loop over `DAT_008964c0` array, stride 0x36, count `DAT_00898994`.  
Typed dispatch: entry[1]==0 → `FUN_00445aa0(entry, param_1)`; ==1 → `FUN_00441d40`; ==2 → `FUN_00442440`.  
Camera-path entry type dispatcher. Ghidra comment `[C1 2026-05-07]` was present but no hooks.csv row existed.

---

## D-6470 — 0x00442a60 — 530B — C1
`void FUN_00442a60(void)`  
Clears `DAT_008989b0/b4/b8/bc` (4-slot float array, one per player slot).  
`FUN_0040e180(&local_3c, &local_38)` → two nearest-competitor indices.  
`FUN_00408ad0` converts to float; applies track-wrap: if `local_34 > _DAT_005cc730 && local_30 <= _DAT_005ccd6c` subtract `_DAT_005cc568` from local_34; else-branch similarly.  
`FUN_0046cbb0` on each index → more info.  
Selects reference (closer competitor).  
Inner 4-player loop: `FUN_0040e370 + FUN_0046c7b0` active check; `FUN_0046d4a0(&local_2c, local_3c)` → position; XZ distance from reference (+0x30, +0x38); scaled by `_DAT_005cc9bc`; stored at `DAT_008989b0 + player*4`.  
Spectator mode per-player distance array computation.

---

## D-6471 — 0x004a3384 — 8B — C1
MSVC CRT `acos()` — `fpatan(SQRT((1-x²)))` with `__startOneArgErrorHandling` / `__math_exit` error paths. CRT external wrapper.

---

## D-6472 — 0x004a3620 — 78B — C1
MSVC CRT `atan()` — checks `DAT_008aa6a4 != 0 && MXCSR==0x1f80 && FPUControl==0x7f`; fast-path `FUN_004a7940`; else `FUN_004a7118 + FUN_004a3678`. CRT external wrapper.

---

## D-6474 — 0x00407600 — 28B — C1
`int FUN_00407600(int param_1)`  
`iVar1 = FUN_0047d150((&DAT_00639dc4)[param_1 * 0x3B]); return iVar1 + 0x30;`  
Player array at 0x00639dc4 stride 0x3B; passes element to FUN_0047d150; adds +0x30. Returns float* position pointer for mode-5 spectator.

---

## D-6475 — 0x0041f120 — 149B — C1
`undefined4 FUN_0041f120(int param_1, float *param_2, float *param_3)`  
`param_1 *= 0x2AC`; checks `*(DAT_0063dc7c + param_1)` (vehicle ptr at player slot).  
`iVar1 = *(*(DAT_0063dc7c+param_1)+4)` (vehicle handle).  
`local_c/8/4 = *(DAT_0063dc38/3c/40+param_1) - *(iVar1+0x40/44/48)` (camera target - vehicle position).  
`if param_3`: `*param_3 = local_c² + local_8² + local_4²` (squared distance).  
`if param_2`: `*param_2 = (float)FUN_004c3ac0(&local_c)` (distance via normalize).  
Returns 1 / 0 (has vehicle / no vehicle). Per-player loop setup.

---

## D-6476 — 0x004427c0 — 597B — C1 (__thiscall)
`void __thiscall FUN_004427c0(undefined4 param_1, int param_2)`  
Increments `DAT_00897fc0`; `uVar3 = DAT_00897fc0 & 0xFFF`.  
Mode dispatch on `DAT_007f0fd0`: 5→iVar5=0; 10→FUN_0046cbb0 check→0; 8→FUN_00417740(0)==-1→0; else path-selection:  
  - `FUN_0046c7b0(*(iVar2+0x20))` active check  
  - `iVar5 = *(DAT_005f9550 + (uVar3>>8)*4)` — frame-counter indexed path type  
  - type 2→`DAT_0089899c` or `DAT_008964a4` (0x100 frame bit); type 3→`DAT_00898998` if float@stride-0xd8 > threshold; else scan min-float in stride-0xd8 table across DAT_008964a0..DAT_0089649c range  
Hysteresis: `DAT_006831d4++`; if `iVar5 != DAT_006831cc` then `DAT_006831d0++`; switch only when `DAT_006831d0>15 && DAT_006831d4>60`.  
Writes selected entry: `DAT_00896498 = (&DAT_0089658c)[iVar6*0x36]`; `*(iVar2+0x58) = (&DAT_00896580)[iVar6*0x36]`.  
Calls `FUN_00441700(iVar2)`. Copies 16 dwords from `&DAT_0089650c + iVar6*0x36` to camera matrix.  
Main spectator camera selection with hysteresis filter.

---

## D-6477 — 0x00442a20 — 50B — C1
`void FUN_00442a20(int param_1)`  
`*(param_1+0x58) = DAT_00896580`  
`FUN_00441700(param_1)` (FOV/aspect setup)  
`puVar3 = &DAT_0089650c; puVar9 = *(*(param_1+0x84)+4)+0x10`  
16-dword copy from DAT_0089650c into camera object matrix.  
Camera state init with matrix copy.

---

## D-6478 — 0x004a3700 — 19B — C1
`void FUN_004a3700(void)`  
`FUN_004a7118((double)in_ST0); FUN_004a371d();`  
FPU float wrapper dispatching to FUN_004a371d. Called 9× in jitter loop.

---

## D-6479 — 0x004a37b0 — 19B — C1
`void FUN_004a37b0(void)`  
`FUN_004a7118((double)in_ST0); FUN_004a37cd();`  
FPU float wrapper dispatching to FUN_004a37cd. Time-based float getter.

---

## D-6480 — 0x004b4430 — 273B — C1
`void FUN_004b4430(float *param_1, float *param_2, float *param_3)`  
Forward: `param_1[8..10] = param_2[0..2] - param_1[0xC..0xE]`; normalize via `FUN_004c39b0`.  
Right: `param_1[0..2] = forward × param_3` (cross product); normalize.  
Up: `param_1[4..6] = forward × right` (cross product); normalize.  
`FUN_004c45f0(param_1)` finalize.  
Builds orthonormal camera frame from look-target and up-reference.

---

## D-6481 — 0x004b4cd0 — 56B — C1
`void FUN_004b4cd0(undefined4 param_1, undefined4 *param_2, undefined4 param_3)`  
`local_4 = 1`; copies 6 dwords from `param_2` to `local_1c`.  
`FUN_004b4c80(param_1, local_1c, param_3)`.  
Bezier path query wrapper — 6-float local copy then dispatch.

---

## D-6482 — 0x0045bfe0 — 5B — C1
`undefined4 FUN_0045bfe0(void)`  
`return DAT_0080332c;`  
Bezier locate — returns global 0x0080332c.

---

## D-6483 — 0x0045c350 — 302B — C1
`undefined4 FUN_0045c350(int param_1, undefined4 param_2)`  
Deep ptr chain: `iVar2 = *(*(*(param_1+0x3c)+0x10) + (*(ushort*)(*(*(param_1+0x34)+4)+6+*(param_1+0xc)*8) + *(ushort*)(*(*(param_1+0x34)+4)+0x80)) * 4)`.  
`iVar1 = *(iVar2+4)`.  
`FUN_0045c110(iVar2)` → if non-zero return 1 (early exit).  
`FUN_00426c00()` track-type dispatch:  
  - type 2 → `FUN_0044d5e0(param_2)` → return 1 if non-zero.  
  - type 0x21 && `iVar1 == -0x7FBFC0` → builds 3×3-ish identity matrix (1.0f diagonal, 0.0f off-diag) + `local_34 |= 0x20003`; calls `FUN_004c51a0(&local_40, param_2, 0)` then `FUN_00452140(&local_40, &local_40)`; return 1 if zero.  
Returns 0.  
Bezier segment interpolation with per-track-type branches.
