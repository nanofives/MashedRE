// Mashed RE — D-11072 leg P1: the original's static path data (course block 0x00657498).
// See PathNodes.cpp. PREREG: verify/d3_modes37_20261009/PREREG_PATHPROG.md.
#pragma once

#include <cstddef>
#include <vector>

#include "RaceCamera.h"   // RaceCamNode

namespace mashed_re {
namespace Track { class World; }
namespace Race {

// Default-OFF knob MASHED_PATHPROG (env). Read once.
bool PathProg_Enabled();

// [U-9685] Arm R, verify/d3_u9685_20261016/PREREG_u9685.md: the original's race_pct writer on a private
// shadow row, with only race_pct published (no live lap counter, no live node progress). Env
// MASHED_RACEPCT_FUN408610 (opt-in until the PREREG decision). Read once. Builds the course block
// like PathProg_Enabled() does.
bool RacePctOrig_Enabled();

// Rebuilds the course block exactly as the original's track load does:
//   FUN_0047a0f0  zero 0x16264 bytes at 0x00657498, run the LAPDATA.LUA handlers
//   FUN_00478660  node records 0x00663658 + i*0x4c and count 0x0066d6d8 from the AI BSP
//   FUN_004783f0  per-variation distance (+0x0) and angle (+0x6000) tables
// `lap` is the raw LAPDATA.LUA text. No-op unless PathProg_Enabled().
// MASHED_PATHPROG_DUMP=<path> writes the 0x16278-byte block after the build (gate G-P1).
void PathNodes_Build(const Track::World& ai, const char* lap, std::size_t lapLen);

// [camribbon] The camera-path ribbon: FUN_00478660's node records (the same BuildNode that
// PathNodes_Build runs, under the same PC_24 scope) kept as dir (record +0x00), c0 (+0x18, what
// FUN_00426d00(n,0) returns) and c3 (+0x3c, FUN_00426d00(n,3)) for every AI node. Unlike
// PathNodes_Build it is NOT gated by MASHED_PATHPROG and it does not write the course block at
// 0x00657498, so the default build can use it. One node per AI BSP material (world+0x14).
void PathNodes_CameraNodes(const Track::World& ai, std::vector<RaceCamNode>& out);

// The AI BSP triangles kept for leg P2's sphere query, in stream order (sector order = the
// original's left-first BSP walk): 9 floats each (v0,v1,v2) and the node id (material colour
// red | green << 8, the ushort FUN_00407a90 reads at RpMaterial+4).
int           PathNodes_TriCount();
const float*  PathNodes_Tri(int t);
unsigned      PathNodes_TriNode(int t);

}  // namespace Race
}  // namespace mashed_re
