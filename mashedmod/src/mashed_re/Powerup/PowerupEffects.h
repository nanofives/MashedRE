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

// R_FLAME spark record, original stride 0xd dwords (0x34 B) at &DAT_0068bd00.
// The field map and its citations live with the pool in PowerupEffects.cpp.
struct FlameSpark {
    float pos[3];   // [0..2]
    float vel[3];   // [3..5]  PER-FRAME, not per-second: the probe is pos -> pos+vel
    float ageN;     // [6]     age * _DAT_005cd114, clamped to 1.0; >= 1.0 = done
    float age;      // [8]     seconds
    float angle;    // [10]    the emitter's random angle (&DAT_0068bd28); the port never
                    //         reads it, pu_replay's sort check uses it as the record's identity
    int   landed;   // [11]    latch set on a hit
    int   alive;    // [12]
};
// FUN_0045ac40 on one group of 5 records (d3rflame 2026-10-24). RFlame_Tick calls
// it under MASHED_RFLAME_SORT=1; pu_replay calls it directly for its sort check.
void RFlame_SortFive(FlameSpark g[5], const float ref[3]);
// The 100-record spark pool, read-only, for pu_replay's state check.
const FlameSpark* RFlame_Pool();

}  // namespace Effect
}  // namespace Powerup
}  // namespace mashed_re
