// Powerup/PowerupSystem.cpp — dispatcher + lifecycle + 9-entry type table.
// Verbatim port of FUN_0045bba0 (dispatch), FUN_0045baa0 (lookup), FUN_0045bfa0
// (activate), FUN_0045bac0 (deactivate). See PowerupSystem.h header for the
// PORT-vs-STUB ledger and powerup_effects_decomp.md for the decompilation.
#include "PowerupSystem.h"
#include "PowerupEffects.h"

namespace mashed_re {
namespace Powerup {

// ---- the 9-entry type table (orig @0x005f9998, stride 0x40) ----------------
// Order is table order (not sorted by code), matching powerup_system.md §4.
// armRva/fireRva/... are the real per-type effect-fn RVAs (for diff-original
// citation); the function pointers are the ported reimplementations.
static const TypeEntry s_table[] = {
    // code            name        ARM        FIRE       CANFIRE    DEACT      TICK        fns
    { kGun,     "GUN",     0x456040, 0x4561c0, 0x4566d0, 0x4566f0, 0x4568d0,
      Effect::Gun_Arm,     Effect::Gun_CanFire,     Effect::Gun_Deact,     Effect::Gun_Fire,     Effect::Gun_Tick },
    { kDrum,    "DRUM",    0x453fe0, 0x454740, 0x457ab0, 0x457ad0, 0x454820,
      Effect::Drum_Arm,    Effect::Drum_CanFire,    Effect::Drum_Deact,    Effect::Drum_Fire,    Effect::Drum_Tick },
    { kMissile, "MISSILE", 0x455060, 0x455150, 0x455360, 0x455390, 0x455c90,
      Effect::Missile_Arm, Effect::Missile_CanFire, Effect::Missile_Deact, Effect::Missile_Fire, Effect::Missile_Tick },
    { kPMine,   "P_MINE",  0x457a30, 0x457ef0, 0x457ab0, 0x457ad0, 0x4582f0,
      Effect::PMine_Arm,   Effect::PMine_CanFire,   Effect::PMine_Deact,   Effect::PMine_Fire,   Effect::PMine_Tick },
    { kRFlame,  "R_FLAME", 0x45a7c0, 0x45a850, 0x45a890, 0x45a8b0, 0x45ae80,
      Effect::RFlame_Arm,  Effect::RFlame_CanFire,  Effect::RFlame_Deact,  Effect::RFlame_Fire,  Effect::RFlame_Tick },
    { kShotgun, "SHOTGUN", 0x45b200, 0x45b6e0, 0x45b260, 0x45b290, 0x45b700,
      Effect::Shotgun_Arm, Effect::Shotgun_CanFire, Effect::Shotgun_Deact, Effect::Shotgun_Fire, Effect::Shotgun_Tick },
    { kFlash,   "FLASH",   0x454a40, 0x454db0, 0x454a90, 0x454ab0, 0x454e00,
      Effect::Flash_Arm,   Effect::Flash_CanFire,   Effect::Flash_Deact,   Effect::Flash_Fire,   Effect::Flash_Tick },
    { kOil,     "OIL",     0x456d80, 0x457800, 0x456dd0, 0x456e00, 0x4577b0,
      Effect::Oil_Arm,     Effect::Oil_CanFire,     Effect::Oil_Deact,     Effect::Oil_Fire,     Effect::Oil_Tick },
    { kMortar,  "MORTAR",  0x453350, 0x4533b0, 0x453610, 0x453630, 0x453b80,
      Effect::Mortar_Arm,  Effect::Mortar_CanFire,  Effect::Mortar_Deact,  Effect::Mortar_Fire,  Effect::Mortar_Tick },
};
static const int s_count = (int)(sizeof(s_table) / sizeof(s_table[0]));  // == 9 (DAT_005f9bd8)

const TypeEntry* PowerupSystem::Table()      { return s_table; }
int              PowerupSystem::TableCount() { return s_count; }

// 0x0045baa0 PowerupTypeLookup — linear search of the 9 entries for code==entry+0.
//   for(i=0;i<DAT_005f9bd8;i++) if(ESI==*(0x005f9998+i*0x40)) return entry; return 0;
int PowerupSystem::Lookup(int code) {
    for (int i = 0; i < s_count; ++i)
        if (s_table[i].code == code) return i;
    return -1;
}

// 0x0045bfa0 PowerupSlotActivate (reached via the pickup wrapper 0x0045c010):
//   slot+0xa8 = lookup(code);  slot+0xac = (*(entry+0x04))(slot)   // ARM
// The original has NO null-guard (absent codes 6/8/21 would deref NULL+4); we
// guard because the standalone normalises codes upstream (PickupField).
void PowerupSystem::Activate(int code) {
    if (slot_.armed) return;                 // already holding
    int e = Lookup(code);
    if (e < 0) return;                       // not a real effect-table type
    slot_.activeCode = code;                 // +0xa8
    slot_.armed      = true;                 // +0xac (the ARM-result handle)
    s_table[e].arm(*this, slot_);            // (entry+0x04)(slot)
}

// 0x0045bac0 PowerupSlotDeactivate:
//   (*( *(slot+0xa8) +0x10))();  slot+0xa8 = 0;  slot+0xac = 0
void PowerupSystem::Deactivate() {
    if (!slot_.armed) return;
    int e = Lookup(slot_.activeCode);
    if (e >= 0) s_table[e].deact(*this, slot_);   // (entry+0x10)(slot)
    slot_.activeCode = kCodeNone;            // +0xa8 = 0
    slot_.armed      = false;                // +0xac = 0
    slot_.ammo = 0; slot_.cooldown = 0.f; slot_.charge = 0.f;
    slot_.jetState = 0; slot_.subState = 0;
}

// fire_mode decode, exactly the FUN_0045bba0 branch lattice:
//   cVar3 = primary;  if(cVar3) m=3;
//   if(secondary==0){ if(cVar3==0) -> none; else m=2; }
//   else { if(cVar3==0) m=1; ... -> m stays as set }  (both -> 3)
int PowerupSystem::DecodeFireMode(bool primary, bool secondary) {
    int m = 0;
    if (primary) m = kFireBoth;              // 3 (refined below)
    if (!secondary) {
        if (!primary) return kFireNone;      // neither -> pickup branch
        m = kFirePrimary;                    // 2
    } else {
        if (!primary) m = kFireSecondary;    // 1
        // both set: m stays kFireBoth (3)
    }
    return m;
}

// 0x0045bba0 dispatcher — the player-slot pass + the per-type TICK pass. The
// original walks 4 slots (0x0088fbe0 stride 0xb4) and gates the whole pass on
// FUN_0040e350()==6; standalone drives the single interactive player slot here
// and lets the host advance the AI/instances. The contact-sweep / state-machine
// (FUN_0045bfe0->FUN_004b4b60->FUN_0045c350, DAT_0068d1f0 states 2/3/4) is the
// WS-B/RW path — STUBBED out of this loop; only the armed fire path is ported.
void PowerupSystem::Tick(float dt, const HostCar& player, int gameMode) {
    if (gameMode != 6) return;               // FUN_0040e350()==6 gate
    owner_ = player;
    slot_.owner = player.owner;

    if (slot_.armed) {
        int e = Lookup(slot_.activeCode);
        if (e >= 0) {
            // CANFIRE (entry+0x0c): nonzero -> auto-deactivate (FUN_0045bac0).
            if (s_table[e].canfire(*this, slot_)) {
                Deactivate();
            } else {
                // fire_mode from the controller bytes, then FIRE (entry+0x08).
                int mode = DecodeFireMode(slot_.firePrimary, slot_.fireSecondary);
                if (mode != kFireNone)
                    s_table[e].fire(*this, slot_, mode);   // (entry+0x08)(slot,mode)
            }
        }
    }

    // Per-type TICK pass (entry+0x1c) for all 9 types — advances live instances.
    for (int i = 0; i < s_count; ++i)
        s_table[i].tick(*this, dt);
}

}  // namespace Powerup
}  // namespace mashed_re
