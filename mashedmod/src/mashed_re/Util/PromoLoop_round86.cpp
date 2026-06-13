// Mashed RE — promote-round round 86 (global swap-return + 3-byte-globals + indexed float-square).
//
// Display-independent; diffed via early_window_leaf_diff.py (NEW global_swap,
// byte_args_to_globals, indexed_float_sq handlers). Works despite the env D3D9 wedge.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x005a7b40  audio — byte-verified: 8B 4C 24 04 A1 BC CA 7D 00 89 0D BC CA 7D 00 C3
//   fn(v): old = *(u32*)0x007dcabc ; *(u32*)0x007dcabc = v ; return old  (audio-ctx swap; caller 005a7b60 C2)
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioCtxSwap5a7b40(std::uint32_t v) {
    std::uint32_t old = *reinterpret_cast<std::uint32_t*>(0x007dcabcu);
    *reinterpret_cast<std::uint32_t*>(0x007dcabcu) = v;
    return old;
}
RH_ScopedInstall(AudioCtxSwap5a7b40, 0x005a7b40);

// 0x004924c0  boot — byte-verified: 8A 44 24 04 8A 4C 24 08 8A 54 24 0C A2 B4 47 61 00 88 0D B5 47 61 00 88 15 B6 47 61 00 C3
//   fn(a,b,c): *(u8*)0x006147b4=a ; *(u8*)0x006147b5=b ; *(u8*)0x006147b6=c   (callers 00426810/00426e10 C2)
extern "C" __declspec(dllexport) void __cdecl Set6147b4Triple(unsigned char a, unsigned char b, unsigned char c) {
    *reinterpret_cast<unsigned char*>(0x006147b4u) = a;
    *reinterpret_cast<unsigned char*>(0x006147b5u) = b;
    *reinterpret_cast<unsigned char*>(0x006147b6u) = c;
}
RH_ScopedInstall(Set6147b4Triple, 0x004924c0);

// 0x0047cdc0  render — byte-verified: D9 44 24 08 8B 44 24 04 D8 4C 24 08 D9 1C 85 70 68 6C 00 C3
//   fn(i,f): FLD [esp+8]=f ; FMUL [esp+8]=f ; FSTP [eax*4+0x6c6870] -> *(float*)(0x006c6870 + i*4) = f*f
//   (store-squared-distance setter; callers 00407800/00412050/0044c4f0 C2)
extern "C" __declspec(dllexport) void __cdecl StoreDistSq47cdc0(std::uint32_t i, float f) {
    *reinterpret_cast<float*>(0x006c6870u + i * 4u) = f * f;
}
RH_ScopedInstall(StoreDistSq47cdc0, 0x0047cdc0);
