// ============================================================================
//  CONTIGUOUS-STACK-BUFFER fix APPLIED (2026-07-18, K8/K10 class): the decomp passed
//  `&local_XX` into vec3-reading/writing callees (be0/cb0 write the output point;
//  ec0/be0 read the direction; 5784a0 reads the new point), so the 7 consecutively-
//  offset local groups are ONE stack array each — declared gF/gE/gD/gA/gN/gO/gP[3]:
//    gF=[f8,f4,f0] gE=[ec,e8,e4] gD=[d0,cc,c8] gA=[ac,a8,a4] gN=[94,90,8c]
//    gO=[a0,9c,98] gP=[18,14,10]. (C4700 on the a8/a4 slots was the tell.) The ec0
//    scalar out-params (local_100/local_fc/local_c0/local_4c) stay SEPARATE floats.
// ============================================================================
//  RwpSolverCore15.cpp  —  B5e solver-island cluster K15
//  Ported function: FUN_00576880 (0x00576880, 4958 B) — the RWP-3.7 narrow-phase
//  SAT / edge-edge CLOSEST-FEATURE DRIVER between two convex polygons A/B: handles
//  the degenerate (0/1/2-vert) cases directly, runs the walking edge-edge search
//  otherwise, emits contacts via FUN_005784a0, and writes the manifold normal/basis.
//  Callees: FUN_005667c0 (K1 normalize) + FUN_00577be0/cb0/ec0/005784a0 (K14).
//  Own cluster; deps K1 + K14. Version anchor: MASHED.exe 2,846,720 B / SHA-256
//  BDCAE093…3C0E (verify before hook authoring).
//
//  x87-float throughout (FLD/FCOMP/FSQRT/FDIVR confirmed in disasm) → plain C float
//  under /arch:IA32 is bit-faithful. The census decomp already recovered every call's
//  args (leaf callees, unlike K13) and they match the verified K1/K14 signatures.
//
//  TRAPS handled:
//   * param_11 FLAG reinterpret — decomp `(uint)param_11[0x1f] & 0x20` / `[0x3f] & 0x20`;
//     disasm @0x577732 = `MOV DL,byte ptr [EDI+0x7c]; TEST 0x20` (integer byte flag read
//     of param_11's memory, NOT a float→uint convert). Ported as `*(uint*)&param_11[N]`.
//   * FUN_005667c0 returns float10 (x87 ST0). Declared float10 (NOT void) so MSVC pops
//     ST0 on the discarded result — else the x87 stack leaks (feedback_x87_st0_float10…).
//   * ABS→fabsf, SQRT→sqrtf; be0/ec0 float literals 0→0.0f, 0x3f800000→1.0f.
//   * `(a<b)==(a==b)` = Ghidra's `a>=b` idiom; kept verbatim (C evaluates identically).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>       // sqrtf (FSQRT), fabsf (ABS)

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef long double    float10;      // x87 80-bit return of FUN_005667c0 (K1)

#define _DAT_005cc320    (*(const float*)0x005cc320u)   //  1.0f
#define _DAT_005cc32c    (*(const float*)0x005cc32cu)   //  0.5f
#define _DAT_005cc33c    (*(const float*)0x005cc33cu)   // -1.0f
#define DAT_005d757c     (*(const float*)0x005d757cu)   //  0.0f
#define _DAT_005ce54c    (*(const float*)0x005ce54cu)   //  1.00060076e-06 (contact eps)
#define _DAT_005e4568    (*(const float*)0x005e4568u)   // -1.00060076e-06 (barycentric low bound)
#define _DAT_005e4564    (*(const float*)0x005e4564u)   //  1.00000095f     (barycentric high bound)
#define _DAT_005ceac0    (*(const float*)0x005ceac0u)   //  3.40282347e+38  (FLT_MAX early-out)
#define PTR_DAT_005ceabc (*(const float*)0x005ceabcu)   //  1.17549435e-38  (FLT_MIN)

// --- extern callees (K1 + K14) ----------------------------------------------
extern "C" float10 __cdecl FUN_005667c0(float *p1,float *p2);                                  // K1
extern "C" undefined4 __cdecl FUN_00577be0(float*,float*,float,float,float*,float*);           // K14
extern "C" undefined4 __cdecl FUN_00577cb0(float*,uint,float*,float*,float*,uint*);            // K14
extern "C" undefined4 __cdecl FUN_00577ec0(float*,float*,float,float,float*,float*,
                                           float,float,float*,float*,float*);                  // K14
extern "C" undefined4 __cdecl FUN_005784a0(uint*,float*,float*,float*,float);                  // K14

// ---------------------------------------------------------------------------
// 0x00576880  polygon↔polygon narrow-phase closest-feature driver
// ---------------------------------------------------------------------------
extern "C" uint __cdecl
FUN_00576880(float *param_1,uint param_2,float *param_3,uint param_4,float param_5,
             float param_6,float *param_7,float *param_8,uint *param_9,uint *param_10,
             float *param_11,float *param_12)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float *pfVar4;
  int iVar5;
  float *pfVar6;
  uint uVar7;
  uint uVar8;
  float *pfVar9;
  int iVar10;
  uint uVar11;
  uint local_104;
  float local_100;
  float local_fc;
  float local_e0;
  float local_dc;
  float local_d8;
  int local_d4;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  uint local_b4;
  uint local_b0;
  uint local_88;
  float local_84;
  float local_80;
  uint local_7c;
  uint local_78;
  uint local_74;
  float local_70;
  uint local_6c;
  float local_68;
  uint local_64;
  uint local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  uint local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  uint local_2c;
  uint local_28;
  float local_20;
  float local_1c;
  float gF[3];
  float gE[3];
  float gD[3];
  float gA[3];
  float gN[3];
  float gO[3];
  float gP[3];
  float local_4;

  local_b4 = param_2 + param_4;
  local_28 = 0;
  local_104 = 0;
  if (param_2 == 0) {
    if (param_4 != 0) {
      pfVar4 = param_3 + 1;
      pfVar6 = param_12;
      uVar11 = param_4;
      do {
        uVar11 = uVar11 - 1;
        fVar1 = param_6 - (param_7[2] * pfVar4[1] + pfVar4[-1] * *param_7 + *pfVar4 * param_7[1]);
        *pfVar6 = fVar1 * *param_7 + pfVar4[-1];
        *(float *)((int)param_12 + (-0x10 - (int)param_3) + (int)(pfVar4 + 4)) =
             fVar1 * param_7[1] + *pfVar4;
        pfVar6[2] = fVar1 * param_7[2] + pfVar4[1];
        pfVar4 = pfVar4 + 4;
        pfVar6 = pfVar6 + 4;
      } while (uVar11 != 0);
    }
    return param_4;
  }
  if (param_4 == 0) {
    if (param_2 != 0) {
      pfVar4 = param_1 + 1;
      pfVar6 = param_12;
      uVar11 = param_2;
      do {
        uVar11 = uVar11 - 1;
        fVar1 = param_6 - (param_7[2] * pfVar4[1] + pfVar4[-1] * *param_7 + *pfVar4 * param_7[1]);
        *pfVar6 = fVar1 * *param_7 + pfVar4[-1];
        *(float *)((int)param_12 + (-0x10 - (int)param_1) + (int)(pfVar4 + 4)) =
             fVar1 * param_7[1] + *pfVar4;
        pfVar6[2] = fVar1 * param_7[2] + pfVar4[1];
        pfVar4 = pfVar4 + 4;
        pfVar6 = pfVar6 + 4;
      } while (uVar11 != 0);
    }
    return param_2;
  }
  if (param_2 == 1) {
    *param_9 = 0;
    if (param_4 == 1) {
      gE[0] = *param_3;
      gE[1] = param_3[1];
      gE[2] = param_3[2];
      *param_10 = 0;
    }
    else if (param_4 == 2) {
      gF[0] = param_3[4] - *param_3;
      gF[1] = param_3[5] - param_3[1];
      gF[2] = param_3[6] - param_3[2];
      uVar11 = FUN_00577be0(param_3,&gF[0],0.0f,1.0f,param_1,&gE[0]);
      *param_10 = uVar11;
    }
    else {
      uVar11 = FUN_00577cb0(param_3,param_4,param_7,param_1,&gE[0],&local_b0);
      *param_10 = uVar11;
      if (uVar11 == 2) {
        if (local_b4 <= local_104) {
          return local_104;
        }
        pfVar4 = &gE[0];
LAB_00576bbe:
        FUN_005784a0(&local_104,param_12,pfVar4,param_7,param_6);
        return local_104;
      }
      if (uVar11 == 1) {
        pfVar4 = param_3 + ((int)((local_b0 - param_4) + 1) >> 0x1f & local_b0 + 1) * 4;
        param_3 = param_3 + local_b0 * 4;
        gF[0] = *pfVar4 - *param_3;
        gF[1] = pfVar4[1] - param_3[1];
        gF[2] = pfVar4[2] - param_3[2];
      }
    }
    local_c0 = 0.0f;
    local_e0 = *param_1 - gE[0];
    gA[0] = *param_1;
    gA[1] = param_1[1];
    local_dc = param_1[1] - gE[1];
    local_d8 = param_1[2] - gE[2];
    gA[2] = param_1[2];
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else if (param_4 == 1) {
    *param_10 = 0;
    if (param_2 == 2) {
      gD[0] = param_1[4] - *param_1;
      gD[1] = param_1[5] - param_1[1];
      gD[2] = param_1[6] - param_1[2];
      uVar11 = FUN_00577be0(param_1,&gD[0],0.0f,1.0f,param_3,&gA[0]);
      *param_9 = uVar11;
    }
    else {
      uVar11 = FUN_00577cb0(param_1,param_2,param_7,param_3,&gA[0],&local_b0);
      *param_9 = uVar11;
      if (uVar11 == 2) {
        if (local_b4 <= local_104) {
          return local_104;
        }
        pfVar4 = &gA[0];
        goto LAB_00576bbe;
      }
      if (uVar11 == 1) {
        pfVar4 = param_1 + ((int)((local_b0 - param_2) + 1) >> 0x1f & local_b0 + 1) * 4;
        param_1 = param_1 + local_b0 * 4;
        gD[0] = *pfVar4 - *param_1;
        gD[1] = pfVar4[1] - param_1[1];
        gD[2] = pfVar4[2] - param_1[2];
      }
    }
    local_e0 = gA[0] - *param_3;
    gE[0] = *param_3;
    gE[1] = param_3[1];
    gE[2] = param_3[2];
    local_c0 = 0.0f;
    local_dc = gA[1] - param_3[1];
    local_d8 = gA[2] - param_3[2];
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else if (local_b4 < 5) {
    gD[0] = param_1[4] - *param_1;
    gD[1] = param_1[5] - param_1[1];
    gD[2] = param_1[6] - param_1[2];
    gF[0] = param_3[4] - *param_3;
    gF[1] = param_3[5] - param_3[1];
    gF[2] = param_3[6] - param_3[2];
    FUN_00577ec0(param_3,&gF[0],0.0f,1.0f,param_1,&gD[0],0.0f,1.0f,&local_100,&local_fc,
                 &local_c0);
    gA[0] = local_fc * gD[0] + *param_1;
    gA[1] = local_fc * gD[1] + param_1[1];
    gA[2] = local_fc * gD[2] + param_1[2];
    gE[0] = local_100 * gF[0] + *param_3;
    gE[1] = local_100 * gF[1] + param_3[1];
    gE[2] = local_100 * gF[2] + param_3[2];
    local_e0 = gA[0] - gE[0];
    local_dc = gA[1] - gE[1];
    local_d8 = gA[2] - gE[2];
    *param_9 = (uint)(local_fc * local_fc != local_fc);
    *param_10 = (uint)(local_100 * local_100 != local_100);
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else {
    local_6c = local_b4 * 2 + 1;
    iVar10 = 0;
    local_40 = 0;
    local_70 = 0.0f;
    local_68 = 0.0f;
    local_7c = 0;
    local_88 = 0;
    local_78 = 1;
    local_60 = 1;
    local_d4 = 0;
    local_44 = 0.0f;
    local_48 = 0.0f;
    local_74 = 0;
    local_64 = 0;
    local_c4 = 3.4028235e+38f;
    local_c0 = 0.0f;
    local_b0 = 0;
    local_2c = 0;
    if (local_6c != 0) {
      do {
        local_6c = local_6c - 1;
        if (iVar10 != 2) {
          pfVar4 = param_1 + local_60 * 4;
          pfVar6 = param_1 + local_88 * 4;
          gO[0] = *pfVar4 - *pfVar6;
          gO[1] = pfVar4[1] - pfVar6[1];
          gO[2] = pfVar4[2] - pfVar6[2];
        }
        if (iVar10 != 1) {
          pfVar4 = param_3 + local_78 * 4;
          pfVar6 = param_3 + local_7c * 4;
          gN[0] = *pfVar4 - *pfVar6;
          gN[1] = pfVar4[1] - pfVar6[1];
          gN[2] = pfVar4[2] - pfVar6[2];
        }
        pfVar4 = param_1 + local_88 * 4;
        pfVar6 = param_3 + local_7c * 4;
        FUN_00577ec0(pfVar6,&gN[0],0.0f,1.0f,pfVar4,&gO[0],0.0f,1.0f,&local_fc,
                     &local_100,&local_4c);
        local_4 = local_100 * gO[2] + pfVar4[2];
        local_20 = local_fc * gN[1] + pfVar6[1];
        local_1c = local_fc * gN[2] + pfVar6[2];
        local_58 = (local_100 * gO[0] + *pfVar4) - (local_fc * gN[0] + *pfVar6);
        local_54 = (local_100 * gO[1] + pfVar4[1]) - local_20;
        local_50 = local_4 - local_1c;
        fVar1 = local_50 * local_50 + local_58 * local_58 + local_54 * local_54;
        if ((fVar1 < local_c4 + _DAT_005ce54c) &&
           ((local_c0 == DAT_005d757c || (local_4c != DAT_005d757c)))) {
          local_2c = (uint)(local_100 * local_100 != local_100);
          local_b0 = (uint)(local_fc * local_fc != local_fc);
          local_40 = local_7c;
          local_70 = local_100;
          local_28 = local_88;
          local_c0 = local_4c;
          local_68 = local_fc;
          local_e0 = local_58;
          local_dc = local_54;
          local_d8 = local_50;
          local_c4 = fVar1;
        }
        pfVar9 = param_3 + local_78 * 4;
        local_3c = param_1[local_60 * 4] - *pfVar6;
        local_38 = param_1[local_60 * 4 + 1] - pfVar6[1];
        local_34 = param_1[local_60 * 4 + 2] - pfVar6[2];
        local_5c = (gN[0] * param_7[1] - gN[1] * *param_7) * gO[2] +
                   (gN[1] * param_7[2] - gN[2] * param_7[1]) * gO[0] +
                   (gN[2] * *param_7 - gN[0] * param_7[2]) * gO[1];
        local_b8 = ((*pfVar9 - *pfVar4) * param_7[1] - (pfVar9[1] - pfVar4[1]) * *param_7) *
                   gO[2] +
                   ((pfVar9[1] - pfVar4[1]) * param_7[2] - (pfVar9[2] - pfVar4[2]) * param_7[1]) *
                   gO[0] +
                   ((pfVar9[2] - pfVar4[2]) * *param_7 - (*pfVar9 - *pfVar4) * param_7[2]) *
                   gO[1];
        local_bc = (local_3c * param_7[1] - local_38 * *param_7) * gN[2] +
                   (local_38 * param_7[2] - local_34 * param_7[1]) * gN[0] +
                   (local_34 * *param_7 - local_3c * param_7[2]) * gN[1];
        if ((iVar10 == 0) ||
           (((local_b8 * local_48 < DAT_005d757c == (local_b8 * local_48 == DAT_005d757c) &&
             (local_bc * local_44 < DAT_005d757c == (local_bc * local_44 == DAT_005d757c))) ||
            (fabsf(local_5c) < _DAT_005ce54c)))) {
LAB_00577479:
          if (local_d4 == 1) {
LAB_00577480:
            if ((local_bc < DAT_005d757c) && (DAT_005d757c < local_b8)) goto LAB_005774a4;
            goto LAB_005774eb;
          }
          if (local_d4 == 2) goto LAB_005774b5;
          if (local_d4 != 0) goto LAB_005774eb;
        }
        else {
          fVar2 = (pfVar4[1] - pfVar6[1]) * param_7[2] - (pfVar4[2] - pfVar6[2]) * param_7[1];
          local_30 = (pfVar4[2] - pfVar6[2]) * *param_7 - (*pfVar4 - *pfVar6) * param_7[2];
          fVar1 = (*pfVar4 - *pfVar6) * param_7[1] - (pfVar4[1] - pfVar6[1]) * *param_7;
          local_80 = (local_30 * gN[1] + fVar1 * gN[2] + fVar2 * gN[0]) *
                     (_DAT_005cc320 / local_5c);
          local_84 = (fVar1 * gO[2] + fVar2 * gO[0] + local_30 * gO[1]) *
                     (_DAT_005cc320 / local_5c);
          if ((((local_80 <= _DAT_005e4568) || (_DAT_005e4564 <= local_80)) ||
              (local_84 <= _DAT_005e4568)) || (_DAT_005e4564 <= local_84)) goto LAB_00577479;
          gP[0] = local_80 * gO[0] + *pfVar4;
          gP[1] = local_80 * gO[1] + pfVar4[1];
          gP[2] = local_80 * gO[2] + pfVar4[2];
          if (local_b4 <= local_104) {
            return local_104;
          }
          iVar10 = FUN_005784a0(&local_104,param_12,&gP[0],param_7,param_6);
          if (iVar10 != 0) {
            return local_104;
          }
          if (local_5c < DAT_005d757c) {
            local_d4 = 1;
            goto LAB_00577480;
          }
LAB_005774a4:
          local_d4 = 2;
LAB_005774b5:
          if (local_b8 < DAT_005d757c) {
            if (DAT_005d757c < local_bc) {
              local_d4 = 1;
            }
LAB_005774eb:
            if ((local_b8 < DAT_005d757c) && (local_bc < DAT_005d757c)) {
              local_d4 = 0;
            }
          }
        }
        if (local_5c <= DAT_005d757c) {
          iVar10 = 2;
          if (local_bc <= DAT_005d757c) goto LAB_0057755d;
        }
        else if (DAT_005d757c < local_b8) {
LAB_0057755d:
          iVar10 = 1;
        }
        else {
          iVar10 = 2;
        }
        if ((local_b8 == DAT_005d757c) && (local_bc == DAT_005d757c)) {
          iVar10 = 2 - (uint)(local_d4 != 1);
        }
        if (iVar10 == 1) {
          local_88 = local_60;
          uVar7 = (int)((local_60 - param_2) + 1) >> 0x1f & local_60 + 1;
          uVar11 = local_78;
          if (local_d4 == 1) {
            pfVar4 = param_1;
            uVar8 = local_60;
            local_60 = uVar7;
            if (local_b4 <= local_104) {
              return local_104;
            }
LAB_00577645:
            iVar5 = FUN_005784a0(&local_104,param_12,pfVar4 + uVar8 * 4,param_7,param_6);
            uVar11 = local_78;
            uVar7 = local_60;
            if (iVar5 != 0) {
              return local_104;
            }
          }
        }
        else {
          local_7c = local_78;
          uVar11 = (int)((local_78 - param_4) + 1) >> 0x1f & local_78 + 1;
          uVar7 = local_60;
          if (local_d4 == iVar10) {
            pfVar4 = param_3;
            uVar8 = local_78;
            local_78 = uVar11;
            if (local_b4 <= local_104) {
              return local_104;
            }
            goto LAB_00577645;
          }
        }
        local_60 = uVar7;
        local_78 = uVar11;
        local_44 = local_bc;
        local_48 = local_b8;
        local_64 = local_64 | local_b8 < DAT_005d757c != (local_b8 == DAT_005d757c);
        local_74 = local_74 | local_bc < DAT_005d757c != (local_bc == DAT_005d757c);
        if ((local_104 == 0) && (local_6c < local_b4)) {
          if (local_74 == 0) {
            uVar11 = param_2;
            pfVar4 = param_1;
            if (local_64 != 0) goto LAB_005778a4;
LAB_0057788b:
            uVar11 = param_4;
            pfVar4 = param_3;
            if (local_74 != 0) {
LAB_005778a4:
              if (uVar11 == 0) {
                return 0;
              }
              pfVar6 = pfVar4 + 1;
              pfVar9 = param_12;
              uVar7 = uVar11;
              do {
                uVar7 = uVar7 - 1;
                fVar1 = param_6 - (param_7[2] * pfVar6[1] +
                                  pfVar6[-1] * *param_7 + *pfVar6 * param_7[1]);
                *pfVar9 = fVar1 * *param_7 + pfVar6[-1];
                *(float *)((int)param_12 + (-0x10 - (int)pfVar4) + (int)(pfVar6 + 4)) =
                     fVar1 * param_7[1] + *pfVar6;
                pfVar9[2] = fVar1 * param_7[2] + pfVar6[1];
                pfVar6 = pfVar6 + 4;
                pfVar9 = pfVar9 + 4;
              } while (uVar7 != 0);
              return uVar11;
            }
          }
          else if (local_64 == 0) goto LAB_0057788b;
          if (_DAT_005ceac0 <= local_c4) {
            return 0;
          }
          pfVar4 = param_1 + ((int)((local_28 - param_2) + 1) >> 0x1f & local_28 + 1) * 4;
          param_1 = param_1 + local_28 * 4;
          gD[0] = *pfVar4 - *param_1;
          gD[1] = pfVar4[1] - param_1[1];
          gD[2] = pfVar4[2] - param_1[2];
          uVar11 = (int)((local_40 - param_4) + 1) >> 0x1f & local_40 + 1;
          gF[0] = param_3[uVar11 * 4] - param_3[local_40 * 4];
          pfVar4 = param_3 + local_40 * 4;
          gF[1] = param_3[uVar11 * 4 + 1] - pfVar4[1];
          gF[2] = param_3[uVar11 * 4 + 2] - pfVar4[2];
          gA[0] = local_70 * gD[0] + *param_1;
          gA[1] = local_70 * gD[1] + param_1[1];
          gA[2] = local_70 * gD[2] + param_1[2];
          gE[0] = local_68 * gF[0] + *pfVar4;
          gE[1] = local_68 * gF[1] + pfVar4[1];
          gE[2] = local_68 * gF[2] + pfVar4[2];
          *param_9 = local_2c;
          *param_10 = local_b0;
          goto LAB_005776fc;
        }
      } while (local_6c != 0);
      if (local_104 != 0) {
        return local_104;
      }
    }
  }
LAB_005776fc:
  uVar11 = local_b4;
  if ((((DAT_005d757c <= *param_8) && (PTR_DAT_005ceabc < local_c4)) &&
      ((*(uint*)&param_11[0x1f] & 0x20) == 0)) && ((*(uint*)&param_11[0x3f] & 0x20) == 0)) {
    fVar1 = _DAT_005cc320 / sqrtf(local_c4);
    *param_8 = local_c4 * fVar1;
    *param_7 = local_e0 * fVar1;
    param_7[1] = fVar1 * local_dc;
    param_7[2] = fVar1 * local_d8;
    param_6 = (param_5 + *param_8) * _DAT_005cc32c +
              fVar1 * local_d8 * gE[2] +
              local_e0 * fVar1 * gE[0] + fVar1 * local_dc * gE[1];
  }
  if (local_104 < local_b4) {
    FUN_005784a0(&local_104,param_12,&gE[0],param_7,param_6);
  }
  if ((local_c0 <= DAT_005d757c) || (uVar11 <= local_104)) {
    if (*param_9 == 0) {
      *param_11 = *param_7;
      param_11[1] = param_7[1];
      param_11[2] = param_7[2];
      goto LAB_00577ad6;
    }
  }
  else {
    gE[0] = local_c0 * gF[0] + gE[0];
    gE[1] = local_c0 * gF[1] + gE[1];
    gE[2] = local_c0 * gF[2] + gE[2];
    FUN_005784a0(&local_104,param_12,&gE[0],param_7,param_6);
    *param_10 = 1;
    *param_9 = 1;
  }
  fVar1 = param_7[2] * gD[1] - param_7[1] * gD[2];
  fVar3 = gD[2] * *param_7 - param_7[2] * gD[0];
  fVar2 = param_7[1] * gD[0] - gD[1] * *param_7;
  *param_11 = fVar3 * gD[2] - fVar2 * gD[1];
  param_11[1] = fVar2 * gD[0] - fVar1 * gD[2];
  param_11[2] = fVar1 * gD[1] - fVar3 * gD[0];
  FUN_005667c0(param_11,param_11);
LAB_00577ad6:
  fVar1 = *param_11 * _DAT_005cc33c;
  *param_11 = fVar1;
  fVar3 = param_11[1] * _DAT_005cc33c;
  param_11[1] = fVar3;
  fVar2 = param_11[2] * _DAT_005cc33c;
  param_11[2] = fVar2;
  param_11[3] = fVar2 * gA[2] + fVar1 * gA[0] + fVar3 * gA[1];
  if (*param_10 == 0) {
    param_11[0x20] = *param_7;
    param_11[0x21] = param_7[1];
    param_11[0x22] = param_7[2];
  }
  else {
    pfVar4 = param_11 + 0x20;
    fVar1 = param_7[2] * gF[1] - param_7[1] * gF[2];
    fVar3 = gF[2] * *param_7 - param_7[2] * gF[0];
    fVar2 = param_7[1] * gF[0] - gF[1] * *param_7;
    *pfVar4 = fVar3 * gF[2] - fVar2 * gF[1];
    param_11[0x21] = fVar2 * gF[0] - fVar1 * gF[2];
    param_11[0x22] = fVar1 * gF[1] - fVar3 * gF[0];
    FUN_005667c0(pfVar4,pfVar4);
  }
  param_11[0x23] = param_11[0x22] * gE[2] + param_11[0x20] * gE[0] + param_11[0x21] * gE[1];
  return local_104;
}

// --- gta-reversed-style hook registration — CLUSTER 15. ---
RH_ScopedInstall(FUN_00576880, 0x00576880);

}  // namespace Collision
}  // namespace mashed_re
