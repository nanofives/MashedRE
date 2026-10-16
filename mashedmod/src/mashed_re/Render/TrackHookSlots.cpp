// Mashed RE — per-track hook-slot draw model for Clump_Exclude_From_World clumps (U-9713, rhooks).
//
// Static decode, read-only (Mashed_pool10 MCP, decomp_pc.py --create on Mashed_pool11, capstone over
// original\MASHED.exe.unpatched): verify/rhooks_20261016/RESULT_rhooks.md, PREREG_rhooks.md.
//
// The hook table 0x005f33f8 holds 13 records of 0x48 bytes {char name[0x10]; u32 Course_Id; void* slot[13]}
// (count at [0x005f37a0]); selector 0x0041e870 stores the record matching the course in DAT_0063d7e4, and
// slot k is called through the thunk at 0x0041e8b0 + 0x10*k (`jmp [rec + 0x14 + 4*k]`). FUN_00410b30 @
// 0x00410b30 orders the passes this file models: FUN_004270f0 (world, then slot 2 from 0x004272df), FOGENABLE 1,
// FUN_004725c0 (the switch table), the cars (FUN_00420050 x4), the copters (FUN_00425e40, 0x00410c08), then
// FUN_00426640 (slot 10 from 0x00426667).
//
// Building blocks, as read:
//   FUN_00479f30 @ 0x00479f30  group = FUN_00479e80(atomic, count) then FUN_00479f10(group, i, records + 0x40*i)
//   FUN_00479e80 @ 0x00479e80  returns 0 when count > 0x30; per copy: a new frame (FUN_004c0b30), identity
//                              (FUN_004c15c0), a clone of the atomic (FUN_004e69a0) set on it (FUN_004e7e30)
//   FUN_00479f10 @ 0x00479f10  FUN_004c1480(frame[i], record, 0): the copy's frame := the Mts record (REPLACE)
//   FUN_00479f70 @ 0x00479f70  copies 0 .. [group+0x188]-1, each through its atomic render callback [+0x48]
//                              (no frustum test, no RENDER-flag test)
//   FUN_004b3f90 @ 0x004b3f90  the clump's first atomic in RpClumpForAllAtomics order (callback 0x004b4070)
// Every group / LOD clump of the five covered tracks is single-atomic (verify/rhooks_20261016 dffcount), so the
// copied atomic is the whole DFF.

#include "TrackHookSlots.h"

#include <cstdlib>

namespace mashed_re {
namespace Render {
namespace {

using K = HookDrawKind;

// Roundabout, record "rouabout" (Course_Id 2). Slot 2 FUN_0044d6f0 @ 0x0044d6f0 draws these clumps in this
// order (clump slot read at the cited address, RpClumpRender 0x004e6680 right after; rstates F6 read the same
// order) under cull NONE (`push 1; push 0x14` 0x0044d756, ZWRITE 1 at 0x0044d749), then the barrel / crane shadows FUN_0044bd50 / FUN_0044be70 (RwIm3D, not modelled here).
// Slot 0 FUN_0044cb60 @ 0x0044cb60 hooks the two LOD pairs. Not drawn by any slot: railcar 2 (its three
// clones join the world, FUN_0044c8b0 @ 0x0044c8b0, and move: U-row), Hut01 10, Pylon_L 18, Frame_L 20.
const HookDraw kRouaboutDraws[] = {
    { K::Clump,  3, -1, kHookPassSlot2, 0x0044d761u, 1 },   // FatTree (Clump_Mts_Pair 3,2)
    { K::Clump,  0, -1, kHookPassSlot2, 0x0044d777u, 1 },   // Shrub01 (Clump_Mts_Pair 0,0)
    { K::Clump,  5, -1, kHookPassSlot2, 0x0044d78au, 1 },   // Girders
    { K::Clump, 11, -1, kHookPassSlot2, 0x0044d79du, 1 },   // Girders02
    { K::Clump,  9, -1, kHookPassSlot2, 0x0044d7b0u, 1 },   // Hrail
    { K::Clump,  8, -1, kHookPassSlot2, 0x0044d7c3u, 1 },   // Handrail
    { K::Clump, 19, -1, kHookPassSlot2, 0x0044d7d6u, 1 },   // Frame_H (Clump_Mts_Pair 19,4)
    { K::Clump, 17, -1, kHookPassSlot2, 0x0044d7e9u, 1 },   // Pylon_H (Clump_Mts_Pair 17,3)
};
const HookLod kRouaboutLods[] = {
    { 17, 18, 60.0f, 0x0044d325u },   // DAT_006915fc = 0x42700000 at 0x0044d314
    { 19, 20, 30.0f, 0x0044d348u },   // DAT_006915fc = 0x41f00000 at 0x0044d336
};

// Sands, record "sands" (Course_Id 11). Slot 2 FUN_00450910 @ 0x00450910: the indexed loop over clump slots
// 14..16 (`lea esi,[ebx+0x10148]` 0x00450917, count 3), clump 5, clump 1, then the Debris clones (drawn only
// while their timers DAT_00684b84 / DAT_00684be4 are above 0; FUN_00450300 sets both to -1.0). The switch
// table: FUN_00450300 @ 0x00450300 (slots 6/7 FUN_00450460 when DAT_0063ba8c == 1) sets entry 0 = clump 20
// ON (FUN_00472560(0,1), FUN_00472520(0, [course+0x10160]) at 0x00450403), entries 1..3 = clumps 18, 19, 7
// OFF. Not drawn: Jump 7, Gantrys 9, Debris 10 (base), Bridgehafbroke 18, BridgeBroke 19.
const HookDraw kSandsDraws[] = {
    { K::Clump,  14, -1, kHookPassSlot2,  0x00450917u },   // Newglow1
    { K::Clump,  15, -1, kHookPassSlot2,  0x00450917u },   // Newglow2
    { K::Clump,  16, -1, kHookPassSlot2,  0x00450917u },   // Newglow3
    { K::Clump,   5, -1, kHookPassSlot2,  0x00450933u },   // OakTrunk (Clump_Mts_Pair 5,0)
    { K::Clump,   1, -1, kHookPassSlot2,  0x0045093fu },   // Water
    { K::Switch, 20, -1, kHookPassSwitch, 0x004725c1u },   // BridgeWhole, switch entry 0
};

// Warzone, record "warzone" (Course_Id 33). Slot 0 FUN_004527f0 @ 0x004527f0 builds four groups:
//   DAT_00684e28 = OakTrunk 3 x Mts 2 (OakTrees_1, 48), DAT_00684e2c = OakTrunk 3 x Mts 3 (OakTrees_2, 46),
//   DAT_00684e30 = CrossTree 2 x Mts 0 (Crosstrees, 30), DAT_00684e34 = Fence_2 22 x Mts 6 (fence_2, 15),
// and hooks Pylon_H 20 with Pylon_L 21. Slot 2 FUN_00452d10 @ 0x00452d10: clump 19, clump 20, group
// DAT_00684e34, the boat (FUN_00452130(&DAT_00684d18), the RpClumpRender loop over clump 1), clump 0, the
// Chunk groups (FUN_00452490: only copies with a set flag, all 0 at creation, FUN_00452310). Slot 10
// FUN_00452da0 @ 0x00452da0: groups DAT_00684e28, DAT_00684e2c, DAT_00684e30, then the leaf sprites
// (0x0047cd30, not modelled here). Slot 2 draws under cull NONE (0x00452d16) up to the fence group and CULLBACK
// (0x00452d5c) after it; slot 10 under ZWRITE 0 (0x00452db2). The craters Crater_1..8 (clumps 9..16) are switch
// entries 0..7, all ON: FUN_00452620 @ 0x00452620 (`add edi,0x10134` 0x00452628, FUN_00472520 0x00452634,
// FUN_00472560(i,1) 0x0045263c), reached from slot 0 (0x00452720) and from slots 6 / 7 (0x00452af0, 0x00452ad0).
// Not drawn: CrossTree 2 / OakTrunk 3 / Fence_2 22 bases, Chunk 17 base, Pylon_L 21.
const HookDraw kWarzoneDraws[] = {
    { K::Clump, 19, -1, kHookPassSlot2,  0x00452d2fu, 1 },   // Fence_1
    { K::Clump, 20, -1, kHookPassSlot2,  0x00452d42u, 1 },   // Pylon_H (Clump_Mts_Pair 20,5)
    { K::Group, 22,  6, kHookPassSlot2,  0x00452d51u, 1 },   // Fence_2 x fence_2.mts
    { K::Clump,  1, -1, kHookPassSlot2,  0x00452d68u, 2 },   // Boat: the pontoon object 0x00684d18 at its rest pose (FUN_00452130; its
                                                          // state machine FUN_00451d80 is not ported, U-9719)
    { K::Clump,  0, -1, kHookPassSlot2,  0x00452d74u, 2 },   // River
    { K::Switch,  9, -1, kHookPassSwitch, 0x004725c1u },   // Crater_1, switch entry 0
    { K::Switch, 10, -1, kHookPassSwitch, 0x004725c1u },   // Crater_2, switch entry 1
    { K::Switch, 11, -1, kHookPassSwitch, 0x004725c1u },   // Crater_3, switch entry 2
    { K::Switch, 12, -1, kHookPassSwitch, 0x004725c1u },   // Crater_4, switch entry 3
    { K::Switch, 13, -1, kHookPassSwitch, 0x004725c1u },   // Crater_5, switch entry 4
    { K::Switch, 14, -1, kHookPassSwitch, 0x004725c1u },   // Crater_6, switch entry 5
    { K::Switch, 15, -1, kHookPassSwitch, 0x004725c1u },   // Crater_7, switch entry 6
    { K::Switch, 16, -1, kHookPassSwitch, 0x004725c1u },   // Crater_8, switch entry 7
    { K::Group,  3,  2, kHookPassSlot10, 0x00452dc0u, 0, 0 },   // OakTrunk x OakTrees_1.mts
    { K::Group,  3,  3, kHookPassSlot10, 0x00452dcbu, 0, 0 },   // OakTrunk x OakTrees_2.mts
    { K::Group,  2,  0, kHookPassSlot10, 0x00452dd7u, 0, 0 },   // CrossTree x Crosstrees.mts
};
const HookLod kWarzoneLods[] = {
    { 20, 21, 60.0f, 0x00452abau },   // DAT_006915fc = 0x42700000 (slot 0, before the call)
};

// Highway, record "highway" (Course_Id 38). Every excluded clump (0..38, 43, 44, 46, 47, 48, 50) is drawn
// only by slot 2 FUN_0044e470 @ 0x0044e470 from the 80-entry traffic array 0x00890060 (stride 0xf8, a
// frustum test FUN_004c1b40 per vehicle) or, inside the same loop, Jet 43 when DAT_005f9748 != 0. The traffic
// simulation (slots 0 / 1) is not ported: nothing excluded is drawn (U-row).
const HookDraw* const kHighwayDraws = nullptr;

// Neustein, record "neustein" (Course_Id 39). Slot 0 FUN_0044ac70 @ 0x0044ac70 builds Trunk 3 x Mts 0
// (Trunks_1, 48) DAT_00684428, x Mts 2 (Trunks_2, 48) DAT_0068442c, x Mts 5 (Trunks_3, 27) DAT_00684430.
// Slot 2 FUN_0044af10 @ 0x0044af10, read from the bytes (the decompiler drops the last two group calls after
// its PIC warning): clump 5, clump 6, FUN_0044a930 (cable lines, not modelled), the three groups.
// Not drawn: Trunk 3 base.
const HookDraw kNeusteinDraws[] = {
    { K::Clump, 5, -1, kHookPassSlot2, 0x0044af20u },   // Rock
    { K::Clump, 6, -1, kHookPassSlot2, 0x0044af34u },   // Edge
    { K::Group, 3,  0, kHookPassSlot2, 0x0044af47u },   // Trunk x Trunks_1.mts
    { K::Group, 3,  2, kHookPassSlot2, 0x0044af53u },   // Trunk x Trunks_2.mts
    { K::Group, 3,  5, kHookPassSlot2, 0x0044af65u },   // Trunk x Trunks_3.mts (tail jmp)
};

const HookTrack kTracks[] = {
    {  2, "rouabout", kRouaboutDraws, static_cast<int>(sizeof(kRouaboutDraws) / sizeof(kRouaboutDraws[0])),
       kRouaboutLods, static_cast<int>(sizeof(kRouaboutLods) / sizeof(kRouaboutLods[0])) },
    { 11, "sands",    kSandsDraws,    static_cast<int>(sizeof(kSandsDraws) / sizeof(kSandsDraws[0])),
       nullptr, 0 },
    { 33, "warzone",  kWarzoneDraws,  static_cast<int>(sizeof(kWarzoneDraws) / sizeof(kWarzoneDraws[0])),
       kWarzoneLods, static_cast<int>(sizeof(kWarzoneLods) / sizeof(kWarzoneLods[0])) },
    { 38, "highway",  kHighwayDraws,  0, nullptr, 0 },
    { 39, "neustein", kNeusteinDraws, static_cast<int>(sizeof(kNeusteinDraws) / sizeof(kNeusteinDraws[0])),
       nullptr, 0 },
};

}  // namespace

const HookTrack* HookTrackFor(int course_id) {
    static const bool s_off = std::getenv("MASHED_NO_HOOKSLOTS") != nullptr;
    if (s_off) return nullptr;
    for (const HookTrack& t : kTracks)
        if (t.course_id == course_id) return &t;
    return nullptr;
}

bool HookClumpDrawn(const HookTrack& t, int clump, int* pass, int* order) {
    for (int i = 0; i < t.n_draws; ++i) {
        const HookDraw& d = t.draws[i];
        if (d.kind == HookDrawKind::Group || d.clump != clump) continue;
        if (pass) *pass = d.pass;
        if (order) *order = i;
        return true;
    }
    return false;
}

// 0x00557a60
int HookLodIndex(const float atomic_pos[3], const float cam_pos[3], const float cam_at[3], float range) {
    // FUN_00558030 @ 0x00558030: [lod+0x10] = 10.0f [0x005cc55c] / range (stored as a float)
    const float k = 10.0f / range;
    // 0x00557ace..0x00557ae8: dx, dy, dz = atomic LTM pos - camera LTM pos (camera = *RwGlobals, frame +4)
    const float dx = atomic_pos[0] - cam_pos[0];
    const float dy = atomic_pos[1] - cam_pos[1];
    const float dz = atomic_pos[2] - cam_pos[2];
    // 0x00557af9..0x00557b16: (at.y*dy + at.x*dx) + at.z*dz, times k; fstp to a float at 0x00557b19
    float s = cam_at[1] * dy + cam_at[0] * dx;
    s = s + cam_at[2] * dz;
    const float r = s * k;
    // 0x00557b23..0x00557b3c: RC forced to 11 (`or ah,0xc`) for the fistp = truncation toward zero
    int lod = static_cast<int>(r);
    if (lod < 0) return 0;      // 0x00557b48
    if (lod >= 10) lod = 9;     // 0x00557b54
    return lod;
}

}  // namespace Render
}  // namespace mashed_re
