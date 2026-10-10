// Mashed RE — mode 7 leg M2a: the original's pickup tick FUN_00459000 (logic half), verbatim.
// Exe-only TU. PREREG: verify/d3_mode7_m2_20261010/PREREG_M2.md.
#pragma once

namespace mashed_re {
namespace Gameplay {

// MASHED_PICKUPS_ORIG (default OFF) AND MASHED_REGISTRY: the pickup pool lives at the original's
// 0x0068b198 / 0x0068b9a8 and FUN_00459000 runs each frame.
bool PickupsOrig_Enabled();

// Called by PickupPoolSpawn (FUN_00458e00) after its writes: binds the slot's fake RW object
// (+0x00) and resets its frame as FUN_004c15c0 does (identity, translation 0).
void PickupsOrig_OnSpawn(unsigned char* entry);

// FUN_0045bba0's FUN_00459000 call (0x0040fcd0), before the registry publish. No-op unless enabled.
void Pickups_FrameTick();

// G-M2a-REPLAY: MASHED_PICKUPS_REPLAY=<o_*.msd.pool.tsv>, report to MASHED_PICKUPS_REPLAY_OUT.
void Pickups_MaybeReplay();

// ---- mode 7 leg M3 (PREREG: verify/d3_mode7_m3_20261010/PREREG_M3.md) ----
// MASHED_REG_CALLBACKS (default OFF) AND MASHED_REGISTRY AND MASHED_PICKUPS_ORIG AND MASHED_REG_CARS:
// the registry's pickup callback FUN_00458920 runs, and PickupField stands down.
bool PickupCallbacks_Enabled();

// Exe hosts for FUN_00458920's bound callees (set by TrackRenderer).
struct PickupCbHost {
    int  (*slot_empty)(int v);          // FUN_0045bff0: [0x0088fc88 + v*0xb4] == 0
    void (*grant)(int v, int type);     // FUN_0045c010 -> FUN_0045bfa0 (PowerupSystem::Activate)
    int  (*sub_mode)();                 // FUN_0040e350
    int  (*held_code)(int v);           // trace only: the held type code, 0 when none
};
void Pickups_SetCbHost(const PickupCbHost& host);

// RG_CbDispatch (Race/WorldRegistry.cpp) for target 0x00458920: a = the pickup's slot, b = the other's.
void Pickups_Callback(void* a, void* b);

// Pool entry +0x20 (1 live, 2 respawning) of pool index i; -1 when out of range or not enabled.
int PickupsOrig_EntryState(int i);

// Trace only (MASHED_PKCB_TRACE): a PickupField collection on the knob-OFF path (car slot, type code).
void PickupCb_NoteFieldCollect(int car, int type);

// G-M3-REPLAY: MASHED_PKCB_REPLAY=<a.cb.tsv>[;<b.cb.tsv>...] (PK rows), report to MASHED_PKCB_REPLAY_OUT.
void PickupCb_MaybeReplay();

}  // namespace Gameplay
}  // namespace mashed_re
