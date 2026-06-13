// Mashed RE — promote-round round 87 (double-deref vec3 getter + gated float-compare predicate).
//
// Display-independent; early_window_leaf_diff.py (NEW double_deref_vec3_get + global_float_predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0044dff0  gameplay — byte-verified: 8B 44 24 04 69 C0 F8 00 00 00 8B 88 80 00 89 00 8B 51 04 8B 44 24 08 83 C2 40 8B 0A 89 08 8B 4A 04 89 48 04 8B 52 08 89 50 08 C3
//   fn(i,out): rec=*(u32*)(0x00890080 + i*0xf8) ; t=*(u32*)(rec+4) ;
//              out[0]=*(u32*)(t+0x40) ; out[1]=*(u32*)(t+0x44) ; out[2]=*(u32*)(t+0x48)  (item world-pos; caller 00461650 C2)
extern "C" __declspec(dllexport) void __cdecl ItemWorldPos44dff0(std::uint32_t i, std::uint32_t* out) {
    std::uint32_t rec = *reinterpret_cast<std::uint32_t*>(0x00890080u + i * 0xf8u);
    std::uint32_t t = *reinterpret_cast<std::uint32_t*>(rec + 4);
    out[0] = *reinterpret_cast<std::uint32_t*>(t + 0x40);
    out[1] = *reinterpret_cast<std::uint32_t*>(t + 0x44);
    out[2] = *reinterpret_cast<std::uint32_t*>(t + 0x48);
}
RH_ScopedInstall(ItemWorldPos44dff0, 0x0044dff0);

// 0x00405430  gameplay — byte-verified: A1 78 9D 63 00 85 C0 74 15 A1 70 9D 63 00 D9 05 74 9D 63 00 D8 58 0C DF E0 F6 C4 41 75 03 33 C0 C3 B8 01 00 00 00 C3
//   fn(): if (*(int*)0x639d78 == 0) return 0 ; return (*(float*)0x639d74 <= *(float*)(*(u32*)0x639d70 + 0xc)) ? 1 : 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred405430(void) {
    if (*reinterpret_cast<std::int32_t*>(0x00639d78u) == 0) return 0;
    float thr = *reinterpret_cast<float*>(0x00639d74u);
    float val = *reinterpret_cast<float*>(*reinterpret_cast<std::uint32_t*>(0x00639d70u) + 0xc);
    return (thr <= val) ? 1u : 0u;
}
RH_ScopedInstall(Pred405430, 0x00405430);
