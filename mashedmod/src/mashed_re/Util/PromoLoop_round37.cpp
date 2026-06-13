// Mashed RE — promote-round round 37 (absolute-table getters via early-window table-seed).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified as `8B 44 24 04 8B 04 85 <tbl32> C3`
// (mov eax,[esp+4]; mov eax,[eax*4 + tbl]; ret). Diffed via early_window_leaf_diff
// with seed_table (the live tables are zero at menu/race -> seeding makes the
// index*4 read non-degenerate). Callers confirmed C2 (round-37 reference_to):
//   0x00452ea0  Table88ff50Get — [0x0088ff50 + i*4] (caller 00414f00 ai       C2)
//   0x0045dd50  Table8aa300Get — [0x008aa300 + i*4] (caller 0040ca00 gameplay C2)
//   0x0041f320  Table63d830Get — [0x0063d830 + i*4] (caller 0040cf80 boot     C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00452ea0  8B 44 24 04 8B 04 85 50 FF 88 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table88ff50Get(std::uint32_t param_1) {
    return reinterpret_cast<const std::uint32_t*>(0x0088ff50u)[param_1];  // cited at 0x00452ea0
}
RH_ScopedInstall(Table88ff50Get, 0x00452ea0);

// 0x0045dd50  8B 44 24 04 8B 04 85 00 A3 8A 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table8aa300Get(std::uint32_t param_1) {
    return reinterpret_cast<const std::uint32_t*>(0x008aa300u)[param_1];  // cited at 0x0045dd50
}
RH_ScopedInstall(Table8aa300Get, 0x0045dd50);

// 0x0041f320  8B 44 24 04 8B 04 85 30 D8 63 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table63d830Get(std::uint32_t param_1) {
    return reinterpret_cast<const std::uint32_t*>(0x0063d830u)[param_1];  // cited at 0x0041f320
}
RH_ScopedInstall(Table63d830Get, 0x0041f320);
