// Mashed RE — game-state global getters.
// Three trivial `MOV EAX, [imm32]; RET` leaves from the game_state_d2 cluster.
// Semantic meaning of the globals is still [UNCERTAIN] — names are
// address-based until the cb4/cb8/ea64 fields are pinned. Per the NO-GUESSING
// rule, we are NOT inferring purpose; we are only reproducing the literal
// dereference. The 5-byte inline-JMP patch fits cleanly: each original is
// `A1 <addr32> C3` (6 bytes) padded with NOPs to a 16-byte boundary, so the
// patch overwrites the MOV but leaves the trailing RET intact (it never
// executes — reimpl returns to caller).
//
// 0x0042c2d0  FUN_0042c2d0  size=6  game_state_d2-20260503  C2 -> C3 candidate
//   Disasm: A1 B4 EC 67 00            MOV EAX, [0x0067ecb4]
//           C3                        RET
//
// 0x0042c2e0  FUN_0042c2e0  size=6  game_state_d2-20260503  C2 -> C3 candidate
//   Disasm: A1 B8 EC 67 00            MOV EAX, [0x0067ecb8]
//           C3                        RET
//
// 0x0042f500  FUN_0042f500  size=6  game_state_d2-20260503  C2 -> C3 candidate
//   Disasm: A1 64 EA 67 00            MOV EAX, [0x0067ea64]
//           C3                        RET
#include "../Core/HookSystem.h"

#include <cstdint>

extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0067ecb4() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ecb4);
}
RH_ScopedInstall(GetDat0067ecb4, 0x0042c2d0);

extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0067ecb8() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ecb8);
}
RH_ScopedInstall(GetDat0067ecb8, 0x0042c2e0);

extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0067ea64() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ea64);
}
RH_ScopedInstall(GetDat0067ea64, 0x0042f500);
