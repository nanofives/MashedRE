// Mashed RE — B5e: RwpSolver-island CLUSTER 6 clean-room port (per-body quaternion /
// orientation integration + the two body-list integration sweeps).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-16). VERBATIM transcription of the 6 K6 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverLeaves1.cpp (K1) / RwpSolverMath2.cpp (K2) /
// RwpSolverBroadphase3.cpp (K3) / RwpSolverCore4.cpp (K4) / RwpSolverGlue5.cpp (K5):
// extern "C" per-function, // 0x00xxxxxx RVA comments, absolute-address global binds,
// RH_ScopedInstall registration block.
//
// Members (rva:size): 0056bb80:337 0056bce0:260 0056bdf0:135 0056be80:534 0056c0a0:618
//   0056c310:614  — 6 fns / 2498 B, deps DONE (FUN_004c3b30 C4 FastSqrt, FUN_004c45f0 C3),
//   0 indirect sites.
//
// NO-GUESSING verifications against live pool0 disasm (2026-07-16):
//  1. Calling convention: all 6 end in a plain RET (0xC3, caller cleanup) => __cdecl. RET
//     bytes: 0x0056bcd0, 0x0056bde3, 0x0056be76, 0x0056c095, 0x0056c309, 0x0056c575.
//  2. Float constants (memory_read; bound below as absolute-address floats, K1 note-2 idiom):
//        _DAT_005cc320 = 0000803f = 1.0f          _DAT_005cc32c = 0000003f = 0.5f
//        _DAT_005cd03c = 17b7d138 = 1.0e-4f        _DAT_005cc574 = 00000040 = 2.0f
//  3. FUN_0056bb80 — the FSIN/FCOS at 0x0056bc0f/0x0056bc15 are HARDWARE x87 instructions on
//     the 80-bit angle held in ST0 (NOT library sinf/cosf, NOT a float-rounded reload). Ported
//     as an inline __asm block mirroring 0x0056bbff..0x0056bc17 instruction-for-instruction
//     (FLD sqrt; FMUL param_3; FMUL 0.5; FLD ST0; FSIN; FSTP sin; FCOS; FSTP cos) — the
//     RwMatrixRotate.cpp precedent. MSVC std::sin would emit the SSE2 polynomial (divergent).
//  4. FUN_0056bb80 — TWO summation orders differ from the decomp's printed left-assoc text
//     (same finding class as K2/K5); disasm groups them thus:
//       (a) FastSqrt argument (0x0056bb91..0x0056bb9f): a2^2 + (a1^2 + a0^2), i.e.
//           param_2[2]^2 + (param_2[1]^2 + *param_2 * *param_2)  — RIGHT-associated tail.
//       (b) param_1[3] flat 3-term sum (0x0056bc34..0x0056bc3a):
//           fVar5*fVar6 + (fVar4*fVar7 + fVar3*fVar8)            — RIGHT-associated tail.
//     The four output-quaternion lines' inner 2-term cross products were each re-verified
//     against the FSUBP order and MATCH the decomp exactly (fVar5*fVar8-fVar3*fVar6 @c50,
//     fVar4*fVar6-fVar5*fVar7 @c60, fVar3*fVar7-fVar4*fVar8 @c72); ABS(fVar9) = FABS
//     (0x0056bbb3) -> fabsf (K1 note-3).
//  5. FUN_0056bdf0 — plain __cdecl scale-add loop over param_4[1] bodies; per body it MAD-s
//     6 fields at offsets 0,4,8,0x10,0x14,0x18 (offset 0xc skipped) — verbatim, no correction.
//  6. FUN_0056c0a0 / FUN_0056c310 — the decompiler's `extraout_EDX` is the caller's own live
//     EDX after CALL FUN_0056bce0 (bce0 never writes EDX — verified across its whole body): at
//     0x0056c16a / 0x0056c40c the caller sets `EDX = iVar5*0x10 + param_3` (the quaternion ptr
//     it just handed bce0 as arg1), so extraout_EDX == that pointer (the SAME buffer bce0
//     normalized in place — cf. the FUN_0056be80 twin which reads pfVar7 directly). Ported as
//     an explicit `float* q = (float*)(iVar5*0x10 + param_3);`.
//  7. FUN_0056be80 / FUN_0056c0a0 / FUN_0056c310 share two grouping corrections, derived once
//     from each disasm and identical in shape across all three:
//       (a) pre-branch dot product (0x0056bed4 / 0x0056c0ff / 0x0056c367, each after a PUSH
//           that shifts ESP by 4): disasm groups (v[1]*d_mid + v[0]*d_first) + v[2]*d_last,
//           NOT the decomp's printed v[2]*d_last + v[0]*d_first + v[1]*d_mid.
//       (b) 4-term normalization denominator (0x0056bf6a / 0x0056c180 / 0x0056c3e7, all three
//           the SAME FLD/FMUL-ST/FADDP cascade — byte-identical): the binary combines
//           {q0^2,q1^2} first, then +q2^2, then +q3^2, i.e. ((q0^2+q1^2)+q2^2)+q3^2. The
//           C form q3^2 + (q2^2 + (q1^2 + q0^2)) is the COMMUTATIVE match (each FADD pair is
//           commutative and the combination tree is identical), hence bit-identical — whereas
//           the decomp's PRINTED order q3^2+q2^2+q1^2+q0^2 combines {q3,q2} first and is WRONG.
//           (Re-traced live on pool0 2026-07-17, all three sites.)
//     The remaining ~24 matrix-write stores are each <=2-term or already parenthesized as
//     _DAT_005cc320 - (a + b) in the decomp and MATCH the disasm store block verbatim.
//  8. FUN_0056be80 / FUN_0056c0a0 / FUN_0056c310 — the `pfVarN[3] = 4.2039e-45;` store is a
//     raw 32-bit MOV of the integer 3 (0x0056c022 / 0x0056c238 / 0x0056c4a0 =
//     `C7 4x 0C 03000000` MOV dword[reg+0xc],0x3) — a type tag, NOT a float computation.
//     Ported as `*(undefined4*)(pfVarN + 3) = 3;` so the exact bits 0x00000003 are written.
//  9. FUN_004c45f0 (C3, `*(u32*)(p+0xc) &= 0xfffdfffc`, Util/PromoLoop_sessionB.cpp) is called
//     via the RVA-fn-ptr idiom — under the .asi it converges on the ported/hooked body (same
//     rationale as RwpSolverMath2.cpp's error-funnel forwards). Standalone rebind = KV/lane-end.
//
// x87 note: 80-bit ST0 chains carry the accepted <=1-ULP angular floor under MSVC's 64-bit
// long double (project_phys_chain_float10_methodology). Build is x87 (no /arch:SSE2), so the
// plain-float product/sum expressions below accumulate in 80-bit and round once per store,
// matching the original FMUL/FADDP/FSTP stream.
#include "../Core/HookSystem.h"
#include <cmath>                     // std::sqrtl — x87 FSQRT floor (RwpSolverLeaves1.cpp idiom)

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim (as RwpSolverLeaves1.cpp). ---
typedef unsigned char  byte;
typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef long double    float10;     // x87 80-bit extended — MSVC = 64-bit double [X87]

// --- Float constants (absolute-address binds; values proven in header note 2). ---
#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cd03c  (*(const float*)0x005cd03cu)   // 1.0e-4f
#define _DAT_005cc574  (*(const float*)0x005cc574u)   // 2.0f

// --- 0x004c3b30 — C4 FastSqrt port (Math/RwSqrt.cpp), called directly (K2 idiom). ---
extern "C" float __cdecl FastSqrt(float x);
#define FUN_004c3b30  FastSqrt

// --- 0x004c45f0 — C3 orthonormal-flag clearer (Util/PromoLoop_sessionB.cpp): p[3] &=
//     0xfffdfffc. RVA-fn-ptr forward (converges on the ported body under the .asi). ---
typedef void (__cdecl *RwpClearFlagsFn)(void*);
#define FUN_004c45f0(m)  ((reinterpret_cast<RwpClearFlagsFn>(0x004c45f0u))((void*)(m)))

// --- Cluster-internal forward decls (be80/c0a0/c310 call bb80 and bce0). ---
extern "C" void __cdecl FUN_0056bb80(float *param_1, float *param_2, float param_3);
extern "C" void __cdecl FUN_0056bce0(float *param_1, float *param_2, float param_3);

// ---------------------------------------------------------------------------
// 0x0056bb80  Quaternion rotate-by-axis-angle: normalizes the axis param_2 (length via
//             FastSqrt), forms the half-angle |axis|*param_3*0.5, and rotates the unit
//             quaternion param_1 about the normalized axis by that angle. No-op when the
//             axis length is below _DAT_005cd03c (1e-4). Header notes 3, 4.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056bb80(float *param_1,float *param_2,float param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float10 fVar9;

  // note 4(a): FastSqrt argument groups a2^2 + (a1^2 + a0^2).
  fVar9 = (float10)FUN_004c3b30(param_2[2] * param_2[2] +
                                (param_2[1] * param_2[1] + *param_2 * *param_2));
  if (_DAT_005cd03c <= (float)((fVar9 < (float10)0.0) ? -fVar9 : fVar9)) {   // ABS => FABS
    fVar6 = _DAT_005cc320 / (float)fVar9;
    fVar3 = param_1[1];
    fVar4 = *param_1;
    fVar5 = param_1[2];
    fVar7 = fVar6 * *param_2;
    fVar8 = fVar6 * param_2[1];
    fVar6 = fVar6 * param_2[2];

    // note 3: half-angle * FSIN/FCOS kept in one x87 block (angle stays 80-bit into FSIN).
    float sr = (float)fVar9;         // FastSqrt result (float), = [ESP+0x30]
    float sinv = 0.0f;
    float cosv = 0.0f;
    __asm {
        fld   dword ptr [sr]
        fmul  dword ptr [param_3]
        fmul  dword ptr ds:[0x005cc32cu]     // * 0.5f  (_DAT_005cc32c)
        fld   st(0)
        fsin
        fstp  dword ptr [sinv]
        fcos
        fstp  dword ptr [cosv]
    }
    fVar1 = sinv;
    fVar2 = cosv;

    *param_1     = fVar4 * fVar2 + (fVar7 * param_1[3] + (fVar5 * fVar8 - fVar3 * fVar6)) * fVar1;
    param_1[1]   = fVar3 * fVar2 + (fVar8 * param_1[3] + (fVar4 * fVar6 - fVar5 * fVar7)) * fVar1;
    param_1[2]   = fVar5 * fVar2 + (fVar6 * param_1[3] + (fVar3 * fVar7 - fVar4 * fVar8)) * fVar1;
    // note 4(b): param_1[3] tail sum groups fVar5*fVar6 + (fVar4*fVar7 + fVar3*fVar8).
    param_1[3]   = fVar2 * param_1[3] - (fVar5 * fVar6 + (fVar4 * fVar7 + fVar3 * fVar8)) * fVar1;
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056bce0  Quaternion small-angle integrate + renormalize: dq = 0.5*param_3*(omega x q),
//             q += dq, then q *= 1/|q| (FSQRT). Writes the 4 quaternion floats back to
//             param_1 in the order [3],[0],[1],[2]. Printed order matches disasm (inner
//             adds commutative) — verbatim, no correction.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056bce0(float *param_1,float *param_2,float param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;

  fVar2 = ((-(*param_1 * *param_2) - param_2[1] * param_1[1]) - param_2[2] * param_1[2]) *
          _DAT_005cc32c * param_3 + param_1[3];
  fVar3 = ((param_1[3] * *param_2 + param_2[1] * param_1[2]) - param_2[2] * param_1[1]) *
          _DAT_005cc32c * param_3 + *param_1;
  fVar5 = (param_2[2] * *param_1 + (param_2[1] * param_1[3] - param_1[2] * *param_2)) *
          _DAT_005cc32c * param_3 + param_1[1];
  fVar1 = (param_2[2] * param_1[3] + (param_1[1] * *param_2 - param_2[1] * *param_1)) *
          _DAT_005cc32c * param_3 + param_1[2];
  fVar4 = _DAT_005cc320 / (float)std::sqrtl((float10)(fVar2 * fVar2 + fVar1 * fVar1 +
                                                 fVar5 * fVar5 + fVar3 * fVar3));  // FSQRT [X87]
  param_1[3] = fVar2 * fVar4;
  *param_1   = fVar3 * fVar4;
  param_1[1] = fVar5 * fVar4;
  param_1[2] = fVar1 * fVar4;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056bdf0  Body-list scaled accumulate: for each of param_4[1] indices, MAD the 6 velocity
//             fields (offsets 0,4,8,0x10,0x14,0x18 of a 0x20-byte record) from source base
//             param_3 into dest base param_1 by param_5. Header note 5 — verbatim.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056bdf0(int param_1,undefined4 param_2,int param_3,int *param_4,float param_5)
{
  float *pfVar1;
  int iVar2;
  int iVar3;

  iVar3 = 0;
  if (param_4[1] != 0) {
    do {
      iVar2 = *(int *)(*param_4 + iVar3 * 4) * 0x20;
      iVar3 = iVar3 + 1;
      pfVar1 = (float *)(iVar2 + 0x18 + param_1);
      *(float *)(iVar2 + param_1) =
           param_5 * *(float *)(iVar2 + param_3) + *(float *)(iVar2 + param_1);
      *(float *)(iVar2 + 4 + param_1) =
           param_5 * *(float *)(iVar2 + 4 + param_3) + *(float *)(iVar2 + 4 + param_1);
      *(float *)(iVar2 + 8 + param_1) =
           param_5 * *(float *)(iVar2 + 8 + param_3) + *(float *)(iVar2 + 8 + param_1);
      *(float *)(iVar2 + 0x10 + param_1) =
           param_5 * *(float *)(iVar2 + 0x10 + param_3) + *(float *)(iVar2 + 0x10 + param_1);
      *(float *)(iVar2 + 0x14 + param_1) =
           param_5 * *(float *)(iVar2 + 0x14 + param_3) + *(float *)(iVar2 + 0x14 + param_1);
      *pfVar1 = param_5 * *(float *)(iVar2 + 0x18 + param_3) + *pfVar1;
    } while (iVar3 != param_4[1]);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056be80  Single-body orientation+matrix update: optionally removes the constraint
//             component of the angular velocity (branch on param_1[6] flag), integrates the
//             orientation quaternion (FUN_0056bb80 / FUN_0056bce0), advances position, then
//             rebuilds the body's 0x40-byte transform (rotation matrix from the quaternion,
//             raw-3 tag at +0xc) and finalizes flags via FUN_004c45f0. Header notes 7, 8.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056be80(int *param_1,int param_2,float param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  float *pfVar6;
  float *pfVar7;
  float *pfVar8;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_c;

  pfVar5 = (float *)(param_2 * 0x20 + param_1[2]);
  local_3c = pfVar5[4];
  pfVar8 = (float *)(param_2 * 0x40 + *param_1);
  local_38 = pfVar5[5];
  local_34 = pfVar5[6];
  pfVar7 = (float *)(param_1[4] + param_2 * 0x10);
  if ((*(byte *)(param_1[6] + param_2 * 4) & 1) != 0) {
    pfVar6 = (float *)(param_1[0x14] + param_2 * 0x10);
    // note 7(a): dot groups (pfVar6[1]*local_38 + local_3c*pfVar6[0]) + pfVar6[2]*local_34.
    fVar1 = (pfVar6[1] * local_38 + local_3c * *pfVar6) + pfVar6[2] * local_34;
    FUN_0056bb80(pfVar7,pfVar6,fVar1 * param_3);
    fVar1 = -fVar1;
    local_3c = fVar1 * *pfVar6 + local_3c;
    local_38 = pfVar6[1] * fVar1 + local_38;
    local_34 = pfVar6[2] * fVar1 + local_34;
  }
  FUN_0056bce0(pfVar7,&local_3c,param_3);
  local_24 = param_3 * *pfVar5 + pfVar8[0xc];
  local_20 = param_3 * pfVar5[1] + pfVar8[0xd];
  local_1c = param_3 * pfVar5[2] + pfVar8[0xe];
  // note 7(b): normalization denominator is fully right-associated.
  local_c = _DAT_005cc574 /
            (pfVar7[3] * pfVar7[3] +
            (pfVar7[2] * pfVar7[2] + (pfVar7[1] * pfVar7[1] + *pfVar7 * *pfVar7)));
  fVar3 = local_c * *pfVar7;
  local_14 = pfVar7[1] * local_c;
  local_c = pfVar7[2] * local_c;
  local_3c = pfVar7[3] * fVar3;
  local_38 = pfVar7[3] * local_14;
  local_34 = pfVar7[3] * local_c;
  local_30 = fVar3 * *pfVar7;
  local_2c = local_14 * pfVar7[1];
  local_28 = local_c * pfVar7[2];
  local_c = local_c * pfVar7[1];
  fVar1 = pfVar7[2];
  fVar2 = *pfVar7;
  pfVar8[0xc] = 0.0;
  pfVar8[0xd] = 0.0;
  pfVar8[0xe] = 0.0;
  fVar4 = _DAT_005cc320 - (local_28 + local_2c);
  pfVar8[0xc] = local_24;
  pfVar8[0xd] = local_20;
  *(undefined4 *)(pfVar8 + 3) = 3;                 // note 8: raw int 3 (was 4.2039e-45)
  pfVar8[0xe] = local_1c;
  *pfVar8 = fVar4;
  pfVar8[1] = local_34 + local_14 * fVar2;
  pfVar8[2] = fVar3 * fVar1 - local_38;
  pfVar8[4] = local_14 * fVar2 - local_34;
  pfVar8[5] = _DAT_005cc320 - (local_28 + local_30);
  pfVar8[6] = local_c + local_3c;
  pfVar8[8] = fVar3 * fVar1 + local_38;
  pfVar8[9] = local_c - local_3c;
  pfVar8[10] = _DAT_005cc320 - (local_2c + local_30);
  FUN_004c45f0(pfVar8);
  return;
}

// ---------------------------------------------------------------------------
// 0x0056c0a0  Body-list orientation+matrix update (indexed-source variant): the be80 body run
//             over param_6[1] indices, source angular velocity from param_5/param_7, position
//             base param_1, flag byte at param_9. Header notes 6, 7, 8.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056c0a0(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
                 int *param_6,int param_7,undefined4 param_8,int param_9,undefined4 param_10,
                 float param_11)
{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float *pfVar9;
  float *q;                         // note 6: was extraout_EDX = iVar5*0x10 + param_3
  int iVar10;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_c;

  iVar10 = 0;
  if (param_6[1] != 0) {
    do {
      iVar5 = *(int *)(*param_6 + iVar10 * 4);
      local_48 = *(float *)(iVar5 * 0x20 + 0x10 + param_5);
      pfVar1 = (float *)(iVar5 * 0x20 + param_5);
      local_44 = pfVar1[5];
      local_40 = pfVar1[6];
      if ((*(byte *)(param_9 + iVar5 * 4) & 1) != 0) {
        iVar8 = iVar5 * 0x10;
        pfVar2 = (float *)(iVar8 + param_7);
        // note 7(a): dot groups (pfVar2[1]*local_44 + pfVar2[0]*local_48) + pfVar2[2]*local_40.
        fVar3 = (*(float *)(iVar8 + 4 + param_7) * local_44 + *pfVar2 * local_48) +
                pfVar2[2] * local_40;
        FUN_0056bb80((float *)(iVar8 + param_3),pfVar2,fVar3 * param_11);
        local_48 = local_48 - fVar3 * *pfVar2;
        local_44 = local_44 - fVar3 * pfVar2[1];
        local_40 = local_40 - fVar3 * pfVar2[2];
      }
      FUN_0056bce0((float *)(iVar5 * 0x10 + param_3),&local_48,param_11);
      q = (float *)(iVar5 * 0x10 + param_3);       // note 6: bce0's normalized quaternion (EDX)
      pfVar9 = (float *)(param_1 + iVar5 * 0x40);
      local_24 = pfVar9[0xc];
      local_20 = pfVar9[0xd];
      // note 7(b): denom parenthesized to the FADD-cascade combination order (commutative match
      //   to the binary's ((q0^2+q1^2)+q2^2)+q3^2 at 0x0056c180; verified live 2026-07-17).
      local_c = _DAT_005cc574 /
                (q[3] * q[3] + (q[2] * q[2] + (q[1] * q[1] + *q * *q)));
      local_1c = pfVar9[0xe];
      fVar6 = *q * local_c;
      local_14 = q[1] * local_c;
      local_c = q[2] * local_c;
      local_3c = q[3] * fVar6;
      local_38 = q[3] * local_14;
      local_34 = q[3] * local_c;
      local_30 = fVar6 * *q;
      local_2c = local_14 * q[1];
      local_28 = local_c * q[2];
      local_c = local_c * q[1];
      fVar3 = q[2];
      fVar4 = *q;
      *(undefined4 *)(pfVar9 + 3) = 3;             // note 8: raw int 3 (was 4.2039e-45)
      pfVar9[0xc] = 0.0;
      pfVar9[0xd] = 0.0;
      fVar7 = _DAT_005cc320 - (local_28 + local_2c);
      pfVar9[0xe] = 0.0;
      *pfVar9 = fVar7;
      pfVar9[1] = local_34 + local_14 * fVar4;
      pfVar9[2] = fVar6 * fVar3 - local_38;
      pfVar9[4] = local_14 * fVar4 - local_34;
      pfVar9[5] = _DAT_005cc320 - (local_28 + local_30);
      pfVar9[6] = local_c + local_3c;
      pfVar9[8] = fVar6 * fVar3 + local_38;
      pfVar9[9] = local_c - local_3c;
      pfVar9[10] = _DAT_005cc320 - (local_2c + local_30);
      // position advance: pos = vel(pfVar1)*dt then += saved old pos (local_24/20/1c).
      pfVar9[0xc] = *pfVar1 * param_11;
      pfVar9[0xd] = pfVar1[1] * param_11;
      pfVar9[0xe] = pfVar1[2] * param_11;
      pfVar9[0xc] = pfVar9[0xc] + local_24;
      pfVar9[0xd] = pfVar9[0xd] + local_20;
      pfVar9[0xe] = pfVar9[0xe] + local_1c;
      FUN_004c45f0(pfVar9);
      iVar10 = iVar10 + 1;
    } while (iVar10 != param_6[1]);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056c310  Body-list orientation+matrix update (walked-source variant): as FUN_0056c0a0
//             but the source angular velocity is walked through param_5 by 0x20 bytes per
//             body (pfVar10 += 8 floats). Header notes 6, 7, 8.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056c310(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
                 undefined4 param_6,undefined4 param_7,int param_8,int *param_9,int param_10,
                 undefined4 param_11,float param_12)
{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float *q;                         // note 6: was extraout_EDX = iVar5*0x10 + param_3
  int iVar9;
  float *pfVar10;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_c;

  iVar9 = 0;
  if (param_9[1] != 0) {
    pfVar10 = (float *)(param_5 + 0x14);
    do {
      local_48 = pfVar10[-1];
      local_44 = *pfVar10;
      iVar5 = *(int *)(*param_9 + iVar9 * 4);
      local_40 = pfVar10[1];
      if ((*(byte *)(param_10 + iVar5 * 4) & 1) != 0) {
        iVar8 = iVar5 * 0x10;
        pfVar1 = (float *)(iVar8 + param_8);
        // note 7(a): dot groups (pfVar1[1]*local_44 + pfVar1[0]*local_48) + pfVar1[2]*local_40.
        fVar3 = (*(float *)(iVar8 + 4 + param_8) * local_44 + *pfVar1 * local_48) +
                pfVar1[2] * local_40;
        FUN_0056bb80((float *)(iVar8 + param_3),pfVar1,fVar3 * param_12);
        local_48 = local_48 - fVar3 * *pfVar1;
        local_44 = local_44 - fVar3 * pfVar1[1];
        local_40 = local_40 - fVar3 * pfVar1[2];
      }
      FUN_0056bce0((float *)(iVar5 * 0x10 + param_3),&local_48,param_12);
      q = (float *)(iVar5 * 0x10 + param_3);       // note 6: bce0's normalized quaternion (EDX)
      pfVar2 = (float *)(iVar5 * 0x40 + param_1);
      local_24 = pfVar2[0xc];
      local_20 = pfVar2[0xd];
      // note 7(b): denom parenthesized to the FADD-cascade combination order (commutative match
      //   to the binary's ((q0^2+q1^2)+q2^2)+q3^2 at 0x0056c3e7; verified live 2026-07-17).
      local_c = _DAT_005cc574 /
                (q[3] * q[3] + (q[2] * q[2] + (q[1] * q[1] + *q * *q)));
      local_1c = pfVar2[0xe];
      fVar6 = *q * local_c;
      local_14 = q[1] * local_c;
      local_c = q[2] * local_c;
      local_3c = q[3] * fVar6;
      local_38 = q[3] * local_14;
      local_34 = q[3] * local_c;
      local_30 = fVar6 * *q;
      local_2c = local_14 * q[1];
      local_28 = local_c * q[2];
      local_c = local_c * q[1];
      fVar3 = q[2];
      fVar4 = *q;
      *(undefined4 *)(pfVar2 + 3) = 3;             // note 8: raw int 3 (was 4.2039e-45)
      pfVar2[0xc] = 0.0;
      pfVar2[0xd] = 0.0;
      fVar7 = _DAT_005cc320 - (local_28 + local_2c);
      pfVar2[0xe] = 0.0;
      *pfVar2 = fVar7;
      pfVar2[1] = local_34 + local_14 * fVar4;
      pfVar2[2] = fVar6 * fVar3 - local_38;
      pfVar2[4] = local_14 * fVar4 - local_34;
      pfVar2[5] = _DAT_005cc320 - (local_28 + local_30);
      pfVar2[6] = local_c + local_3c;
      pfVar2[8] = fVar6 * fVar3 + local_38;
      pfVar2[9] = local_c - local_3c;
      pfVar2[10] = _DAT_005cc320 - (local_2c + local_30);
      // position advance: pos = walked-source vel(pfVar10[-5..-3])*dt then += saved old pos.
      pfVar2[0xc] = pfVar10[-5] * param_12;
      pfVar2[0xd] = pfVar10[-4] * param_12;
      pfVar2[0xe] = pfVar10[-3] * param_12;
      pfVar2[0xc] = pfVar2[0xc] + local_24;
      pfVar2[0xd] = pfVar2[0xd] + local_20;
      pfVar2[0xe] = pfVar2[0xe] + local_1c;
      FUN_004c45f0(pfVar2);
      iVar9 = iVar9 + 1;
      pfVar10 = pfVar10 + 8;
    } while (iVar9 != param_9[1]);
  }
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 6. ---
RH_ScopedInstall(FUN_0056bb80, 0x0056bb80);
RH_ScopedInstall(FUN_0056bce0, 0x0056bce0);
RH_ScopedInstall(FUN_0056bdf0, 0x0056bdf0);
RH_ScopedInstall(FUN_0056be80, 0x0056be80);
RH_ScopedInstall(FUN_0056c0a0, 0x0056c0a0);
RH_ScopedInstall(FUN_0056c310, 0x0056c310);

}  // namespace Collision
}  // namespace mashed_re
