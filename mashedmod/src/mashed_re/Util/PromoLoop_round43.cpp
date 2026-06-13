// Mashed RE — promote-round round 43 (large-stride imul FLOAT table getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `8B 44 24 04 69 C0 <stride32> D9 80 <base32> C3`
//   mov eax,[esp+4]; imul eax,eax,stride; fld dword[eax+base]; ret
//   => return *(float*)(base + i*stride). Diffed via early_window_leaf_diff
//   float_table_read. Callers C2:
//   0x0041f100  FloatTable63dc64Get — base 0x0063dc64 stride 0x2ac (caller 0040c690 gameplay C2)
//   0x0044e050  FloatTable8900a8Get — base 0x008900a8 stride 0xf8  (caller 00461650 audio    C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041f100  imul 0x2ac; fld [eax+0x0063dc64]
extern "C" __declspec(dllexport) float __cdecl FloatTable63dc64Get(std::uint32_t param_1) {
    return *reinterpret_cast<const float*>(0x0063dc64u + param_1 * 0x2acu);  // cited at 0x0041f100
}
RH_ScopedInstall(FloatTable63dc64Get, 0x0041f100);

// 0x0044e050  imul 0xf8; fld [eax+0x008900a8]
extern "C" __declspec(dllexport) float __cdecl FloatTable8900a8Get(std::uint32_t param_1) {
    return *reinterpret_cast<const float*>(0x008900a8u + param_1 * 0xf8u);  // cited at 0x0044e050
}
RH_ScopedInstall(FloatTable8900a8Get, 0x0044e050);
