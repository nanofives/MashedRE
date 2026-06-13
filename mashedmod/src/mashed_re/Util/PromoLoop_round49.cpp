// Mashed RE — promote-round round 49 (pointer-to-table field getter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `8B 44 24 04 8B 0D 70 27 5F 00 8B 14 81 8B 42 04 C3`
//   mov eax,[esp+4]; mov ecx,[0x005f2770]; mov edx,[ecx+eax*4]; mov eax,[edx+4]; ret
//   => return *( ((u32*)*(u32*)0x005f2770)[i] + 4 ). Self-contained. Diffed via
//   early_window_leaf_diff ptr_table_field_read. Caller MenuTableSearch (0x0042a940) C3.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0040ce80
extern "C" __declspec(dllexport) std::uint32_t __cdecl PtrTable5f2770Get(std::uint32_t param_1) {
    const std::uint32_t table = *reinterpret_cast<const std::uint32_t*>(0x005f2770u);
    const std::uint32_t entry = reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(table))[param_1];
    return *reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(entry) + 4u);  // cited at 0x0040ce80
}
RH_ScopedInstall(PtrTable5f2770Get, 0x0040ce80);
