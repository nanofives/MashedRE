// Mashed RE — promote-round round 62 (two ghost-vehicle-slot indexed setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//   8b442404      MOV EAX,[ESP+4]          ; idx
//   8b4c2408      MOV ECX,[ESP+8]          ; val
//   69c0c4000000  IMUL EAX,EAX,0xc4
//   8988 <base>   MOV [EAX+base],ECX       ; *(u32*)(base + idx*0xc4) = val
//   c3            RET
// Byte-verified base: 0041a5b0 -> 0x0063c6ec   0041a8b0 -> 0x0063c6f0

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041a5b0
extern "C" __declspec(dllexport) void __cdecl GhostSlotSet63c6ec(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x0063c6ecu + i * 0xc4u) = v;   // MOV [EAX+0x63c6ec],ECX
}
RH_ScopedInstall(GhostSlotSet63c6ec, 0x0041a5b0);

// 0x0041a8b0
extern "C" __declspec(dllexport) void __cdecl GhostSlotSet63c6f0(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x0063c6f0u + i * 0xc4u) = v;   // MOV [EAX+0x63c6f0],ECX
}
RH_ScopedInstall(GhostSlotSet63c6f0, 0x0041a8b0);
