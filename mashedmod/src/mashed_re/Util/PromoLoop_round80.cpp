// Mashed RE — promote-round round 80 (frontier/classifier batch: byte-getter + arg-field-clear).
//
// Surfaced by scripts/promote_frontier.py + promote_classify.py (extended recognizers:
// read_global_u8, ptr_fields_clear). Both display-independent; diffed via early_window_leaf_diff.py.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042a9f0  render — byte-verified: A0 A8 EC 67 00 C3
//   MOV AL,[0x0067eca8] ; RET   (read_global u8; caller FUN_00492e90 C2)
extern "C" __declspec(dllexport) std::uint8_t __cdecl GetFadeAlpha(void) {
    return *reinterpret_cast<std::uint8_t*>(0x0067eca8u);
}
RH_ScopedInstall(GetFadeAlpha, 0x0042a9f0);

// 0x005be930  audio — byte-verified: 8B 44 24 04 33 C9 89 48 0C 89 48 10 C3
//   MOV EAX,[ESP+4] ; XOR ECX,ECX ; MOV [EAX+0xc],ECX ; MOV [EAX+0x10],ECX ; RET
//   -> fn(p): p->[0xc]=0, p->[0x10]=0, return p (eax=arg unchanged). callers 005be8c0/990/5c0c20 C2
extern "C" __declspec(dllexport) void* __cdecl AudioClear5be930(void* p) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x0c) = 0;   // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x10) = 0;   // MOV [EAX+0x10],ECX
    return p;                                                              // EAX = arg at RET
}
RH_ScopedInstall(AudioClear5be930, 0x005be930);
