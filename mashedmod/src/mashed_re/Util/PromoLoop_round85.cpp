// Mashed RE — promote-round round 85 (conditional deref-get + table bool predicates).
//
// Display-independent; diffed via early_window_leaf_diff.py (NEW cond_deref_get +
// table_bool_predicate handlers). early_window works despite the env D3D9 wedge (it
// attaches pre-D3D9-init).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x005c4d30  boot — byte-verified: 8B 44 24 04 8B 48 04 85 C9 74 03 8B 00 C3 33 C0 C3
//   fn(p): if (*(u32*)(p+4) != 0) return *(u32*)p ; else return 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CondGet5c4d30(void* p) {
    if (*reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 4) != 0)
        return *reinterpret_cast<std::uint32_t*>(p);
    return 0;
}
RH_ScopedInstall(CondGet5c4d30, 0x005c4d30);

// 0x0045bff0  gameplay — byte-verified: 8B 44 24 04 69 C0 B4 00 00 00 8B 90 88 FC 88 00 33 C9 85 D2 0F 94 C1 8B C1 C3
//   fn(i): return (*(u32*)(0x0088fc88 + i*0xb4) == 0) ? 1 : 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred45bff0(std::uint32_t i) {
    return (*reinterpret_cast<std::uint32_t*>(0x0088fc88u + i * 0xb4u) == 0) ? 1u : 0u;
}
RH_ScopedInstall(Pred45bff0, 0x0045bff0);

// 0x00497450  input — byte-verified: 8B 44 24 04 83 F8 03 7E 03 33 C0 C3 C1 E0 09 8B 90 FC 96 7E 00 33 C9 85 D2 0F 95 C1 8B C1 C3
//   CMP EAX,3 ; JLE predicate (i<=3) ; else fall-through XOR EAX,EAX;RET (i>3 -> 0)
//   fn(i): if ((int)i <= 3) return (*(u32*)(0x007e96fc + i*0x200) != 0) ? 1 : 0 ; return 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred497450(std::uint32_t i) {
    if (static_cast<std::int32_t>(i) <= 3)
        return (*reinterpret_cast<std::uint32_t*>(0x007e96fcu + i * 0x200u) != 0) ? 1u : 0u;
    return 0;
}
RH_ScopedInstall(Pred497450, 0x00497450);
