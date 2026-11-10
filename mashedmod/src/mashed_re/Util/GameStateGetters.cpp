// Mashed RE — game-state pure getters.
// (0x0042f6a0 GetRaceSubMode: body moved to Match/MatchS07_0042f6a0.cpp)
// (0x005c9d00 GetRaceEndTrigger: body moved to Match/PortBatch_GAMEPLAY_005c9d00.cpp)
#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S07: the body of 0x0042f6a0 (GetRaceSubMode) moved to Match/MatchS07_0042f6a0.cpp,
// which compiles the byte-matched source re/match/s07/0042f6a0.c.

// PORT-BATCH-GAMEPLAY (D4, L-107 (a), 2026-11-09): the hand body of 0x005c9d00 (GetRaceEndTrigger) was replaced by
// the byte-matched re/match/gffu/005c9d00.c, compiled in both targets by Match/PortBatch_GAMEPLAY_005c9d00.cpp.
