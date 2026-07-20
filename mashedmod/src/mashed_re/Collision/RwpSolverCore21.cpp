// ============================================================================
//  RwpSolverCore21.cpp  —  B5e solver-island cluster K21
//  5 verbatim RWP-3.7 narrow-phase / contact-projection helpers:
//    FUN_00561280 (0x00561280, 265 B)  — projects a contact point into world space
//                                         through the shape's world transform and
//                                         accumulates a path-length delta into
//                                         param_3[3]. Gated by FUN_0055bb70 (K5),
//                                         whose return (the +0x08 volume point, EAX)
//                                         is tested for NULL.
//    FUN_00568990 (0x00568990, 675 B)  — pair-list narrow-phase pump: for each input
//                                         pair, fetches both shape transforms
//                                         (FUN_0055abb0 K3 / FUN_0055bd80 K5), runs the
//                                         two-sided closest-feature query FUN_00575c60
//                                         (K18) twice, generates the manifold set with
//                                         FUN_00576640 (K16), drives each with
//                                         FUN_00575880 (K20), then batches the results
//                                         through FUN_00575560 (K20).
//    FUN_005729a0 (0x005729a0, 3138 B) — swept-pair conservative-advancement TOI loop:
//                                         two shape-radius vtable probes (KV frames,
//                                         desc +0x24, x87 ST0 → float10 — see below),
//                                         builds inverse-inertia frames, integrates both
//                                         bodies forward (FUN_0056be80 K6, FUN_004c51a0/
//                                         FUN_004c52f0 K2), runs the K20 single-pair
//                                         driver + K15 polygon SAT (FUN_00576880), and
//                                         refines the time-of-impact over ≤5 iterations.
//    FUN_0056ba30 (0x0056ba30, 244 B)  — per-body contact-batch driver: primes the
//                                         world scratch (FUN_0056b7a0 K18), pumps the
//                                         pairs via FUN_00568990 (within), then threads
//                                         the produced manifolds onto the body's list.
//    FUN_0056bb30 (0x0056bb30, 67 B)   — world sweep: for every enabled body slot (flag
//                                         bit0 clear) calls FUN_0056ba30 (within).
//
//  Deps: K1 (FUN_005667c0), K2 (FUN_004c51a0, FUN_004c52f0), K3 (FUN_0055abb0,
//        FUN_0055bae0), K5 (FUN_0055bb70, FUN_0055bd80), K6 (FUN_0056be80),
//        K15 (FUN_00576880), K16 (FUN_00576640, FUN_005735f0), K18 (FUN_00575c60,
//        FUN_0056b7a0), K20 (FUN_00575880, FUN_00575560). All externed below; the
//        definitions live in sibling TUs of the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  All float math is x87 (FLD/FMUL/FADDP/FSQRT/FDIVR/FSUBR confirmed by disasm) — plain
//  C float compiles to x87 under /arch:IA32, so this is bit-faithful. float10 == the x87
//  80-bit intermediate; typed `long double` (== double under MSVC x86), matching the
//  established Core-cluster convention.
//
//  DISASM-RESOLVED CONVENTIONS (pool0 read-only session, verified against listing):
//   * TWO KV (vtable) frames in FUN_005729a0 — the shape volume-descriptor +0x24 slot,
//     called `__cdecl` with ONE arg (the shape ptr) and returning a float in x87 ST0:
//       0x005729ba  CALL [ECX+0x24]   (ECX = *(*(*param_2   +8)+0x5c))
//       0x005729d7  CALL [EDX+0x24]   (EDX = *(*(param_2[1] +8)+0x5c))
//     Shared `ADD ESP,0x8` @0x005729e6 confirms two 1-arg cdecl pushes. Both results are
//     consumed in x87 arithmetic, so the fn-ptr is typed to RETURN float10 (RwpVolFn24) —
//     NOT void: a void fn-ptr never pops ST0 and leaks the x87 stack (memory
//     x87_st0_float10_return_fnptr).
//   * FUN_00561280 tests FUN_0055bb70's return (0x005612c0 TEST EAX,EAX): the K5 chain
//     FUN_0055bb70 → FUN_0055bae0 forwards the +0x08 volume point in EAX. Both were typed
//     void in K3/K5; corrected to return `float *` (return-ignoring callers unaffected).
//   * FUN_00576880 (K15) 12-arg call @0x0057337c: args 1/3/6 (desc slots +0x50/+0xd0/
//     +0x10c) are POINTERS stored in float slots, pushed as raw dwords — externed with
//     `undefined4` params (same choice as K19's caller) and read via `*(undefined4*)`.
//   * FUN_00575880 (K20) @0x0057330b: param_1 = param_1+0xdf (LEA [EAX+0x37c]), param_2 =
//     param_1, param_4 = the float (fStack_230-fStack_250)*fStack_194 FSTP'd onto the
//     stack. FUN_00575880 in FUN_00568990 @0x…: param_4 is the raw dword [param_1+0x3ec]
//     read as float for a byte-exact push.
//   * PTR_DAT_005ceabc @0x00572d47 is an `FCOM m32` (float threshold), NOT a pointer —
//     read as float. 0x7f7fffff @0x00572d64 = FLT_MAX and 0x3 @0x00572f3b/0x005731c5 =
//     denormal-3 are INTEGER-bit stores (MOV dword,imm), reproduced bit-exact.
//   * The `a < b == (a == b)` / `a < b != (a == b)` compare idioms are Ghidra's
//     unordered-NaN rendering of single FCOMP branches; kept verbatim (pure comparison,
//     no rounding).
// ============================================================================
#include "../Core/HookSystem.h"
#include <cmath>                   // sqrtl — x87 FSQRT floor

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned short ushort;
typedef unsigned char  byte;
typedef long double    float10;   // x87 80-bit intermediate (== double under MSVC x86)

// Ghidra SQRT() intrinsic = x87 FSQRT on the 80-bit ST0 chain (RwpSolverLeaves1 idiom).
static inline float10 SQRT(float10 v) { return std::sqrtl(v); }             // [X87]

// --- fixed-address .data constants (live in MASHED.exe for the .asi race) -----
#define DAT_005d757c    (*(const float*)0x005d757cu)   // 0.0f
#define _DAT_005cc320   (*(const float*)0x005cc320u)   // 1.0f            (0x3f800000)
#define _DAT_005cc574   (*(const float*)0x005cc574u)   // inertia numerator
#define _DAT_005cd07c   (*(const float*)0x005cd07cu)   // clamp numerator
#define _DAT_005e4568   (*(const float*)0x005e4568u)   // separation threshold
#define PTR_DAT_005ceabc (*(const float*)0x005ceabcu)  // radius-sum floor (FCOM m32)

// --- extern callees (defined in sibling K-cluster TUs of same .asi) ----------
extern "C" float10 __cdecl FUN_005667c0(float *param_1,float *param_2);                     // 0x005667c0 (K1)
extern "C" void    __cdecl FUN_004c51a0(float *param_1,float *param_2,int param_3);          // 0x004c51a0 (K2)
extern "C" uint *  __cdecl FUN_004c52f0(uint *param_1,uint *param_2,int param_3);            // 0x004c52f0 (K2)
extern "C" undefined4 __cdecl FUN_0055abb0(undefined4 *param_1,int param_2,undefined4 param_3); // 0x0055abb0 (K3)
extern "C" void    __cdecl FUN_0055bae0(int param_1,float *param_2,undefined4 param_3);       // 0x0055bae0 (K3)
extern "C" float * __cdecl FUN_0055bb70(int param_1,int param_2,undefined4 param_3);          // 0x0055bb70 (K5)
extern "C" void    __cdecl FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4); // 0x0055bd80 (K5)
extern "C" void    __cdecl FUN_0056be80(int *param_1,int param_2,float param_3);              // 0x0056be80 (K6)
extern "C" uint    __cdecl FUN_00576880(undefined4 param_1,uint param_2,undefined4 param_3,uint param_4,float param_5,undefined4 param_6,float *param_7,float *param_8,uint *param_9,uint *param_10,float *param_11,float *param_12); // 0x00576880 (K15)
extern "C" uint    __cdecl FUN_00576640(int *param_1,uint param_2,uint param_3,int param_4,int param_5); // 0x00576640 (K16)
extern "C" void    __cdecl FUN_005735f0(float *param_1,float *param_2,float *param_3,float *param_4); // 0x005735f0 (K16)
extern "C" int     __cdecl FUN_00575c60(undefined4 *param_1,uint *param_2,float *param_3,float *param_4,float *param_5,undefined4 param_6); // 0x00575c60 (K18)
extern "C" int     __cdecl FUN_0056b7a0(undefined4 *param_1,int param_2,uint param_3,int param_4); // 0x0056b7a0 (K18)
extern "C" undefined4 __cdecl FUN_00575880(int param_1,int param_2,int *param_3,float param_4); // 0x00575880 (K20)
extern "C" int     __cdecl FUN_00575560(int param_1,uint param_2,uint param_3,int param_4,int param_5); // 0x00575560 (K20)

// KV frame: shape volume-descriptor +0x24 slot, cdecl 1-arg, x87 ST0 float return.
typedef float10 (__cdecl *RwpVolFn24)(int);                                                  // desc +0x24

// forward decls for RH_ScopedInstall / mutual recursion
extern "C" float *    __cdecl FUN_00561280(int param_1,int param_2,float *param_3);
extern "C" uint       __cdecl FUN_00568990(int param_1,int param_2,uint param_3,int param_4,uint param_5);
extern "C" undefined4 __cdecl FUN_005729a0(float *param_1,int *param_2,float *param_3,float *param_4);
extern "C" bool       __cdecl FUN_0056ba30(int param_1,int param_2);
extern "C" int        __cdecl FUN_0056bb30(int param_1);

// ---------------------------------------------------------------------------
// 0x00561280  Project a contact point into world space and update a path accumulator.
//   param_1 = manifold record, param_2 = contact index, param_3 = 4-float point+len.
//   Gated by FUN_0055bb70 (K5) whose EAX (the volume point) is tested for NULL.
// ---------------------------------------------------------------------------
extern "C" float * __cdecl
FUN_00561280(int param_1,int param_2,float *param_3)
{
  float *pfVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  int *piVar11;
  float *pfVar12;
  undefined4 *puVar13;
  int iVar14;

  param_2 = param_2 * 0xc;
  puVar13 = (undefined4 *)(*(int *)(param_1 + 0x10) + param_2);
  piVar11 = (int *)puVar13[1];
  pfVar1 = (float *)(**(int **)(*piVar11 + 0x10) + 0x30 + piVar11[1] * 0x40);
  iVar14 = (int)FUN_0055bb70(*puVar13,0,(undefined4)(int)param_3);           // 0x005612b8; EAX tested
  if (iVar14 == 0) {
    return (float *)0x0;
  }
  pfVar12 = *(float **)(param_2 + 8 + *(int *)(param_1 + 0x10));
  fVar2 = pfVar12[5];
  fVar3 = pfVar12[1];
  fVar4 = *param_3;
  fVar5 = pfVar12[9];
  fVar6 = pfVar12[6];
  fVar7 = param_3[1];
  fVar8 = pfVar12[2];
  fVar9 = *param_3;
  fVar10 = pfVar12[10];
  *param_3 = pfVar12[0xc] + *pfVar12 * *param_3 + pfVar12[8] * param_3[2] + pfVar12[4] * param_3[1];
  param_3[1] = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x34) +
               fVar5 * param_3[2] + fVar3 * fVar4 + fVar2 * param_3[1];
  fVar2 = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x38) +
          fVar10 * param_3[2] + fVar8 * fVar9 + fVar6 * fVar7;
  param_3[2] = fVar2;
  fVar2 = pfVar1[2] - fVar2;
  param_3[3] = SQRT((pfVar1[1] - param_3[1]) * (pfVar1[1] - param_3[1]) +
                    (*pfVar1 - *param_3) * (*pfVar1 - *param_3) + fVar2 * fVar2) + param_3[3];
  *param_3 = *pfVar1;
  param_3[1] = pfVar1[1];
  param_3[2] = pfVar1[2];
  return param_3;
}

// ---------------------------------------------------------------------------
// 0x00568990  Pair-list narrow-phase pump. param_1 = solver context (World-ish base),
//   param_2 = input pair array (0x14 stride), param_3 = pair count, param_4 = manifold
//   record pool base, param_5 = record cap. Returns manifolds produced.
// ---------------------------------------------------------------------------
extern "C" uint __cdecl
FUN_00568990(int param_1,int param_2,uint param_3,int param_4,uint param_5)
{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  short *psVar8;
  uint uVar9;
  int *piVar10;
  undefined4 *puVar11;
  uint local_54;
  uint local_50;
  uint local_48;
  undefined1 local_40 [32];
  undefined1 local_20 [32];

  local_54 = 0;
  if ((param_3 == 0) || (param_5 == 0)) {
    return 0;
  }
  piVar10 = (int *)(param_1 + 0x37c);
  piVar2 = (int *)**(int **)(param_1 + 0x70);
  *(undefined4 *)(param_1 + 0x3ec) = *(undefined4 *)(*piVar2 + 0xc014);
  *(int *)(param_1 + 0x3f0) = piVar2[3];
  uVar3 = *(undefined4 *)(*piVar2 + 0xc018);
  local_50 = 0;
  uVar7 = 0;
  if (param_3 != 0) {
    do {
      uVar7 = local_50;
      if (param_5 <= local_54) {
        return local_54;
      }
      *(undefined4 *)(param_1 + 0x3ac) = 0;
      *(undefined4 *)(param_1 + 0x3b8) = 0;
      *(undefined4 *)(param_1 + 0x3f4) = 0;
      if (local_50 < param_3) {
        puVar11 = (undefined4 *)(param_2 + 8 + local_50 * 0x14);
        do {
          if (*(int *)(param_1 + 0x3f4) != 0) break;
          puVar1 = puVar11 + -2;
          if (*(short *)(puVar11 + -1) == -1) {
            FUN_0055bd80(*puVar1,0,0,(undefined4)(int)local_40);
          }
          else {
            FUN_0055abb0((undefined4 *)piVar2,*puVar1,(undefined4)(int)local_40);
          }
          if (*(short *)(puVar11 + 1) == -1) {
            FUN_0055bd80(*puVar11,0,0,(undefined4)(int)local_20);
          }
          else {
            FUN_0055abb0((undefined4 *)piVar2,*puVar11,(undefined4)(int)local_20);
          }
          *(undefined4 *)(param_1 + 0x380) = 0;
          *(undefined4 *)(param_1 + 0x398) = 0;
          *(undefined4 *)(param_1 + 0x38c) = 0;
          *(undefined4 *)(param_1 + 0x3d8) = 0;
          iVar4 = FUN_00575c60((undefined4 *)piVar10,puVar1,(float *)0x0,(float *)local_40,(float *)local_20,(undefined4)(int)puVar11);
          if ((iVar4 != 0) &&
             (iVar5 = FUN_00575c60((undefined4 *)piVar10,puVar11,(float *)0x0,(float *)local_20,(float *)local_40,(undefined4)(int)puVar1), iVar5 != 0)) {
            uVar6 = FUN_00576640(piVar10,iVar4,iVar5,puVar11[2],uVar3);
            if ((*(int *)(param_1 + 0x3f4) != 0) && (uVar7 < local_50)) break;
            uVar9 = 0;
            if (uVar6 != 0) {
              do {
                FUN_00575880((int)piVar10,param_1,(int *)(*piVar10 + uVar9 * 8),
                             *(float *)(param_1 + 0x3ec));
                uVar9 = uVar9 + 1;
              } while (uVar9 < uVar6);
            }
          }
          local_50 = local_50 + 1;
          puVar11 = puVar11 + 5;
        } while (local_50 < param_3);
      }
      uVar7 = *(uint *)(param_1 + 0x3ac);
      iVar4 = *(int *)(param_1 + 0x3a8);
      local_48 = 0;
      if (uVar7 != 0) {
        do {
          if (param_5 <= local_54) break;
          uVar6 = 1;
          if (1 < uVar7 - local_48) {
            psVar8 = (short *)(iVar4 + 0x17c);
            do {
              if ((((*(short *)(iVar4 + 0x5c) != -1) || (*psVar8 != -1)) &&
                  ((*(int *)(iVar4 + 0x58) != *(int *)(psVar8 + -2) ||
                   (*(short *)(iVar4 + 0x5c) != *psVar8)))) ||
                 (((*(short *)(iVar4 + 0xdc) != -1 || (psVar8[0x40] != -1)) &&
                  ((*(int *)(iVar4 + 0xd8) != *(int *)(psVar8 + 0x3e) ||
                   (*(short *)(iVar4 + 0xdc) != psVar8[0x40])))))) break;
              uVar6 = uVar6 + 1;
              psVar8 = psVar8 + 0x90;
            } while (uVar6 < uVar7 - local_48);
          }
          iVar5 = FUN_00575560((int)piVar10,iVar4,uVar6,local_54 * 0xe0 + param_4,param_5 - local_54);
          local_54 = local_54 + iVar5;
          local_48 = local_48 + uVar6;
          iVar4 = iVar4 + uVar6 * 0x120;
          uVar7 = *(uint *)(param_1 + 0x3ac);
        } while (local_48 < uVar7);
      }
      uVar7 = local_54;
    } while (local_50 < param_3);
  }
  return uVar7;
}

// ---------------------------------------------------------------------------
// 0x005729a0  Swept-pair conservative-advancement time-of-impact.
//   param_1 = solver context, param_2 = 2-body pair, param_3/param_4 = in/out swept
//   fractions for body A / body B. Returns 1 if a TOI was found (and clamps *param_3 /
//   *param_4), else 0.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_005729a0(float *param_1,int *param_2,float *param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float fVar9;
  float fVar10;
  int iVar11;
  uint uVar12;
  int *piVar13;
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  float10 fVar17;
  float10 fVar18;
  float fStack_250;
  float fStack_24c;
  float fStack_248;
  float fStack_244;
  float fStack_240;
  float fStack_23c;
  float fStack_238;
  float fStack_230;
  int *piStack_22c;
  float fStack_228;
  uint uStack_220;
  float fStack_21c;
  float fStack_218;
  int *piStack_214;
  float *pfStack_210;
  float fStack_20c;
  float fStack_208;
  float fStack_204;
  float fStack_200;
  float fStack_1fc;
  float fStack_1f8;
  float fStack_1f4;
  float fStack_1f0;
  float fStack_1ec;
  float fStack_1e8;
  float *pfStack_1e4;
  float fStack_1c0;
  float fStack_1bc;
  float fStack_1b8;
  float fStack_1b4;
  float fStack_1b0;
  float fStack_1ac;
  float fStack_1a8;
  float fStack_1a4;
  float fStack_1a0;
  float fStack_19c;
  float fStack_198;
  float fStack_194;
  float fStack_190;
  float fStack_18c;
  float fStack_188;
  float fStack_184;
  float fStack_180;
  float fStack_178;
  float fStack_15c;
  float fStack_158;
  float fStack_154;
  float fStack_14c;
  float fStack_144;
  float fStack_140;
  float fStack_13c;
  float fStack_134;
  float fStack_12c;
  float fStack_120;
  undefined1 auStack_114 [4];
  undefined1 auStack_110 [4];
  undefined1 auStack_10c [12];
  float afStack_100 [64];
  // vec output buffers (FUN_00561280 writes 4 floats, FUN_005735f0 writes 3) — arrays force
  // the contiguity the original stack layout relied on; Ghidra scalarized them as fStack_1d0..
  float afStack_1d0 [4];   // body-A world frame  [1d0,1cc,1c8,1c4]
  float afStack_1e0 [4];   // body-B world frame  [1e0,1dc,1d8,1d4]
  float afStack_174 [3];   // FUN_005735f0 out A  [174,170,16c]
  float afStack_168 [3];   // FUN_005735f0 out B  [168,164,160]

  fVar17 = ((RwpVolFn24)(*(void **)(*(int *)(*(int *)(*param_2 + 8) + 0x5c) + 0x24)))
                              (*(int *)(*param_2 + 8));                       // 0x005729ba KV frame
  fVar18 = (float10)*(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_218 = (float)(fVar18 + fVar18 + fVar17);
  fVar17 = ((RwpVolFn24)(*(void **)(*(int *)(*(int *)(param_2[1] + 8) + 0x5c) + 0x24)))
                              (*(int *)(param_2[1] + 8));                     // 0x005729d7 KV frame
  fVar18 = (float10)*(float *)(*(int *)(param_2[1] + 8) + 0x4c);
  fStack_218 = (float)((fVar18 + fVar18 + fVar17 + (float10)fStack_218) * (float10)param_1[10]);
  fVar1 = param_1[9];
  fVar2 = *param_1;
  if (param_3 == (float *)0x0) {
    FUN_0055bae0(*(int *)(*param_2 + 8),*(float **)(*param_2 + 0x10),(undefined4)(int)&afStack_1d0[0]); // 0x00572a84
    piStack_22c = (int *)0x0;
    pfStack_210 = (float *)0x0;
    fStack_23c = 0.0;
    fStack_230 = *param_4;
    pfVar14 = (float *)0x0;
  }
  else {
    uVar12 = (uint)*(ushort *)((int *)*param_2 + 1);
    iVar11 = *(int *)*param_2;
    piStack_22c = *(int **)(*(int *)(iVar11 + 0x10) + 4 + uVar12 * 0xc);
    pfVar14 = (float *)(piStack_22c[1] * 0x20 + *(int *)(*(int *)(*piStack_22c + 0x10) + 8));
    fStack_23c = SQRT(pfVar14[6] * pfVar14[6] + pfVar14[5] * pfVar14[5] + pfVar14[4] * pfVar14[4]);
    pfStack_210 = pfVar14;
    FUN_00561280(iVar11,uVar12,&afStack_1d0[0]);
  }
  if (param_4 == (float *)0x0) {
    FUN_0055bae0(*(int *)(param_2[1] + 8),*(float **)(param_2[1] + 0x10),(undefined4)(int)&afStack_1e0[0]); // 0x00572ba0
    fStack_248 = *pfVar14;
    fStack_230 = *param_3;
    fStack_244 = pfVar14[1];
    piVar13 = (int *)0x0;
    piStack_214 = (int *)0x0;
    pfStack_1e4 = (float *)0x0;
    fStack_240 = pfVar14[2];
    fStack_228 = 0.0;
  }
  else {
    uVar12 = (uint)*(ushort *)((int *)param_2[1] + 1);
    iVar11 = *(int *)param_2[1];
    piVar13 = *(int **)(*(int *)(iVar11 + 0x10) + 4 + uVar12 * 0xc);
    pfVar15 = (float *)(piVar13[1] * 0x20 + *(int *)(*(int *)(*piVar13 + 0x10) + 8));
    fStack_228 = SQRT(pfVar15[6] * pfVar15[6] + pfVar15[5] * pfVar15[5] + pfVar15[4] * pfVar15[4]);
    piStack_214 = piVar13;
    pfStack_1e4 = pfVar15;
    FUN_00561280(iVar11,uVar12,&afStack_1e0[0]);
    if (param_3 == (float *)0x0) {
      fStack_248 = -*pfVar15;
      fStack_244 = -pfVar15[1];
      fStack_240 = -pfVar15[2];
    }
    else {
      if (*param_3 < *param_4 == (*param_3 == *param_4)) {
        fStack_230 = *param_4;
      }
      else {
        fStack_230 = *param_3;
      }
      fStack_248 = *pfVar14 - *pfVar15;
      fStack_244 = pfVar14[1] - pfVar15[1];
      fStack_240 = pfVar14[2] - pfVar15[2];
    }
  }
  fStack_1c0 = afStack_1d0[0] - afStack_1e0[0];
  fStack_1bc = afStack_1d0[1] - afStack_1e0[1];
  fStack_1b8 = afStack_1d0[2] - afStack_1e0[2];
  fVar3 = afStack_1e0[3] + afStack_1d0[3];
  fVar17 = FUN_005667c0((float *)auStack_10c,&fStack_1c0);
  fStack_250 = 0.0;
  fVar4 = fStack_240 * fStack_240 + fStack_248 * fStack_248 + fStack_244 * fStack_244;
  fVar18 = (float10)fStack_218 + (float10)fStack_218;
  fStack_1f4 = (float)fVar18;
  fVar18 = fVar18 + (float10)fVar3;
  if (fVar18 < fVar17) {
    fVar3 = fStack_1b8 * fStack_240 + fStack_1c0 * fStack_248 + fStack_1bc * fStack_244;
    if (_DAT_005e4568 < fVar3) {
      return 0;
    }
    fVar17 = (fVar17 + fVar18) * (fVar17 - fVar18) * (float10)fVar4;
    if ((float10)fVar3 * (float10)fVar3 < fVar17) {
      return 0;
    }
    fStack_250 = (float)((-(float10)fVar3 - SQRT((float10)fVar3 * (float10)fVar3 - fVar17)) /
                        (float10)fVar4);
  }
  if (fStack_250 < fVar1 * fVar2) {
    fStack_250 = fVar1 * fVar2;
  }
  if (fStack_228 + fStack_23c <= PTR_DAT_005ceabc) {
    *(undefined4 *)&fStack_238 = 0x7f7fffff;                                 // FLT_MAX (0x00572d64)
  }
  else {
    fStack_238 = _DAT_005cd07c / (fStack_228 + fStack_23c);
  }
  if (fStack_230 <= fStack_250) {
    return 0;
  }
  uStack_220 = 0;
  fVar1 = *(float *)(*(int *)(param_2[1] + 8) + 0x4c) + *(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_198 = *(float *)(*(int *)(param_2[1] + 8) + 0x4c) -
               *(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_194 = afStack_1d0[3] * fStack_23c + afStack_1e0[3] * fStack_228 + SQRT(fVar4);
  do {
    iVar11 = *param_2;
    if (piStack_22c != (int *)0x0) {
      pfVar15 = *(float **)(iVar11 + 0x10);
      iVar8 = piStack_22c[1];
      piVar13 = *(int **)(*piStack_22c + 0x10);
      pfVar16 = (float *)(iVar8 * 0x10 + piVar13[4]);
      pfVar14 = (float *)(iVar8 * 0x40 + 0x30 + *piVar13);
      fStack_190 = *pfVar16;
      fStack_18c = pfVar16[1];
      fStack_188 = pfVar16[2];
      fStack_184 = pfVar16[3];
      fStack_180 = *pfVar14;
      fVar2 = pfVar14[1];
      fStack_178 = pfVar14[2];
      if (DAT_005d757c < fStack_250) {
        FUN_0056be80(piVar13,iVar8,fStack_250);
      }
      afStack_1d0[0] = *pfVar14;
      afStack_1d0[1] = pfVar14[1];
      afStack_1d0[2] = pfVar14[2];
      fVar9 = _DAT_005cc574 /
              (pfVar16[3] * pfVar16[3] +
              pfVar16[2] * pfVar16[2] + pfVar16[1] * pfVar16[1] + *pfVar16 * *pfVar16);
      fVar10 = fVar9 * *pfVar16;
      fStack_134 = pfVar16[1] * fVar9;
      fVar9 = pfVar16[2] * fVar9;
      fVar3 = pfVar16[3];
      fVar4 = pfVar16[3];
      fVar5 = pfVar16[3];
      fStack_20c = fVar10 * *pfVar16;
      fStack_208 = fStack_134 * pfVar16[1];
      fStack_204 = fVar9 * pfVar16[2];
      fStack_120 = fVar9 * pfVar16[1];
      fVar6 = pfVar16[2];
      fVar7 = *pfVar16;
      pfVar15[0xc] = 0.0;
      pfVar15[0xd] = 0.0;
      pfVar15[0xe] = 0.0;
      *(undefined4 *)(pfVar15 + 3) = 3;                                      // denormal-3 (0x00572f3b)
      *pfVar15 = _DAT_005cc320 - (fStack_204 + fStack_208);
      pfVar15[1] = fVar5 * fVar9 + fStack_134 * fVar7;
      pfVar15[2] = fVar10 * fVar6 - fVar4 * fStack_134;
      pfVar15[4] = fStack_134 * fVar7 - fVar5 * fVar9;
      pfVar15[5] = _DAT_005cc320 - (fStack_204 + fStack_20c);
      pfVar15[6] = fStack_120 + fVar3 * fVar10;
      pfVar15[8] = fVar10 * fVar6 + fVar4 * fStack_134;
      pfVar15[9] = fStack_120 - fVar3 * fVar10;
      pfVar15[10] = _DAT_005cc320 - (fStack_208 + fStack_20c);
      pfVar15[0xc] = *pfVar14;
      pfVar15[0xd] = pfVar14[1];
      pfVar15[0xe] = pfVar14[2];
      fStack_144 = -(float)piStack_22c[2];
      fStack_140 = -(float)piStack_22c[3];
      fStack_13c = -(float)piStack_22c[4];
      FUN_004c51a0(pfVar15,&fStack_144,1);
      iVar11 = *(int *)(iVar11 + 8);
      if ((*(byte *)(*(int *)(iVar11 + 0x5c) + 0x40) & 2) == 0) {
        FUN_004c52f0((uint *)pfVar15,(uint *)iVar11,1);
      }
      *pfVar16 = fStack_190;
      pfVar16[1] = fStack_18c;
      pfVar16[2] = fStack_188;
      pfVar16[3] = fStack_184;
      *pfVar14 = fStack_180;
      pfVar14[1] = fVar2;
      pfVar14[2] = fStack_178;
      piVar13 = piStack_214;
    }
    iVar11 = param_2[1];
    if (piVar13 != (int *)0x0) {
      pfVar15 = *(float **)(iVar11 + 0x10);
      iVar8 = piVar13[1];
      piVar13 = *(int **)(*piVar13 + 0x10);
      pfVar16 = (float *)(iVar8 * 0x10 + piVar13[4]);
      pfVar14 = (float *)(iVar8 * 0x40 + 0x30 + *piVar13);
      fStack_1b4 = *pfVar16;
      fStack_1b0 = pfVar16[1];
      fStack_1ac = pfVar16[2];
      fStack_1a8 = pfVar16[3];
      fStack_1a4 = *pfVar14;
      fStack_1a0 = pfVar14[1];
      fStack_19c = pfVar14[2];
      if (DAT_005d757c < fStack_250) {
        FUN_0056be80(piVar13,iVar8,fStack_250);
      }
      piVar13 = piStack_214;
      afStack_1e0[0] = *pfVar14;
      afStack_1e0[1] = pfVar14[1];
      afStack_1e0[2] = pfVar14[2];
      fStack_12c = _DAT_005cc574 /
                   (pfVar16[3] * pfVar16[3] +
                   pfVar16[2] * pfVar16[2] + pfVar16[1] * pfVar16[1] + *pfVar16 * *pfVar16);
      fVar4 = fStack_12c * *pfVar16;
      fStack_14c = pfVar16[1] * fStack_12c;
      fStack_12c = pfVar16[2] * fStack_12c;
      fStack_1f0 = pfVar16[3] * fVar4;
      fStack_1ec = pfVar16[3] * fStack_14c;
      fStack_1e8 = pfVar16[3] * fStack_12c;
      fStack_200 = fVar4 * *pfVar16;
      fStack_1fc = fStack_14c * pfVar16[1];
      fStack_1f8 = fStack_12c * pfVar16[2];
      fStack_12c = fStack_12c * pfVar16[1];
      fVar2 = pfVar16[2];
      fVar3 = *pfVar16;
      pfVar15[0xc] = 0.0;
      pfVar15[0xd] = 0.0;
      pfVar15[0xe] = 0.0;
      *(undefined4 *)(pfVar15 + 3) = 3;                                      // denormal-3 (0x005731c5)
      *pfVar15 = _DAT_005cc320 - (fStack_1f8 + fStack_1fc);
      pfVar15[1] = fStack_1e8 + fStack_14c * fVar3;
      pfVar15[2] = fVar4 * fVar2 - fStack_1ec;
      pfVar15[4] = fStack_14c * fVar3 - fStack_1e8;
      pfVar15[5] = _DAT_005cc320 - (fStack_1f8 + fStack_200);
      pfVar15[6] = fStack_12c + fStack_1f0;
      pfVar15[8] = fVar4 * fVar2 + fStack_1ec;
      pfVar15[9] = fStack_12c - fStack_1f0;
      pfVar15[10] = _DAT_005cc320 - (fStack_1fc + fStack_200);
      pfVar15[0xc] = *pfVar14;
      pfVar15[0xd] = pfVar14[1];
      pfVar15[0xe] = pfVar14[2];
      fStack_15c = -(float)piStack_214[2];
      fStack_158 = -(float)piStack_214[3];
      fStack_154 = -(float)piStack_214[4];
      FUN_004c51a0(pfVar15,&fStack_15c,1);
      iVar11 = *(int *)(iVar11 + 8);
      if ((*(byte *)(*(int *)(iVar11 + 0x5c) + 0x40) & 2) == 0) {
        FUN_004c52f0((uint *)pfVar15,(uint *)iVar11,1);
      }
      *pfVar16 = fStack_1b4;
      pfVar16[1] = fStack_1b0;
      pfVar16[2] = fStack_1ac;
      pfVar16[3] = fStack_1a8;
      *pfVar14 = fStack_1a4;
      pfVar14[1] = fStack_1a0;
      pfVar14[2] = fStack_19c;
    }
    param_1[0xeb] = 0.0;
    param_1[0xee] = 0.0;
    iVar11 = FUN_00575880((int)(param_1 + 0xdf),(int)param_1,param_2,
                          (fStack_230 - fStack_250) * fStack_194);            // 0x0057330b
    if (iVar11 == 0) {
      return 0;
    }
    pfVar15 = *(float **)(param_1 + 0xea);
    fStack_21c = fVar1 + pfVar15[0x44];
    pfVar14 = pfVar15 + 0x40;
    uVar12 = FUN_00576880(*(undefined4 *)(pfVar15 + 0x14),*(undefined2 *)(pfVar15 + 0x15),
                          *(undefined4 *)(pfVar15 + 0x34),*(undefined2 *)(pfVar15 + 0x35),
                          fStack_198,*(undefined4 *)(pfVar15 + 0x43),pfVar14,
                          &fStack_21c,(uint *)auStack_110,(uint *)auStack_114,pfVar15,afStack_100); // 0x0057337c
    fVar2 = fStack_21c - fVar1;
    fStack_24c = fStack_230;
    fStack_228 = 0.0;
    if (uVar12 != 0) {
      pfVar16 = afStack_100;
      do {
        FUN_005735f0(pfStack_210,&afStack_1d0[0],pfVar16,&afStack_174[0]);
        FUN_005735f0(pfStack_1e4,&afStack_1e0[0],pfVar16,&afStack_168[0]);
        fVar3 = pfVar15[0x42] * (afStack_174[2] - afStack_168[2]) +
                *pfVar14 * (afStack_174[0] - afStack_168[0]) + pfVar15[0x41] * (afStack_174[1] - afStack_168[1]);
        if (fVar3 < DAT_005d757c) {
          fStack_21c = (((pfVar16[2] * pfVar15[0x22] +
                         pfVar16[1] * pfVar15[0x21] + pfVar15[0x20] * *pfVar16) - pfVar15[0x23]) /
                        (pfVar15[0x42] * pfVar15[0x22] +
                        pfVar15[0x41] * pfVar15[0x21] + pfVar15[0x20] * *pfVar14) -
                       ((pfVar15[2] * pfVar16[2] + *pfVar16 * *pfVar15 + pfVar16[1] * pfVar15[1]) -
                       pfVar15[3]) /
                       (pfVar15[2] * pfVar15[0x42] +
                       *pfVar14 * *pfVar15 + pfVar15[0x41] * pfVar15[1])) - fVar1;
          if (fStack_21c < fVar2) {
            fStack_21c = fVar2;
          }
          if (fStack_21c < fStack_218 != (fStack_21c == fStack_218)) goto LAB_00573592;
          fStack_21c = fStack_21c - fStack_1f4;
          fVar3 = -(fStack_21c / fVar3);
          if (fVar3 < fStack_24c) {
            fStack_24c = fVar3;
          }
        }
        fStack_228 = (float)((int)fStack_228 + 1);
        pfVar16 = pfVar16 + 4;
      } while ((uint)fStack_228 < uVar12);
    }
    if (fStack_24c < DAT_005d757c != (fStack_24c == DAT_005d757c)) break;
    if (fStack_238 < fStack_24c) {
      fStack_24c = fStack_238;
    }
    fStack_250 = fStack_24c + fStack_250;
    if (fStack_230 < fStack_250) {
      return 0;
    }
    uStack_220 = uStack_220 + 1;
  } while (uStack_220 < 5);
LAB_00573592:
  if ((param_3 != (float *)0x0) && (fStack_250 < *param_3)) {
    *param_3 = fStack_250;
  }
  if ((param_4 != (float *)0x0) && (fStack_250 < *param_4)) {
    *param_4 = fStack_250;
  }
  return 1;
}

// ---------------------------------------------------------------------------
// 0x0056ba30  Per-body contact-batch driver. param_1 = body handle, param_2 = pair
//   index. Primes the world scratch (FUN_0056b7a0 K18), pumps pairs via FUN_00568990,
//   then threads the produced manifolds onto the body's contact list.
// ---------------------------------------------------------------------------
extern "C" bool __cdecl
FUN_0056ba30(int param_1,int param_2)
{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;

  iVar2 = *(int *)(param_1 + 4);
  *(undefined4 *)(iVar2 + 0x84) = 0;
  if (*(uint *)(iVar2 + 0x90) < *(uint *)(iVar2 + 0x94)) {
    puVar1 = (undefined4 *)(*(int *)(iVar2 + 0xd4) + param_2 * 0x28);
    FUN_0056b7a0((undefined4 *)param_1,*puVar1,puVar1[3],
                 *(undefined4 *)(*(int *)**(undefined4 **)(iVar2 + 0x70) + 0xc018));
    uVar3 = FUN_00568990(iVar2,*(int *)(iVar2 + 0x80),*(uint *)(iVar2 + 0x84),
                         *(int *)(iVar2 + 0x90) * 0xe0 + *(int *)(iVar2 + 0x8c),
                         *(int *)(iVar2 + 0x94) - *(int *)(iVar2 + 0x90));
    if (uVar3 != 0) {
      uVar5 = 0;
      if (uVar3 != 0) {
        do {
          *(undefined4 *)((*(int *)(iVar2 + 0x90) + uVar5) * 0xe0 + 0xdc + *(int *)(iVar2 + 0x8c)) =
               puVar1[1];
          iVar4 = *(int *)(iVar2 + 0x90) + uVar5;
          uVar5 = uVar5 + 1;
          puVar1[1] = iVar4 * 0xe0 + *(int *)(iVar2 + 0x8c);
        } while (uVar5 < uVar3);
      }
      *(int *)(iVar2 + 0x90) = *(int *)(iVar2 + 0x90) + uVar3;
    }
    return uVar3 != 0;
  }
  return false;
}

// ---------------------------------------------------------------------------
// 0x0056bb30  World contact sweep. param_1 = world. For every enabled body slot (flag
//   byte bit0 clear), calls FUN_0056ba30 (within).
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_0056bb30(int param_1)
{
  int iVar1;
  uint uVar2;

  uVar2 = 0;
  if (*(int *)(param_1 + 200) != 0) {
    iVar1 = 0;
    do {
      if ((*(byte *)(*(int *)(param_1 + 0xd4) + 0x18 + iVar1) & 1) == 0) {
        FUN_0056ba30(*(int *)(param_1 + 0x70),uVar2);
      }
      uVar2 = uVar2 + 1;
      iVar1 = iVar1 + 0x28;
    } while (uVar2 < *(uint *)(param_1 + 200));
  }
  return param_1;
}

// --- gta-reversed-style hook registration — CLUSTER 21. ---
RH_ScopedInstall(FUN_00561280, 0x00561280);
RH_ScopedInstall(FUN_00568990, 0x00568990);
RH_ScopedInstall(FUN_005729a0, 0x005729a0);
RH_ScopedInstall(FUN_0056ba30, 0x0056ba30);
RH_ScopedInstall(FUN_0056bb30, 0x0056bb30);

}  // namespace Collision
}  // namespace mashed_re
