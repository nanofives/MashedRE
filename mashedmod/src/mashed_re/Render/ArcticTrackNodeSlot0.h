// Mashed RE — the "arctic" per-track node, slot 0 (0x00448940).
// See ArcticTrackNodeSlot0.cpp for the full RVA-cited transcription.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Render {

// 0x00448a6f `mov ebp,5` x 0x00448a88 `mov ebx,5`.
constexpr int kArcticSeaTileCount = 25;

// The 25 sea-tile translations the node lays down, in the order the node applies
// them (outer loop X, inner loop Z, the same counter that indexes the tile array
// at 0x00448a90 `mov ecx,[edi*4 + 0x8963e0]`).
//
// This is the ONE body for that grid. The node's own loop calls it, and so does
// the standalone's track renderer; there is no second copy.
void ArcticSeaTileGrid(float out[kArcticSeaTileCount][3]);

}  // namespace Render
}  // namespace mashed_re

// 0x00448940
extern "C" __declspec(dllexport) void __cdecl ArcticTrackNodeSlot0(void* course);
