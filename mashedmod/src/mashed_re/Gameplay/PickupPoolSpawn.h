// Mashed RE — pickup-pool spawn (0x00458e00) and the pool it writes.
// See PickupPoolSpawn.cpp for the full RVA-cited transcription.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Gameplay {

// The pool entry is 0x50 bytes (stride from 0x00458e6f: `lea esi,[edx+edx*4]`
// then `shl esi,4`). Field offsets, each cited at the instruction that writes it:
//   +0x00  object handle      read  0x00458eb7 (`mov eax,[esi]`)
//   +0x18  0x42f00000 (120.0) write 0x00458ef8
//   +0x1c  0                  write 0x00458eff
//   +0x20  1                  write 0x00458ef1
//   +0x24  type               write 0x00458e85 (and 0x00458ea7 on the rank-2 arm)
//   +0x28  0, or 1 on rank-2  write 0x00458e7e / 0x00458eaa
//   +0x2c  live position x/y/z  write 0x00458ec9/0x00458ece/0x00458ed4
//   +0x38  anchor position x/y/z write 0x00458edc/0x00458ee4/0x00458f0c
constexpr std::uint32_t kPickupEntryStride = 0x50u;
// 0x00458e0c `cmp edx,0x19` + 0x00458e11 `jl` — a SIGNED test, so the pool holds
// at most 25 entries and a count >= 25 is rejected.
constexpr std::int32_t  kPickupPoolMax     = 25;

// Pool storage. In the .asi these resolve to MASHED's own globals; in the
// standalone they resolve to this TU's private arrays (see the .cpp).
unsigned char* PickupPool_Base();
std::int32_t*  PickupPool_CountPtr();

// Read-only accessors over an entry, for callers that want the pool back out
// (the standalone's PickupField builds its orb list from these).
const float*   PickupPool_EntryPos(std::int32_t index);   // +0x2c
std::int32_t   PickupPool_EntryType(std::int32_t index);  // +0x24

}  // namespace Gameplay
}  // namespace mashed_re

// 0x00458e00
extern "C" __declspec(dllexport) std::int32_t __cdecl
PickupPoolSpawn(const float* pos, std::int32_t type);
