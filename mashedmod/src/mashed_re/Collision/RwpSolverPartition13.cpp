// ============================================================================
//  RwpSolverPartition13.cpp  —  B5e solver-island cluster K13
//  Ported function: FUN_00560260  (0x00560260, 3536 B) — the top-level
//  PER-ISLAND SOLVE-STEP ORCHESTRATOR of the RWP-3.7 solver island.
//  Sole caller: FUN_00561040. Version anchor: MASHED.exe 2,846,720 B /
//  SHA-256 BDCAE093…3C0E (verify before hook authoring).
//
//  METHODOLOGY (this port):
//   * The 16 direct callees carry `unknown` calling-convention, so the stock
//     decomp printed every call as `FUN_xxx()` with the args modelled as
//     pfStack/puStack/iStack stores (the [[feedback-ghidra-prebranch-args]]
//     trap at scale). To recover the args, all 16 callee signatures were set
//     __cdecl on a writable pool0 clone and FUN_00560260 was re-decompiled;
//     the 14 DIRECT calls below are transcribed verbatim from that clean
//     re-decomp (arg order = C param order), each cross-checked against the
//     raw decomp's stack-store offsets. Full reconstruction + citations:
//     re/analysis/b5e/K13_PORT_RECON_2026-07-18.md.
//   * Body math is transcribed from the RAW decomp (undefined4* param_9 /
//     param_3 with explicit reinterpret-to-float), NOT the sig-retyped
//     re-decomp — the retype turned raw dt-bit copies into numeric converts
//     (e.g. `param_9[0x16]==2` mis-rendered as `==2.8026e-45`, and
//     `1.0/(float)param_3[0x49]` where [0x49] holds dt FLOAT bits). See TRAP 1/2.
//
//  TRAP 1 — integrator selector is INTEGER: raw `if (param_9[0x16]==2)` picks
//    the SSE twin FUN_0056dd40, else the x87 FUN_0056d3f0. Disasm @0x560835 =
//    integer MOVs, not FLD/FST. param_9 is a MIXED struct read via int*; the
//    float fields use explicit reinterpret (F9()/F3() macros below).
//  TRAP 2 — *param_9 and param_3[0x49] hold the dt FLOAT bits (raw-copied at
//    0x560235); reinterpret, do not convert.
//
//  [UNCERTAIN U-K13-KV] — the 3 caller-supplied `code*` callbacks (param_11 ×1,
//    param_13 ×2 = the KV1..KV3 scene velocity-integrate targets, UNPORTED →
//    resolve to ORIGINAL code on the .asi) each build a LARGE by-value outgoing
//    stack frame (param_11: 53 words; param_13: 4 ptr args + ~50-word tail) that
//    the decompiler only partly types. Their per-word VALUES are enumerated in
//    K13_PORT_RECON (from the raw decomp), but the exact by-offset ORDER of the
//    two param_13 tails is not yet disasm-verified. Per owner decision
//    (2026-07-18) these invocations are STUBBED here (real side-effect writes
//    kept; the indirect CALL made with no reconstructed frame) and PINNED via a
//    live Frida stack read at [ESP..] on each CALL in the original at race time.
//    Until pinned, the .asi K13 hook must NOT be trusted past a callback.
//    Unported direct callees FUN_005675d0/FUN_0056c310/FUN_0056bdf0 also resolve
//    to ORIGINAL on the .asi (fine for the A/B).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>       // sqrtf — x87 FSQRT floor (penetration-resolve loop)

namespace mashed_re {
namespace Collision {

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned int   undefined4;

#define _DAT_005cc320  (*(const float*)0x005cc320u)   //  1.0f
#define _DAT_005cc33c  (*(const float*)0x005cc33cu)   // -1.0f
#define DAT_005d757c   (*(const float*)0x005d757cu)   //  0.0f
#define _DAT_005e5258  (*(const float*)0x005e5258u)   // -1.17549435e-38 (-FLT_MIN), penetration thresh
#define PTR_DAT_005ceabc (*(const float*)0x005ceabcu) //  1.17549435e-38 (+FLT_MIN)
#define _DAT_00913284  (*(int*)0x00913284u)           // game-global int (written 0/1 around param_13)

// reinterpret helpers — raw decomp read undefined4 fields as float (bit-cast, NOT numeric convert)
#define F9(i)  (*(float*)&param_9[(i)])
#define F3(i)  (*(float*)&param_3[(i)])

// --- extern callees (earlier clusters + hooked leaf + 3 unported) ------------
extern "C" void __cdecl FUN_00567c00(int p1,int p2);                                        // K9
extern "C" void __cdecl FUN_0056efc0(int p1,int p2);                                        // K9
extern "C" void __cdecl FUN_0056f020(int p1);                                               // K9
extern "C" void __cdecl FUN_0056f350(int p1,float *p2,float p3);                            // K10
extern "C" void __cdecl FUN_00570090(int p1,int p2,float p3,float p4);                      // K12
extern "C" void __cdecl FUN_005667c0(float *p1,float *p2);                                  // K1
extern "C" void __cdecl FUN_005601f0(int *p1,int p2,int p3,int p4,int *p5);                 // leaf (hooked)
extern "C" void __cdecl FUN_005675d0(int p1,int p2,int p3,int p4);                          // UNPORTED
extern "C" void __cdecl FUN_0056bdf0(int p1,int p2,int p3,int p4,int p5);                   // UNPORTED
extern "C" void __cdecl FUN_0056c310(int p1,int p2,int p3,int p4,int p5,int p6,int p7,
                                     int p8,int p9,int p10,int p11,float p12);              // UNPORTED
extern "C" void __cdecl FUN_0056c580(int p1,int p2,int p3,int p4,int p5,int p6,int *p7,int p8); // K7
extern "C" void __cdecl FUN_0056d070(int *p1,int p2,uint p3,int p4,int p5,int p6,int p7,int p8,
                                     int *p9,int p10,int p11);                              // K7
extern "C" void __cdecl FUN_0056caa0(int p1,int p2,int *p3,int *p4,int *p5,int p6,int *p7,int p8,
                                     uint p9,int p10,int p11,int p12,int p13,int p14,int p15,
                                     int p16,int p17,int p18);                              // K7
extern "C" void __cdecl FUN_0056e680(int p1,int p2,int *p3,int *p4,int p5,int p6,int p7,int *p8,
                                     int p9,int p10,int p11,int p12,int p13,int p14,float p15); // K8
extern "C" void __cdecl FUN_0056d3f0(int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,
                                     int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,
                                     int p18,int p19,int p20,int p21,int p22,int p23,int p24,int p25,
                                     int p26,int p27,int p28,float p29);                    // K8 (x87)
extern "C" void __cdecl FUN_0056dd40(int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,
                                     int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,
                                     int p18,int p19,int p20,int p21,int p22,int p23,int p24,int p25,
                                     int p26,int p27,int p28,float p29);                    // K8 (SSE twin)

// ---------------------------------------------------------------------------
// 0x00560260  per-island solve-step orchestrator
// ---------------------------------------------------------------------------
extern "C" void __cdecl
FUN_00560260(int *param_1,int *param_2,undefined4 *param_3,int *param_4,int *param_5,
             int param_6,int *param_7,int param_8,undefined4 *param_9,float param_10,
             void *param_11,undefined4 param_12,void *param_13)
{
  undefined4 *puVar1;
  int iVar2;
  float fVar3, fVar4, fVar5;
  undefined4 uVar6;
  int iVar7;
  int *piVar8;
  float fVar9, fVar10, fVar11;
  int *piVar12;
  float *pfVar13;
  undefined4 *puVar14;
  int iVar15;
  float *pfVar16;
  int iVar17;
  float *pfVar18;
  float *pfVar19;
  uint uVar20;
  float invDt;
  int local_38;
  uint local_34;
  int local_2c;
  int local_28;
  int local_24;
  float local_18, local_14, local_10, local_c, local_8, local_4;

  puVar14 = param_3;
  piVar8 = param_1;
  iVar17 = 0;
  local_24 = 0;
  iVar15 = *(int *)(param_8 + 4);
  if (iVar15 != 0) {                                            // 0x00560271
    piVar12 = (int *)(*(int *)(param_8 + 0x10) + 0x14);
    do {
      if ((*(byte *)(piVar12 + 1) & 3) == 0) {
        iVar17 = iVar17 + *piVar12;
      }
      piVar12 = piVar12 + 10;
      iVar15 = iVar15 + -1;
      local_24 = iVar17;
    } while (iVar15 != 0);
  }
  FUN_00567c00((int)param_3, (int)param_4);                     // 0x0056029a  (K9)
  uVar6 = *param_9;                                             // dt FLOAT bits (TRAP 2)
  piVar8[0x17] = 0;
  param_2[1] = 0;
  local_2c = 0;
  if (*(int *)(param_8 + 4) != 0) {
    local_28 = 0;
    do {
      piVar12 = (int *)(*(int *)(param_8 + 0x10) + local_28);
      if ((*(byte *)(piVar12 + 6) & 3) == 0) {
        param_1 = (int *)0x0;                                   // reused as index (raw)
        if (piVar12[3] != 0) {
          do {
            local_34 = 0;
            iVar15 = *(int *)(*piVar12 + (int)param_1 * 4);
            if (*(short *)(iVar15 + 0xc) != 0) {
              local_38 = 0;
              do {
                iVar17 = *(int *)(local_38 + 4 + *(int *)(iVar15 + 0x10));
                iVar7 = *(int *)(iVar17 + 4);
                pfVar13 = (float *)(piVar8[0xc] + iVar7 * 0x20);
                pfVar18 = (float *)(piVar8[2] + iVar7 * 0x20);
                pfVar16 = (float *)(iVar7 * 0x30 + piVar8[8]);
                *(short *)(iVar17 + 0x20) = (short)piVar8[0x17];
                *(int *)(piVar8[0x16] + piVar8[0x17] * 4) = iVar7;
                piVar8[0x17] = piVar8[0x17] + 1;
                *pfVar13     = pfVar16[3] * F9(0x13) + *pfVar13;
                pfVar13[1]   = pfVar16[3] * F9(0x14) + pfVar13[1];
                pfVar13[2]   = pfVar16[3] * F9(0x15) + pfVar13[2];
                *pfVar13     = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * *pfVar18   + *pfVar13;
                pfVar13[1]   = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * pfVar18[1] + pfVar13[1];
                pfVar13[2]   = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * pfVar18[2] + pfVar13[2];
                if ((*(byte *)(piVar8[6] + iVar7 * 4) & 2) == 0) {
                  pfVar13[4] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[4] + pfVar13[4];
                  pfVar13[5] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[5] + pfVar13[5];
                  pfVar13[6] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[6] + pfVar13[6];
                }
                else {
                  pfVar19 = (float *)(iVar7 * 0x40 + *piVar8);
                  fVar3 = pfVar19[2] * pfVar18[6] + *pfVar19 * pfVar18[4] + pfVar19[1] * pfVar18[5];
                  fVar4 = pfVar18[6] * pfVar19[6] +
                          pfVar18[4] * pfVar19[4] + pfVar18[5] * pfVar19[5];
                  local_10 = pfVar18[6] * pfVar19[10] +
                             pfVar19[8] * pfVar18[4] + pfVar18[5] * pfVar19[9];
                  local_4 = pfVar16[2] * fVar3;
                  local_c = pfVar16[4] * fVar4 + fVar3 * *pfVar16;
                  local_8 = pfVar16[5] * fVar4 + pfVar16[1] * fVar3;
                  fVar11 = pfVar16[8] * local_10 + local_c;
                  fVar10 = pfVar16[9] * local_10 + local_8;
                  fVar9  = pfVar16[10] * local_10 + pfVar16[6] * fVar4 + local_4;
                  local_18 = fVar11 * *pfVar19 + fVar10 * pfVar19[4] + fVar9 * pfVar19[8];
                  local_14 = fVar11 * pfVar19[1] + fVar9 * pfVar19[9] + fVar10 * pfVar19[5];
                  fVar3 = pfVar19[10];
                  fVar4 = pfVar19[2];
                  fVar5 = pfVar19[6];
                  pfVar13[4] = -*(float *)(iVar17 + 0x18) * local_18 + pfVar13[4];
                  pfVar13[5] = -*(float *)(iVar17 + 0x18) * local_14 + pfVar13[5];
                  pfVar13[6] = -*(float *)(iVar17 + 0x18) *
                               (fVar10 * fVar5 + fVar11 * fVar4 + fVar9 * fVar3) + pfVar13[6];
                }
                local_34 = local_34 + 1;
                local_38 = local_38 + 0xc;
              } while (local_34 < *(ushort *)(iVar15 + 0xc));
            }
            param_1 = (int *)((int)param_1 + 1);
          } while ((uint)param_1 < (uint)piVar12[3]);
        }
        uVar20 = 0;
        FUN_0056efc0((int)param_3, piVar12[5]);                 // 0x00560556  (K9)
        if (piVar12[4] != 0) {
          do {
            *(int *)(param_4[0xf] + param_4[0x10] * 4) = *(int *)(piVar12[2] + uVar20 * 4) + 0xc;
            param_4[0x10] = param_4[0x10] + 1;
            FUN_00570090((int)param_3, *(int *)(piVar12[2] + uVar20 * 4),
                         0.0f, *(float *)&uVar6);               // 0x00560590  (K12) joint loop 1
            uVar20 = uVar20 + 1;
          } while (uVar20 < (uint)piVar12[4]);
        }
        param_1 = (int *)0x0;
        if (piVar12[3] != 0) {
          do {
            uVar20 = 0;
            iVar15 = *(int *)(*piVar12 + (int)param_1 * 4);
            if (*(short *)(iVar15 + 0x14) != 0) {
              do {
                *(int *)(param_4[0xf] + param_4[0x10] * 4) =
                     *(int *)(*(int *)(iVar15 + 0x18) + uVar20 * 4) + 0xc;
                param_4[0x10] = param_4[0x10] + 1;
                FUN_00570090((int)param_3, *(int *)(*(int *)(iVar15 + 0x18) + uVar20 * 4),
                             0.0f, *(float *)&uVar6);           // 0x005605ef  (K12) joint loop 2
                uVar20 = uVar20 + 1;
              } while (uVar20 < *(ushort *)(iVar15 + 0x14));
            }
            param_1 = (int *)((int)param_1 + 1);
          } while ((uint)param_1 < (uint)piVar12[3]);
        }
        for (iVar15 = piVar12[1]; iVar15 != 0; iVar15 = *(int *)(iVar15 + 0xdc)) {
          if (*(int *)(iVar15 + 0xac) != 0) {
            *(undefined4 *)(*param_2 + param_2[1] * 4) = param_3[0x42];
            param_2[1] = param_2[1] + 1;
            *(undefined4 *)(param_4[0xf] + param_4[0x10] * 4) = *(undefined4 *)(iVar15 + 0xb0);
            param_4[0x10] = param_4[0x10] + 1;
            FUN_0056f350((int)param_3, (float *)iVar15, param_10);  // 0x00560663  (K10)
          }
        }
        FUN_0056f020((int)param_3);                             // 0x00560676  (K9)
      }
      local_2c = local_2c + 1;
      local_28 = local_28 + 0x28;
    } while (local_2c != *(int *)(param_8 + 4));
  }
  param_3[0x4d] = 0;
  param_3[0x51] = 0x43480000;                                  // 200.0f bits
  puVar1 = param_3 + 0x46;
  param_3[0x49] = *param_9;                                     // raw copy of dt bits (TRAP 2)
  param_3[0x53] = param_9[0x16];
  param_3[0x56] = (uint)param_9[0x1a] >> 2 & 1;
  // 0x0056071f  (K8 inertia) — 15 args
  FUN_0056e680(piVar8[0xc], piVar8[0xd], (int *)(param_3 + 0x46), (int *)(param_3 + 0x43),
               piVar8[8], piVar8[9], piVar8[10], (int *)piVar8[0xb], *piVar8, piVar8[1],
               piVar8[2], piVar8[3], piVar8[6], piVar8[7], *(float *)param_9);
  if (local_24 != 0) {
    FUN_005675d0(param_6, (int)param_5, param_3[0x42], param_3[0x3d]);  // 0x0056074b  (UNPORTED)
    param_3[2] = param_3[0x3d];
    iVar15 = 0;
    if (param_3[0x1a] != 0) {
      iVar17 = 0;
      do {
        iVar15 = iVar15 + 1;
        *(float *)(param_3[0x19] + iVar17)       = *(float *)(param_3[0x19] + iVar17)       + F9(0x19);
        *(float *)(iVar17 + 4 + param_3[0x19])   = *(float *)(iVar17 + 4 + param_3[0x19])   + F9(0x19);
        *(float *)(iVar17 + 8 + param_3[0x19])   = *(float *)(iVar17 + 8 + param_3[0x19])   + F9(0x19);
        iVar7 = iVar17 + 0xc;
        iVar2 = iVar17 + 0xc;
        iVar17 = iVar17 + 0x10;
        *(float *)(iVar2 + param_3[0x19]) = *(float *)(iVar7 + param_3[0x19]) + F9(0x19);
      } while (iVar15 != param_3[0x1a]);
    }
    invDt = _DAT_005cc320 / F3(0x49);                          // 1.0f / dt  (TRAP 2)
    param_3[0x4a] = 0;
    // 0x00560876  integrator selector — INTEGER compare (TRAP 1)
    if (param_9[0x16] == 2) {
      FUN_0056dd40(*param_3,param_3[1],param_3[2],param_3[3],param_3[0xd],param_3[0xe],param_3[0xf],
                   param_3[0x10],param_3[0x11],param_3[0x12],param_3[0x13],param_3[0x14],param_3[0x15],
                   param_3[0x16],param_3[0x17],param_3[0x18],param_3[0x19],param_3[0x1a],param_3[0x1b],
                   param_3[7],param_3[8],param_3[9],param_3[10],param_3[0xb],param_3[0xc],*puVar1,
                   param_3[0x47],param_3[0x48],invDt);          // 0x005608b3  (K8 SSE twin)
    }
    else {
      FUN_0056d3f0(*param_3,param_3[1],param_3[2],param_3[3],param_3[0xd],param_3[0xe],param_3[0xf],
                   param_3[0x10],param_3[0x11],param_3[0x12],param_3[0x13],param_3[0x14],param_3[0x15],
                   param_3[0x16],param_3[0x17],param_3[0x18],param_3[0x19],param_3[0x1a],param_3[0x1b],
                   param_3[7],param_3[8],param_3[9],param_3[10],param_3[0xb],param_3[0xc],*puVar1,
                   param_3[0x47],param_3[0x48],invDt);          // 0x0056097e  (K8 x87)
    }
    // 0x005609e5  (K7) — 11 args
    FUN_0056d070(param_5, *param_3, param_3[1], param_3[2], param_3[3], param_3[0x37], param_3[0x38],
                 param_3[0x39], (int *)param_3[0x3a], param_3[0x3b], param_3[0x3c]);

    // ---- [UNCERTAIN U-K13-KV] param_11 KV callback @0x560a9c -------------------
    // Original builds a 53-word by-value outgoing frame:
    //   param_5[0x25..0x27], param_5[0..0x21], param_3[4..0xc],
    //   param_3[0x46..0x48], param_3[0x43..0x45], param_3[0x49]  (K13_PORT_RECON).
    // STUBBED per owner decision — PIN via live Frida stack read before race.
    ((void (__cdecl *)())param_11)();

    puVar14 = param_3;
    param_3 = (undefined4 *)0x0;                               // reused as index (raw)
    if (*(int *)(puVar14 + 0xb) != 0) {
      iVar15 = 0;
      do {
        iVar17 = iVar15 + 0x10;
        *(float *)(iVar15 + param_5[0x22]) =
             *(float *)(iVar15 + puVar14[10]) * *(float *)(puVar14[0x22] + iVar15);
        *(float *)(param_5[0x22] + -0xc + iVar17) =
             *(float *)(iVar15 + 4 + puVar14[10]) * *(float *)(puVar14[0x22] + -0xc + iVar17);
        *(float *)(param_5[0x22] + -8 + iVar17) =
             *(float *)(iVar15 + 8 + puVar14[10]) * *(float *)(puVar14[0x22] + -8 + iVar17);
        *(float *)(param_5[0x22] + -4 + iVar17) =
             *(float *)(iVar15 + 0xc + puVar14[10]) * *(float *)(puVar14[0x22] + -4 + iVar17);
        *(undefined4 *)(iVar15 + puVar14[0x1c])       = 0;
        *(undefined4 *)(iVar15 + 4 + puVar14[0x1c])   = 0;
        *(undefined4 *)(iVar15 + 8 + puVar14[0x1c])   = 0;
        *(undefined4 *)(iVar15 + 0xc + puVar14[0x1c]) = 0;
        *(undefined4 *)(iVar15 + puVar14[0x1f])       = 0;
        *(undefined4 *)(iVar15 + 4 + puVar14[0x1f])   = 0;
        *(undefined4 *)(iVar15 + 8 + puVar14[0x1f])   = 0;
        *(undefined4 *)(iVar15 + 0xc + puVar14[0x1f]) = 0;
        param_3 = (undefined4 *)((int)param_3 + 1);
        iVar15 = iVar17;
      } while (param_3 != (undefined4 *)puVar14[0xb]);
    }
    // param_13 KV callback #1 — real side effects kept; frame [UNCERTAIN]
    puVar14[0x4e] = param_9[1];
    puVar14[0x50] = param_9[0x17];
    _DAT_00913284 = 0;
    // ---- [UNCERTAIN U-K13-KV] param_13 call #1 @~0x560ab? ---------------------
    // explicit args (param_4, param_4+6, param_4+0x15, param_6) + ~50-word by-value
    // tail (K13_PORT_RECON). STUBBED with the 4 typed args — PIN tail via capture.
    ((void (__cdecl *)(void *,void *,void *,int))param_13)(param_4, param_4 + 6,
                                                           param_4 + 0x15, param_6);
    // 0x00560c90  (leaf, hooked) — 5 args
    FUN_005601f0(param_7, param_4[6], param_4[7], param_4[8], param_2);
    // param_13 KV callback #2 — real side effects kept; frame [UNCERTAIN]
    puVar14[0x4e] = param_9[2];
    puVar14[0x50] = param_9[0x18];
    _DAT_00913284 = 1;
    // ---- [UNCERTAIN U-K13-KV] param_13 call #2 @~0x560b?? ---------------------
    ((void (__cdecl *)(void *,void *,void *,int))param_13)(param_4 + 3, param_4 + 9,
                                                           param_4 + 0x18, param_6);
    // 0x00560e25  (K7) — 18 args
    FUN_0056caa0(piVar8[0x12], piVar8[0x13], param_4 + 0xc, param_4 + 0xf, param_4 + 0x12,
                 piVar8[0xc], (int *)piVar8[0xd], *(int *)puVar14, puVar14[1], puVar14[2],
                 puVar14[3], puVar14[0x2b], puVar14[0x2c], puVar14[0x2d], param_4[3],
                 param_4[4], param_4[5], *param_4);
    // 0x00560e6b  (K7) — 8 args
    FUN_0056c580(piVar8[0x10], piVar8[0x11], param_4[0xc], param_4[0xd], param_4[0xe],
                 piVar8[0x12], (int *)piVar8[0x13], *(int *)puVar1);
  }
  // 0x00560ebc  (UNPORTED) — 12 args (p12 = 1.0f)
  FUN_0056c310(*piVar8, piVar8[1], piVar8[4], piVar8[5], param_4[0xc], param_4[0xd], param_4[0xe],
               piVar8[0x14], piVar8[0x15], piVar8[6], piVar8[7], 1.0f);
  // 0x00560ed4  (UNPORTED) — 5 args
  FUN_0056bdf0(piVar8[2], piVar8[3], piVar8[0x10], piVar8[0x11], (int)*param_9);
  piVar12 = (int *)piVar8[0x15];
  uVar20 = 0;
  if (piVar12[1] != 0) {
    param_5 = (int *)0x0;                                       // reused as offset (raw)
    do {
      iVar15 = *(int *)(*piVar12 + uVar20 * 4);
      iVar17 = iVar15 * 0x20;
      pfVar13 = (float *)(iVar17 + piVar8[0xc]);
      pfVar16 = (float *)(param_4[0xc] + (int)param_5);
      fVar5 = pfVar16[2] * pfVar13[2] + *pfVar13 * *pfVar16 + pfVar16[1] * pfVar13[1];
      if (fVar5 < _DAT_005e5258) {
        FUN_005667c0(&local_18, pfVar13);                       // 0x00560f39  (K1 normalize)
        pfVar13 = (float *)(iVar17 + piVar8[2]);
        fVar3 = pfVar13[2] * local_10 +
                *pfVar13 * local_18 + *(float *)(iVar17 + 4 + piVar8[2]) * local_14;
        fVar5 = fVar5 / *(float *)(iVar15 * 0x30 + 0xc + piVar8[8]);
        fVar4 = fVar3 * fVar3 + fVar5 + fVar5;
        fVar5 = DAT_005d757c;
        if ((fVar4 < PTR_DAT_005ceabc) == (fVar4 == PTR_DAT_005ceabc)) {
          fVar5 = _DAT_005cc320;
          if (fVar3 < DAT_005d757c) {
            fVar5 = _DAT_005cc33c;
          }
          fVar5 = sqrtf(fVar4) * fVar5;
        }
        fVar5 = fVar5 - fVar3;
        *pfVar13 = local_18 * fVar5 + *pfVar13;
        pfVar13 = (float *)(iVar17 + 4 + piVar8[2]);
        *pfVar13 = local_14 * fVar5 + *pfVar13;
        pfVar13 = (float *)(iVar17 + 8 + piVar8[2]);
        *pfVar13 = fVar5 * local_10 + *pfVar13;
      }
      piVar12 = (int *)piVar8[0x15];
      uVar20 = uVar20 + 1;
      param_5 = (int *)((int)param_5 + 0x20);
    } while (uVar20 < (uint)piVar12[1]);
  }
  iVar15 = piVar8[0xc];
  piVar8 = (int *)piVar8[0xd];
  iVar17 = 0;
  if (piVar8[1] != 0) {
    do {
      pfVar13 = (float *)(*(int *)(*piVar8 + iVar17 * 4) * 0x20 + iVar15);
      iVar17 = iVar17 + 1;
      pfVar13[2] = 0.0f;
      pfVar13[1] = 0.0f;
      *pfVar13   = 0.0f;
      pfVar13[6] = 0.0f;
      pfVar13[5] = 0.0f;
      pfVar13[4] = 0.0f;
    } while (iVar17 != piVar8[1]);
  }
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 13. ---
RH_ScopedInstall(FUN_00560260, 0x00560260);

}  // namespace Collision
}  // namespace mashed_re
