// Mashed RE — AI driver cluster: line-of-sight + spline-support leaves (WS-C2).
//
// Verbatim ports of clean, isolatable leaves of the FUN_00418860 AI cluster:
//   0x00416060  AiLineOfSight       XZ ray-march tile LOS  (0=blocked, 1=clear)
//   0x004161e0  AiSplineTargetInit  seed target point from own XZ -> FUN_00443dc0
//   0x0046d510  AiVehicleVelocity3  per-vehicle velocity float3 getter
//
// AiLineOfSight is the C3a per-function-diff target named in ai_controller.md
// §10: two float* in, an int flag out -> directly run_diff-able in isolation.
//
// NOT ported (stays called-by-RVA): FUN_004150e0 (wall-lateral query) reads its
// two args off the x87 FPU stack (void sig + __ftol(ST0) of caller values) — not
// expressible as standard C++; needs a naked-asm shim. Carried for a later pass.
//
// Build is x87 (no /arch:SSE2) so plain C++ float math matches the original FPU
// codegen ([[project-wsa2-rwmath-bitident]]); __ftol(ST0) of a locally-computed
// value == (int)expr under x87.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Source decompilations (pool5, 2026-06-16):
//   re/analysis/ai_update_d2/0x00416060.md     + this session's exact decomp
//   re/analysis/bucket_ai_00415d00_00452ea0/0x004161e0.md + exact decomp
//   re/analysis/ai_update_d2/0x0046d510.md     + exact decomp

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>

using namespace Ai;

namespace {

// Vec2 length (0x004c3bf0 = Vec2Length, C4) — returns float10 in ST0.
typedef float (__cdecl* fn_vec2len_t)(float*);
inline float call_004c3bf0(float* v) { return reinterpret_cast<fn_vec2len_t>(0x004c3bf0)(v); }
// Own-vehicle struct ptr getter (0x0046d4a0, C3).
typedef void (__cdecl* fn_ptrget_t)(int*, int);
inline void call_0046d4a0(int* out, int v) { reinterpret_cast<fn_ptrget_t>(0x0046d4a0)(out, v); }
// Spline lookahead target finder (0x00443dc0): f(spline, xz, outIdx, v, 1, 0).
typedef void (__cdecl* fn_443dc0_t)(void*, float*, void*, int, int, int);
inline void call_00443dc0(void* spline, float* xz, void* outIdx, int v) {
    reinterpret_cast<fn_443dc0_t>(0x00443dc0)(spline, xz, outIdx, v, 1, 0);
}
// RW transform-points (0x004c3df0): f(dst, src, 1, matrix).
typedef void (__cdecl* fn_4c3df0_t)(void*, void*, int, void*);
inline void call_004c3df0(void* dst, void* src, void* mtx) {
    reinterpret_cast<fn_4c3df0_t>(0x004c3df0)(dst, src, 1, mtx);
}

// Track tile grid (cited at 0x00416060 / 0x004150e0): 128x128 shorts at 0x007f1a9c,
// 8x8 sub-cell chars per tile at 0x007f9a9c.
static constexpr std::uintptr_t kTileGrid    = 0x007f1a9cu;
static constexpr std::uintptr_t kSubCellGrid = 0x007f9a9cu;

} // namespace

// ===========================================================================
// 0x00416060  AiLineOfSight(posA_xz, posB_xz) -> 0 blocked / 1 clear
//
// Ray-marches A->B in 1/length steps (step _DAT_005cc564); at each sample with
// a moved position, looks up the track tile (>0 && <0x200) and its sub-cell;
// sub-cell type 0 or 3 = wall -> blocked. Cited: 0x00416060 body.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiLineOfSight(float* param_1, float* param_2)
{
    float local_8  = param_2[0] - param_1[0];   // dx
    float local_18 = 0.0f;                       // t
    float local_4  = param_2[1] - param_1[1];   // dz
    float fVar1 = call_004c3bf0(&local_8);       // length(dx,dz)  [&local_8 = {dx,dz}]
    if (F32(0x005d757cu) < fVar1) {              // length > 0
        float fVar4 = F32(0x005cc320u) / fVar1;  // 1.0 / length
        do {
            float sampleX = fVar4 * local_8 * local_18 + param_1[0];
            float sampleZ = fVar4 * local_4 * local_18 + param_1[1];
            if (sampleX != param_1[0] && sampleZ != param_1[1]) {
                int ix = static_cast<int>(sampleX);   // __ftol
                int iz = static_cast<int>(sampleZ);   // __ftol
                short tile = *reinterpret_cast<short*>(
                    kTileGrid + static_cast<std::uintptr_t>(
                        ((( ix + 0x1f0) >> 3) * 0x80 + ((iz + 0x1f0) >> 3)) * 2));
                if (0 < tile && tile < 0x200) {
                    char sub = *reinterpret_cast<char*>(
                        kSubCellGrid + static_cast<std::uintptr_t>(
                            ((iz & 7) + tile * 8) * 8 + (ix & 7)));
                    if (sub == 0 || sub == 3) {
                        return 0;
                    }
                }
            }
            local_18 = local_18 + F32(0x005cc564u);   // t += step
        } while (local_18 < fVar1);
    }
    return 1;
}

RH_ScopedInstall(AiLineOfSight, 0x00416060);

// ===========================================================================
// 0x004161e0  AiSplineTargetInit(spline, outTargetIdx, vehicle)
//
// Reads the vehicle's own XZ (struct +0x30 / +0x38) and calls the spline
// lookahead finder to seed the target point. Cited: 0x004161e0 body.
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiSplineTargetInit(void* param_1, void* param_2, int param_3)
{
    int local_c;
    call_0046d4a0(&local_c, param_3);            // own struct ptr
    float xz[2];
    xz[0] = F32(static_cast<std::uintptr_t>(local_c) + 0x30u);   // local_8 = X
    xz[1] = F32(static_cast<std::uintptr_t>(local_c) + 0x38u);   // local_4 = Z
    call_00443dc0(param_1, xz, param_2, param_3);
}

RH_ScopedInstall(AiSplineTargetInit, 0x004161e0);

// ===========================================================================
// 0x0046d510  AiVehicleVelocity3(outVec3, vehicleIdx) -> 1 ok / 0 OOB
//
// Transforms the per-vehicle velocity (0x00881ec8 + v*0xd04) by 0x00614708 into
// 0x00881f74 + v*0xd04, then copies that float3 out. Cited: 0x0046d510 body.
//   0x00881f74 stride 0x341 dwords (= 0xd04 bytes); 0x0f index bound.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiVehicleVelocity3(std::uint32_t* param_1, std::uint32_t param_2)
{
    if (0xf < param_2) return 0;
    std::uintptr_t off = static_cast<std::uintptr_t>(param_2) * 0xd04u;
    call_004c3df0(reinterpret_cast<void*>(0x00881f74u + off),
                  reinterpret_cast<void*>(0x00614708u),
                  reinterpret_cast<void*>(0x00881ec8u + off));
    param_1[0] = U32(0x00881f74u + off);
    param_1[1] = U32(0x00881f78u + off);
    param_1[2] = U32(0x00881f7cu + off);
    return 1;
}

RH_ScopedInstall(AiVehicleVelocity3, 0x0046d510);

// ---------------------------------------------------------------------------
// More call-throughs for the steering + wall-ahead leaves.
namespace {
typedef void   (__cdecl* fn_norm_t)(float*, float*);            // RwV3dNormalize
typedef double (__cdecl* fn_acos_t)(double);                    // acos (float10 in ST0)
typedef void   (__cdecl* fn_velget_t)(std::uint32_t*, int);     // velocity vec3 getter
typedef char   (__cdecl* fn_tile2_t)(float, float);            // tile query (float args)
inline void   call_004c39b0(float* d, float* s) { reinterpret_cast<fn_norm_t>(0x004c39b0)(d, s); }
inline double call_004a3384(double x)           { return reinterpret_cast<fn_acos_t>(0x004a3384)(x); }
inline void   call_0046d510(std::uint32_t* o, int v) { reinterpret_cast<fn_velget_t>(0x0046d510)(o, v); }
inline char   call_00443d10(float x, float z)   { return reinterpret_cast<fn_tile2_t>(0x00443d10)(x, z); }
} // namespace

// ===========================================================================
// 0x00415e20  AiSteeringAngleError(vehicle, targetX, targetZ) -> signed angle
//
// Returns (via x87 ST0) the signed angular error between the bearing from the
// vehicle to (targetX,targetZ) and the vehicle's current heading: acos of each
// normalized direction dotted with +X, z-sign corrected, scaled by _DAT_005cc970,
// each wrapped to [0,2pi); the difference is re-wrapped to [0,2pi). Cited:
// 0x00415e20 body. Returned as float — MSVC leaves it in ST0 under x87 (the
// caller truncates to 32-bit immediately, so the value matches). acos via the
// original FUN_004a3384 (RVA), not MSVC acos.
// NOTE: (local_c,local_8,local_4) are stack-adjacent in the original and used as
// one 3-vector via &local_c — ported as an explicit float[3] to guarantee that.
// ===========================================================================
extern "C" __declspec(dllexport)
float __cdecl AiSteeringAngleError(int param_1, float param_2, float param_3)
{
    const float kZero = F32(0x005d757cu);
    const float kTwoPi = F32(0x005ccac4u);

    int local_10;
    call_0046d4a0(&local_10, param_1);
    float n[3];                                   // {local_c, local_8, local_4}
    n[0] = param_2 - F32(static_cast<std::uintptr_t>(local_10) + 0x30u);   // local_c
    n[1] = 0.0f;                                                            // local_8
    n[2] = -(param_3 - F32(static_cast<std::uintptr_t>(local_10) + 0x38u)); // local_4
    call_004c39b0(n, n);
    float fVar1 = n[2] * kZero + n[1] * kZero + n[0];
    if (fVar1 < F32(0x005cd0d0u)) fVar1 = F32(0x005cc33cu);
    if (F32(0x005cd0c8u) < fVar1) fVar1 = F32(0x005cc320u);
    float fVar2 = static_cast<float>(call_004a3384(static_cast<double>(fVar1))) * F32(0x005cc970u);
    if (n[2] - n[0] * kZero < kZero) fVar2 = -fVar2;
    while (fVar2 < kZero) fVar2 += kTwoPi;
    float bearing = fVar2;

    call_0046d510(reinterpret_cast<std::uint32_t*>(n), param_1);   // velocity into n[]
    n[2] = -n[2];                                   // local_4 = -local_4
    n[1] = 0.0f;                                     // local_8 = 0
    call_004c39b0(n, n);
    fVar1 = n[2] * kZero + n[1] * kZero + n[0];
    if (fVar1 < F32(0x005cd0d0u)) fVar1 = F32(0x005cc33cu);
    if (F32(0x005cd0c8u) < fVar1) fVar1 = F32(0x005cc320u);
    fVar2 = static_cast<float>(call_004a3384(static_cast<double>(fVar1))) * F32(0x005cc970u);
    if (n[2] - n[0] * kZero < kZero) fVar2 = -fVar2;
    while (fVar2 < kZero) fVar2 += kTwoPi;

    float result = bearing - fVar2;
    while (result < kZero) result += kTwoPi;
    while (result < kZero) result += kTwoPi;   // (decomp has the wrap twice)
    while (kTwoPi < result) result -= kTwoPi;
    return result;
}

RH_ScopedInstall(AiSteeringAngleError, 0x00415e20);

// ===========================================================================
// 0x00415d00  AiWallAhead(vehicle) -> 1 wall ahead / 0 clear
//
// Extrapolates the vehicle's velocity 2x forward and ray-marches that segment
// (1/length steps, step _DAT_005cc564) sampling the track tile via FUN_00443d10;
// tile type 0 or 3 => wall => 1. Suppresses the mode-2 ram. Cited: 0x00415d00.
// (local_c,local_8,local_4) and (local_1c,local_18) are stack-adjacent vectors
// in the original — ported as explicit arrays.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiWallAhead(int param_1)
{
    int local_28;
    call_0046d4a0(&local_28, param_1);
    float local_24 = F32(static_cast<std::uintptr_t>(local_28) + 0x30u);  // own X
    float local_20 = F32(static_cast<std::uintptr_t>(local_28) + 0x38u);  // own Z
    float vel[3];                                   // {local_c, local_8, local_4}
    call_0046d510(reinterpret_cast<std::uint32_t*>(vel), param_1);
    vel[0] = vel[0] + vel[0];                        // 2x velX
    float local_34 = 0.0f;
    vel[1] = vel[1] + vel[1];                        // 2x velY
    vel[2] = vel[2] + vel[2];                        // 2x velZ
    float seg[2];                                    // {local_1c, local_18}
    seg[0] = (vel[0] + local_24) - local_24;
    seg[1] = (vel[2] + local_20) - local_20;
    float fVar1 = call_004c3bf0(seg);               // length(seg)
    if (F32(0x005d757cu) < fVar1) {
        float fVar2 = F32(0x005cc320u) / fVar1;
        do {
            float local_14 = fVar2 * seg[0] * local_34 + local_24;
            float local_10 = fVar2 * seg[1] * local_34 + local_20;
            char cVar3 = call_00443d10(local_14, local_10);
            if (cVar3 == 0 || cVar3 == 3) {
                return 1;
            }
            local_34 = local_34 + F32(0x005cc564u);
        } while (local_34 < fVar1);
    }
    return 0;
}

RH_ScopedInstall(AiWallAhead, 0x00415d00);
