// Mashed RE — B5e: RwpSolver-island CLUSTER 2 clean-room port (RW matrix/quat math, 7 fns).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-16). VERBATIM transcription of the K2 cluster from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm this
// session (see per-function notes). Style/idiom follows RwpSolverLeaves1.cpp (K1).
//
// K2 members (rva:size): 004c4600:111 004c51a0:323 004c52f0:378 00546b10:214 00546bf0:93
// 00546c50:93 00546cb0:93. Retires the RwpBuildExterns.cpp RVA thunks for
// FUN_004c52f0/004c51a0/00546b10 (these definitions replace them at link).
//
// NO-GUESSING disasm verifications (pool14, 2026-07-16):
//  1. Calling convention: all 7 end in plain RET (caller cleanup) => __cdecl
//     (body_end 0x004c466e / 0x004c52e2 / 0x004c5469 / 0x00546be5 / 0x00546c4c /
//      0x00546cac / 0x00546d0c).
//  2. RwMatrix module slot: flag word `*(uint*)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000`
//     (MOV EAX,[ECX+EBP*1+0x4] / AND EAX,0x20000 @ 0x004c461a/0x004c4622, same shape at
//     0x004c5354/0x004c53c9); matrix-mult method at slot +8, called cdecl 3-arg with
//     ADD ESP,0xc (0x004c465c/0x004c4660, 0x004c542d/0x004c5431). Single runtime writer of
//     the +8 slot stores 0x004c40e0 (writer 0x004c44f1 — B5e §2 item 4); the landed
//     RwMatrixRotateInner.cpp port (C3) binds the same slot as void(__cdecl*)(out,a,b).
//  3. FUN_004c51a0 mode-1 x87 summation ORDER differs from the decomp's printed order:
//     disasm sums (m[8]·v2 + m[4]·v1) + m[0]·v0, then + m[12] (FLD/FMUL/FADDP chain
//     0x004c522a..0x004c5241; rows 2/3 identical shape) — the port uses the DISASM order.
//  4. FUN_004c51a0 error path is a literal near-null RMW: XOR EAX,EAX; MOV ECX,[EAX+0xc];
//     AND ECX,0xfffdffff; MOV [EAX+0xc],ECX (0x004c51dd..0x004c51ec) — the original
//     read-modify-writes absolute address 0x0000000c (dead path for modes 0..2; decomp
//     prints it as `uRam0000000c`). Transcribed literally.
//  5. FUN_00546b10 keeps the trace on the x87 stack UNROUNDED for both the threshold FCOM
//     (0x00546b30) and the +1.0 (0x00546b3f); only the FastSqrt argument is rounded to
//     float32 (FSTP [ESP] @ 0x00546b46). Ported with float10 intermediates. [X87]
//  6. FUN_00546b10 internal dispatch targets are immediates 0x546bf0/0x546c50/0x546cb0
//     (MOV EAX,imm @ 0x00546baf/0x00546bcd/0x00546bd4) — bound to the cluster's own ports.
//  7. FUN_004c3b30 is the C4 FastSqrt port (Math/RwSqrt.cpp) — called directly; under the
//     .asi its hook is live at the same RVA, so original and port converge on one body.
//  8. FUN_004c52f0 mode-1/2 identity branches include a literal self-copy of param_1
//     through the stack temp (0x004c53dc..0x004c53f6 / 0x004c538d..0x004c53a7) — kept.
//
// x87 note: 80-bit ST0 chains carry the accepted <=1-ULP floor under MSVC's 64-bit long
// double (project_phys_chain_float10_methodology) — [X87]-tagged below.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim (as RwpSolverLeaves1.cpp). ---
typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef long double    float10;     // x87 80-bit extended — MSVC = 64-bit double [X87]

// --- Game globals, bound to absolute addresses under the decomp's names. ---
#define DAT_007d4028   (*(uint*)0x007d4028u)   // RW module-instance base   (0x004c4600 et al.)
#define DAT_007d3ff8   (*(uint*)0x007d3ff8u)   // RwMatrix module offset    (0x004c460c et al.)
#define DAT_005d757c   (*(float*)0x005d757cu)  // matrix-trace threshold    (FCOM @ 0x00546b30)
#define _DAT_005cc320  (*(float*)0x005cc320u)  // 1.0f                      (FADD @ 0x00546b3f)
#define _DAT_005cc32c  (*(float*)0x005cc32cu)  // 0.5f                      (FLD  @ 0x00546b4e)

// The RwMatrix module's matrix-mult method slot (+8): void __cdecl (out, a, b) — same
// binding as the landed Math/RwMatrixRotateInner.cpp port (verified void there; a float10
// return through a void fn-ptr would leak ST0 — feedback_x87_st0_float10_return_fnptr).
typedef void (__cdecl *RwpMatMulFn)(void* out, void* a, void* b);
#define RWP_MATMUL   (*(RwpMatMulFn*)(DAT_007d4028 + 8 + DAT_007d3ff8))
#define RWP_IDENTW   (*(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8))

// RW error funnel (same RVA-fn-ptr idiom as Math/RwMatrixRotateInner.cpp; both targets are
// C3-ported and hooked, so under the .asi the call converges on the ported body).
typedef int  (__cdecl *RwErrPassFn)(int, const char*);
typedef void (__cdecl *RwErrRecordFn)(int*);
#define FUN_004d7ff0_p  (reinterpret_cast<RwErrPassFn>(0x004d7ff0u))
#define FUN_004d8480_p  (reinterpret_cast<RwErrRecordFn>(0x004d8480u))
#define s_Invalid_combination_type_0061811c  (reinterpret_cast<const char*>(0x0061811cu))

// 0x004c3b30 — C4 FastSqrt port (Math/RwSqrt.cpp).
extern "C" float __cdecl FastSqrt(float x);
#define FUN_004c3b30  FastSqrt

// Forward decls (FUN_00546b10 dispatches to the two siblings below it).
extern "C" void __cdecl FUN_00546bf0(float *param_1, float *param_2);
extern "C" void __cdecl FUN_00546c50(float *param_1, float *param_2);
extern "C" void __cdecl FUN_00546cb0(float *param_1, float *param_2);

// ---------------------------------------------------------------------------
// 0x004c4600  RwMatrixMultiply(dst, a, b): identity-flag short-circuits (flag word
//             slot+4 & 0x20000), else dispatches the module mult method (slot+8);
//             dst[3] = b.flags & a.flags (AND EDI,EBX @ 0x004c4663).
// ---------------------------------------------------------------------------
extern "C" undefined4 * __cdecl FUN_004c4600(undefined4 *param_1,undefined4 *param_2,undefined4 *param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;

  uVar1 = param_2[3];
  uVar2 = param_3[3];
  uVar3 = RWP_IDENTW & 0x20000;                       // 0x004c461a / 0x004c4622
  if ((uVar1 & uVar3) != 0) {
    puVar5 = param_1;
    for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *param_3;
      param_3 = param_3 + 1;
      puVar5 = puVar5 + 1;
    }
    return param_1;
  }
  if ((uVar2 & uVar3) != 0) {
    puVar5 = param_1;
    for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *param_2;
      param_2 = param_2 + 1;
      puVar5 = puVar5 + 1;
    }
    return param_1;
  }
  RWP_MATMUL(param_1,param_2,param_3);                // 0x004c465c call [slot+8]
  param_1[3] = uVar2 & uVar1;                         // 0x004c4663..0x004c4665
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x004c51a0  RwMatrixTranslate(mtx, vec, combineOp):
//             op0 = rebuild identity + set pos; op1 = pos += R·vec; op2 = pos += vec;
//             else error 0x80000003 "Invalid combination type" + literal *(0xc) RMW.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_004c51a0(float *param_1,float *param_2,int param_3)

{
  int local_8[2];   // local_8 / local_4 error record pair (0x004c51c2 / 0x004c51cf)

  if (param_3 == 0) {
    param_1[10] = 1.0f;
    param_1[5] = 1.0f;
    *param_1 = 1.0f;
    param_1[4] = 0.0f;
    { uint f; f = *(uint*)&param_1[3] | 0x20003u; *(uint*)&param_1[3] = f; }   // 0x004c5299
    param_1[2] = 0.0f;
    param_1[1] = 0.0f;
    param_1[9] = 0.0f;
    param_1[8] = 0.0f;
    param_1[6] = 0.0f;
    param_1[0xe] = 0.0f;
    param_1[0xd] = 0.0f;
    param_1[0xc] = 0.0f;
    param_1[0xc] = *param_2;
    param_1[0xd] = param_2[1];
    param_1[0xe] = param_2[2];
    { uint f; f = *(uint*)&param_1[3] & 0xfffdffffu; *(uint*)&param_1[3] = f; } // 0x004c52d6
    return;
  }
  if (param_3 != 1) {
    if (param_3 != 2) {
      local_8[0] = 1;
      local_8[1] = FUN_004d7ff0_p(0x80000003,s_Invalid_combination_type_0061811c); // 0x004c51ca
      FUN_004d8480_p(local_8);                                                     // 0x004c51d8
      // VERBATIM original near-null RMW: XOR EAX,EAX; MOV ECX,[EAX+0xc];
      // AND ECX,0xfffdffff; MOV [EAX+0xc],ECX (0x004c51dd..0x004c51ec). The original
      // read-modify-writes absolute address 0x0000000c here (decomp: uRam0000000c);
      // dead for valid modes, AVs identically if ever reached.
      *(volatile uint*)0x0000000cu = *(volatile uint*)0x0000000cu & 0xfffdffffu;
      return;
    }
    param_1[0xc] = param_1[0xc] + *param_2;           // 0x004c51fc..0x004c5201
    param_1[0xd] = param_2[1] + param_1[0xd];
    param_1[0xe] = param_2[2] + param_1[0xe];
    { uint f; f = *(uint*)&param_1[3] & 0xfffdffffu; *(uint*)&param_1[3] = f; } // 0x004c5216
    return;
  }
  // op1 rows in DISASM summation order (0x004c522a..0x004c5277): ((m·v terms hi->lo) + pos).
  // The decomp prints the reversed order; x87 addition is order-sensitive. [X87]
  param_1[0xc] = param_1[8] * param_2[2] + param_1[4] * param_2[1] + *param_1 * *param_2 +
                 param_1[0xc];
  param_1[0xd] = param_1[9] * param_2[2] + param_1[5] * param_2[1] + param_1[1] * *param_2 +
                 param_1[0xd];
  param_1[0xe] = param_1[10] * param_2[2] + param_1[6] * param_2[1] + param_1[2] * *param_2 +
                 param_1[0xe];
  { uint f; f = *(uint*)&param_1[3] & 0xfffdffffu; *(uint*)&param_1[3] = f; }   // 0x004c527a
  return;
}

// ---------------------------------------------------------------------------
// 0x004c52f0  RwMatrixCombine(dst, src, combineOp): op0 copy; op1/op2 identity-flag
//             short-circuits (incl. the literal self-copy no-ops, kept verbatim), else
//             mult into a stack temp (op1: temp=src·dst, op2: temp=dst·src),
//             temp[3] = dst.flags & src.flags, copy temp -> dst. Error -> NULL.
// ---------------------------------------------------------------------------
extern "C" uint * __cdecl FUN_004c52f0(uint *param_1,uint *param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  int local_48[2];  // local_48 / local_44 error record pair (0x004c5318 / 0x004c5325)
  uint local_40 [16];

  if (param_3 == 0) {
    puVar4 = param_1;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *param_2;
      param_2 = param_2 + 1;
      puVar4 = puVar4 + 1;
    }
    return param_1;
  }
  if (param_3 == 1) {
    uVar3 = param_2[3];
    uVar6 = param_1[3];
    uVar1 = RWP_IDENTW & 0x20000;                     // 0x004c53c9 / 0x004c53d3
    if ((uVar3 & uVar1) != 0) {
      // literal self-copy param_1 -> temp -> param_1 (0x004c53dc..0x004c53f6)
      puVar4 = param_1;
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_2;
    puVar5 = param_1;
    if ((uVar6 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  else {
    if (param_3 != 2) {
      local_48[0] = 1;
      local_48[1] = FUN_004d7ff0_p(0x80000003,s_Invalid_combination_type_0061811c); // 0x004c5320
      FUN_004d8480_p(local_48);                                                     // 0x004c532e
      return (uint *)0x0;                                                           // 0x004c5336
    }
    uVar3 = param_1[3];
    uVar6 = param_2[3];
    uVar1 = RWP_IDENTW & 0x20000;                     // 0x004c5354 / 0x004c535e
    if ((uVar3 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_1;
    puVar5 = param_2;
    if ((uVar6 & uVar1) != 0) {
      // literal self-copy param_1 -> temp -> param_1 (0x004c538d..0x004c53a7)
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  RWP_MATMUL(local_40,puVar4,puVar5);                 // 0x004c542d call [slot+8]
  local_40[3] = uVar6 & uVar3;                        // 0x004c5434..0x004c5436
  puVar4 = local_40;
  puVar5 = param_1;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x00546b10  Matrix->quaternion (Shoemake): trace branch, else dispatch to the
//             largest-diagonal sibling (X: 0x546bf0, Y: 0x546c50, Z: 0x546cb0).
//             Returns 0 on a null arg, else 1.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl FUN_00546b10(float *param_1,float *param_2)

{
  float10 fVar1;
  void (__cdecl *pcVar2)(float*, float*);
  float10 fVar3;

  if ((param_1 == (float *)0x0) || (param_2 == (float *)0x0)) {
    return 0;
  }
  // trace stays on the x87 stack UNROUNDED through the FCOM and the +1.0
  // (0x00546b23..0x00546b46); disasm sum order (m[10] + m[0]) + m[5]. [X87]
  fVar1 = (float10)param_2[10] + (float10)*param_2 + (float10)param_2[5];
  if ((float10)DAT_005d757c < fVar1) {
    fVar3 = (float10)FUN_004c3b30((float)(fVar1 + (float10)_DAT_005cc320));  // 0x00546b49 [X87]
    param_1[3] = (float)((float10)_DAT_005cc32c * fVar3);
    fVar3 = (float10)_DAT_005cc32c / fVar3;                                  // [X87]
    *param_1 = (float)(((float10)param_2[6] - (float10)param_2[9]) * fVar3);
    param_1[1] = (float)(((float10)param_2[8] - (float10)param_2[2]) * fVar3);
    param_1[2] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar3);
    return 1;
  }
  if (*param_2 <= param_2[5]) {
    pcVar2 = FUN_00546c50;                            // 0x00546bcd
    if (param_2[10] < param_2[5]) goto LAB_00546bd9;
  }
  else if (param_2[10] < *param_2) {
    FUN_00546bf0(param_1,param_2);                    // 0x00546baf/0x00546bb5
    return 1;
  }
  pcVar2 = FUN_00546cb0;                              // 0x00546bd4
LAB_00546bd9:
  (*pcVar2)(param_1,param_2);
  return 1;
}

// ---------------------------------------------------------------------------
// 0x00546bf0  Matrix->quat, "X is largest" Shoemake branch.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00546bf0(float *param_1,float *param_2)

{
  float10 fVar1;

  // arg = m[0] - (m[10] + m[5]) + 1.0, single float32 rounding at the call
  // (FLD/FADD/FSUBR/FADD/FSTP 0x00546bf5..0x00546c03). [X87]
  fVar1 = (float10)FUN_004c3b30((float)((float10)*param_2 -
              ((float10)param_2[10] + (float10)param_2[5]) + (float10)_DAT_005cc320));
  *param_1 = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;                                    // [X87]
  param_1[3] = (float)(((float10)param_2[6] - (float10)param_2[9]) * fVar1);
  param_1[1] = (float)(((float10)param_2[4] + (float10)param_2[1]) * fVar1);
  param_1[2] = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  return;
}

// ---------------------------------------------------------------------------
// 0x00546c50  Matrix->quat, "Y is largest" Shoemake branch.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00546c50(float *param_1,float *param_2)

{
  float10 fVar1;

  // arg = m[5] - (m[10] + m[0]) + 1.0 (0x00546c55..0x00546c63). [X87]
  fVar1 = (float10)FUN_004c3b30((float)((float10)param_2[5] -
              ((float10)param_2[10] + (float10)*param_2) + (float10)_DAT_005cc320));
  param_1[1] = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;                                    // [X87]
  param_1[3] = (float)(((float10)param_2[8] - (float10)param_2[2]) * fVar1);
  param_1[2] = (float)(((float10)param_2[9] + (float10)param_2[6]) * fVar1);
  *param_1 = (float)(((float10)param_2[4] + (float10)param_2[1]) * fVar1);
  return;
}

// ---------------------------------------------------------------------------
// 0x00546cb0  Matrix->quat, "Z is largest" Shoemake branch.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00546cb0(float *param_1,float *param_2)

{
  float10 fVar1;

  // arg = m[10] - (m[5] + m[0]) + 1.0 (0x00546cb5..0x00546cc3). [X87]
  fVar1 = (float10)FUN_004c3b30((float)((float10)param_2[10] -
              ((float10)param_2[5] + (float10)*param_2) + (float10)_DAT_005cc320));
  param_1[2] = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;                                    // [X87]
  param_1[3] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar1);
  *param_1 = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  param_1[1] = (float)(((float10)param_2[9] + (float10)param_2[6]) * fVar1);
  return;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp; installs
//     the inline-JMP under the .asi for the diff-original A/B acceptance) — CLUSTER 2. ---
RH_ScopedInstall(FUN_004c4600, 0x004c4600);
RH_ScopedInstall(FUN_004c51a0, 0x004c51a0);
RH_ScopedInstall(FUN_004c52f0, 0x004c52f0);
RH_ScopedInstall(FUN_00546b10, 0x00546b10);
RH_ScopedInstall(FUN_00546bf0, 0x00546bf0);
RH_ScopedInstall(FUN_00546c50, 0x00546c50);
RH_ScopedInstall(FUN_00546cb0, 0x00546cb0);

}  // namespace Collision
}  // namespace mashed_re
