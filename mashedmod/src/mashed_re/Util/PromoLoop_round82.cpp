// Mashed RE — promote-round round 82 (more indexed/ptr-struct setters + 5-field clear).
//
// Ghidra-decompiled STATE leaves; display-independent; diffed via early_window_leaf_diff.py
// (indexed_table_set + deref_struct_set + ptr_fields_clear handlers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x005b0dc0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 89 48 04 8B 08 80 C9 80 89 08 C3
//   fn(p,v): *(p+4)=v ; *p |= 0x80 ; RET   (cmd-builder: field + flag bit 0x80; caller 005a8960 C2)
extern "C" __declspec(dllexport) void __cdecl CmdBuild5b0dc0Set(void* p, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x04) = v;    // MOV [EAX+4],ECX
    *reinterpret_cast<std::uint32_t*>(p) |= 0x80u;                          // MOV ECX,[EAX];OR CL,0x80;MOV [EAX],ECX
}
RH_ScopedInstall(CmdBuild5b0dc0Set, 0x005b0dc0);

// 0x005bde50  audio — byte-verified: 8B 44 24 04 33 C9 89 08 89 48 04 89 48 08 89 48 0C 89 48 10 C3
//   fn(p): p[0..4 dwords]=0 ; return p   (zero a 5-dword media-buffer descriptor; callers 005bd6f0/005bdad0 C2)
extern "C" __declspec(dllexport) void* __cdecl ClearDesc5bde50(void* p) {
    char* b = static_cast<char*>(p);
    *reinterpret_cast<std::uint32_t*>(b + 0x00) = 0;   // MOV [EAX],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x04) = 0;   // MOV [EAX+4],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x08) = 0;   // MOV [EAX+8],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x0c) = 0;   // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x10) = 0;   // MOV [EAX+0x10],ECX
    return p;                                          // EAX = arg at RET
}
RH_ScopedInstall(ClearDesc5bde50, 0x005bde50);

// 0x00477e00  render — byte-verified: 8B 44 24 04 8B 4C 24 08 69 C0 C0 02 00 00 89 88 8C 31 69 00 C3
//   fn(i,v): IMUL EAX,EAX,0x2c0 ; *(u32*)(0x0069318c + i*0x2c0) = v ; RET   (caller 0040d8f0 C2)
extern "C" __declspec(dllexport) void __cdecl Table69318cSet(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x0069318cu + i * 0x2c0u) = v;
}
RH_ScopedInstall(Table69318cSet, 0x00477e00);
