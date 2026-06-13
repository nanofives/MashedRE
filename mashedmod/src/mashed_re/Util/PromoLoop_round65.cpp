// Mashed RE — promote-round round 65 (ghost-slot vec3/vec4 setters + per-player field setter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All three are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0041a500 (0x0041a500): ghost-slot vec3 setter-or-zero.
//   MOV EAX,[ESP+4] ; MOV ECX,[ESP+8] ; IMUL EAX,0xc4 ; XOR EDX,EDX ; ADD EAX,0x63c630
//   CMP ECX,EDX ; JZ zero ; MOV [EAX+0xa0],in[0] ; [EAX+0xa4],in[1] ; [EAX+0xa8],in[2]
//   zero: MOV [EAX+0xa0..0xa8],0
//
// FUN_0041a550 (0x0041a550): ghost-slot vec4 setter-or-zero (same shape, fields +0x80..+0x8c).
//
// FUN_0041ef60 (0x0041ef60): per-player indexed field setter.
//   MOV EAX,[ESP+4] ; MOV ECX,[ESP+8] ; IMUL EAX,0x2ac ; MOV [EAX+0x63dc78],ECX ; RET
//   -> *(u32*)(0x0063dc78 + idx*0x2ac) = val

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041a500
extern "C" __declspec(dllexport) void __cdecl GhostVec3Set63c6d0(std::uint32_t idx, const std::uint32_t* in) {
    std::uint32_t a = 0x0063c630u + idx * 0xc4u;                     // IMUL 0xc4 ; ADD 0x63c630
    if (in) { *reinterpret_cast<std::uint32_t*>(a + 0xa0) = in[0]; *reinterpret_cast<std::uint32_t*>(a + 0xa4) = in[1]; *reinterpret_cast<std::uint32_t*>(a + 0xa8) = in[2]; }
    else    { *reinterpret_cast<std::uint32_t*>(a + 0xa0) = 0;     *reinterpret_cast<std::uint32_t*>(a + 0xa4) = 0;     *reinterpret_cast<std::uint32_t*>(a + 0xa8) = 0; }
}
RH_ScopedInstall(GhostVec3Set63c6d0, 0x0041a500);

// 0x0041a550
extern "C" __declspec(dllexport) void __cdecl GhostVec4Set63c6b0(std::uint32_t idx, const std::uint32_t* in) {
    std::uint32_t a = 0x0063c630u + idx * 0xc4u;
    if (in) { *reinterpret_cast<std::uint32_t*>(a + 0x80) = in[0]; *reinterpret_cast<std::uint32_t*>(a + 0x84) = in[1]; *reinterpret_cast<std::uint32_t*>(a + 0x88) = in[2]; *reinterpret_cast<std::uint32_t*>(a + 0x8c) = in[3]; }
    else    { *reinterpret_cast<std::uint32_t*>(a + 0x80) = 0;     *reinterpret_cast<std::uint32_t*>(a + 0x84) = 0;     *reinterpret_cast<std::uint32_t*>(a + 0x88) = 0;     *reinterpret_cast<std::uint32_t*>(a + 0x8c) = 0; }
}
RH_ScopedInstall(GhostVec4Set63c6b0, 0x0041a550);

// 0x0041ef60
extern "C" __declspec(dllexport) void __cdecl PlayerFieldSet63dc78(std::uint32_t idx, std::uint32_t val) {
    *reinterpret_cast<std::uint32_t*>(0x0063dc78u + idx * 0x2acu) = val;   // IMUL 0x2ac ; MOV [EAX+0x63dc78]
}
RH_ScopedInstall(PlayerFieldSet63dc78, 0x0041ef60);
