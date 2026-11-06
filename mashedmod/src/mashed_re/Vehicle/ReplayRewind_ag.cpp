// Mashed RE - c3_batch_ag harness-ext: Replay::Rewind leaf.
// Pure single-store leaf, no callees — safe to A/B-diff via Frida.
#include "../Core/HookSystem.h"
#include <cstdint>

// MATCH-S13: the body of 0x00483a30 (ReplayRewind) moved to Match/MatchS13_00483a30.cpp,
// which compiles the byte-matched source re/match/s13/00483a30.c.
