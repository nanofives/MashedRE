// ============================================================================
//  RwpSolverCore14.cpp  —  B5e solver-island cluster K14
//  4 verbatim x87-float leaves (RWP-3.7 segment/polygon closest-feature helpers):
//    FUN_00577be0 (0x00577be0, 196 B)  — line-param clamp: t=dot(dir,p5-p1)/|dir|² clamped [t3,t4]
//    FUN_00577cb0 (0x00577cb0, 513 B)  — polygon edge probe; tail-calls FUN_00577be0
//    FUN_00577ec0 (0x00577ec0, 1497 B) — segment↔segment closest-approach clamp (capsule core)
//    FUN_005784a0 (0x005784a0, 366 B)  — contact-array dedup/insert
//  Sole external dep: none (cb0→be0 is intra-cluster). Version anchor: MASHED.exe
//  2,846,720 B / SHA-256 BDCAE093…3C0E (verify before hook authoring).
//
//  All four are x87 (FLD/FMUL/FSTP/FDIV/FCOMP confirmed in disasm) — plain C float
//  compiles to x87 under the project's /arch:IA32 build, so this is bit-faithful.
//  Ghidra's `(a<b)==(a==b)` idiom = `a>=b`; kept verbatim (C evaluates identically).
//
//  TRAP (K10/K12 class) — FUN_00577ec0 float-in-POINTER-var reuse: the decomp reuses
//    `param_1` (a float*) to hold a selected FLOAT (`param_1=(float*)param_8` then
//    `(float)param_1 * fVar5`). Disasm @0x578005+ is x87 (FCOMP 0x005d757c; FLD [ESP+0x38])
//    = a float value select, NOT a pointer load. Ported as a distinct `float p1sel` scratch
//    (param_1-as-pointer is only used before the reuse, so no lifetime overlap).
// ============================================================================
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;

#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005cd03c  (*(const float*)0x005cd03cu)   // 9.99999975e-05  (parallel-segment det eps)
#define _DAT_005ce54c  (*(const float*)0x005ce54cu)   // 1.00060076e-06  (contact-dedup dist² eps)

// ---------------------------------------------------------------------------
// 0x00577be0  line-parameter clamp: out = p1 + clamp(dot(dir,p5-p1)/|dir|², [t3,t4]) * dir
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00577be0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6)
{
  float fVar1;
  float fVar2;
  undefined4 uVar3;

  fVar1 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar2 = param_2[2] * (param_5[2] - param_1[2]) +
          (*param_5 - *param_1) * *param_2 + param_2[1] * (param_5[1] - param_1[1]);
  if (fVar1 * param_3 < fVar2) {
    if (fVar1 * param_4 < fVar2 == (fVar1 * param_4 == fVar2)) {
      param_3 = fVar2 / fVar1;
      uVar3 = 1;
    }
    else {
      uVar3 = 0;
      param_3 = param_4;
    }
  }
  else {
    uVar3 = 0;
  }
  *param_6 = param_3 * *param_2 + *param_1;
  param_6[1] = param_3 * param_2[1] + param_1[1];
  param_6[2] = param_3 * param_2[2] + param_1[2];
  return uVar3;
}

// ---------------------------------------------------------------------------
// 0x00577cb0  polygon edge probe: pick the edge maximizing a scaled cross-metric,
//             then clamp along it via FUN_00577be0; else project onto probe plane.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00577cb0(float *param_1,uint param_2,float *param_3,float *param_4,float *param_5,uint *param_6)
{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float *pfVar6;
  undefined4 uVar7;
  float *pfVar8;
  uint uVar9;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  local_14 = -3.4028235e+38f;
  local_10 = 1.0f;
  *param_6 = param_2;
  if (param_2 != 0) {
    pfVar8 = param_1 + 2;
    uVar9 = 0;
    do {
      uVar1 = uVar9 + 1;
      pfVar6 = param_1 + ((int)((1 - param_2) + uVar9) >> 0x1f & uVar1) * 4;
      fVar2 = *pfVar6 - pfVar8[-2];
      fVar3 = pfVar6[1] - pfVar8[-1];
      fVar4 = pfVar6[2] - *pfVar8;
      fVar5 = ((param_4[1] - pfVar8[-1]) * *param_3 - (*param_4 - pfVar8[-2]) * param_3[1]) * fVar4
              + ((param_4[2] - *pfVar8) * param_3[1] - (param_4[1] - pfVar8[-1]) * param_3[2]) *
                fVar2 + ((*param_4 - pfVar8[-2]) * param_3[2] - (param_4[2] - *pfVar8) * *param_3) *
                        fVar3;
      if (DAT_005d757c <= fVar5) {
        fVar2 = fVar4 * fVar4 + fVar2 * fVar2 + fVar3 * fVar3;
        if (fVar2 * local_14 < local_10 * fVar5 * fVar5) {
          *param_6 = uVar9;
          local_14 = fVar5 * fVar5;
          local_10 = fVar2;
        }
      }
      pfVar8 = pfVar8 + 4;
      uVar9 = uVar1;
    } while (uVar1 < param_2);
    uVar9 = *param_6;
    if (uVar9 != param_2) {
      pfVar8 = param_1 + ((int)((uVar9 - param_2) + 1) >> 0x1f & uVar9 + 1) * 4;
      param_1 = param_1 + uVar9 * 4;
      local_c = *pfVar8 - *param_1;
      local_8 = pfVar8[1] - param_1[1];
      local_4 = pfVar8[2] - param_1[2];
      uVar7 = FUN_00577be0(param_1,&local_c,0.0f,1.0f,param_4,param_5);
      return uVar7;
    }
  }
  fVar2 = *param_1 - *param_4;
  fVar4 = param_1[1] - param_4[1];
  fVar3 = param_1[2] - param_4[2];
  *param_5 = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * *param_3 + *param_4;
  param_5[1] = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * param_3[1] +
               param_4[1];
  param_5[2] = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * param_3[2] +
               param_4[2];
  return 2;
}

// ---------------------------------------------------------------------------
// 0x00577ec0  segment↔segment closest-approach with per-segment [min,max] clamping
//             (RWP capsule/edge-edge core). Outputs param_9=s, param_10=t, param_11=overlap.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00577ec0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6,
             float param_7,float param_8,float *param_9,float *param_10,float *param_11)
{
  bool bVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float p1sel;                         // TRAP: decomp's reused `param_1` float scratch

  fVar2 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar3 = param_6[2] * param_6[2] + param_6[1] * param_6[1] + *param_6 * *param_6;
  fVar4 = *param_5 - *param_1;
  fVar5 = param_5[1] - param_1[1];
  fVar6 = param_5[2] - param_1[2];
  if (fVar2 == DAT_005d757c) {
    *param_9 = param_3;
    bVar1 = fVar3 == DAT_005d757c;
    *param_11 = 0.0f;
    if (bVar1) {
      *param_10 = param_7;
      return 0;
    }
    fVar2 = -(param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5);
    if (fVar3 * param_7 < fVar2) {
      if (fVar3 * param_8 < fVar2 == (fVar3 * param_8 == fVar2)) {
        *param_10 = fVar2 / fVar3;
        return 0;
      }
      *param_10 = param_8;
      return 0;
    }
    *param_10 = param_7;
    return 0;
  }
  if (fVar3 != DAT_005d757c) {
    fVar7 = param_2[2] * param_6[2] + *param_6 * *param_2 + param_6[1] * param_2[1];
    fVar9 = fVar3 * fVar2;
    fVar10 = fVar9 - fVar7 * fVar7;
    fVar8 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
    fVar4 = param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5;
    if (fVar10 <= _DAT_005cd03c * fVar9) {
      fVar9 = _DAT_005cc320 / fVar9;
      fVar3 = fVar9 * fVar3;
      fVar5 = fVar7 * fVar3;
      fVar3 = fVar3 * fVar8;
      if (fVar5 <= DAT_005d757c) {
        p1sel = param_8;
        param_8 = param_7;
      }
      else {
        p1sel = param_7;
      }
      fVar6 = p1sel * fVar5 + fVar3;
      fVar3 = fVar5 * param_8 + fVar3;
      if (fVar3 < param_3 == (fVar3 == param_3)) {
        if (fVar6 < param_4) {
          if (fVar6 < param_3) {
            fVar6 = param_3;
          }
          *param_9 = fVar6;
          if (param_4 < fVar3) {
            fVar3 = param_4;
          }
          *param_11 = fVar3 - fVar6;
          *param_10 = fVar9 * fVar2 * (fVar7 * *param_9 - fVar4);
          return 0;
        }
        *param_9 = param_4;
        *param_11 = 0.0f;
        *param_10 = p1sel;
        return 0;
      }
      *param_9 = param_3;
      *param_11 = 0.0f;
      *param_10 = param_8;
      return 0;
    }
    fVar5 = fVar8 * fVar3 - fVar4 * fVar7;
    fVar6 = param_3;
    if ((fVar10 * param_3 <= fVar5) && (fVar6 = param_4, fVar5 <= fVar10 * param_4)) {
      fVar3 = fVar8 * fVar7 - fVar4 * fVar2;
      if ((fVar3 < fVar10 * param_7) || (param_7 = param_8, fVar10 * param_8 < fVar3)) {
        *param_10 = param_7;
        fVar8 = fVar7 * *param_10 + fVar8;
        if (fVar8 < fVar2 * param_3) {
          *param_9 = param_3;
          *param_11 = 0.0f;
          return 1;
        }
        param_10 = param_9;
        if (fVar8 <= fVar2 * param_4) {
          *param_9 = fVar8 / fVar2;
          *param_11 = 0.0f;
          return 1;
        }
      }
      else {
        param_4 = _DAT_005cc320 / fVar10;
        *param_9 = fVar5 * param_4;
        param_4 = param_4 * fVar3;
      }
      *param_10 = param_4;
      *param_11 = 0.0f;
      return 1;
    }
    *param_9 = fVar6;
    fVar4 = fVar7 * *param_9 - fVar4;
    if ((fVar3 * param_7 <= fVar4) && (param_7 = param_8, fVar4 <= fVar3 * param_8)) {
      *param_10 = fVar4 / fVar3;
      *param_11 = 0.0f;
      return 1;
    }
    *param_10 = param_7;
    fVar8 = fVar7 * *param_10 + fVar8;
    if (fVar8 < fVar2 * param_3) {
      *param_9 = param_3;
      *param_11 = 0.0f;
      return 1;
    }
    if (fVar8 <= fVar2 * param_4) {
      *param_9 = fVar8 / fVar2;
      *param_11 = 0.0f;
      return 1;
    }
    *param_9 = param_4;
    *param_11 = 0.0f;
    return 1;
  }
  *param_10 = param_7;
  *param_11 = 0.0f;
  fVar3 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
  if (fVar3 <= fVar2 * param_3) {
    *param_9 = param_3;
    return 0;
  }
  if (fVar2 * param_4 < fVar3 == (fVar2 * param_4 == fVar3)) {
    *param_9 = fVar3 / fVar2;
    return 0;
  }
  *param_9 = param_4;
  return 0;
}

// ---------------------------------------------------------------------------
// 0x005784a0  contact-array dedup/insert: reject a new point within eps of the
//             last or first contact; else append and bump the count.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_005784a0(uint *param_1,float *param_2,float *param_3,float *param_4,float param_5)
{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;

  uVar1 = *param_1;
  if (uVar1 != 0) {
    fVar3 = -(param_4[2] * (param_3[2] - param_2[uVar1 * 4 + -2]) +
             (*param_3 - param_2[uVar1 * 4 + -4]) * *param_4 +
             param_4[1] * (param_3[1] - param_2[uVar1 * 4 + -3]));
    fVar2 = fVar3 * *param_4 + (*param_3 - param_2[uVar1 * 4 + -4]);
    fVar4 = fVar3 * param_4[1] + (param_3[1] - param_2[uVar1 * 4 + -3]);
    fVar3 = fVar3 * param_4[2] + (param_3[2] - param_2[uVar1 * 4 + -2]);
    if (fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < _DAT_005ce54c) {
      return 0;
    }
  }
  if ((1 < uVar1) &&
     (fVar3 = -(param_4[2] * (param_3[2] - param_2[2]) +
               (*param_3 - *param_2) * *param_4 + param_4[1] * (param_3[1] - param_2[1])),
     fVar2 = fVar3 * *param_4 + (*param_3 - *param_2),
     fVar4 = fVar3 * param_4[1] + (param_3[1] - param_2[1]),
     fVar3 = fVar3 * param_4[2] + (param_3[2] - param_2[2]),
     fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < _DAT_005ce54c)) {
    return 1;
  }
  param_5 = param_5 - (param_3[2] * param_4[2] + *param_3 * *param_4 + param_3[1] * param_4[1]);
  param_2[uVar1 * 4] = param_5 * *param_4 + *param_3;
  param_2[*param_1 * 4 + 1] = param_5 * param_4[1] + param_3[1];
  param_2[*param_1 * 4 + 2] = param_5 * param_4[2] + param_3[2];
  *param_1 = *param_1 + 1;
  return 0;
}

// --- gta-reversed-style hook registration — CLUSTER 14. ---
RH_ScopedInstall(FUN_00577be0, 0x00577be0);
RH_ScopedInstall(FUN_00577cb0, 0x00577cb0);
RH_ScopedInstall(FUN_00577ec0, 0x00577ec0);
RH_ScopedInstall(FUN_005784a0, 0x005784a0);

}  // namespace Collision
}  // namespace mashed_re
