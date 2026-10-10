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

}  // namespace Gameplay
}  // namespace mashed_re
