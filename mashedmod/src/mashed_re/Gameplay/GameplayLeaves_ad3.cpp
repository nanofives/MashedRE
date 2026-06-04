// Mashed RE — Gameplay pure-leaf C2->C3 promotions (c3_batch_ad session 3).
//
// Three viable leaves harvested from the gameplay bucket
// (re/analysis/bucket_gameplay_0045dff0_0046dd90 + frontend_gate_unblock_u):
//   0x0046bce0  VehicleVec3At94Get   — per-vehicle vec3 getter   (out3_idx)
//   0x0046d740  VehicleVec3At6E4Set  — per-vehicle vec3 setter   (out3_idx)
//   0x00461e90  SurfaceCodeClassify  — pure (sel,key)->code pair (int2_ptr2_out)
//
// The other 8 candidates in the session were SKIPPED (see PROMOTION_QUEUE row):
//   0046bd60/0046d2e0 (two-index getters), 0046be10/0046be50 (4-arg setters),
//   0046dd90 (void indexed setter), 00417450/00417530 (stateful arena grid),
//   0041f880 (thiscall w/ callee + structured this) — none expressible by an
//   EXISTING diff_template.js arg_type; routed to harness-extension.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// Per-vehicle record: stride 0x341 DWORDs == 0xD04 bytes (same layout family as
// Vehicle/VehicleState.cpp).
static constexpr std::uintptr_t kVehicleBase_882094 = 0x00882094u;  // vec3.x dword base
static constexpr std::uintptr_t kVehicleBase_8816e4 = 0x008816e4u;  // vec3 (+0x6e4) byte base
static constexpr std::uint32_t  kDWordStride        = 0x341u;
static constexpr std::uint32_t  kByteStride         = 0xD04u;

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046bce0  VehicleVec3At94Get
// Ghidra decomp (FUN_0046bce0):
//   if (0xf < param_2) return 0;
//   *param_1     = (&DAT_00882094)[param_2 * 0x341];
//   param_1[1]   = (&DAT_00882098)[param_2 * 0x341];
//   param_1[2]   = (&DAT_0088209c)[param_2 * 0x341];
//   return 1;
// Pure getter of the per-vehicle vec3 at record +0x94 (contiguous dwords at
// 0x00882094/98/9c, dword stride 0x341). U-8408: vec3 semantic unconfirmed —
// does not affect offset/bounds correctness.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleVec3At94Get(std::uint32_t* outVec, std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0;
    const std::uint32_t* base = reinterpret_cast<const std::uint32_t*>(kVehicleBase_882094);
    outVec[0] = base[vehicleIdx * kDWordStride];
    outVec[1] = base[vehicleIdx * kDWordStride + 1];
    outVec[2] = base[vehicleIdx * kDWordStride + 2];
    return 1;
}
RH_ScopedInstall(VehicleVec3At94Get, 0x0046bce0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046d740  VehicleVec3At6E4Set
// Ghidra decomp (FUN_0046d740):
//   if (0xf < param_2) return 0;
//   iVar1 = param_2 * 0xd04;
//   *(undefined4*)(&DAT_008816e4 + iVar1) = *param_1;
//   *(undefined4*)(&DAT_008816e8 + iVar1) = param_1[1];
//   *(undefined4*)(&DAT_008816ec + iVar1) = param_1[2];
//   return 1;
// Setter for the per-vehicle vec3 at record +0x6e4 (byte stride 0xd04).
// param_1 = input vec3 (3 dwords). U-8421: vec3 semantic unconfirmed.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleVec3At6E4Set(const std::uint32_t* inVec, std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0;
    std::uintptr_t rec = kVehicleBase_8816e4 + vehicleIdx * kByteStride;
    *reinterpret_cast<std::uint32_t*>(rec)     = inVec[0];
    *reinterpret_cast<std::uint32_t*>(rec + 4) = inVec[1];
    *reinterpret_cast<std::uint32_t*>(rec + 8) = inVec[2];
    return 1;
}
RH_ScopedInstall(VehicleVec3At6E4Set, 0x0046d740);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00461e90  SurfaceCodeClassify
// Pure leaf: undefined4 FUN(undefined4 sel, int key, undefined4* out1, undefined4* out2).
// switch(sel) over {0,2,3,0xb,0x19,0x1a,0x1e,0x21,0x22,0x24,0x25,0x26,0x27}; each
// case compares `key` against fixed (negative) constants and writes a pair of
// codes in {0,1,2,3} to *out1/*out2, returning 0/1. No globals, no callees.
// Ported verbatim from the Ghidra decompilation (labels/gotos preserved 1:1 so
// the control flow is bit-identical). U-8393/U-8394: comparand + output-code
// semantics unconfirmed; mechanical mapping is exact.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
SurfaceCodeClassify(std::uint32_t sel, int key, std::uint32_t* out1, std::uint32_t* out2) {
    switch (sel) {
    case 0:
        if (key == -0x9b9b9c) {
        LAB_00461ec1:
            *out1 = 0; *out2 = 1; return 1;
        }
        if (key != -0x697f80) { *out1 = 0; *out2 = 1; return 0; }
        break;
    default:
        *out1 = 3; *out2 = 3; return 0;
    case 2:
        if (key == -0x4b7f80) { *out1 = 2; *out2 = 1; return 1; }
        if (key != -0x377f80) {
            if (key == -0x237f80) { *out1 = 2; *out2 = 2; return 1; }
            *out1 = 2; *out2 = 2; return 0;
        }
        break;
    case 3:
        if (key == -0x7fc000) { *out1 = 1; *out2 = 0; return 0; }
        if (key == -0x237f80) { *out1 = 2; *out2 = 2; return 1; }
        goto LAB_00461f4f;
    case 0xb:
        if (key == -0xff9b4c) {
        LAB_00461f66:
            *out1 = 3; *out2 = 3; return 1;
        }
        if (key == -0x7f3800) { *out1 = 2; *out2 = 2; return 1; }
        goto joined_r0x004622d6;
    case 0x19:
        if (key == -0xe17f4c) { *out1 = 1; *out2 = 0; return 1; }
        if (key == -0x69e1a6) {
        LAB_00461f9a:
            *out1 = 0; *out2 = 0; return 1;
        }
        goto LAB_00462267;
    case 0x1a:
        if (key == -0x7f3800) { *out1 = 1; *out2 = 1; return 1; }
        if (key != -0x697f80) {
            if (key == -0x377f80) { *out1 = 2; *out2 = 0; return 1; }
            *out1 = 0; *out2 = 0; return 0;
        }
        goto LAB_00462074;
    case 0x1e:
        if (key == -0x4b7f80) { *out1 = 2; *out2 = 1; return 1; }
        if (key == -0x377f80) goto LAB_00461ec1;
        goto LAB_00461f4f;
    case 0x21:
        if (key < -0x7fbfbf) {
            if (key == -0x7fbfc0) break;
            if (key == -0xff8000) { *out1 = 1; *out2 = 0; return 1; }
            if (key == -0xff7f38) { *out1 = 0; *out2 = 3; return 1; }
            goto LAB_004622dc;
        }
    joined_r0x004622d6:
        if (key != -0x697f80) {
        LAB_004622dc:
            *out1 = 0; *out2 = 1; return 0;
        }
    LAB_00462074:
        *out1 = 0; *out2 = 1; return 1;
    case 0x22:
        if (key == -0xffa54c) { *out1 = 3; *out2 = 2; return 1; }
        if (key == -0xff9b4c) { *out1 = 3; *out2 = 3; return 1; }
        if (key != -0x697f80) { *out1 = 2; *out2 = 1; return 0; }
        goto LAB_00462074;
    case 0x24:
        if (key < -0x4b7f7f) {
            if (key == -0x4b7f80) { *out1 = 1; *out2 = 2; return 1; }
            if (key == -0x9b9b9c) { *out1 = 0; *out2 = 2; return 1; }
        }
        else if (key == -0x237f80) { *out1 = 1; *out2 = 1; return 0; }
        *out1 = 0; *out2 = 2; return 0;
    case 0x25:
        if (key < -0x7f37ff) {
            if (key == -0x7f3800) { *out1 = 1; *out2 = 1; return 1; }
            if (key == -0xff8000) { *out1 = 1; *out2 = 0; return 1; }
            if (key == -0xebaf74) { *out1 = 1; *out2 = 2; return 1; }
        }
        else if (key == -0x697f80) break;
    LAB_00462267:
        *out1 = 0; *out2 = 0; return 0;
    case 0x26:
        if (key != -0x697f80) { *out1 = 3; *out2 = 3; return 0; }
        break;
    case 0x27:
        if (key == -0x69e1a6) goto LAB_00461f9a;
        if (key == -0x4b4b4c) { *out1 = 3; *out2 = 2; return 1; }
        if (key == -1) goto LAB_00461f66;
    LAB_00461f4f:
        *out1 = 3; *out2 = 3; return 0;
    }
    *out1 = 0; *out2 = 1; return 1;
}
RH_ScopedInstall(SurfaceCodeClassify, 0x00461e90);
