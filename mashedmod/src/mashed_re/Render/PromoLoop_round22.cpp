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
    // Replicates the original's EXACT x87 precision behavior (full disasm read
    // round 23). The original stores each diff (b[i]-a[i]) to out[i] as f32
    // (fstp), then for the multiply-add it is ASYMMETRIC:
    //   out[0], out[1]: add the 80-bit (t*diff) straight from ST0
    //                   -> (f32)(80bit(t * f32diff) + a)
    //   out[2]:         spills (t*diff2) to f32 (fst [esp+0x10]) and reloads it
    //                   -> (f32)((f32)(t*diff2) + a)
    // A uniform C expression can't reproduce this stack-scheduling artifact;
    // the asymmetric volatile on out[2] does.
    out[0] = b[0] - a[0];                  // f32 diff0
    out[1] = b[1] - a[1];                  // f32 diff1
    out[2] = b[2] - a[2];                  // f32 diff2
    const float d0 = out[0], d1 = out[1], d2 = out[2];   // reload f32 diffs
    out[0] = t * d0 + a[0];                // 80-bit t*d0 + a0
    out[1] = t * d1 + a[1];                // 80-bit t*d1 + a1
    volatile float td2 = t * d2;           // out[2] path truncates t*diff2 to f32
    out[2] = td2 + a[2];
}

// DEFERRED 2026-06-13 (round 23): bit-identity x87-blocked. The original's
// out[0] = (f32)(80bit(t * f32diff0) + a[0]) keeps the t*diff product in 80-bit
// FPU precision while truncating the diff to f32 — an asymmetric stack-
// scheduling artifact (out[2] additionally truncates t*diff2 to f32). MSVC
// /O2 /fp:precise compiles every C variant of this (plain float, volatile
// diff, store/reload, asymmetric volatile) to the SAME sequence that keeps
// diff0 in 80-bit -> 3 ulp on one component (t=0.9, log/diff_vec3_lerp.csv).
// The formula is verified correct; bit-identity needs inline __asm replicating
// the exact FPU op order, or compiling THIS file with /fp:strict. Re-enable +
// re-diff after that. Handler 'vec3_lerp' (diff_template.js) stays banked.
// RH_ScopedInstall(Vec3Lerp, 0x004b4650);
