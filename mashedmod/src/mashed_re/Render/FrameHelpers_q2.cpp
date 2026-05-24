// Mashed RE — Render ParticleEmitter state setters (c3-batch-q session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004769a0  ParticleEmitter_SetPosition — void(float*); NULL→default 0x006925a8; writes XYZ to 0x00692528/2c/30
//   0x004769d0  ParticleEmitter_SetVelocity — void(float*); NULL→default 0x00613288; writes XY to 0x006924dc/e0
//   0x004769f0  ParticleEmitter_SetColour   — void(uint32_t*); NULL→default 0x00613290; writes dword to 0x00692554
//   0x00476a30  ParticleEmitter_SetScalar   — void(uint32_t); direct value write to 0x006924d8 (no NULL guard)
//   0x00476a40  ParticleEmitter_SetRGBA     — void(uint32_t*); NULL→default 0x00613294; writes 4 dwords to 0x00692598/9c/a0/a4
//
// All five are pure leaves (callees_depth1: []).  Leaf-function exemption applies.
// Bit-identical Frida A/B via void_setter_observe (NULL-path for ptr variants; value tests for SetScalar).
//
// Analysis notes:
//   re/analysis/promote_c2_render_frame/0x004769a0.md
//   re/analysis/promote_c2_render_frame/0x004769d0.md
//   re/analysis/promote_c2_render_frame/0x004769f0.md
//   re/analysis/promote_c2_render_frame/0x00476a30.md
//   re/analysis/promote_c2_render_frame/0x00476a40.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// ParticleEmitter_SetPosition  --  0x004769a0
//
// Original: FUN_004769a0 (located at 0x004769a0)
// Signature: void FUN_004769a0(float *param_1)
// Returns: void
//
// Body (cited from 0x004769a0):
//   if (param_1 == NULL) param_1 = &DAT_006925a8;  // NULL guard — use default XYZ
//   DAT_00692528 = param_1[0];   // X
//   DAT_0069252c = param_1[1];   // Y
//   DAT_00692530 = param_1[2];   // Z
//
// Globals (cited from 0x004769a0 body):
//   0x006925a8 — default position buffer (3 × float, likely 0.0f each)
//   0x00692528 — active emitter position X staging
//   0x0069252c — active emitter position Y staging
//   0x00692530 — active emitter position Z staging
//
// Caller: FUN_004212b0 (C1), FUN_00418e70 (C1), FUN_00413cb0 (C1).
// Callee: none (pure leaf; leaf-function exemption applies).
//
// Consumed by: FUN_00476d00 (C2) which copies DAT_00692528/2c/30 into the batch pool.
//
// ref: re/analysis/promote_c2_render_frame/0x004769a0.md
// ---------------------------------------------------------------------------

// 0x004769a0
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetPosition(float* param_1)
{
    // NULL guard: use default position buffer. [0x004769a0 body, NULL check]
    if (param_1 == nullptr) {
        param_1 = reinterpret_cast<float*>(0x006925a8u);  // DAT_006925a8 — default XYZ
    }
    // Write XYZ into emitter position staging globals. [0x004769a0 body, writes]
    *reinterpret_cast<float*>(0x00692528u) = param_1[0];  // DAT_00692528 = X
    *reinterpret_cast<float*>(0x0069252cu) = param_1[1];  // DAT_0069252c = Y
    *reinterpret_cast<float*>(0x00692530u) = param_1[2];  // DAT_00692530 = Z
}

RH_ScopedInstall(ParticleEmitter_SetPosition, 0x004769a0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// ParticleEmitter_SetVelocity  --  0x004769d0
//
// Original: FUN_004769d0 (located at 0x004769d0)
// Signature: void FUN_004769d0(float *param_1)
// Returns: void
//
// Body (cited from 0x004769d0):
//   if (param_1 == NULL) param_1 = &DAT_00613288;  // NULL guard — use default XY
//   DAT_006924dc = param_1[0];   // Vx
//   DAT_006924e0 = param_1[1];   // Vy
//
// Globals (cited from 0x004769d0 body):
//   0x00613288 — default velocity buffer (2 × float, likely 0.0f)
//   0x006924dc — active emitter velocity Vx staging
//   0x006924e0 — active emitter velocity Vy staging
//
// Caller: FUN_004212b0 (C1), FUN_00413cb0 (C1).
// Callee: none (pure leaf; leaf-function exemption applies).
//
// Consumed by: FUN_00476d00 (C2) which copies DAT_006924dc/e0 into the batch pool.
//
// ref: re/analysis/promote_c2_render_frame/0x004769d0.md
// ---------------------------------------------------------------------------

// 0x004769d0
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetVelocity(float* param_1)
{
    // NULL guard: use default velocity buffer. [0x004769d0 body, NULL check]
    if (param_1 == nullptr) {
        param_1 = reinterpret_cast<float*>(0x00613288u);  // DAT_00613288 — default XY
    }
    // Write XY into emitter velocity staging globals. [0x004769d0 body, writes]
    *reinterpret_cast<float*>(0x006924dcu) = param_1[0];  // DAT_006924dc = Vx
    *reinterpret_cast<float*>(0x006924e0u) = param_1[1];  // DAT_006924e0 = Vy
}

RH_ScopedInstall(ParticleEmitter_SetVelocity, 0x004769d0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// ParticleEmitter_SetColour  --  0x004769f0
//
// Original: FUN_004769f0 (located at 0x004769f0)
// Signature: void FUN_004769f0(uint32_t *param_1)
// Returns: void
//
// Body (cited from 0x004769f0):
//   if (param_1 == NULL):
//       DAT_00692554 = DAT_00613290;   // copy default colour dword
//   else:
//       DAT_00692554 = param_1[0];     // copy caller's colour dword
//
// Globals (cited from 0x004769f0 body):
//   0x00613290 — default colour dword
//   0x00692554 — active emitter colour staging
//
// Caller: FUN_00413cb0 (C1), FUN_00418e70 (C1), VehicleShadowRender/0x0041faf0 (C2 — sibling cluster).
// Callee: none (pure leaf; leaf-function exemption applies).
//
// Consumed by: FUN_00476d00 (C2) which copies DAT_00692554 into the batch pool.
//
// ref: re/analysis/promote_c2_render_frame/0x004769f0.md
// ---------------------------------------------------------------------------

// 0x004769f0
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetColour(std::uint32_t* param_1)
{
    if (param_1 == nullptr) {
        // NULL branch: copy default colour dword from DAT_00613290. [0x004769f0 body, NULL branch]
        *reinterpret_cast<std::uint32_t*>(0x00692554u) =
            *reinterpret_cast<std::uint32_t*>(0x00613290u);  // DAT_00692554 = DAT_00613290
    } else {
        // Non-NULL branch: copy caller's colour dword. [0x004769f0 body, else branch]
        *reinterpret_cast<std::uint32_t*>(0x00692554u) = param_1[0];  // DAT_00692554 = *param_1
    }
}

RH_ScopedInstall(ParticleEmitter_SetColour, 0x004769f0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// ParticleEmitter_SetScalar  --  0x00476a30
//
// Original: FUN_00476a30 (located at 0x00476a30)
// Signature: void FUN_00476a30(uint32_t param_1)
// Returns: void
//
// Body (cited from 0x00476a30):
//   DAT_006924d8 = param_1     // direct write; no NULL guard; param_1 is a VALUE not a pointer
//
// Note: decompiler emits `_DAT_006924d8` (overlap warning from a smaller symbol at the same
// address); raw dword write confirmed. param_1 carries the emitter scalar (e.g. rotation/size).
//
// Globals (cited from 0x00476a30 body):
//   0x006924d8 — active emitter scalar staging (_DAT_006924d8)
//
// Caller: FUN_00413cb0 (C1), FUN_00418e70 (C1), FUN_004212b0 (C1), FUN_00456140 (C1).
// Callee: none (pure leaf; leaf-function exemption applies).
//
// Consumed by: FUN_00476d00 (C2) which copies _DAT_006924d8 into the scalar_array channel.
//
// ref: re/analysis/promote_c2_render_frame/0x00476a30.md
// ---------------------------------------------------------------------------

// 0x00476a30
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetScalar(std::uint32_t param_1)
{
    // Direct write; no NULL guard — param_1 is a value, not a pointer. [0x00476a30 body]
    *reinterpret_cast<std::uint32_t*>(0x006924d8u) = param_1;  // _DAT_006924d8 = param_1
}

RH_ScopedInstall(ParticleEmitter_SetScalar, 0x00476a30);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// ParticleEmitter_SetRGBA  --  0x00476a40
//
// Original: FUN_00476a40 (located at 0x00476a40)
// Signature: void FUN_00476a40(uint32_t *param_1)
// Returns: void
//
// Body (cited from 0x00476a40):
//   if (param_1 == NULL) param_1 = &DAT_00613294;  // NULL guard — use default RGBA
//   DAT_00692598 = param_1[0];   // R
//   DAT_0069259c = param_1[1];   // G
//   DAT_006925a0 = param_1[2];   // B
//   DAT_006925a4 = param_1[3];   // A
//
// Globals (cited from 0x00476a40 body):
//   0x00613294 — default RGBA buffer (4 × uint32_t)
//   0x00692598 — active emitter R staging
//   0x0069259c — active emitter G staging
//   0x006925a0 — active emitter B staging
//   0x006925a4 — active emitter A staging
//
// Caller: FUN_00413cb0 (C1), FUN_004212b0 (C1), FUN_00456140 (C1).
// Callee: none (pure leaf; leaf-function exemption applies).
//
// Consumed by: FUN_00476d00 (C2) which copies DAT_00692598/9c/a0/a4 into the rgba_array channel.
//
// ref: re/analysis/promote_c2_render_frame/0x00476a40.md
// ---------------------------------------------------------------------------

// 0x00476a40
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetRGBA(std::uint32_t* param_1)
{
    // NULL guard: use default RGBA buffer. [0x00476a40 body, NULL check]
    if (param_1 == nullptr) {
        param_1 = reinterpret_cast<std::uint32_t*>(0x00613294u);  // DAT_00613294 — default RGBA
    }
    // Write RGBA into emitter RGBA staging globals. [0x00476a40 body, writes]
    *reinterpret_cast<std::uint32_t*>(0x00692598u) = param_1[0];  // DAT_00692598 = R
    *reinterpret_cast<std::uint32_t*>(0x0069259cu) = param_1[1];  // DAT_0069259c = G
    *reinterpret_cast<std::uint32_t*>(0x006925a0u) = param_1[2];  // DAT_006925a0 = B
    *reinterpret_cast<std::uint32_t*>(0x006925a4u) = param_1[3];  // DAT_006925a4 = A
}

RH_ScopedInstall(ParticleEmitter_SetRGBA, 0x00476a40);  // re-enabled 2026-05-24 c3-render-b
