// Mashed RE — promote-round round 30 (worklist batch: global + table getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified; all callers confirmed C2 (round-30 reference_to):
//   0x004b68e0  Global7d3e4cGet       — uint32()->[0x007d3e4c]   (caller 004b65a0 vehicle C2)
//   0x004b65a0  Global7d3e4cGetThunk  — jmp->004b68e0; [0x007d3e4c] (caller 00411f30 vehicle C2)
//   0x00496930  Table773030Get        — uint32(i)->[0x00773030+i*4] (caller 0040fc00 Race::Tick util C2)
//   0x004840d0  JointPtr6ce81cGet     — uint32(i)->i<4?[0x006ce81c+i*4]:0 (caller 004491e0 SkyDomeUpdatePos render C2)
//   0x00455b40  PowerupTable6885e0Get — uint32(i)->[0x006885e0+i*0x2c] (caller 00415220 ai C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004b68e0  A1 4C 3E 7D 00 C3  (mov eax,[0x007d3e4c]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global7d3e4cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x007d3e4cu);  // cited at 0x004b68e0
}
RH_ScopedInstall(Global7d3e4cGet, 0x004b68e0);

// 0x004b65a0  E9 3B 03 00 00  (jmp 0x004b68e0 — thunk to the 0x007d3e4c getter)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global7d3e4cGetThunk(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x007d3e4cu);  // cited at 0x004b65a0 -> 0x004b68e0
}
RH_ScopedInstall(Global7d3e4cGetThunk, 0x004b65a0);

// 0x00496930  8B 44 24 04 8B 04 85 30 30 77 00 C3
//   mov eax,[esp+4]; mov eax,[eax*4 + 0x00773030]; ret  (dword table, index*4)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Table773030Get(std::uint32_t param_1) {
    return reinterpret_cast<const std::uint32_t*>(0x00773030u)[param_1];  // cited at 0x00496930
}
RH_ScopedInstall(Table773030Get, 0x00496930);

// 0x004840d0  8B 44 24 04 83 F8 04 72 03 33 C0 C3 8B 04 85 1C E8 6C 00 C3
//   mov eax,[esp+4]; cmp eax,4; jb hit; xor eax,eax; ret; hit: mov eax,[eax*4+0x006ce81c]; ret
extern "C" __declspec(dllexport) std::uint32_t __cdecl JointPtr6ce81cGet(std::uint32_t param_1) {
    if (param_1 < 4u)
        return reinterpret_cast<const std::uint32_t*>(0x006ce81cu)[param_1];  // cited at 0x004840d0
    return 0u;  // cmp/jb out-of-bounds -> xor eax,eax at 0x004840d9
}
RH_ScopedInstall(JointPtr6ce81cGet, 0x004840d0);

// 0x00455b40  8B 44 24 04 6B C0 2C 8B 80 E0 85 68 00 C3
//   mov eax,[esp+4]; imul eax,eax,0x2c; mov eax,[eax + 0x006885e0]; ret  (0x2c-byte stride)
extern "C" __declspec(dllexport) std::uint32_t __cdecl PowerupTable6885e0Get(std::uint32_t param_1) {
    return *reinterpret_cast<const std::uint32_t*>(0x006885e0u + param_1 * 0x2cu);  // cited at 0x00455b40
}
RH_ScopedInstall(PowerupTable6885e0Get, 0x00455b40);
