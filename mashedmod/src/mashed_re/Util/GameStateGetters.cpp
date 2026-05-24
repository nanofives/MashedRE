// Mashed RE — game-state pure getters.
// 0x0042f6a0  FUN_0042f6a0  game_state_d2  C2
// 0x005c9d00  FUN_005c9d00  game_state_d2  C2
#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0042f6a0 — getter: returns *(uint32*)0x0067e9fc.
// Race-end check in FUN_004929d0 case 3: result compared to 0xb (11).
// Asm: A1 FC E9 67 00  MOV EAX, [0x0067e9fc]  /  C3 RET
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRaceSubMode() {
    return *reinterpret_cast<const std::uint32_t*>(0x0067e9fc);
}

// 0x005c9d00 — constant-return stub; always returns 0.
// Called from FUN_004929d0 case 3 as the end-of-race trigger input
// passed to FUN_00432080.
// Asm: 33 C0  XOR EAX, EAX  /  C3 RET
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRaceEndTrigger() {
    return 0u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(GetRaceSubMode,    0x0042f6a0);
// RH_ScopedInstall(GetRaceEndTrigger, 0x005c9d00);  // DISABLED 2026-05-22: 2-byte function; 5-byte JMP patch overwrites past boundary — see hooks.csv C3->C2 demotion
