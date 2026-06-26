// Mashed RE — sky/cloud-element removal function.
//
// 0x00487e40  FUN_00487e40  sky subsystem
//   Removes the sky/cloud particle element at index param_1 from the 128-slot
//   element array (stride 0xe dwords = 0x38 bytes; array base DAT_0086ae20).
//   Zeros the slot's active flag (base+0x14 = DAT_0086ae34) and its
//   transform/model pointer (base+0x2c = DAT_0086ae4c), then clears the
//   system-wide dirty/valid flag DAT_007030b4.  Returns 1 on success, 0 if
//   the index is out of range (>= 128; signed compare CMP EAX,0x80 / JL).
//
// Calling convention: cdecl.  Body: 0x00487e40..0x00487e6a (42 bytes). Leaf.
// Source: re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00487e40.md
#include "../Core/HookSystem.h"

#include <cstdint>

// Per-element stride in bytes (0xe dwords).                   // 0x00487e4e IMUL *0x38
static constexpr int kSkyElemStride = 0x38;

// Base addresses of the two per-element fields inside the global array.
static constexpr std::uintptr_t kSkyElemActive    = 0x0086ae34u; // DAT_0086ae34 (base+0x14)
static constexpr std::uintptr_t kSkyElemTransform = 0x0086ae4cu; // DAT_0086ae4c (base+0x2c)

// System-wide dirty/valid flag for the sky element array.
static constexpr std::uintptr_t kSkyDirtyFlag = 0x007030b4u;     // DAT_007030b4

// 0x00487e40
extern "C" __declspec(dllexport) int __cdecl SkyElementRemove(int param_1)
{
    // 0x00487e44: CMP EAX,0x80 / JL — signed compare; out-of-range if >= 128
    if (param_1 >= 0x80) return 0;                       // 0x00487e44..0x00487e4d

    int off = param_1 * kSkyElemStride;                  // 0x00487e4e IMUL EAX,EAX,0x38
    // 0x00487e53: MOV [EAX+0x86ae34], ECX (ECX=0)
    *reinterpret_cast<std::uint32_t*>(kSkyElemActive   + static_cast<std::uintptr_t>(off)) = 0u;
    // 0x00487e59: MOV [EAX+0x86ae4c], ECX
    *reinterpret_cast<std::uint32_t*>(kSkyElemTransform + static_cast<std::uintptr_t>(off)) = 0u;
    // 0x00487e5f: MOV [0x007030b4], ECX
    *reinterpret_cast<std::uint32_t*>(kSkyDirtyFlag) = 0u;
    return 1;                                             // 0x00487e65 MOV EAX,1
}

RH_ScopedInstall(SkyElementRemove, 0x00487e40);
