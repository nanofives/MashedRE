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

// MATCH-S16: the body of 0x00496930 (Table773030Get) moved to Match/MatchS16_00496930.cpp,
// which compiles the byte-matched source re/match/s16/00496930.c.

// MATCH-S14: the body of 0x004840d0 (JointPtr6ce81cGet) moved to Match/MatchS14_004840d0.cpp,
// which compiles the byte-matched source re/match/s14/004840d0.c.

// MATCH-S09: the body of 0x00455b40 (PowerupTable6885e0Get) moved to Match/MatchS09_00455b40.cpp,
// which compiles the byte-matched source re/match/s09/00455b40.c.
