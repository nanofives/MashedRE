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
//               calls; needs new harness arg_type (spin_angle_observe or global-reset
//               wrapper); deferred to harness-extension session.

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

RH_ScopedInstall(FontCtx_ResetTransform, 0x00552750);
