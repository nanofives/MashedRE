// Mashed RE — promote-round round 81 (indexed table setter + ptr-struct field setters).
//
// Surfaced by promote_frontier.py; decompiled via Ghidra. Display-independent; diffed via
// early_window_leaf_diff.py (0046dd90 = existing indexed_table_set; 005bf7d0/005b0ca0 = NEW
// deref_struct_set handler, SWEEP-CRITICAL).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0046dd90  gameplay — byte-verified: 8B 44 24 04 8B 4C 24 08 69 C0 04 0D 00 00 89 88 F4 16 88 00 C3
//   MOV EAX,[ESP+4]=i ; MOV ECX,[ESP+8]=v ; IMUL EAX,EAX,0xd04 ; MOV [EAX+0x8816f4],ECX ; RET
//   -> *(u32*)(0x008816f4 + i*0xd04) = v   (per-vehicle 0xd04-stride field setter; caller 004177b0 C2)
extern "C" __declspec(dllexport) void __cdecl VehField8816f4Set(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x008816f4u + i * 0xd04u) = v;
}
RH_ScopedInstall(VehField8816f4Set, 0x0046dd90);

// 0x005bf7d0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 8B 54 24 0C 89 88 44 01 00 00 89 90 48 01 00 00 C3
//   fn(p,a,b): *(p+0x144)=a ; *(p+0x148)=b ; RET   (audio object 2-callback setter; caller 005be260 C2)
extern "C" __declspec(dllexport) void __cdecl AudioCb5bf7d0Set(void* p, std::uint32_t a, std::uint32_t b) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x144) = a;   // MOV [EAX+0x144],ECX
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x148) = b;   // MOV [EAX+0x148],EDX
}
RH_ScopedInstall(AudioCb5bf7d0Set, 0x005bf7d0);

// 0x005b0ca0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 89 48 0C 8B 08 83 C9 08 89 08 C3
//   fn(p,v): *(p+0xc)=v ; *p |= 8 ; RET   (cmd-builder: set field + flag bit 0x8; caller 005a86a0 C2)
extern "C" __declspec(dllexport) void __cdecl CmdBuild5b0ca0Set(void* p, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x0c) = v;    // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(p) |= 8u;                             // MOV ECX,[EAX];OR ECX,8;MOV [EAX],ECX
}
RH_ScopedInstall(CmdBuild5b0ca0Set, 0x005b0ca0);
