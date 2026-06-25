// Mashed RE — per-vehicle world-space velocity getter (0x0046d510).
//
// Direct twin of 0x0046d700 VehicleVec3At9C8Get, but transforms the +0xac
// velocity float3 by the matrix at 0x00614708 (in place) via the RW device
// transform dispatch FUN_004c3df0 (RwV3dTransformPoints, C4) BEFORE copying it
// out. out3_idx evidence validates the 0/1 BOUNDS RETURN (the accepted observable
// for this family); the transformed vec3 values are not part of the diff fingerprint.
//
// Disasm 0x0046d510 (verbatim):
//   push esi ; mov esi,[esp+0xc]        ; param_2 (vehicleIdx)
//   cmp esi,0x10 ; jb body              ; idx < 16
//   xor eax,eax ; pop esi ; ret         ; OOB -> 0
//  body:
//   imul esi,esi,0xd04                  ; idx * 0xd04 (byte offset, stride 0x341 dwords)
//   lea eax,[esi+0x881ec8] ; push eax   ; arg3 = per-vehicle struct base
//   push 1                              ; arg2 = 1
//   lea edi,[esi+0x881f74] ; push 0x614708 ; push edi  ; arg1=matrix arg0=veldst
//   call 0x4c3df0                       ; FUN_004c3df0(veldst, 0x614708, 1, structbase)
//   out[0]=*(0x881f74+off) ; out[1]=*(0x881f78+off) ; out[2]=*(0x881f7c+off) ; ret 1
// Ref: re/analysis/bucket_ai_00452eb0_004c3df0/0046d510.md
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
constexpr std::uintptr_t kVehBase_881ec8 = 0x00881ec8u;  // per-vehicle struct base
constexpr std::uintptr_t kVelX_881f74    = 0x00881f74u;  // velocity float3 (+0xac)
constexpr std::uintptr_t kVelY_881f78    = 0x00881f78u;
constexpr std::uintptr_t kVelZ_881f7c    = 0x00881f7cu;
constexpr std::uintptr_t kMatrix_614708  = 0x00614708u;
constexpr std::uint32_t  kByteStride     = 0xd04u;        // 0x341 dwords

// 0x004c3df0  FUN_004c3df0  RwV3dTransformPoints — void __cdecl(dst, mtx, count, src).
typedef void(__cdecl* fn_4c3df0_t)(void*, void*, int, void*);
} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046d510  VehicleVelocityWorldGet(out_vec3, vehicleIdx) -> 1 in-range / 0 OOB
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleVelocityWorldGet(std::uint32_t* outVec,
                                                                     std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0;
    const std::uintptr_t off = static_cast<std::uintptr_t>(vehicleIdx) * kByteStride;
    reinterpret_cast<fn_4c3df0_t>(0x004c3df0u)(
        reinterpret_cast<void*>(kVelX_881f74 + off),     // dst = velocity float3
        reinterpret_cast<void*>(kMatrix_614708),         // matrix
        1,                                               // count
        reinterpret_cast<void*>(kVehBase_881ec8 + off)); // src = per-vehicle struct base
    outVec[0] = *reinterpret_cast<const std::uint32_t*>(kVelX_881f74 + off);
    outVec[1] = *reinterpret_cast<const std::uint32_t*>(kVelY_881f78 + off);
    outVec[2] = *reinterpret_cast<const std::uint32_t*>(kVelZ_881f7c + off);
    return 1;
}
RH_ScopedInstall(VehicleVelocityWorldGet, 0x0046d510);
