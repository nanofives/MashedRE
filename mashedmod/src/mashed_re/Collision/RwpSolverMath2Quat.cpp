// Mashed RE — B5e K2: 0x00546b10 matrix->quaternion and its X / Z Shoemake branches
// 0x00546bf0 / 0x00546cb0.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
// Split out of Collision/RwpSolverMath2.cpp on 2026-11-01 (D4-WSB-55B080-ULP) so these three
// bodies are compiled /arch:IA32 (x87) via mashedmod/x87_tus.txt, as 0x00546c50 already is
// (Collision/RwpSolverMath2QuatY.cpp, U-9220). The other four K2 bodies stay SSE2.
//
// Why x87: the original runs these chains on the x87 stack under the game thread's control
// word, 0x007f (PC_24) in a race, so every FADD / FSUB / FDIVR / FMUL rounds to 24 bits.
// Under SSE2 the port's float10 (= double) intermediates do not follow precision control:
// `0.5 / s` (FDIVR @ 0x00546b5e / 0x00546c22 / 0x00546ce3), the m[i] -/+ m[j] terms and the
// trace sum (0x00546b23..0x00546b3f) stayed 53-bit, which gave 1-ulp differences in the
// quaternion 0x0055b080 writes through 0x00546b10 (4 of 24 shadow samples,
// verify/d4wsb_portbroad2_20261101/shadow_summary_pass1.tsv). Under x87 codegen the thread's
// PC applies to the float10 temporaries exactly as it does to the original. At PC_53 (the
// standalone) the x87 and SSE2 builds compute the same double chain.
//
// Bodies unchanged from RwpSolverMath2.cpp (VERBATIM transcription, re-verified against live
// disasm 2026-07-16 and again 2026-11-01 by capstone over MASHED.exe.unpatched: operand
// order and op tree of every chain match the C below; body_end 0x00546b89 / 0x00546c4c /
// 0x00546d0c, plain RET => __cdecl).
#include "../Core/HookSystem.h"
#include "../Core/ShadowTrack.h"
#include "../Core/ShadowAB.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   undefined4;
typedef long double    float10;     // x87 80-bit extended — MSVC = 64-bit double [X87]

#define DAT_005d757c   (*(float*)0x005d757cu)  // matrix-trace threshold    (FCOM @ 0x00546b30)
#define _DAT_005cc320  (*(float*)0x005cc320u)  // 1.0f                      (FADD @ 0x00546b3f)
#define _DAT_005cc32c  (*(float*)0x005cc32cu)  // 0.5f                      (FLD  @ 0x00546b4e)

// 0x004c3b30 — C4 FastSqrt port (Math/RwSqrt.cpp).
extern "C" float __cdecl FastSqrt(float x);
#define FUN_004c3b30  FastSqrt

// FUN_00546b10 dispatches to the three siblings (0x00546c50 lives in RwpSolverMath2QuatY.cpp).
extern "C" void __cdecl FUN_00546bf0(float *param_1, float *param_2);
extern "C" void __cdecl FUN_00546c50(float *param_1, float *param_2);
extern "C" void __cdecl FUN_00546cb0(float *param_1, float *param_2);

// ---------------------------------------------------------------------------
// 0x00546b10  Matrix->quaternion (Shoemake): trace branch, else dispatch to the
//             largest-diagonal sibling (X: 0x546bf0, Y: 0x546c50, Z: 0x546cb0).
//             Returns 0 on a null arg, else 1.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl FUN_00546b10(float *param_1,float *param_2);
static undefined4 __cdecl FUN_00546b10_impl(float *param_1,float *param_2) {
  float10 fVar1;
  void (__cdecl *pcVar2)(float*, float*);
  float10 fVar3;

  if ((param_1 == (float *)0x0) || (param_2 == (float *)0x0)) {
    return 0;
  }
  // trace stays on the x87 stack (no float32 store) through the FCOM and the +1.0
  // (0x00546b23..0x00546b46); disasm sum order (m[10] + m[0]) + m[5]. [X87]
  fVar1 = (float10)param_2[10] + (float10)*param_2 + (float10)param_2[5];
  if ((float10)DAT_005d757c < fVar1) {
    fVar3 = (float10)FUN_004c3b30((float)(fVar1 + (float10)_DAT_005cc320));  // 0x00546b49 [X87]
    param_1[3] = (float)((float10)_DAT_005cc32c * fVar3);
    fVar3 = (float10)_DAT_005cc32c / fVar3;                                  // 0x00546b5e [X87]
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
static void __cdecl FUN_00546bf0_impl(float *param_1,float *param_2) {
  float10 fVar1;

  // arg = m[0] - (m[10] + m[5]) + 1.0, single float32 rounding at the call
  // (FLD/FADD/FSUBR/FADD/FSTP 0x00546bf5..0x00546c03). [X87]
  fVar1 = (float10)FUN_004c3b30((float)((float10)*param_2 -
              ((float10)param_2[10] + (float10)param_2[5]) + (float10)_DAT_005cc320));
  *param_1 = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;                                    // 0x00546c22 [X87]
  param_1[3] = (float)(((float10)param_2[6] - (float10)param_2[9]) * fVar1);
  param_1[1] = (float)(((float10)param_2[4] + (float10)param_2[1]) * fVar1);
  param_1[2] = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  return;
}

// ---------------------------------------------------------------------------
// 0x00546cb0  Matrix->quat, "Z is largest" Shoemake branch.
// ---------------------------------------------------------------------------
static void __cdecl FUN_00546cb0_impl(float *param_1,float *param_2) {
  float10 fVar1;

  // arg = m[10] - (m[5] + m[0]) + 1.0 (0x00546cb5..0x00546cc3). [X87]
  fVar1 = (float10)FUN_004c3b30((float)((float10)param_2[10] -
              ((float10)param_2[5] + (float10)*param_2) + (float10)_DAT_005cc320));
  param_1[2] = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;                                    // 0x00546ce3 [X87]
  param_1[3] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar1);
  *param_1 = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  param_1[1] = (float)(((float10)param_2[9] + (float10)param_2[6]) * fVar1);
  return;
}

// --- 0x00546b10 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
extern "C" undefined4 __cdecl FUN_00546b10(float *param_1,float *param_2) {
    SHADOW_AB_COUNTER(ab, "FUN_00546b10", 0x00546b10u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, FUN_00546b10_impl, param_1, param_2);
}
RH_ScopedInstall(FUN_00546b10, 0x00546b10);
// --- 0x00546bf0 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// OUTPUT REGION: 0x10 bytes at (param_1) -- supplied by hand via --region;
// the claim that this span covers every write MUST be backed by the analysis note.
extern "C" void __cdecl FUN_00546bf0(float *param_1,float *param_2) {
    SHADOW_AB_COUNTER(ab, "FUN_00546bf0", 0x00546bf0u, ShadowAB::kPhaseRace);
    ShadowAB::RunRegion(ab, FUN_00546bf0_impl, reinterpret_cast<void*>(param_1), 0x10, param_1, param_2);
}
RH_ScopedInstall(FUN_00546bf0, 0x00546bf0);
// --- 0x00546cb0 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// OUTPUT REGION: 0x10 bytes at (param_1) -- supplied by hand via --region;
// the claim that this span covers every write MUST be backed by the analysis note.
extern "C" void __cdecl FUN_00546cb0(float *param_1,float *param_2) {
    SHADOW_AB_COUNTER(ab, "FUN_00546cb0", 0x00546cb0u, ShadowAB::kPhaseRace);
    ShadowAB::RunRegion(ab, FUN_00546cb0_impl, reinterpret_cast<void*>(param_1), 0x10, param_1, param_2);
}
RH_ScopedInstall(FUN_00546cb0, 0x00546cb0);

}  // namespace Collision
}  // namespace mashed_re
