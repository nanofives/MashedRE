// Mashed RE — promote-round round 38 (bounds-checked 0xd04-stride table getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified: 8B 44 24 04 83 F8 10 7C 03 33 C0 C3 69 C0 04 0D 00 00 8B 80 <disp32> C3
//   mov eax,[esp+4]; cmp eax,0x10; jl hit; xor eax,eax; ret;
//   hit: imul eax,eax,0x0d04; mov eax,[eax + disp]; ret
// i.e. (int)i < 0x10 ? *(u32*)(disp + i*0xd04) : 0   (0xd04-byte per-element stride).
// Diffed via early_window_leaf_diff table-seed. Callers confirmed C2:
//   0x0046c750  Table882194Get — disp 0x00882194 (caller 004103a0 TimeTrial::LapFinishProcessor util C2)
//   0x0046c730  Table882198Get — disp 0x00882198 (caller 004642f0 audio C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0046c750  ... 8B 80 94 21 88 00 C3  (disp 0x00882194)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table882194Get(std::uint32_t param_1) {
    if (static_cast<std::int32_t>(param_1) < 0x10)
        return *reinterpret_cast<const std::uint32_t*>(0x00882194u + param_1 * 0x0d04u);  // cited at 0x0046c750
    return 0u;
}
RH_ScopedInstall(Table882194Get, 0x0046c750);

// 0x0046c730  ... 8B 80 98 21 88 00 C3  (disp 0x00882198 — sibling +4 field)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table882198Get(std::uint32_t param_1) {
    if (static_cast<std::int32_t>(param_1) < 0x10)
        return *reinterpret_cast<const std::uint32_t*>(0x00882198u + param_1 * 0x0d04u);  // cited at 0x0046c730
    return 0u;
}
RH_ScopedInstall(Table882198Get, 0x0046c730);
