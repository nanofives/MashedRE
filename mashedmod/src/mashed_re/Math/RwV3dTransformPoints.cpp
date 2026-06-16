// Mashed RE - RwV3dTransformPoints: RW-device transform-method dispatch thunk.
//
// 0x004c3df0  FUN_004c3df0  WS-A2 (vehicle physics RW-math prereq)
//
// Verbatim from Ghidra (pool2, read-only, 2026-06-16). Body 0x004c3df0..0x004c3e1a
// (42 bytes). This is a thin __cdecl indirect-dispatch thunk into the RW/D3D device
// function table; it INVOKES an RW device method but is itself application-side glue
// (NOT a vendored library symbol — see re/analysis/bucket_ai_00452eb0_004c3df0/004c3df0.md,
// library_confirm).
//
// Disasm 0x004c3df0:
//   MOV EAX,[0x007d3ff8]              ; rw_globals
//   MOV ECX,[0x007d3ffc]             ; rw_offset
//   ... push param_4,param_3,param_2,param_1 ...
//   CALL [ECX + EAX + 0x14]          ; (**(code**)(rw_offset + rw_globals + 0x14))(p1,p2,p3,p4)
//   ADD ESP,0x10                     ; caller-cleanup => dispatched method is __cdecl
//   MOV EAX,ESI ; RET                ; return param_1 (unchanged)
//
// Decompiler:
//   undefined4 FUN_004c3df0(p1,p2,p3,p4) {
//       (**(code **)(DAT_007d3ffc + 0x14 + DAT_007d3ff8))(p1,p2,p3,p4);
//       return p1;
//   }
//
// Device slot +0x14 is the "transform points/vectors" method (default installed by
// FUN_004c35f0). Reading the same two globals (0x007d3ff8 / 0x007d3ffc) that the LUT
// math (Vec3Magnitude / RwV3dNormalize) reads makes this dispatch identical to the
// original by construction. [UNCERTAIN U-1891, pre-existing] the full contract of the
// method at slot +0x14 is data-dependent; the dispatch mechanics here are exact.
//
// Usage (caller FUN_0046d510): RwV3dTransformPoints(dst_vec3, matrix, 1, src_struct).
#include "../Core/HookSystem.h"

#include <cstdint>

static constexpr std::uintptr_t kRwGlobalsBase   = 0x007d3ff8u;  // DAT_007d3ff8
static constexpr std::uintptr_t kRwDeviceTblSlot = 0x007d3ffcu;  // DAT_007d3ffc
static constexpr std::uint32_t  kTransformMethod = 0x14u;        // device vtable slot

typedef void (__cdecl *RwDeviceTransformFn)(void*, void*, void*, void*);

// 0x004c3df0
extern "C" __declspec(dllexport)
void* __cdecl RwV3dTransformPoints(void* p1, void* p2, void* p3, void* p4)
{
    const std::uint32_t globals = *reinterpret_cast<const std::uint32_t*>(kRwGlobalsBase);
    const std::uint32_t offset  = *reinterpret_cast<const std::uint32_t*>(kRwDeviceTblSlot);

    RwDeviceTransformFn method =
        *reinterpret_cast<RwDeviceTransformFn*>(offset + globals + kTransformMethod);
    method(p1, p2, p3, p4);

    return p1;  // pass-through of the first (destination) argument
}

RH_ScopedInstall(RwV3dTransformPoints, 0x004c3df0);
