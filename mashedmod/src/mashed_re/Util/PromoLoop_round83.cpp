// Mashed RE — promote-round round 83 (3-field struct setters + byte-OR RMW; deref_struct_set).
//
// 005173d0/005209d0 auto-surfaced by promote_classify.py's new deref_struct_set recognizer;
// 00518570 hand-authored (or ch,4 byte-OR form). Display-independent; early_window_leaf_diff.py.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x005173d0  util — byte-verified: 8B 44 24 04 8B 4C 24 08 8B 54 24 0C 89 48 48 8B 4C 24 10 89 50 40 89 48 44 C3
//   fn(p,a,b,c): *(p+0x48)=a ; *(p+0x40)=b ; *(p+0x44)=c ; RET   (callers 00514d00/005173f0 C2)
extern "C" __declspec(dllexport) void __cdecl Set5173d0(void* p, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x48) = a;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x40) = b;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x44) = c;
}
RH_ScopedInstall(Set5173d0, 0x005173d0);

// 0x005209d0  util — byte-verified: ... 89 88 E8 01 00 00 ... 89 90 EC 01 00 00 89 88 F0 01 00 00 C3
//   fn(p,a,b,c): *(p+0x1e8)=a ; *(p+0x1ec)=b ; *(p+0x1f0)=c ; RET   (callers 00514d00/005173f0 C2)
extern "C" __declspec(dllexport) void __cdecl Set5209d0(void* p, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1e8) = a;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1ec) = b;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1f0) = c;
}
RH_ScopedInstall(Set5209d0, 0x005209d0);

// 0x00518570  util — byte-verified: 8B 44 24 04 8B 48 70 80 CD 04 89 48 70 C3
//   fn(p): MOV ECX,[EAX+0x70] ; OR CH,4 ; MOV [EAX+0x70],ECX ; RET  -> *(p+0x70) |= 0x400
extern "C" __declspec(dllexport) void __cdecl Rmw518570(void* p) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x70) |= 0x400u;
}
RH_ScopedInstall(Rmw518570, 0x00518570);
