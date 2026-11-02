// Mashed RE — promote-round round 27 (worklist batch: global getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Five single-global getter leaves; all bodies byte-verified, all callers
// confirmed C2 via a round-27 Ghidra reference_to pass.
//   0x0042b900  Global67eca4Get   — render; mov eax,[0x0067eca4]; ret  (caller 00466b50 util C2)
//   0x00430820  Global67ed6cGet   — read_global 0x0067ed6c            (caller 004103a0 util C2)
//   0x00492d10  Global771968Get   — read_global 0x00771968            (caller 00465a30 audio C2)
//   0x00431b30  Float67eaa8Get    — fld [0x0067eaa8]; ret (float)      (caller 0043dfd0 frontend C2)
//   0x00452eb0  PowerupRangeGet   — fld [0x00684de0]; ret (float)      (caller 00417640 ai C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S07: the body of 0x0042b900 (Global67eca4Get) moved to Match/MatchS07_0042b900.cpp,
// which compiles the byte-matched source re/match/s07/0042b900.c.

// MATCH-S07: the body of 0x00430820 (Global67ed6cGet) moved to Match/MatchS07_00430820.cpp,
// which compiles the byte-matched source re/match/s07/00430820.c.

// MATCH-S15: the body of 0x00492d10 (Global771968Get) moved to Match/MatchS15_00492d10.cpp,
// which compiles the byte-matched source re/match/s15/00492d10.c.

// MATCH-S07: the body of 0x00431b30 (Float67eaa8Get) moved to Match/MatchS07_00431b30.cpp,
// which compiles the byte-matched source re/match/s07/00431b30.c.

// 0x00452eb0  D9 05 E0 4D 68 00 C3  (FLD m32; powerup pursuit range)
extern "C" __declspec(dllexport) float __cdecl PowerupRangeGet(void) {
    return *reinterpret_cast<const float*>(0x00684de0u);  // cited at 0x00452eb0
}
RH_ScopedInstall(PowerupRangeGet, 0x00452eb0);
