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
    // diff truncated to f32 before the scale (matches the original's
    // store/reload at 0x004b4650 body).
    const float diffx = b[0] - a[0];
    const float diffy = b[1] - a[1];
    const float diffz = b[2] - a[2];
    out[0] = t * diffx + a[0];
    out[1] = t * diffy + a[1];
    out[2] = t * diffz + a[2];
}

RH_ScopedInstall(Vec3Lerp, 0x004b4650);
