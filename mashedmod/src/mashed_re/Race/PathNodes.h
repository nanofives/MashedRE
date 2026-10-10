// Mashed RE — D-11072 leg P1: the original's static path data (course block 0x00657498).
// See PathNodes.cpp. PREREG: verify/d3_modes37_20261009/PREREG_PATHPROG.md.
#pragma once

#include <cstddef>

namespace mashed_re {
namespace Track { class World; }
namespace Race {

// Default-OFF knob MASHED_PATHPROG (env). Read once.
bool PathProg_Enabled();

// Rebuilds the course block exactly as the original's track load does:
//   FUN_0047a0f0  zero 0x16264 bytes at 0x00657498, run the LAPDATA.LUA handlers
//   FUN_00478660  node records 0x00663658 + i*0x4c and count 0x0066d6d8 from the AI BSP
//   FUN_004783f0  per-variation distance (+0x0) and angle (+0x6000) tables
// `lap` is the raw LAPDATA.LUA text. No-op unless PathProg_Enabled().
// MASHED_PATHPROG_DUMP=<path> writes the 0x16278-byte block after the build (gate G-P1).
void PathNodes_Build(const Track::World& ai, const char* lap, std::size_t lapLen);

}  // namespace Race
}  // namespace mashed_re
