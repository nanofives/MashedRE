// Mashed RE — the pickup IconCube (D5-ICONCUBE-SPIN, D-11076 cube half). Exe-only TU.
// PREREG: verify/iconcube_20261028/PREREG_D5-ICONCUBE-SPIN.md.
//
// MASHED_ICONCUBE (default ON since D5-PREGO-CUBE-FLIP, MASHED_NO_ICONCUBE=1 opts out) AND MASHED_PICKUPS_ORIG: one IconCube copy per live pool entry
// (FUN_00458b10 @ 0x00458b10), skinned by the entry's type (FUN_00458dd0 @ 0x00458dd0 /
// FUN_00458630 @ 0x00458630) and posed from the pool state FUN_00458a40 @ 0x00458a40 leaves
// (Gameplay/PickupTick_naked.inc). Reads the pool only; makes no RNG call. The PUGlow glow
// (D5-ICONCUBE-GLOW) is not drawn here.
#pragma once

#include <cstdint>
#include <cstdio>

namespace mashed_re {
namespace D3d9Render {

bool IconCube_Enabled();

// FUN_004587a0 @ 0x004587a0, the parts the port lacks: the pool writes +0x08 = 1.0 and
// +0x0c..+0x14 = (0, 1, 0) on the 25 entries, and IconCube.dff + PowerUpIcons.txd
// (Common/POWERUPS/Powerups.piz) registered with librw, one model per FUN_00458630 texture.
// Call after LibRw::RaceSubmit_BeginTrackLoad (the models are per track load). No-op when off.
void IconCube_OnTrackLoad(std::uint32_t ambient, std::FILE* log);

// FUN_00458b10's cube loop on this frame's pool: queues one LibRw::kInstAfterFx copy per kept
// entry. eye = camera position, at = a point on the view axis (eye + forward). No-op when off.
void IconCube_Submit(const float eye[3], const float at[3]);

}  // namespace D3d9Render
}  // namespace mashed_re
