// Mashed RE — HB-6 (2026-10-26): x87 re-transcription of two RwpSolverLeaves1 bodies, in
// DISASSEMBLY order, for FINDING VECCAP-2 (re/analysis/plans/veccap_finding_2026-07-24.md).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (listing read from original/MASHED.exe.unpatched with capstone, 2026-10-26; neither body
// overlaps a boot patch). Compiled /arch:IA32 via mashedmod/x87_tus.txt so every float10
// intermediate below lives on the x87 stack and follows the caller's precision control
// (CW 0x007f = PC_24 in a race), exactly as the original's does.
//
// DEFAULT OFF. Collision/RwpSolverLeaves1.cpp keeps its SSE2 decomp-order bodies as the
// default; its ShadowAB wrappers dispatch here only under MASHED_LEAVES1_X87=1. Flipping that
// default is a decision row, not this file's call. Until then this TU is a knob-gated second
// body for both RVAs. scripts/lint_rva_bodies.py does not count it (no RVA-header comment
// anchors these definitions); the flip must delete the SSE2 bodies so one body remains.
//
// Why the decomp order is wrong (both bodies): x87 FADD is order-sensitive, and the decomp
// prints sums in an order the code does not execute. The listing addresses below are the
// evaluation order; `float` locals are the values the original stores to a dword slot
// (rounded to 32 bits there), `float10` locals are the values it keeps in an ST register.
#include <cmath>

namespace mashed_re {
namespace Collision {

typedef long double float10;   // MSVC: 64-bit double, held on the x87 stack under /arch:IA32 [X87]

#define _DAT_005cc32c     (*(float*)0x005cc32cu)   // 0.5f  (FMUL m32 @ 0x0056623f..0x0056627d)
#define _DAT_005cc320     (*(float*)0x005cc320u)   // 1.0f  (FDIVR m32 @ 0x005667f9)
#define PTR_DAT_005ceabc  (*(float*)0x005ceabcu)   // FLT_MIN bits 0x00800000 (FCOM m32 @ 0x005667e2)

// ---------------------------------------------------------------------------
// x87 body for FUN_00566200 (RVA 0x00566200, 411 B, __cdecl, RET @ 0x0056639a).
// Re-fits the AABB param_2 (min +0..+2, max +4..+6) under the 4x3 matrix param_3
// (rows +0x00/+0x10/+0x20, translation +0x30) into param_1.
// Every read of param_2/param_3 precedes the first store to param_1 (0x00566364), so the
// Core17 call FUN_00566200(box, box, m) with param_1 == param_2 is safe in this order too.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00566200_x87(float *param_1, float *param_2, float *param_3) {
  const float10 h = (float10)_DAT_005cc32c;
  // 0x00566207..0x00566215: three differences, d0/d1 kept in ST, d2 stored at 0x00566218
  float10 d0 = (float10)param_2[0] - (float10)param_2[4];                  // FLD +0x10; FSUBR +0
  float10 d1 = (float10)param_2[1] - (float10)param_2[5];                  // FLD +4;   FSUB +0x14
  float   s14 = (float)((float10)param_2[2] - (float10)param_2[6]);        // FSTP [esp+0x14]
  // 0x0056621c..0x00566239: three sums, each stored as a dword
  float   s0 = (float)((float10)param_2[4] + (float10)param_2[0]);         // FSTP [esp]
  float   s4 = (float)((float10)param_2[5] + (float10)param_2[1]);         // FSTP [esp+4]
  float   s8 = (float)((float10)param_2[6] + (float10)param_2[2]);         // FSTP [esp+8]
  // 0x0056623d..0x00566283: halve. hx/hz/c0..c2 are stored; hy stays in ST0
  float   hx = (float)(d0 * h);                                            // FSTP [esp+0xc]
  float10 hy = d1 * h;                                                     // 0x00566249 (kept)
  float   hz = (float)((float10)s14 * h);                                  // FSTP [esp+0x14]
  float   c0 = (float)((float10)s0 * h);                                   // FSTP [esp]
  float   c1 = (float)((float10)s4 * h);                                   // FSTP [esp+4]
  float   c2 = (float)((float10)s8 * h);                                   // FSTP [esp+8]
  // 0x00566287..0x005662fb: |row| . half-extent, summed (y*hy + z*hz) + x*hx
  float   e0 = (float)(((float10)std::fabs(param_3[1]) * hy
                        + (float10)std::fabs(param_3[2]) * (float10)hz)
                       + (float10)std::fabs(param_3[0]) * (float10)hx);    // FSTP [esp+0x18]
  float   e1 = (float)(((float10)std::fabs(param_3[5]) * hy
                        + (float10)std::fabs(param_3[6]) * (float10)hz)
                       + (float10)std::fabs(param_3[4]) * (float10)hx);    // FSTP [esp+0x1c]
  float10 e2 = ((float10)std::fabs(param_3[9]) * hy
                + (float10)std::fabs(param_3[10]) * (float10)hz)
               + (float10)std::fabs(param_3[8]) * (float10)hx;             // 0x005662fb (kept)
  // 0x005662fd..0x0056630e: center minus translation, all three kept in ST
  float10 q0 = (float10)c0 - (float10)param_3[0xc];                        // FSUBR [esp]
  float10 q1 = (float10)c1 - (float10)param_3[0xd];                        // FSUBR [esp+4]
  float10 q2 = (float10)c2 - (float10)param_3[0xe];                        // FSUBR [esp+8]
  // 0x00566312..0x00566354: row . q, summed (y*q1 + x*q0) + z*q2
  float   r0 = (float)(((float10)param_3[1] * q1 + q0 * (float10)param_3[0])
                       + (float10)param_3[2] * q2);                        // FSTP [esp]
  float   r1 = (float)(((float10)param_3[5] * q1 + (float10)param_3[4] * q0)
                       + (float10)param_3[6] * q2);                        // FSTP [esp+4]
  float10 r2 = ((float10)param_3[9] * q1 + (float10)param_3[8] * q0)
               + (float10)param_3[10] * q2;                                // 0x00566354 (kept)
  // 0x0056635c..0x00566392: stores, in this order
  param_1[4] = (float)((float10)r0 - (float10)e0);                         // 0x00566364
  param_1[5] = (float)((float10)r1 - (float10)e1);                         // 0x0056636f
  param_1[6] = (float)(r2 - e2);                                           // 0x00566376
  param_1[0] = (float)((float10)e0 + (float10)r0);                         // 0x00566381
  param_1[1] = (float)((float10)e1 + (float10)r1);                         // 0x0056638b
  param_1[2] = (float)(e2 + r2);                                           // 0x00566392
}

// ---------------------------------------------------------------------------
// x87 body for FUN_005667c0 (RVA 0x005667c0, 109 B, __cdecl, returns ST0).
// Normalizes the vec3 param_2 into param_1; returns |v| (inv * |v|^2) in ST0, or |v|^2
// on the zero path. param_1 == param_2 is a real call shape (Core17, Core15): each
// component is re-read from memory after the previous component's store, as the listing does.
// ---------------------------------------------------------------------------
extern "C" float10 __cdecl FUN_005667c0_x87(float *param_1, float *param_2) {
  // 0x005667c4..0x005667da: loads y, x, z; sum = (x*x + y*y) + z*z
  float10 y = (float10)param_2[1];
  float10 x = (float10)param_2[0];
  float10 z = (float10)param_2[2];
  float10 s = (x * x + y * y) + z * z;
  // 0x005667e2..0x005667f3: FCOM m32; JNE (C0|C3) -> zero path when s <= thr or unordered
  if ((float10)PTR_DAT_005ceabc < s) {
    float10 inv = (float10)_DAT_005cc320 / std::sqrt(s);                   // FSQRT; FDIVR m32
    param_1[0] = (float)(inv * (float10)param_2[0]);                       // 0x00566803
    param_1[1] = (float)((float10)param_2[1] * inv);                       // 0x0056680a
    param_1[2] = (float)((float10)param_2[2] * inv);                       // 0x00566812
    return s * inv;                                                        // FMULP @ 0x00566815
  }
  param_1[0] = 0.0f;                                                       // 0x00566818
  param_1[1] = 0.0f;
  param_1[2] = 1.0f;                                                       // 0x3f800000
  return s;
}

}  // namespace Collision
}  // namespace mashed_re
