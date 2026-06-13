// Mashed RE — promote-round round 42 (large-stride imul table getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `8B 44 24 04 69 C0 <stride32> 8B 80 <base32> C3`
//   mov eax,[esp+4]; imul eax,eax,stride; mov eax,[eax+base]; ret
//   => return *(u32*)(base + i*stride). Diffed via early_window_leaf_diff
//   int_scalar + seed_table. Callers C2:
//   0x0041f300  Table63dc6cGet — base 0x0063dc6c stride 0x2ac (caller 00412620 render C2)
//   0x00407a20  Table8a9648Get — base 0x008a9648 stride 0x30c (caller 00471ac0 camera C2)
//   0x00407a40  Table8a9640Get — base 0x008a9640 stride 0x30c (caller 00418560 ai     C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041f300  imul 0x2ac; [eax+0x0063dc6c]
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table63dc6cGet(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x0063dc6cu + param_1 * 0x2acu);  // cited at 0x0041f300
}
RH_ScopedInstall(Table63dc6cGet, 0x0041f300);

// 0x00407a20  imul 0x30c; [eax+0x008a9648]
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table8a9648Get(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x008a9648u + param_1 * 0x30cu);  // cited at 0x00407a20
}
RH_ScopedInstall(Table8a9648Get, 0x00407a20);

// 0x00407a40  imul 0x30c; [eax+0x008a9640]
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table8a9640Get(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x008a9640u + param_1 * 0x30cu);  // cited at 0x00407a40
}
RH_ScopedInstall(Table8a9640Get, 0x00407a40);
