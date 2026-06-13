// Mashed RE — promote-round round 22 (L5 vec3_lerp harness-ext).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004b4650  Vec3Lerp — render; out = A + t*(B - A), pure float leaf
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-12 (x87 float
// arithmetic, no globals). Diffed via the new vec3_lerp handler.
//
// Analysis: re/analysis/render_3_c1_to_c2_s3/FUN_004b4650.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Vec3Lerp  --  0x004b4650   (subsystem: render)
//
// Original: FUN_004b4650 (0x5b bytes, 0x004b4650..0x004b46aa)
// Signature: void FUN_004b4650(float* out, float* a, float* b, float t)
//
// The decompiler shows redundant intermediate stores; the net behavior is
// per-component: out[i] = a[i] + t*(b[i] - a[i]). CRITICAL for bit-identity:
// the original stores the diff (b[i]-a[i]) to memory and reloads it as a
// 32-bit float BEFORE the multiply (line 19-23 of the decomp), so the diff is
// truncated to f32. The named float locals below force the same truncation.
//
// No constants, no globals, no callees. Pure deterministic math.
//
// Callers (7, all C2): FUN_00418bd0 (render), FUN_00453730/00454350/00457c10/
//   00459620/0045ae80/0045b390 (gameplay).
// ---------------------------------------------------------------------------

// 0x004b4650
extern "C" __declspec(dllexport) void __cdecl Vec3Lerp(float* out, float* a,
                                                       float* b, float t) {
    // The original STORES the diff (b[i]-a[i]) to memory and RELOADS it as a
    // 32-bit float before the scale (decomp lines 19-23), truncating away the
    // x87 80-bit excess precision. A plain `float` local lets /O2 keep the
    // diff in an 80-bit register -> 1-3 ulp drift (caught RED round 23, t=0.9).
    // `volatile` forces the same memory round-trip -> bit-identical.
    volatile float diffx = b[0] - a[0];
    volatile float diffy = b[1] - a[1];
    volatile float diffz = b[2] - a[2];
    const float fx = diffx, fy = diffy, fz = diffz;
    out[0] = t * fx + a[0];
    out[1] = t * fy + a[1];
    out[2] = t * fz + a[2];
}

RH_ScopedInstall(Vec3Lerp, 0x004b4650);
