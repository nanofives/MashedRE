// Mashed RE — promote-round round 31 (byte-scan batch: single-global getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified as `A1 <addr32> C3` (mov eax,[global]; ret). Callers
// confirmed C2 (round-31 reference_to):
//   0x0042f760  Global67f19cGet — [0x0067f19c] (caller 004631f0 audio C2)
//   0x0042f770  Global67f1a0Get — [0x0067f1a0] (caller 004631f0 audio C2)
//   0x0042f780  Global67f1a4Get — [0x0067f1a4] (caller 004631f0 audio C2)
//   [MOVED 2026-11-01, D4-DC-INC-pathprog] Global66d6d8Get (FUN_00426bb0) -> Camera/CameraPathCount.cpp
//   0x0045bfe0  Global80332cGet — [0x0080332c] (caller 00406160 gameplay C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S07: the body of 0x0042f760 (Global67f19cGet) moved to Match/MatchS07_0042f760.cpp,
// which compiles the byte-matched source re/match/s07/0042f760.c.

// MATCH-S07: the body of 0x0042f770 (Global67f1a0Get) moved to Match/MatchS07_0042f770.cpp,
// which compiles the byte-matched source re/match/s07/0042f770.c.

// MATCH-S07: the body of 0x0042f780 (Global67f1a4Get) moved to Match/MatchS07_0042f780.cpp,
// which compiles the byte-matched source re/match/s07/0042f780.c.

// 0x0045bfe0  A1 2C 33 80 00 C3  (mov eax,[0x0080332c]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global80332cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0080332cu);  // cited at 0x0045bfe0
}
RH_ScopedInstall(Global80332cGet, 0x0045bfe0);
