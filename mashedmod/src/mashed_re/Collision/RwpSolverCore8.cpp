// Mashed RE — B5e: RwpSolver-island CLUSTER 8 clean-room port (the 0x0056d/e constraint
// pre-solve stage: the row-transform helper, the x87-scalar Gram/normalize pass, its SSE2
// vectorized twin, and the 3x3 congruence transform + its driver).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-17). VERBATIM transcription of the 5 K8 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverCore7.cpp (K7): extern "C" per-function, // 0x00xxxxxx
// RVA comments, cluster-internal forward decls, RH_ScopedInstall block.
//
// Members (rva:size): 0056d350:158 0056d3f0:2375 0056dd40:2357 0056ed60:463 0056e680:1756
//   — 5 fns / 7109 B. Within-cluster deps: d3f0->d350, e680->ed60 (both confirmed via
//   function_callees). dd40 is the SSE2 twin of the x87-scalar d3f0 — BOTH are called by the
//   un-ported dispatcher FUN_00560260 (K13) which CPU-branches scalar (d3f0 @0x00560979) vs SSE
//   (dd40 @0x005608ae); only one arm runs per CPU. dd40 ported via SSE intrinsics per owner
//   decision (highest fidelity — reproduces rsqrtps + Newton, not an exact-sqrt substitute).
//
// NO-GUESSING verifications against live pool14 disasm (2026-07-17):
//  1. Calling convention: all 5 end in a plain RET (0xC3) => __cdecl. RET bytes:
//     0x0056d3ec (d350), 0x0056dd35 (d3f0 body_end 0056dd36), 0x0056e674 (dd40),
//     0x0056ed5b (e680), 0x0056ef2e (ed60). (dd40/e680/ed60 verified: 0x0056e670 = 5E 8B E5 5D
//     C3, 0x0056ed57 = 5B 83 C4 68 C3, 0x0056ef2b = 83 C4 18 C3.)
//  2. FUN_0056d350 three 3-term row sums (param_2[4]/[5]/[6]) group REVERSE-PAIR:
//     (v[0x18]*p[k+2] + v[0x14]*p[k+1]) + v[0x10]*p[k]  — FADDP chain @0x0056d3a5/d3ac,
//     d3c1/d3ca, d3df/d3e7 (the two higher-offset products combined first, then + the low one),
//     NOT the decomp's printed low-to-high left-assoc. CORRECTED below. param_2[0/1/2] are single
//     FMULs (v*p[3]). __cdecl (RET @0x0056d3ec).
//  3. FUN_0056d3f0 is a __cdecl with NO register params and MANY stack args at [ESP+0x14..0x74]
//     (Ghidra `parameters:[]`, body reads in_stack_00000014.., 12-byte-strided — the caller
//     FUN_00560260 pushes vec3-ish structs, d3f0 reads specific dwords). Ported with a positional
//     dword arg list so each used offset lands: param_1@0x04, in_stack_00000014@0x14,
//     _20@0x20, _2c@0x2c, _38@0x38, _44@0x44, _50@0x50, _5c@0x5c, _60@0x60, _68@0x68, _74@0x74
//     (pads fill the gaps). SQRT() = x87 FSQRT -> 1.0f/sqrtf (<=1-ULP x87 floor). DAT_005d757c =
//     0.0f (memory_read 0x005d757c = 00000000); the FLT_MAX sentinels (3.4028235e+38 /
//     -3.4028235e+38 / int bits 0x7f7fffff / -0x800001) kept verbatim. The 6-term square-sum
//     reductions retain the decomp order (all-positive sum of squares; <=1-ULP x87 floor -> U-9020).
//  4. FUN_0056dd40 (SSE2 twin): the `(float)(_DAT_005e5a60 & X)` idioms are `_mm_and_ps` lane
//     masks — memory_read 0x005e5a60 = FF FF FF FF | FF FF FF FF | FF FF FF FF | 00 00 00 00, i.e.
//     lanes 0-2 kept (identity), lane 3 zeroed. Only lanes 0-2 appear in the decomp's 3-term dots
//     => each mask read is an identity float reinterpret (ported as *(float*)&int). The four
//     reciprocal-sqrts use rsqrtps + one Newton step: _DAT_005e5a20 = 3.0f x4, _DAT_005e5a30 =
//     0.5f x4 (memory_read 0x005e5a20) => y = 0.5*seed*(3 - x*seed*seed) with seed = rsqrtps(x),
//     masked to 0 where x==0 — ported per-lane via _mm_rsqrt_ss (bit-identical seed to rsqrtps).
//     _DAT_005e5a40 = 1.0f x4 (the x==sentinel default scale); _DAT_005e5a70 = 0x80000000 x4 so
//     (_DAT_005e5a70 ^ 0x7f7fffff) = 0xFF7FFFFF = -FLT_MAX (the "unset" min sentinel). __cdecl.
//  5. FUN_0056ed60 (3x3 congruence M_out = A^T . B . A over the +0x10-strided rows / +0/4/8 cols):
//     18 three-term dot products, FADDP-chained (e.g. fVar1 @0x0056ed75/ed7d = (C+B)+A). Data-flow
//     transcribed verbatim; fine associativity carries the <=1-ULP x87 floor (U-9020). __cdecl.
//  6. FUN_0056e680 (scalar, calls ed60): per-body 3x3 transforms + cross products + a SQRT
//     renormalize gated by PTR_DAT_005ceabc (= FLT_MIN, memory_read 0x005ceabc = 00008000) and
//     _DAT_005cc564 (= 0.25f, memory_read 0x005cc564 = 0000803e); _DAT_005cc32c = 0.5f,
//     _DAT_005cc320 = 1.0f. Data-flow verbatim; multi-term sums keep decomp order (U-9020). __cdecl.
//
// DEEP-STACK / SSE fine-associativity note: the multi-term x87 reductions in d3f0/ed60/e680 and
// dd40's horizontal sums retain the decomp's printed order except d350 (traced + corrected). They
// carry the accepted <=1-ULP x87 partial-rounding floor -> U-9020, settled at the B5e lane-end
// whole-loop per-field body-state diff (NOT a C2 gate; project_phys_chain_float10_methodology).
// dd40's rsqrtps approximation IS reproduced exactly (rsqrt_ss + Newton), not floored.
#include "../Core/HookSystem.h"
#include <xmmintrin.h>              // _mm_rsqrt_ss — reproduce FUN_0056dd40's rsqrtps seed
#include <cmath>                    // sqrtf — d3f0/e680 x87 FSQRT floor
#include <cstring>                  // memcpy — bit-reinterpret for dd40 lane/scalar reads

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;

// --- Float constants (absolute-address binds; values proven in header notes 3/4/6). ---
#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cc564  (*(const float*)0x005cc564u)   // 0.25f
#define PTR_DAT_005ceabc (*(const float*)0x005ceabcu) // FLT_MIN threshold
#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f

// reinterpret raw 32-bit as float (dd40 identity-mask lane reads: `_mm_and_ps` w/ all-ones mask,
// and the puVar8[3] scalar which the original loads via MOV+MOVSS = bit reinterpret @0x0056de15).
static inline float asf(unsigned int i) { float f; memcpy(&f, &i, 4); return f; }

// rsqrtps + one Newton-Raphson step, masked to 0 at x==0 (FUN_0056dd40 lane law, note 4).
static inline float rsqrt_nr(float x)
{
  float s = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));   // rsqrtps seed (per-lane == rsqrtss)
  s = 0.5f * s * (3.0f - x * s * s);                      // _DAT_005e5a30 / _DAT_005e5a20
  return (x != 0.0f) ? s : 0.0f;                          // -(uint)(x!=0) & ...
}

// --- Cluster-internal forward decls. ---
extern "C" void __cdecl FUN_0056d350(int *param_1,float *param_2,float *param_3,int param_4,
                                     int param_5,int param_6);
extern "C" void __cdecl FUN_0056ed60(float *param_1,float *param_2,float *param_3);

// ---------------------------------------------------------------------------
// 0x0056d350  Row-transform helper: scales row3 of the (param_4/param_5/param_6-indexed) 0x20-byte
//             block by param_3[3], and forms three 3-term dot products of rows 4/5/6 with the
//             param_3 sub-vectors. Header note 2 (reverse-pair grouping on the dots).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056d350(int *param_1,float *param_2,float *param_3,int param_4,
                                     int param_5,int param_6)
{
  int iVar1 = (param_6 + (param_5 + param_4 * 4) * 2) * 0x20;
  float *b = (float *)(*param_1 + iVar1);
  param_2[0] = b[0] * param_3[3];
  param_2[1] = b[1] * param_3[3];
  param_2[2] = b[2] * param_3[3];
  // note 2: (v6*p2 + v5*p1) + v4*p0  — two high-offset products first, then + low.
  param_2[4] = (b[6] * param_3[2] + b[5] * param_3[1]) + b[4] * param_3[0];
  param_2[5] = (b[6] * param_3[6] + b[5] * param_3[5]) + b[4] * param_3[4];
  param_2[6] = (b[6] * param_3[10] + b[5] * param_3[9]) + b[4] * param_3[8];
  return;
}

// ---------------------------------------------------------------------------
// 0x0056d3f0  x87-scalar constraint normalize pass (twin of the SSE FUN_0056dd40): per body in
//             the [in_stack_00000060] list, accumulates 4 squared row-norms via FUN_0056d350,
//             adds the dt*diag term, 1/sqrt-normalizes each (guarded on !=0), then scales the
//             jacobian rows and the min/max-bound arrays by the resulting factors with FLT_MAX
//             sentinel guards. Header note 3 (positional stack-arg ABI). Verbatim data-flow.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056d3f0(
    int param_1, int a1_, int a2_, int a3_,
    int in_stack_00000014, int a5_, int a6_,
    int in_stack_00000020, int a8_, int a9_,
    int in_stack_0000002c, int a11_, int a12_,
    int in_stack_00000038, int a14_, int a15_,
    int in_stack_00000044, int a17_, int a18_,
    int in_stack_00000050, int a20_, int a21_,
    int in_stack_0000005c, int in_stack_00000060, int a24_,
    int in_stack_00000068, int a26_, int a27_,
    float in_stack_00000074)
{
  (void)a1_;(void)a2_;(void)a3_;(void)a5_;(void)a6_;(void)a8_;(void)a9_;(void)a11_;(void)a12_;
  (void)a14_;(void)a15_;(void)a17_;(void)a18_;(void)a20_;(void)a21_;(void)a24_;(void)a26_;(void)a27_;

  float fVar1,fVar2,fVar3;
  float *pfVar4;
  int iVar5;
  float *pfVar6,*pfVar7,*pfVar8,*pfVar9,*pfVar10;
  float local_94,local_90,local_8c,local_88;
  // d350 output buffer — ONE contiguous stack block (K7 contiguous-block class): d350 writes
  // param_2[0/1/2/4/5/6]; index 3 (offset -0x78) is a gap. Was 6 separate locals -> C4700.
  float local_84[7];
  int local_64,local_60;
  int *local_5c;
  float local_58,local_54,local_50,local_4c;
  int local_48,local_44,local_40,local_3c,local_38,local_34,local_30,local_2c,local_28,local_24,
      local_20,local_1c,local_18,local_14,local_10,local_c,local_8,local_4;

  local_64 = 0;
  if (in_stack_00000060 != 0) {
    pfVar10 = (float *)(in_stack_00000020 + 0xc);
    local_30 = in_stack_00000020 - in_stack_00000014;
    local_4 = in_stack_0000002c - in_stack_00000014;
    local_3c = in_stack_00000038 - in_stack_00000014;
    pfVar9 = (float *)(in_stack_00000014 + 8);
    local_48 = in_stack_00000044 - in_stack_00000014;
    local_20 = in_stack_00000050 - in_stack_00000014;
    local_5c = (int *)(param_1 + 0x2c);
    local_24 = in_stack_0000005c - in_stack_00000014;
    local_10 = in_stack_0000002c - in_stack_00000020;
    local_18 = in_stack_00000038 - in_stack_00000020;
    local_44 = in_stack_00000044 - in_stack_00000020;
    local_38 = in_stack_00000050 - in_stack_00000020;
    local_1c = in_stack_0000005c - in_stack_00000020;
    local_c = in_stack_0000002c - in_stack_00000038;
    local_40 = in_stack_00000044 - in_stack_00000038;
    local_8 = in_stack_00000050 - in_stack_00000038;
    local_2c = in_stack_0000005c - in_stack_00000038;
    local_14 = in_stack_0000002c - in_stack_00000044;
    local_28 = in_stack_00000050 - in_stack_00000044;
    local_34 = in_stack_0000005c - in_stack_00000044;
    pfVar6 = (float *)(in_stack_00000038 + 4);
    pfVar7 = (float *)in_stack_00000044;
    do {
      local_94 = 0.0f; local_90 = 0.0f; local_8c = 0.0f; local_88 = 0.0f;
      local_58 = 0.0f; local_54 = 0.0f; local_50 = 0.0f; local_4c = 0.0f;
      if (local_5c[-8] != -1) {
        local_60 = local_5c[-8] * 0x30 + in_stack_00000068;
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,0,0);
        local_94 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6];
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,1,0);
        local_90 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6];
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,2,0);
        local_8c = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6];
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,3,0);
        local_88 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6];
      }
      if (*local_5c != -1) {
        local_60 = *local_5c * 0x30 + in_stack_00000068;
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,0,1);
        local_94 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6] + local_94;
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,1,1);
        local_90 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6] + local_90;
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,2,1);
        local_8c = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6] + local_8c;
        FUN_0056d350((int*)&param_1,local_84,(float*)local_60,local_64,3,1);
        local_88 = local_84[0]*local_84[0] + local_84[1]*local_84[1] + local_84[2]*local_84[2] +
                   local_84[4]*local_84[4] + local_84[5]*local_84[5] + local_84[6]*local_84[6] + local_88;
      }
      fVar1 = in_stack_00000074 * *pfVar7 + local_94;
      fVar2 = in_stack_00000074 * *(float *)(local_40 + (int)pfVar6) + local_90;
      fVar3 = in_stack_00000074 * *(float *)(local_48 + (int)pfVar9) + local_8c;
      local_88 = in_stack_00000074 * *(float *)(local_44 + (int)pfVar10) + local_88;
      local_94 = fVar1;
      if (fVar1 != DAT_005d757c) { local_94 = _DAT_005cc320 / sqrtf(fVar1); local_58 = fVar1 * local_94; }
      local_90 = fVar2;
      if (fVar2 != DAT_005d757c) { local_90 = _DAT_005cc320 / sqrtf(fVar2); local_54 = fVar2 * local_90; }
      local_8c = fVar3;
      if (fVar3 != DAT_005d757c) { local_8c = _DAT_005cc320 / sqrtf(fVar3); local_50 = fVar3 * local_8c; }
      fVar1 = local_88;
      if (local_88 != DAT_005d757c) { fVar1 = _DAT_005cc320 / sqrtf(local_88); local_4c = local_88 * fVar1; }
      iVar5 = 2;
      pfVar4 = (float *)(local_5c + -9);
      do {
        iVar5 = iVar5 + -1;
        pfVar4[-2] = local_94 * pfVar4[-2];
        pfVar4[-1] = local_94 * pfVar4[-1];
        *pfVar4    = local_94 * *pfVar4;
        pfVar4[2]  = local_94 * pfVar4[2];
        pfVar4[3]  = local_94 * pfVar4[3];
        pfVar4[4]  = local_94 * pfVar4[4];
        pfVar4[0xe]  = local_90 * pfVar4[0xe];
        pfVar4[0xf]  = local_90 * pfVar4[0xf];
        pfVar4[0x10] = local_90 * pfVar4[0x10];
        pfVar4[0x12] = local_90 * pfVar4[0x12];
        pfVar4[0x13] = local_90 * pfVar4[0x13];
        pfVar4[0x14] = local_90 * pfVar4[0x14];
        pfVar4[0x1e] = local_8c * pfVar4[0x1e];
        pfVar4[0x1f] = local_8c * pfVar4[0x1f];
        pfVar4[0x20] = local_8c * pfVar4[0x20];
        pfVar4[0x22] = local_8c * pfVar4[0x22];
        pfVar4[0x23] = local_8c * pfVar4[0x23];
        pfVar4[0x24] = local_8c * pfVar4[0x24];
        pfVar4[0x2e] = fVar1 * pfVar4[0x2e];
        pfVar4[0x2f] = fVar1 * pfVar4[0x2f];
        pfVar4[0x30] = fVar1 * pfVar4[0x30];
        pfVar4[0x32] = fVar1 * pfVar4[0x32];
        pfVar4[0x33] = fVar1 * pfVar4[0x33];
        pfVar4[0x34] = fVar1 * pfVar4[0x34];
        pfVar4 = pfVar4 + 8;
      } while (iVar5 != 0);
      if (pfVar9[-2] != -3.4028235e+38f) pfVar9[-2] = local_58 * pfVar9[-2];
      if (pfVar9[-1] != -3.4028235e+38f) pfVar9[-1] = local_54 * pfVar9[-1];
      if (*pfVar9   != -3.4028235e+38f) *pfVar9   = local_50 * *pfVar9;
      if (pfVar9[1] != -3.4028235e+38f) pfVar9[1] = local_4c * pfVar9[1];
      if (pfVar10[-3] != 3.4028235e+38f) pfVar10[-3] = local_58 * pfVar10[-3];
      if (pfVar10[-2] != 3.4028235e+38f) pfVar10[-2] = local_54 * pfVar10[-2];
      if (*(int *)(local_30 + (int)pfVar9) != 0x7f7fffff)
        *(float *)(local_30 + (int)pfVar9) = local_50 * *(float *)(local_30 + (int)pfVar9);
      if (*pfVar10 != 3.4028235e+38f) *pfVar10 = local_4c * *pfVar10;
      if (*(int *)(local_14 + (int)pfVar7) != -0x800001)
        *(float *)(local_14 + (int)pfVar7) = local_58 * *(float *)(local_14 + (int)pfVar7);
      if (*(int *)(local_c + (int)pfVar6) != -0x800001)
        *(float *)(local_c + (int)pfVar6) = local_54 * *(float *)(local_c + (int)pfVar6);
      if (*(int *)(local_4 + (int)pfVar9) != -0x800001)
        *(float *)(local_4 + (int)pfVar9) = local_50 * *(float *)(local_4 + (int)pfVar9);
      if (*(int *)(local_10 + (int)pfVar10) != -0x800001)
        *(float *)(local_10 + (int)pfVar10) = local_4c * *(float *)(local_10 + (int)pfVar10);
      if (pfVar6[-1] != 3.4028235e+38f) pfVar6[-1] = local_58 * pfVar6[-1];
      if (*pfVar6   != 3.4028235e+38f) *pfVar6   = local_54 * *pfVar6;
      if (*(int *)(local_3c + (int)pfVar9) != 0x7f7fffff)
        *(float *)(local_3c + (int)pfVar9) = local_50 * *(float *)(local_3c + (int)pfVar9);
      if (*(int *)(local_18 + (int)pfVar10) != 0x7f7fffff)
        *(float *)(local_18 + (int)pfVar10) = local_4c * *(float *)(local_18 + (int)pfVar10);
      local_64 = local_64 + 1;
      local_5c = local_5c + 0x40;
      pfVar9 = pfVar9 + 4;
      pfVar10 = pfVar10 + 4;
      pfVar4 = pfVar6 + 4;
      pfVar8 = pfVar7 + 4;
      *pfVar7 = local_94 * *pfVar7 * local_94;
      *(float *)(local_40 + -0x10 + (int)pfVar4) =
           local_90 * local_90 * *(float *)(local_40 + -0x10 + (int)pfVar4);
      *(float *)(local_48 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_48 + -0x10 + (int)pfVar9) * local_8c;
      *(float *)(local_44 + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_44 + -0x10 + (int)pfVar10) * fVar1;
      *(float *)(local_28 + -0x10 + (int)pfVar8) =
           local_94 * *(float *)(local_28 + -0x10 + (int)pfVar8);
      *(float *)((int)pfVar6 + local_8) = local_90 * *(float *)((int)pfVar6 + local_8);
      *(float *)(local_20 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_20 + -0x10 + (int)pfVar9);
      *(float *)(local_38 + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_38 + -0x10 + (int)pfVar10);
      *(float *)(local_34 + -0x10 + (int)pfVar8) =
           local_94 * *(float *)(local_34 + -0x10 + (int)pfVar8);
      *(float *)((int)pfVar6 + local_2c) = local_90 * *(float *)((int)pfVar6 + local_2c);
      *(float *)(local_24 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_24 + -0x10 + (int)pfVar9);
      *(float *)(local_1c + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_1c + -0x10 + (int)pfVar10);
      pfVar6 = pfVar4;
      pfVar7 = pfVar8;
    } while (local_64 != in_stack_00000060);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056dd40  SSE2 twin of FUN_0056d3f0 (rsqrtps normalize pass). Same math; the reciprocal-sqrt
//             is rsqrtps + one Newton step (reproduced exactly via rsqrt_nr). Lane masks
//             (_DAT_005e5a60..) are identity on lanes 0-2, so the scalar dot products transcribe
//             directly. Header note 4. Ported from re/analysis/b5e/decomp/FUN_0056dd40.c.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056dd40(
    int param_1, int a1_, int a2_, int a3_,
    int in_stack_00000014, int a5_, int a6_,
    int in_stack_00000020_, int a8_, int a9_,
    int in_stack_0000002c, int a11_, int a12_,
    int in_stack_00000038, int a14_, int a15_,
    int in_stack_00000044, int a17_, int a18_,
    int in_stack_00000050, int a20_, int a21_,
    int in_stack_0000005c, int in_stack_00000060, int a24_,
    int in_stack_00000068, int a26_, int a27_,
    float in_stack_00000074)
{
  (void)a1_;(void)a2_;(void)a3_;(void)a5_;(void)a6_;(void)a8_;(void)a9_;(void)a11_;(void)a12_;
  (void)a14_;(void)a15_;(void)a17_;(void)a18_;(void)a20_;(void)a21_;(void)a24_;(void)a26_;(void)a27_;

  float *pfVar1,*pfVar10,*pfVar12;
  uint *puVar8;
  int iVar9,local_f8;
  int *piVar13;
  uint uVar14,uVar15,uVar16,uVar17,uVar48,uVar49,uVar50;
  // NB: the 0x20 arg is a float* base in this SSE twin.
  float *base20 = (float *)in_stack_00000020_;

  if (in_stack_00000060 != 0) {
    piVar13 = (int *)(param_1 + 0x2c);
    local_f8 = in_stack_00000060;
    in_stack_00000044 = in_stack_00000044 - (int)base20;
    pfVar12 = base20;
    do {
      float fVar41=0.0f,fVar22=0.0f,fVar29=0.0f,fVar31=0.0f;
      float fVar18,fVar20,fVar21,fVar23,fVar27,fVar28,fVar30,fVar32,fVar33,fVar34,fVar35,fVar36,
            fVar37,fVar38,fVar39,fVar42,fVar43,fVar44,fVar45,fVar46,fVar47;
      (void)fVar37;(void)fVar44;(void)fVar46;(void)fVar47;
      if (piVar13[-8] != -1) {
        puVar8 = (uint *)(piVar13[-8] * 0x30 + in_stack_00000068);
        fVar41 = asf(puVar8[3]);
        uVar14=puVar8[4]; uVar15=puVar8[5]; uVar16=puVar8[6];
        uVar17=*puVar8;  uVar48=puVar8[1]; uVar49=puVar8[2];
        fVar35 = asf(piVar13[-0xb]) * fVar41;
        fVar38 = asf(piVar13[-10]) * fVar41;
        fVar41 = asf(piVar13[-9])  * fVar41;
        fVar29 = asf(piVar13[-7]);
        fVar31 = asf(piVar13[-6]);
        fVar33 = asf(piVar13[-5]);
        uVar50=puVar8[8]; uint uVar5=puVar8[9]; uint uVar6=puVar8[10];
        fVar22 = asf(puVar8[3]);
        fVar18 = asf((int)uVar49)*fVar33 + asf((int)uVar17)*fVar29 + asf((int)uVar48)*fVar31;
        fVar20 = asf((int)uVar16)*fVar33 + asf((int)uVar14)*fVar29 + asf((int)uVar15)*fVar31;
        fVar21 = asf((int)uVar6)*fVar33  + asf((int)uVar50)*fVar29 + asf((int)uVar5)*fVar31;
        fVar31 = asf(piVar13[5]) * fVar22;
        fVar33 = asf(piVar13[6]) * fVar22;
        fVar22 = asf(piVar13[7]) * fVar22;
        fVar36 = asf(piVar13[9]);
        fVar39 = asf(piVar13[10]);
        fVar42 = asf(piVar13[0xb]);
        fVar29 = asf(puVar8[3]);
        fVar23 = asf((int)uVar49)*fVar42 + asf((int)uVar17)*fVar36 + asf((int)uVar48)*fVar39;
        fVar27 = asf((int)uVar16)*fVar42 + asf((int)uVar14)*fVar36 + asf((int)uVar15)*fVar39;
        fVar42 = asf((int)uVar6)*fVar42  + asf((int)uVar50)*fVar36 + asf((int)uVar5)*fVar39;
        fVar28 = fVar22*fVar22 + fVar31*fVar31 + fVar33*fVar33;
        fVar23 = fVar23*fVar23; fVar27 = fVar27*fVar27; fVar42 = fVar42*fVar42;
        fVar31 = asf(piVar13[0x15]) * fVar29;
        fVar33 = asf(piVar13[0x16]) * fVar29;
        fVar29 = asf(piVar13[0x17]) * fVar29;
        fVar36 = asf(piVar13[0x19]);
        fVar39 = asf(piVar13[0x1a]);
        fVar45 = asf(piVar13[0x1b]);
        fVar22 = asf(puVar8[3]);
        fVar30 = asf((int)uVar49)*fVar45 + asf((int)uVar17)*fVar36 + asf((int)uVar48)*fVar39;
        fVar32 = asf((int)uVar16)*fVar45 + asf((int)uVar14)*fVar36 + asf((int)uVar15)*fVar39;
        fVar45 = asf((int)uVar6)*fVar45  + asf((int)uVar50)*fVar36 + asf((int)uVar5)*fVar39;
        fVar34 = fVar29*fVar29 + fVar31*fVar31 + fVar33*fVar33;
        fVar45 = fVar45*fVar45;
        fVar29 = asf(piVar13[0x25]) * fVar22;
        fVar39 = asf(piVar13[0x26]) * fVar22;
        fVar22 = asf(piVar13[0x27]) * fVar22;
        fVar36 = asf(piVar13[0x29]);
        fVar43 = asf(piVar13[0x2a]);
        fVar46 = asf(piVar13[0x2b]);
        fVar31 = asf((int)uVar49)*fVar46 + asf((int)uVar17)*fVar36 + asf((int)uVar48)*fVar43;
        fVar33 = asf((int)uVar16)*fVar46 + asf((int)uVar14)*fVar36 + asf((int)uVar15)*fVar43;
        fVar36 = asf((int)uVar6)*fVar46  + asf((int)uVar50)*fVar36 + asf((int)uVar5)*fVar43;
        fVar39 = fVar22*fVar22 + fVar29*fVar29 + fVar39*fVar39;
        fVar33 = fVar33*fVar33; fVar36 = fVar36*fVar36;
        fVar41 = fVar20*fVar20 + fVar41*fVar41 + fVar35*fVar35 + fVar38*fVar38 +
                 fVar18*fVar18 + fVar21*fVar21;
        fVar22 = fVar27 + fVar28 + fVar23 + fVar42;
        fVar29 = fVar32*fVar32 + fVar34 + fVar30*fVar30 + fVar45;
        fVar31 = fVar33 + fVar39 + fVar31*fVar31 + fVar36;
      }
      if (*piVar13 != -1) {
        puVar8 = (uint *)(*piVar13 * 0x30 + in_stack_00000068);
        fVar33 = asf(puVar8[3]);
        uVar14=*puVar8; uVar15=puVar8[1]; uVar16=puVar8[2];
        fVar37 = asf(piVar13[-3]) * fVar33;
        float fVar40 = asf(piVar13[-2]) * fVar33;
        fVar33 = asf(piVar13[-1]) * fVar33;
        uVar17=puVar8[4]; uVar48=puVar8[5]; uVar49=puVar8[6];
        fVar39 = asf(piVar13[1]);
        fVar18 = asf(piVar13[2]);
        fVar20 = asf(piVar13[3]);
        uVar50=puVar8[8]; uint uVar5=puVar8[9]; uint uVar6=puVar8[10];
        fVar36 = asf(puVar8[3]);
        fVar21 = asf((int)uVar16)*fVar20 + asf((int)uVar14)*fVar39 + asf((int)uVar15)*fVar18;
        fVar27 = asf((int)uVar49)*fVar20 + asf((int)uVar17)*fVar39 + asf((int)uVar48)*fVar18;
        fVar28 = asf((int)uVar6)*fVar20  + asf((int)uVar50)*fVar39 + asf((int)uVar5)*fVar18;
        fVar18 = asf(piVar13[0xd]) * fVar36;
        fVar20 = asf(piVar13[0xe]) * fVar36;
        fVar36 = asf(piVar13[0xf]) * fVar36;
        fVar23 = asf(piVar13[0x11]);
        fVar42 = asf(piVar13[0x12]);
        fVar45 = asf(piVar13[0x13]);
        fVar39 = asf(puVar8[3]);
        fVar30 = asf((int)uVar16)*fVar45 + asf((int)uVar14)*fVar23 + asf((int)uVar15)*fVar42;
        fVar32 = asf((int)uVar49)*fVar45 + asf((int)uVar17)*fVar23 + asf((int)uVar48)*fVar42;
        fVar45 = asf((int)uVar6)*fVar45  + asf((int)uVar50)*fVar23 + asf((int)uVar5)*fVar42;
        fVar34 = fVar36*fVar36 + fVar18*fVar18 + fVar20*fVar20;
        fVar30 = fVar30*fVar30; fVar32 = fVar32*fVar32; fVar45 = fVar45*fVar45;
        fVar18 = asf(piVar13[0x1d]) * fVar39;
        fVar20 = asf(piVar13[0x1e]) * fVar39;
        fVar39 = asf(piVar13[0x1f]) * fVar39;
        fVar23 = asf(piVar13[0x21]);
        fVar42 = asf(piVar13[0x22]);
        fVar43 = asf(piVar13[0x23]);
        fVar36 = asf(puVar8[3]);
        fVar35 = asf((int)uVar16)*fVar43 + asf((int)uVar14)*fVar23 + asf((int)uVar15)*fVar42;
        fVar38 = asf((int)uVar49)*fVar43 + asf((int)uVar17)*fVar23 + asf((int)uVar48)*fVar42;
        fVar43 = asf((int)uVar6)*fVar43  + asf((int)uVar50)*fVar23 + asf((int)uVar5)*fVar42;
        fVar46 = fVar39*fVar39 + fVar18*fVar18 + fVar20*fVar20;
        fVar43 = fVar43*fVar43;
        fVar23 = asf(piVar13[0x2d]) * fVar36;
        fVar42 = asf(piVar13[0x2e]) * fVar36;
        fVar36 = asf(piVar13[0x2f]) * fVar36;
        fVar20 = asf(piVar13[0x31]);
        fVar44 = asf(piVar13[0x32]);
        fVar47 = asf(piVar13[0x33]);
        fVar39 = asf((int)uVar16)*fVar47 + asf((int)uVar14)*fVar20 + asf((int)uVar15)*fVar44;
        fVar18 = asf((int)uVar49)*fVar47 + asf((int)uVar17)*fVar20 + asf((int)uVar48)*fVar44;
        fVar20 = asf((int)uVar6)*fVar47  + asf((int)uVar50)*fVar20 + asf((int)uVar5)*fVar44;
        fVar36 = fVar36*fVar36 + fVar23*fVar23 + fVar42*fVar42;
        fVar18 = fVar18*fVar18; fVar20 = fVar20*fVar20;
        fVar41 = fVar41 + fVar27*fVar27 + fVar33*fVar33 + fVar37*fVar37 + fVar40*fVar40 +
                          fVar21*fVar21 + fVar28*fVar28;
        fVar22 = fVar22 + fVar32 + fVar34 + fVar30 + fVar45;
        fVar29 = fVar29 + fVar38*fVar38 + fVar46 + fVar35*fVar35 + fVar43;
        fVar31 = fVar31 + fVar18 + fVar36 + fVar39*fVar39 + fVar20;
      }
      {
        float fVar42s=0.0f,fVar28s=0.0f,fVar30s=0.0f,fVar32s=0.0f;
        pfVar10 = (float *)((int)pfVar12 + in_stack_00000044);
        fVar33 = pfVar10[0]; fVar36 = pfVar10[1]; fVar39 = pfVar10[2]; fVar18 = pfVar10[3];
        fVar41 = fVar41 + in_stack_00000074 * fVar33;
        fVar22 = fVar22 + in_stack_00000074 * fVar36;
        fVar29 = fVar29 + in_stack_00000074 * fVar39;
        fVar31 = fVar31 + in_stack_00000074 * fVar18;
        iVar9 = ((fVar41!=0.0f)?1:0)|((fVar22!=0.0f)?2:0)|((fVar29!=0.0f)?4:0)|((fVar31!=0.0f)?8:0);
        float fVar20s=fVar41, fVar21s=fVar22, fVar23s=fVar29, fVar27s=fVar31;
        if (iVar9 != 0) {
          fVar20s = rsqrt_nr(fVar41);
          fVar21s = rsqrt_nr(fVar22);
          fVar23s = rsqrt_nr(fVar29);
          fVar27s = rsqrt_nr(fVar31);
          fVar42s = fVar41 * fVar20s;
          fVar28s = fVar22 * fVar21s;
          fVar30s = fVar29 * fVar23s;
          fVar32s = fVar31 * fVar27s;
        }
        pfVar10 = (float *)(piVar13 + 9);
        iVar9 = 2;
        do {
          pfVar10[-0x14]=pfVar10[-0x14]*fVar20s; pfVar10[-0x13]=pfVar10[-0x13]*fVar20s;
          pfVar10[-0x12]=pfVar10[-0x12]*fVar20s; /* pfVar10[-0x11] unchanged */
          pfVar10[-4]=pfVar10[-4]*fVar21s; pfVar10[-3]=pfVar10[-3]*fVar21s;
          pfVar10[-2]=pfVar10[-2]*fVar21s;
          pfVar10[0xc]=pfVar10[0xc]*fVar23s; pfVar10[0xd]=pfVar10[0xd]*fVar23s;
          pfVar10[0xe]=pfVar10[0xe]*fVar23s;
          pfVar10[0x1c]=pfVar10[0x1c]*fVar27s; pfVar10[0x1d]=pfVar10[0x1d]*fVar27s;
          pfVar10[0x1e]=pfVar10[0x1e]*fVar27s;
          pfVar10[-0x10]=pfVar10[-0x10]*fVar20s; pfVar10[-0xf]=pfVar10[-0xf]*fVar20s;
          pfVar10[-0xe]=pfVar10[-0xe]*fVar20s;
          pfVar10[0]=pfVar10[0]*fVar21s; pfVar10[1]=pfVar10[1]*fVar21s; pfVar10[2]=pfVar10[2]*fVar21s;
          pfVar10[0x10]=pfVar10[0x10]*fVar23s; pfVar10[0x11]=pfVar10[0x11]*fVar23s;
          pfVar10[0x12]=pfVar10[0x12]*fVar23s;
          pfVar10[0x20]=pfVar10[0x20]*fVar27s; pfVar10[0x21]=pfVar10[0x21]*fVar27s;
          pfVar10[0x22]=pfVar10[0x22]*fVar27s;
          pfVar10 = pfVar10 + 8;
          iVar9 = iVar9 + -1;
        } while (iVar9 != 0);
        pfVar10 = (float *)((int)pfVar12 + (in_stack_00000014 - (int)base20));
        fVar41 = pfVar10[1]; fVar22 = pfVar10[2]; fVar29 = pfVar10[3];
        piVar13 = piVar13 + 0x40;
        const float negMax = -3.4028235e+38f;   // (_DAT_005e5a70 ^ 0x7f7fffff) x4
        // scale-with-min-sentinel: if v != -FLT_MAX use factor else 1.0
        pfVar1 = (float *)((int)pfVar12 + (in_stack_00000014 - (int)base20));
        pfVar1[0] = pfVar10[0] * ((pfVar10[0]!=negMax)?fVar42s:1.0f);
        pfVar1[1] = fVar41 * ((fVar41!=negMax)?fVar28s:1.0f);
        pfVar1[2] = fVar22 * ((fVar22!=negMax)?fVar30s:1.0f);
        pfVar1[3] = fVar29 * ((fVar29!=negMax)?fVar32s:1.0f);
        // scale-with-max-sentinel: if v != FLT_MAX use factor else 1.0
        pfVar10 = (float *)((in_stack_0000002c - (int)base20) + (int)pfVar12);
        pfVar12[0] = pfVar12[0] * ((pfVar12[0]!=3.4028235e+38f)?fVar42s:1.0f);
        pfVar12[1] = pfVar12[1] * ((pfVar12[1]!=3.4028235e+38f)?fVar28s:1.0f);
        pfVar12[2] = pfVar12[2] * ((pfVar12[2]!=3.4028235e+38f)?fVar30s:1.0f);
        pfVar12[3] = pfVar12[3] * ((pfVar12[3]!=3.4028235e+38f)?fVar32s:1.0f);
        pfVar10[0] = pfVar10[0] * ((pfVar10[0]!=negMax)?fVar42s:1.0f);
        pfVar10[1] = pfVar10[1] * ((pfVar10[1]!=negMax)?fVar28s:1.0f);
        pfVar10[2] = pfVar10[2] * ((pfVar10[2]!=negMax)?fVar30s:1.0f);
        pfVar10[3] = pfVar10[3] * ((pfVar10[3]!=negMax)?fVar32s:1.0f);
        pfVar10 = (float *)((in_stack_00000038 - (int)base20) + (int)pfVar12);
        pfVar10[0] = pfVar10[0] * ((pfVar10[0]!=3.4028235e+38f)?fVar42s:1.0f);
        pfVar10[1] = pfVar10[1] * ((pfVar10[1]!=3.4028235e+38f)?fVar28s:1.0f);
        pfVar10[2] = pfVar10[2] * ((pfVar10[2]!=3.4028235e+38f)?fVar30s:1.0f);
        pfVar10[3] = pfVar10[3] * ((pfVar10[3]!=3.4028235e+38f)?fVar32s:1.0f);
        pfVar10 = (float *)((in_stack_00000050 - (int)base20) + (int)pfVar12);
        pfVar10[0]=fVar20s*pfVar10[0]; pfVar10[1]=fVar21s*pfVar10[1];
        pfVar10[2]=fVar23s*pfVar10[2]; pfVar10[3]=fVar27s*pfVar10[3];
        float *pauVar11 = (float *)((in_stack_0000005c - (int)base20) + (int)pfVar12);
        pauVar11[0]=fVar20s*pauVar11[0]; pauVar11[1]=fVar21s*pauVar11[1];
        pauVar11[2]=fVar23s*pauVar11[2]; pauVar11[3]=fVar27s*pauVar11[3];
        pfVar10 = (float *)((int)pfVar12 + in_stack_00000044);
        pfVar10[0]=fVar33*fVar20s*fVar20s; pfVar10[1]=fVar36*fVar21s*fVar21s;
        pfVar10[2]=fVar39*fVar23s*fVar23s; pfVar10[3]=fVar18*fVar27s*fVar27s;
      }
      local_f8 = local_f8 + -1;
      pfVar12 = pfVar12 + 4;
    } while (local_f8 != 0);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056ed60  3x3 congruence transform param_1 = param_2^T . param_3 . param_2 (rows +0/+0x10/
//             +0x20, cols +0/+4/+8). 18 three-term dot products. Header note 5. Verbatim.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056ed60(float *param_1,float *param_2,float *param_3)
{
  float fVar1,fVar2,fVar3,fVar4,fVar5,fVar6,fVar7,fVar8,fVar9;

  fVar1 = param_3[8] * param_2[8] + *param_2 * *param_3 + param_2[4] * param_3[4];
  fVar3 = param_2[1] * *param_3 + param_2[9] * param_3[8] + param_2[5] * param_3[4];
  fVar2 = param_2[6] * param_3[4] + param_2[2] * *param_3 + param_2[10] * param_3[8];
  fVar4 = param_2[8] * param_3[9] + param_3[1] * *param_2 + param_2[4] * param_3[5];
  fVar5 = param_2[1] * param_3[1] + param_2[5] * param_3[5] + param_2[9] * param_3[9];
  fVar6 = param_2[2] * param_3[1] + param_2[6] * param_3[5] + param_2[10] * param_3[9];
  fVar7 = *param_2 * param_3[2] + param_2[4] * param_3[6] + param_3[10] * param_2[8];
  fVar8 = param_2[1] * param_3[2] + param_3[6] * param_2[5] + param_3[10] * param_2[9];
  fVar9 = param_3[6] * param_2[6] + param_3[10] * param_2[10] + param_2[2] * param_3[2];
  *param_1    = fVar3 * param_3[4] + fVar1 * *param_3 + fVar2 * param_3[8];
  param_1[1]  = fVar3 * param_3[5] + fVar2 * param_3[9] + fVar1 * param_3[1];
  param_1[2]  = fVar1 * param_3[2] + fVar3 * param_3[6] + fVar2 * param_3[10];
  param_1[4]  = fVar5 * param_3[4] + fVar4 * *param_3 + fVar6 * param_3[8];
  param_1[5]  = fVar5 * param_3[5] + fVar6 * param_3[9] + fVar4 * param_3[1];
  param_1[6]  = fVar4 * param_3[2] + fVar5 * param_3[6] + fVar6 * param_3[10];
  param_1[8]  = fVar8 * param_3[4] + fVar7 * *param_3 + fVar9 * param_3[8];
  param_1[9]  = fVar8 * param_3[5] + fVar9 * param_3[9] + fVar7 * param_3[1];
  param_1[10] = fVar7 * param_3[2] + fVar8 * param_3[6] + fVar9 * param_3[10];
  return;
}

// ---------------------------------------------------------------------------
// 0x0056e680  Per-body inertia/impulse assembly (calls FUN_0056ed60): builds each body's world
//             inertia block (identity-diagonal when the flag bit 2 is clear, else the congruence
//             transform), a cross-product Jacobian with a SQRT renormalization gated on FLT_MIN /
//             0.25 constants, then the final 3x(mat3) accumulate into param_4. Header note 6.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056e680(int param_1,undefined4 param_2,int *param_3,int *param_4,
                 int param_5,undefined4 param_6,int param_7,int *param_8,int param_9,
                 undefined4 param_10,int param_11,undefined4 param_12,int param_13,
                 undefined4 param_14,float param_15)
{
  float *pfVar1;
  undefined4 uVar2;
  float fVar3,fVar4,fVar5,fVar6,fVar7,fVar8,fVar9,fVar10;
  float fVar11,fVar12,fVar13,fVar14,fVar15,fVar16,fVar17,fVar18,fVar19,fVar20,fVar21,fVar22,fVar23,
        fVar24,fVar25,fVar26,fVar27,fVar28,fVar29,fVar30,fVar31,fVar32,fVar33,fVar34,fVar35,fVar36,
        fVar37,fVar38,fVar39,fVar40,fVar41,fVar42,fVar43,fVar44,fVar45,fVar46,fVar47,fVar48;
  int iVar49,iVar50,iVar51,iVar53;
  float *pfVar52;
  int local_68;
  float local_44,local_40;
  // ed60 output buffer — ONE contiguous stack block: ed60 writes param_1[0/1/2/4/5/6/8/9/10]
  // (a 3x3 with +0x10 row stride); indices 3/7 are gaps. Was 9 separate locals -> C4700.
  float local_30[11];

  iVar50 = param_11;
  iVar49 = param_1;
  local_68 = 0;
  if (param_8[1] != 0) {
    param_1 = 0;
    do {
      iVar53 = *(int *)(*param_8 + local_68 * 4);
      if ((*(char *)(param_13 + iVar53 * 4) & 2) == 0) {
        uVar2 = *(undefined4 *)(iVar53 * 0x30 + 0x2c + param_7);
        *(undefined4 *)(param_1 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 4 + *param_3) = 0;
        *(undefined4 *)(param_1 + 8 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x10 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x14 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 0x18 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x20 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x24 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x28 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 0xc + *param_3) = *(undefined4 *)(iVar53 * 0x30 + 0xc + param_7);
      }
      else {
        iVar51 = iVar53 * 0x30 + param_7;
        FUN_0056ed60((float *)(*param_3 + param_1),(float *)iVar51,(float *)(iVar53 * 0x40 + param_9));
        *(undefined4 *)(param_1 + 0xc + *param_3) = *(undefined4 *)(iVar51 + 0xc);
      }
      param_11 = iVar53 * 0x30;
      if ((*(char *)(param_13 + iVar53 * 4) & 2) != 0) {
        iVar51 = iVar53 * 0x20;
        FUN_0056ed60(local_30,(float *)(param_11 + param_5),(float *)(iVar53 * 0x40 + param_9));
        fVar6 = local_30[0] * *(float *)(iVar51 + 0x10 + iVar50) +
                local_30[4] * *(float *)(iVar51 + 0x14 + iVar50) +
                local_30[8] * *(float *)(iVar51 + 0x18 + iVar50);
        fVar7 = local_30[1] * *(float *)(iVar51 + 0x10 + iVar50) +
                local_30[5] * *(float *)(iVar51 + 0x14 + iVar50) +
                local_30[9] * *(float *)(iVar51 + 0x18 + iVar50);
        fVar8 = local_30[2] * *(float *)(iVar51 + 0x10 + iVar50) +
                local_30[6] * *(float *)(iVar51 + 0x14 + iVar50) +
                local_30[10] * *(float *)(iVar51 + 0x18 + iVar50);
        fVar9 = *(float *)(iVar51 + 0x18 + iVar50) * fVar7 -
                *(float *)(iVar51 + 0x14 + iVar50) * fVar8;
        local_44 = fVar8 * *(float *)(iVar51 + 0x10 + iVar50) -
                   *(float *)(iVar51 + 0x18 + iVar50) * fVar6;
        local_40 = *(float *)(iVar51 + 0x14 + iVar50) * fVar6 -
                   fVar7 * *(float *)(iVar51 + 0x10 + iVar50);
        fVar10 = fVar9 * fVar9 + local_44 * local_44 + local_40 * local_40;
        if (PTR_DAT_005ceabc < fVar10) {
          fVar3 = *(float *)(iVar51 + 0x18 + iVar50);
          fVar4 = *(float *)(iVar51 + 0x14 + iVar50);
          fVar5 = *(float *)(iVar51 + 0x10 + iVar50);
          fVar10 = (fVar3 * fVar3 + fVar4 * fVar4 + fVar5 * fVar5) * sqrtf(fVar10) * param_15 *
                   param_15 * _DAT_005cc564;
          if (PTR_DAT_005ceabc < fVar10) {
            fVar3 = (*(float *)(iVar51 + 0x18 + iVar50) * fVar8 +
                    fVar6 * *(float *)(iVar51 + 0x10 + iVar50) +
                    *(float *)(iVar51 + 0x14 + iVar50) * fVar7) * _DAT_005cc32c;
            fVar10 = sqrtf((fVar10 + fVar3) / fVar3);
            fVar3 = (_DAT_005cc320 - fVar10) / param_15;
            fVar10 = _DAT_005cc320 / fVar10;
            fVar9 = (fVar6 * fVar3 + fVar9) * fVar10;
            local_44 = (fVar7 * fVar3 + local_44) * fVar10;
            local_40 = (fVar8 * fVar3 + local_40) * fVar10;
          }
          *(float *)(iVar51 + 0x10 + iVar49) = fVar9 + *(float *)(iVar51 + 0x10 + iVar49);
          *(float *)(iVar51 + 0x14 + iVar49) = local_44 + *(float *)(iVar51 + 0x14 + iVar49);
          *(float *)(iVar51 + 0x18 + iVar49) = local_40 + *(float *)(iVar51 + 0x18 + iVar49);
        }
      }
      local_68 = local_68 + 1;
      param_1 = param_1 + 0x30;
    } while (local_68 != param_8[1]);
  }
  param_15 = _DAT_005cc320 / param_15;
  iVar53 = 0;
  param_3[1] = param_8[1];
  local_68 = 0;
  if (param_8[1] == 0) { param_4[1] = param_8[1]; return; }
  param_13 = 0;
  do {
    pfVar52 = (float *)(*param_3 + param_13);
    fVar48 = pfVar52[3] * pfVar52[3];
    fVar6=*pfVar52; fVar7=pfVar52[1]; fVar8=*pfVar52; fVar9=pfVar52[2]; fVar10=*pfVar52;
    fVar3=pfVar52[4]; fVar4=pfVar52[1]; fVar5=pfVar52[1]; fVar11=pfVar52[5]; fVar12=pfVar52[1];
    fVar13=pfVar52[6]; fVar14=pfVar52[2]; fVar15=pfVar52[8]; fVar16=pfVar52[9]; fVar17=pfVar52[2];
    fVar18=pfVar52[2]; fVar19=pfVar52[10]; fVar20=pfVar52[4]; fVar21=*pfVar52; fVar22=pfVar52[4];
    fVar23=pfVar52[2]; fVar24=pfVar52[4]; fVar25=pfVar52[5]; fVar26=pfVar52[5]; fVar27=pfVar52[6];
    fVar28=pfVar52[5]; fVar29=pfVar52[8]; fVar30=pfVar52[6]; fVar31=pfVar52[9]; fVar32=pfVar52[6];
    fVar33=pfVar52[6]; fVar34=pfVar52[10];
    iVar51 = *(int *)(*param_8 + local_68 * 4) * 0x20;
    fVar35=pfVar52[8]; fVar36=*pfVar52; fVar37=pfVar52[1]; fVar38=pfVar52[8]; fVar39=pfVar52[9];
    fVar40=pfVar52[4]; fVar41=pfVar52[9]; fVar42=pfVar52[5]; fVar43=pfVar52[8]; fVar44=pfVar52[10];
    fVar45=pfVar52[9]; fVar46=pfVar52[10]; fVar47=pfVar52[10];
    *(float *)(iVar53 + *param_4) =
         param_15 * *(float *)(iVar51 + iVar50) + fVar48 * *(float *)(iVar51 + iVar49);
    *(float *)(*param_4 + 4 + iVar53) =
         param_15 * *(float *)(iVar51 + 4 + iVar50) + fVar48 * *(float *)(iVar51 + 4 + iVar49);
    *(float *)(*param_4 + 8 + iVar53) =
         param_15 * *(float *)(iVar51 + 8 + iVar50) + fVar48 * *(float *)(iVar51 + 8 + iVar49);
    *(float *)(*param_4 + 0x10 + iVar53) = param_15 * *(float *)(iVar51 + 0x10 + iVar50);
    *(float *)(*param_4 + 0x14 + iVar53) = param_15 * *(float *)(iVar51 + 0x14 + iVar50);
    *(float *)(*param_4 + 0x18 + iVar53) = param_15 * *(float *)(iVar51 + 0x18 + iVar50);
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar6*fVar6 + fVar3*fVar4 + fVar14*fVar15) * *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    param_13 = param_13 + 0x30;
    *pfVar52 = (fVar16*fVar17 + fVar5*fVar11 + fVar7*fVar8) * *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    *pfVar52 = (fVar18*fVar19 + fVar12*fVar13 + fVar9*fVar10) * *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar29*fVar30 + fVar24*fVar25 + fVar20*fVar21) * *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    *pfVar52 = (fVar26*fVar26 + fVar3*fVar4 + fVar31*fVar32) * *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    pfVar1 = (float *)(iVar51 + 0x18 + iVar49);
    *pfVar52 = (fVar33*fVar34 + fVar27*fVar28 + fVar22*fVar23) * *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar43*fVar44 + fVar39*fVar40 + fVar35*fVar36) * *pfVar1 + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    *pfVar52 = (fVar45*fVar46 + fVar41*fVar42 + fVar37*fVar38) * *pfVar1 + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    local_68 = local_68 + 1;
    iVar53 = iVar53 + 0x20;
    *pfVar52 = (fVar47*fVar47 + fVar14*fVar15 + fVar31*fVar32) * *pfVar1 + *pfVar52;
  } while (local_68 != param_8[1]);
  param_4[1] = param_8[1];
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 8. ---
RH_ScopedInstall(FUN_0056d350, 0x0056d350);
RH_ScopedInstall(FUN_0056d3f0, 0x0056d3f0);
RH_ScopedInstall(FUN_0056dd40, 0x0056dd40);
RH_ScopedInstall(FUN_0056ed60, 0x0056ed60);
RH_ScopedInstall(FUN_0056e680, 0x0056e680);

}  // namespace Collision
}  // namespace mashed_re
