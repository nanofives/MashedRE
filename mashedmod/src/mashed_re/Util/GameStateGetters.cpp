// Mashed RE — game-state pure getters.
// (0x0042f6a0 GetRaceSubMode: body moved to Match/MatchS07_0042f6a0.cpp)
// 0x005c9d00  FUN_005c9d00  game_state_d2  C2
#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S07: the body of 0x0042f6a0 (GetRaceSubMode) moved to Match/MatchS07_0042f6a0.cpp,
// which compiles the byte-matched source re/match/s07/0042f6a0.c.

// 0x005c9d00 — constant-return stub; always returns 0.
// Called from FUN_004929d0 case 3 as the end-of-race trigger input
// passed to FUN_00432080.
// Asm: 33 C0  XOR EAX, EAX  /  C3 RET
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRaceEndTrigger() {
    return 0u;
}

// RH_ScopedInstall(GetRaceEndTrigger, 0x005c9d00);  // DISABLED 2026-05-22: 2-byte function; 5-byte JMP patch overwrites past boundary — see hooks.csv C3->C2 demotion
