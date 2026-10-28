// Mashed RE — D5-ICONCUBE-GLOW: the pickup glow batch (exe-only TU).
//
// FUN_004587a0 @ 0x004587a0 creates the glow batch DAT_0068b968 (FUN_004770c0(&DAT_0068b968, 0x817, 0x19,
// FUN_0040bb30("puglow")), 0x004587b4..0x004587d2). FUN_00459000 @ 0x00459000 stages one particle per live
// (state 1) and per fading respawning (state 2) pool entry and flushes once after the pool loop; FUN_00458b10
// @ 0x00458b10 draws it with Z-write off. The exe runs FUN_00459000 verbatim (Gameplay/PickupTick.cpp), so
// this TU only supplies the six sprite-queue callees it calls (the PK_Glow* shims, re/tools/naked_specs/
// pickups_m2a.json) and owns the batch. The draw is LibRw/RwRaceFx.cpp (RaceFx_DrawWorldFx).
// PREREG: verify/iconcube_20261028/PREREG_D5-ICONCUBE-GLOW.md.
#pragma once

namespace mashed_re {
namespace Gameplay {

// MASHED_RFX_EMITTERS=1 (default OFF, not in FaithDefaults) AND PickupsOrig_Enabled().
// MASHED_NO_PICKUP_GLOW=1 keeps the emitter layer on and the glow off.
bool PickupGlow_Enabled();

// The flushed batch (an Rfx::Emitter*), or nullptr when disabled or before the first pickup tick.
void* PickupGlow_Emitter();

// MASHED_DBG_PUGLOW=<file>, DIAGNOSTIC ONLY: one PRE and one POST pool snapshot per pickup tick and the
// flushed particles, for verify/iconcube_20261028/check_glow.py. Called by Pickups_FrameTick.
void PickupGlow_DbgPre();
void PickupGlow_DbgPost();

}  // namespace Gameplay
}  // namespace mashed_re
