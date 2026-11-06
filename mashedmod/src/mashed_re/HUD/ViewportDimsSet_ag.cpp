// Mashed RE - c3_batch_ag harness-ext: viewport dims/ratio setter.
// One conditional callee (FUN_004c0e50, C2) gated on *(this+4)!=0; the diff seeds a
// zeroed scratch struct so the gate is 0 and the callee path is skipped.
#include "../Core/HookSystem.h"
#include <cstdint>

// MATCH-S19: the body of 0x004c1c80 (ViewportDimsSet) moved to Match/MatchS19_004c1c80.cpp,
// which compiles the byte-matched source re/match/s19/004c1c80.c.
