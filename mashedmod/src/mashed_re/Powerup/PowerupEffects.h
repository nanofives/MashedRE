// Powerup/PowerupEffects.h — the 9 per-type effect functions referenced by the
// type table in PowerupSystem.cpp. Each is a ported reimplementation of the
// original ARM/FIRE/CANFIRE/DEACT/TICK fn (RVAs in the table + decomp doc); the
// LEAF operations route through IPowerupBackend. See PowerupSystem.h for the
// PORT-vs-STUB ledger.
#pragma once

namespace mashed_re {
namespace Powerup {

class PowerupSystem;
struct Slot;

namespace Effect {

#define PU_DECL(T) \
    void T##_Arm    (PowerupSystem&, Slot&);        \
    bool T##_CanFire(PowerupSystem&, Slot&);        \
    void T##_Deact  (PowerupSystem&, Slot&);        \
    void T##_Fire   (PowerupSystem&, Slot&, int);   \
    void T##_Tick   (PowerupSystem&, float);

PU_DECL(Gun)
PU_DECL(Drum)
PU_DECL(Missile)
PU_DECL(PMine)
PU_DECL(RFlame)
PU_DECL(Shotgun)
PU_DECL(Flash)
PU_DECL(Oil)
PU_DECL(Mortar)

#undef PU_DECL

}  // namespace Effect
}  // namespace Powerup
}  // namespace mashed_re
