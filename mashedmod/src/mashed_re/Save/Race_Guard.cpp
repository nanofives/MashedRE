// Mashed RE — Save/Race_Guard.cpp
// 0x0040dd60  Race::GuardConcludedAndP1Won
// Session: save-sdone-a-s1 (2026-05-22)
//
// 23-byte predicate: returns non-zero iff race is concluded AND player-0 won.
// Used as gate in Championship::Complete (FUN_00430290 @ 0x00430292).
//
// Decompiled C (Ghidra, Mashed_pool1, 2026-05-05):
//   uint FUN_0040dd60(void) {
//       return (DAT_0063b90c != 1) - 1 & DAT_007f0fcc;
//   }
//
// Evaluation:
//   DAT_0063b90c == 1 (race concluded) → (0) - 1 = 0xFFFFFFFF → & DAT_007f0fcc → DAT_007f0fcc
//   DAT_0063b90c != 1 (race not done) → (1) - 1 = 0x00000000 → & DAT_007f0fcc → 0
//
// Constants (cited from 0x0040dd60 body):
//   0x0063b90c: race-concluded flag; 0 at race start, 1 when FUN_00410510 posts a winner
//   0x007f0fcc: P0-won flag; 1 iff winner index (1-based) == 1; 0 on draw or non-P0 winner
//
// Cited from: re/analysis/profile_career_d2/FUN_0040dd60.md

#include "../Core/HookSystem.h"
#include <cstdint>

static constexpr std::uintptr_t kRaceConcludedFlag = 0x0063b90cu;  // 0x0040dd60 body
static constexpr std::uintptr_t kP0WonFlag         = 0x007f0fccu;  // 0x0040dd60 body

static inline std::uint32_t readU32at(std::uintptr_t addr) {
    return *reinterpret_cast<const std::uint32_t*>(addr);
}

// 0x0040dd60
extern "C" __declspec(dllexport) std::uint32_t __cdecl GuardConcludedAndP1Won() {
    // (DAT_0063b90c != 1) evaluates to 0 when race concluded, 1 otherwise.
    // Subtracting 1 yields 0xFFFFFFFF when concluded, 0 otherwise.
    // ANDed with P0-won flag gives the win predicate.
    const std::uint32_t concluded = readU32at(kRaceConcludedFlag);
    const std::uint32_t p0won     = readU32at(kP0WonFlag);
    return static_cast<std::uint32_t>((concluded != 1u) - 1) & p0won;
}

RH_ScopedInstall(GuardConcludedAndP1Won, 0x0040dd60);
