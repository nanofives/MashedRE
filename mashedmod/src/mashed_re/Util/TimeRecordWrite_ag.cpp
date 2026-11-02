// Mashed RE - c3_batch_ag harness-ext: TimeRecord::WriteTrackBest.
// Calls FUN_00430790 (GetDat0067f17c, C3) to resolve the current track index, then
// writes a decoded (min,sec,frac) time triple into three parallel per-track arrays.
#include "../Core/HookSystem.h"
#include <cstdint>

// MATCH-S06: the body of 0x004299d0 (TimeRecordWriteTrackBest) moved to Match/MatchS06_004299d0.cpp,
// which compiles the byte-matched source re/match/s06/004299d0.c.
