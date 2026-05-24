// Mashed RE — HUD text/font cluster, batch-k session 4 (C2->C3 promotions).
// Analysis notes: re/analysis/hud_promote_c2_b/, re/analysis/hud_ingame_promote_c2/
// Subsystem: HUD (font context / text rendering)
//
// Binary anchor:
//   original\MASHED.exe.unpatched  SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Hooks in this file:
//   0x00552750  FontCtx_ResetTransform  pure leaf; identity RwMatrix on cur ctx
//   0x00428450  HudSpinCoinAnim         (C2 stub; smoke-test for spin_angle_observe arg_type)
//
// Refusals from prior sessions (documented in re/PROMOTION_QUEUE.md):
//   0x004c1c80  callee FUN_004c0e50 is C1 — fails C2->C3 "callee at C2+" rule;
//               batch header described as "pure leaf" but analysis note shows
//               guarded conditional callee — tracker drift, skip rather than promote.
//   0x00427680  repeat refusal from HudBatch.cpp session — non-standard ESI
//               implicit output ptr + U-2127 EDI artifact; no matching arg_type
//               in diff_template.js; deferred pending harness arg_type extension.
//   0x00450b10  PROMOTED C2->C3 in c3-batch-n-s1 (DrawQuadPrimitives.cpp);
//               draw_quad_observe arg_type landed 2026-05-21; Frida GREEN 12/12.
//   0x00428450  spin-angle accumulator side-effect — _DAT_0067d974 is incremented
//               by each call; draw_quad_observe cannot reset it between Orig+Reimpl
//               calls; new harness arg_type spin_angle_observe resets the accumulator
//               before each sub-call; smoke-tested 2026-05-22.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Font context globals (all addresses Ghidra-cited in analysis notes)
// ---------------------------------------------------------------------------

// DAT_00912b04 = current font context stack depth (uint32_t index)
static std::uint32_t* const g_FontCtxDepth_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912b04u);

// DAT_00912a84 = base of font context pointer array (void*[32])
static void** const g_FontCtxPtrs_k4 =
    reinterpret_cast<void**>(0x00912a84u);

// DAT_00912bd8 = matrix dirty/valid flag (zeroed by ResetTransform)
static std::uint32_t* const g_DirtyBd8_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912bd8u);

// DAT_00912bec = secondary dirty flag (zeroed by ResetTransform)
static std::uint32_t* const g_DirtyBec_k4 =
    reinterpret_cast<std::uint32_t*>(0x00912becu);

// ===========================================================================
// 0x00552750  FontCtx_ResetTransform
//
// Original: FUN_00552750 (0x8e bytes; 0x00552750–0x0055283d).
// Pure leaf (no named direct callees; only global reads/writes).
//
// Resets the current font context's 3×3 rotation matrix to identity and
// clears dirty flags. The "current context" is:
//   (&DAT_00912a84)[DAT_00912b04]  (a pointer loaded from the font context
//   stack array at index given by the depth counter).
//
// RwMatrix layout (offsets relative to ctx pointer, from analysis note):
//   +0x00  right.x = 1.0f
//   +0x04  right.y = 0.0f
//   +0x08  right.z = 0.0f
//   +0x0c  flags   |= 0x20003  (identity marker bits; OR not assignment)
//   +0x10  up.x    = 0.0f
//   +0x14  up.y    = 1.0f
//   +0x18  up.z    = 0.0f
//   +0x20  at.x    = 0.0f
//   +0x24  at.y    = 0.0f
//   +0x28  at.z    = 1.0f
//   +0x30  pos.x   = 0.0f
//   +0x34  pos.y   = 0.0f
//   +0x38  pos.z   = 0.0f
//
// After matrix writes:
//   DAT_00912bd8 = 0  (dirty flag reset, 0x00552820)
//   DAT_00912bec = 0  (secondary dirty flag reset, 0x0055282a)
//
// Returns: 1 (always, uint32_t).
//
// Callers (from C1 notes and call sites): 0x00427ff0, 0x00552c10, 0x00427f00.
// Analysis note: re/analysis/hud_promote_c2_b/0x00552750.md
// Leaf-exemption: no named callees (callees_depth1: []). C2->C3 leaf rule applies.
// ===========================================================================

// 0x00552750
extern "C" __declspec(dllexport)
std::uint32_t __cdecl FontCtx_ResetTransform()
{
    // Load current context pointer: (&DAT_00912a84)[DAT_00912b04]
    float* ctx = reinterpret_cast<float*>(
        g_FontCtxPtrs_k4[*g_FontCtxDepth_k4]);

    // Identity matrix writes (linear sequence, 0x00552750–0x005527fe):
    // +0x00: right.x = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x00u) = 1.0f;  // 0x3f800000
    // +0x04: right.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x04u) = 0.0f;
    // +0x08: right.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x08u) = 0.0f;
    // +0x0c: flags |= 0x20003  (0x00552804 — OR not assignment)
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x0cu) |= 0x20003u;
    // +0x10: up.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x10u) = 0.0f;
    // +0x14: up.y = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x14u) = 1.0f;  // 0x3f800000
    // +0x18: up.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x18u) = 0.0f;
    // +0x20: at.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x20u) = 0.0f;
    // +0x24: at.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x24u) = 0.0f;
    // +0x28: at.z = 1.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x28u) = 1.0f;  // 0x3f800000
    // +0x30: pos.x = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x30u) = 0.0f;
    // +0x34: pos.y = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x34u) = 0.0f;
    // +0x38: pos.z = 0.0f
    *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(ctx) + 0x38u) = 0.0f;

    // Dirty flag resets (0x00552820, 0x0055282a):
    *g_DirtyBd8_k4 = 0u;
    *g_DirtyBec_k4 = 0u;

    // Returns 1 always.
    return 1u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(FontCtx_ResetTransform, 0x00552750);

// ---------------------------------------------------------------------------
// HudSpinCoinAnim  --  0x00428450
//
// Original: FUN_00428450 (305 bytes, 0x00428450..0x00428580)
// Signature: void FUN_00428450(int param_1, int param_2)
//   param_1: horizontal position offset (screen units)
//   param_2: vertical position offset (screen units)
//
// Computes sin of current spin angle (_DAT_0067d974), scales by screen dims
// and constants, then calls FUN_00450b10 (HudIm2DQuad) with computed x/y/w/h.
// Advances spin angle: _DAT_0067d974 += _DAT_005cc56c.
//
// Constants (cited from 0x00428450 body):
//   0x0067d974 — spin angle accumulator (float32)
//   0x005cc56c — angular velocity constant (float32)
//   0x005d757c — sin threshold for UV horizontal flip (float32)
//   0x00771960 — pointer to texture handle (ptr-to-undefined4)
//   0x005cd5a8 — screen-width scale factor (float32)
//   0x005cd65c — x-position base constant (float32)
//   0x005cd274 — x-position sin scale constant (float32)
//   0x005cc560 — y/height scale constant (float32)
//   0x005cd658 — y-position offset constant (float32)
//   0x005cc730 — size ratio constant (float32)
//
// Smoke-test target for spin_angle_observe harness arg_type (2026-05-22).
// Full C3 promotion is c3_batch_p's job.
// Analysis note: re/analysis/hud_ingame_promote_c2/0x00428450.md
// ---------------------------------------------------------------------------

// 0x00428450
extern "C" __declspec(dllexport) void __cdecl HudSpinCoinAnim(int param_1, int param_2) {
    // Spin angle accumulator (float32) at DAT_0067d974.
    float& spinAngle = *reinterpret_cast<float*>(0x0067d974u);

    // Compute sin of current spin angle using x87 fsin (matches original MSVC codegen).
    // Original uses: fsin((float10)_DAT_0067d974) -> narrowed to float32.
    // std::sin uses SSE2 double; fsin uses x87 80-bit extended — bit-level diverges.
    // We load the raw float from the angle pointer directly to avoid reference aliasing.
    float sinVal;
    {
        float* pAngle = &spinAngle;
        __asm {
            mov eax, pAngle
            fld dword ptr [eax]
            fsin
            fstp sinVal
        }
    }

    // Default UV layout (coin face, forward-facing).
    std::uint32_t local_10 = 0u;            // u0 = 0.0f
    std::uint32_t local_c  = 0u;            // v0 = 0.0f
    std::uint32_t local_8  = 0x3f000000u;   // u1 = 0.5f
    std::uint32_t local_4  = 0x3f800000u;   // v1 = 1.0f

    std::uint32_t texHandle = 0u;

    // Screen width (int16_t from FUN_0042b8b0, sign-extended).
    typedef short (__cdecl *GetWidth_t)();
    typedef short (__cdecl *GetHeight_t)();
    auto GetWidth  = reinterpret_cast<GetWidth_t>(0x0042b8b0u);
    auto GetHeight = reinterpret_cast<GetHeight_t>(0x0042b8c0u);
    short sw = GetWidth();
    float screenW = static_cast<float>(static_cast<std::int32_t>(sw));
    float scaleW  = *reinterpret_cast<float*>(0x005cd5a8u);
    float fVar2   = screenW * scaleW;

    short sh = GetHeight();
    float screenH = static_cast<float>(static_cast<std::int32_t>(sh));

    // UV horizontal flip when sin < threshold.
    float threshold = *reinterpret_cast<float*>(0x005d757cu);
    if (sinVal < threshold) {
        local_10 = 0x3f800000u;  // u0 = 1.0f (flip left edge)
        local_8  = 0x3f000000u;  // u1 = 0.5f (unchanged)
    }

    // Texture handle from global pointer (if non-null).
    std::uint32_t** texPtrPtr = reinterpret_cast<std::uint32_t**>(0x00771960u);
    if (*texPtrPtr != nullptr) {
        texHandle = **texPtrPtr;
    }

    // Draw call through HudIm2DQuad (0x00450b10).
    float baseCx  = *reinterpret_cast<float*>(0x005cd65cu);
    float sinScX  = *reinterpret_cast<float*>(0x005cd274u);
    float scaleYH = *reinterpret_cast<float*>(0x005cc560u);
    float yOff    = *reinterpret_cast<float*>(0x005cd658u);
    float ratio   = *reinterpret_cast<float*>(0x005cc730u);

    float drawX = fVar2 * ((baseCx - sinVal * sinScX) + static_cast<float>(param_1));
    float drawY = screenH * scaleYH * (static_cast<float>(param_2) + yOff);
    float drawW = sinVal * ratio * fVar2;
    float drawH = screenH * scaleYH * ratio;

    struct UVArray { std::uint32_t u0, v0, u1, v1; };
    UVArray uvs = { local_10, local_c, local_8, local_4 };

    typedef void (__cdecl *DrawQuad_t)(std::uint32_t, float, float, float, float, std::uint32_t, UVArray*);
    auto DrawQuad = reinterpret_cast<DrawQuad_t>(0x00450b10u);
    DrawQuad(texHandle, drawX, drawY, drawW, drawH, 0xffffffffu, &uvs);

    // Advance spin angle accumulator.
    float angVel = *reinterpret_cast<float*>(0x005cc56cu);
    spinAngle += angVel;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(HudSpinCoinAnim, 0x00428450);
// Re-enable refused 2026-05-24: diff-original 5/10 mismatches — reimpl diverges from
// original on real test vectors. The hooks.csv C4 promotion claim was based on
// canonical-observation evidence from the loader-broken window, not actual diff.
// Needs decomp re-read and reimpl rewrite.
