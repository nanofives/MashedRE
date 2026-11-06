// Mashed RE — promote-round round 40 (first-party constant-return leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified `B8 <imm32> C3` (mov eax,imm; ret) — return a fixed constant, no
// input, no state. Diffed via early_window_leaf_diff const_return. Callers C2:
//   0x0044dfe0  Ret50         -> 0x50       (caller 00461650 audio         C2)
//   0x00493b40  Ret3          -> 0x3        (caller 00493b50 util fn-ptr    C2)
//   0x00443090  Ret897ff0     -> 0x00897ff0 (caller 004847d0 world-objects C2)
//   0x004098a0  Ret63a5f0     -> 0x0063a5f0 (caller 00441820 util          C2)
//   0x004924e0  Ret6147b4     -> 0x006147b4 (caller 004270f0 render        C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S08: the body of 0x0044dfe0 (Ret50) moved to Match/MatchS08_0044dfe0.cpp,
// which compiles the byte-matched source re/match/s08/0044dfe0.c.

// MATCH-S15: the body of 0x00493b40 (Ret3) moved to Match/MatchS15_00493b40.cpp,
// which compiles the byte-matched source re/match/s15/00493b40.c.

// MATCH-S08: the body of 0x00443090 (Ret897ff0) moved to Match/MatchS08_00443090.cpp,
// which compiles the byte-matched source re/match/s08/00443090.c.

// 0x004098a0  B8 F0 A5 63 00 C3
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ret63a5f0(void) { return 0x0063a5f0u; }  // cited at 0x004098a0
RH_ScopedInstall(Ret63a5f0, 0x004098a0);

// MATCH-S15: the body of 0x004924e0 (Ret6147b4) moved to Match/MatchS15_004924e0.cpp,
// which compiles the byte-matched source re/match/s15/004924e0.c.
