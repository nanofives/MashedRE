// Mashed RE — sky/cloud-element removal function.
//
// 0x00487e40  FUN_00487e40  sky subsystem
//   Removes the sky/cloud particle element at index param_1 from the 128-slot
//   element array (stride 0xe dwords = 0x38 bytes; array base DAT_0086ae20).
//   Zeros the slot's active flag (base+0x14 = DAT_0086ae34) and its
//   transform/model pointer (base+0x2c = DAT_0086ae4c), then clears the
//   system-wide dirty/valid flag DAT_007030b4.  Returns 1 on success, 0 if
//   the index is out of range (>= 128; signed compare CMP EAX,0x80 / JL).
//
// Calling convention: cdecl.  Body: 0x00487e40..0x00487e6a (42 bytes). Leaf.
// Source: re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00487e40.md
#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S14: the body of 0x00487e40 (SkyElementRemove) moved to Match/MatchS14_00487e40.cpp,
// which compiles the byte-matched source re/match/s14/00487e40.c.
