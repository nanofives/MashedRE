// Mashed RE — per-track hook-slot draw model for Clump_Exclude_From_World clumps (U-9713, rhooks).
// See TrackHookSlots.cpp for the RVA-cited tables.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Render {

// How the original issues one draw of an excluded clump. An excluded clump is not in the RpWorld
// (Course::CreateFromDescription 0x00479da6 / 0x00479db3), so it is drawn ONLY where a per-track hook
// slot (table 0x005f33f8) or the switch table (FUN_004725c0 @ 0x004725c0) draws it.
enum class HookDrawKind : std::uint8_t {
    Clump,    // RpClumpRender (0x004e6680) of course clump slot [course + 0x10110 + 4*clump]
    Group,    // FUN_00479f70 @ 0x00479f70 over a FUN_00479f30 @ 0x00479f30 Mts copy group
    Switch,   // FUN_004725c0: a 0x00691500 entry whose flag (+8) is set
};

// Draw passes, in FUN_00410b30 @ 0x00410b30 order.
constexpr int kHookPassSlot2  = 2;    // hook slot 2 (+0x1c), thunk 0x0041e8d0, from 0x004272df (after the world)
constexpr int kHookPassSwitch = 3;    // FUN_004725c0, called at 0x00410b61 after FUN_004270f0, before the cars
constexpr int kHookPassSlot10 = 10;   // hook slot 10 (+0x3c), thunk 0x0041e950, from 0x00426667 (FUN_00426640,
                                      // called at 0x00410c19 after the copters at 0x00410c03)

struct HookDraw {
    HookDrawKind  kind;
    int           clump;   // course clump slot
    int           mts;     // Group: the Mts slot (records course + mts*0x1000, count course + 0x10000 + mts*4); else -1
    int           pass;    // kHookPass*
    std::uint32_t site;    // the instruction that issues the draw
    // RW render states the slot set before this draw (RwGlobals+0x20 calls): cull 1 = rwCULLMODECULLNONE,
    // 2 = CULLBACK, 0 = the state the pass inherits; zwrite 0 / 1, -1 = inherited.
    int           cull = 0;
    int           zwrite = -1;
};

// RpLODAtomic hook (FUN_004758d0 @ 0x004758d0 -> FUN_00475830 @ 0x00475830): every atomic of clump_h gets
// LOD slots 0..4 = its own geometry, 5..9 = the geometry of clump_l's first atomic (FUN_004b3f90).
struct HookLod {
    int           clump_h;
    int           clump_l;
    float         range;   // DAT_006915fc at the call
    std::uint32_t site;    // the FUN_004758d0 call
};

struct HookTrack {
    int             course_id;
    const char*     name;      // the 0x005f33f8 record name
    const HookDraw* draws;     // per pass, in the slot's code order
    int             n_draws;
    const HookLod*  lods;
    int             n_lods;
};

// The decoded record for course_id, or nullptr when the track is not covered (its excluded clumps keep the
// pre-rhooks handling) or MASHED_NO_HOOKSLOTS is set.
const HookTrack* HookTrackFor(int course_id);

// True when the covered track draws course clump `clump` as itself (a Clump or Switch entry); `pass` gets the
// entry's pass and `order` its index in t.draws. False = an excluded clump of this track is never drawn as itself.
bool HookClumpDrawn(const HookTrack& t, int clump, int* pass, int* order);

// FUN_00557a60 @ 0x00557a60, the LOD index RpLODAtomic's render callback FUN_00557b70 @ 0x00557b70 uses:
// trunc(((at.y*dy + at.x*dx) + at.z*dz) * (10.0 [0x005cc55c] / range)), below 0 -> 0, above 9 -> 9, with
// d = atomic LTM pos - camera LTM pos and `at` the camera LTM's at row.
int HookLodIndex(const float atomic_pos[3], const float cam_pos[3], const float cam_at[3], float range);

}  // namespace Render
}  // namespace mashed_re
