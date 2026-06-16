// Powerup/PowerupEffects.cpp — the 9 per-type effect functions.
//
// Each function ports the DECISION LOGIC of its original (ammo/cooldown/
// fire-mode gating, charge/jet state transitions) verbatim from the
// decompilation in re/analysis/structs/powerup_effects_decomp.md, and routes
// every LEAF (projectile spawn, RW model attach, contacts, FX) through
// IPowerupBackend. The tuning band DAT_005c*/005d* is UNHARVESTED, so the
// numeric rates/durations below are clearly-marked stand-ins, NOT the original
// constants — they make the effect playable, they are not bit-faithful. This is
// C2-grade; do NOT mark C4. (See PowerupSystem.h ledger.)
#include "PowerupSystem.h"
#include "PowerupEffects.h"

#include <cmath>

namespace mashed_re {
namespace Powerup {
namespace Effect {

// ---- tuning constants ------------------------------------------------------
// REAL (harvested 2026-06-16 from MASHED.exe .rdata; values in
// powerup_effects_decomp.md §6): these are scale-independent counts/rates/seconds
// and are bit-faithful. STAND-IN: the projectile launch *velocities* and the flame
// duration live in the world-coordinate / frame-count domain and do not translate
// to this radius-relative host backend, so they stay playable stand-ins (the real
// model is cited for the future verbatim de-stub when WS-E lands).
namespace tune {
    constexpr int   kGunAmmo      = 50;    // REAL: pool +0x10 = 0x32 (FUN_00456040)
    constexpr float kGunRate      = 0.06f; // REAL: timer reset 0x3d75c28f (FUN_004561c0)
    constexpr int   kMissileAmmo  = 1;     // REAL: DAT_006885d8 = 1 (FUN_00455060)
    constexpr int   kMortarAmmo   = 3;     // REAL: (&DAT_00684e44)[] = 3 (FUN_00453350)
    constexpr float kMortarRate   = 0.4f;  // REAL: (&DAT_00684e48)[] = 0x3ecccccd (FUN_004533b0)
    constexpr int   kShotgunPellets = 4;   // REAL: +0x10 = 4 (FUN_0045b200)
    constexpr float kOilDrop        = 0.1f;// REAL: _DAT_005cc56c = 0.1 -> supply 1.0/0.1 = 10 drops (FUN_00457800)
    // STAND-INS (world-scale / frame-count in the original; host-scaled here):
    constexpr float kFlameDuration  = 1.2f;// real gate = +0x14 frame counter >4 (FUN_0045a850); ~playable
    constexpr float kMissileSpeed   = 0.6f;// real aim = trig * _DAT_005ce480/005ccad0/005cd988 (FUN_00455150)
    constexpr float kMortarSpeed    = 0.5f;// real = fwd*_DAT_005cd02c(0.13) + ownerVel*_DAT_005cc9c8(0.9) (FUN_004533b0)
}

// helper: forward unit vec from the firing car (owner +0x20 fwd; fall back to yaw)
static void OwnerForward(const HostCar& c, float out[3]) {
    float fx = c.fwd[0], fy = c.fwd[1], fz = c.fwd[2];
    float m = std::sqrt(fx*fx + fy*fy + fz*fz);
    if (m < 1e-4f) { out[0] = std::cos(c.yaw); out[1] = 0.f; out[2] = std::sin(c.yaw); return; }
    out[0] = fx / m; out[1] = fy / m; out[2] = fz / m;
}

// ============================== GUN (9) ====================================
// arm 0x456040 / fire 0x4561c0 / canfire 0x4566d0 / deact 0x4566f0 / tick 0x4568d0
// Forward hitscan auto-cannon: fire-rate-gated, ammo-limited. Original spins a
// lock indicator + auto-targets via FUN_0045a0d0/004058e0; we hitscan forward.
void Gun_Arm(PowerupSystem&, Slot& s) {
    s.ammo = tune::kGunAmmo;                 // orig pool +0x10 = 0x32
    s.cooldown = 0.f; s.charge = 0.f;
}
bool Gun_CanFire(PowerupSystem&, Slot& s) {  // 0x4566d0: ammo < 1
    return s.ammo < 1;
}
void Gun_Deact(PowerupSystem&, Slot&) {}     // 0x4566f0: RwFrameRemoveChild (stub)
void Gun_Fire(PowerupSystem& sys, Slot& s, int /*mode*/) {  // 0x4561c0 (ignores mode)
    if (s.cooldown > 0.f || s.ammo < 1) return;   // orig: timer==0.0 && ammo>0
    s.ammo--;
    s.cooldown = tune::kGunRate;
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->HitscanForward(sys.owner().pos, fwd);
    sys.backend()->SfxByName("shotgun", 0.7f);
}
void Gun_Tick(PowerupSystem& sys, float dt) {  // 0x4568d0 (lock indicator; timers)
    if (sys.slot().activeCode == kGun && sys.slot().cooldown > 0.f)
        sys.slot().cooldown -= dt;
}

// ============================== DRUM (10) ==================================
// arm 0x453fe0 / fire 0x454740 / canfire 0x457ab0 / deact 0x457ad0 / tick 0x454820
// Depth-charge: dropped once behind the car, then disarms.
void Drum_Arm(PowerupSystem&, Slot& s) { s.subState = 1; }      // armed
bool Drum_CanFire(PowerupSystem&, Slot& s) { return s.subState == 0; } // *(armed+8)==0
void Drum_Deact(PowerupSystem&, Slot&) {}                        // mark teardown (stub)
void Drum_Fire(PowerupSystem& sys, Slot& s, int mode) {         // 0x454740: mode==2
    if (mode != kFirePrimary || s.subState == 0) return;
    sys.backend()->DropHazard(sys.owner().pos, /*proximity=*/false);
    sys.backend()->SfxByName("drop mine", 0.8f);
    s.subState = 0;                          // consumed -> canfire true next frame
}
void Drum_Tick(PowerupSystem&, float) {}     // 0x454820: host advances the hazard

// ============================== MISSILE (11) ===============================
// arm 0x455060 / fire 0x455150 / canfire 0x455360 / deact 0x455390 / tick 0x455c90
// Homing missile (the §7 worked exemplar): fire on PRIMARY (mode==2), one shot.
void Missile_Arm(PowerupSystem&, Slot& s) { s.ammo = tune::kMissileAmmo; }
bool Missile_CanFire(PowerupSystem&, Slot& s) {  // 0x455360: inflight==0 && ammo<1
    return s.ammo < 1;
}
void Missile_Deact(PowerupSystem&, Slot&) {}     // 0x455390
void Missile_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x455150: only mode==2
    if (mode != kFirePrimary || s.ammo < 1) return;
    s.ammo--;
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    const float* p = sys.owner().pos;
    float vel[3] = { fwd[0] * tune::kMissileSpeed,
                     0.f,
                     fwd[2] * tune::kMissileSpeed };
    sys.backend()->SpawnMissile(p, vel, /*target=*/-1);  // host picks nearest ahead
    sys.backend()->SfxByName("missile exhaust", 0.8f);
}
void Missile_Tick(PowerupSystem&, float) {}  // 0x455c90: host flies/homes the missile

// ============================== P_MINE (12) ================================
// arm 0x457a30 / fire 0x457ef0 / canfire 0x457ab0(shared) / deact 0x457ad0(shared)
// Proximity mine: dropped once behind, arms on the ground.
void PMine_Arm(PowerupSystem&, Slot& s) { s.subState = 1; }
bool PMine_CanFire(PowerupSystem&, Slot& s) { return s.subState == 0; }
void PMine_Deact(PowerupSystem&, Slot&) {}
void PMine_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x457ef0: mode==2
    if (mode != kFirePrimary || s.subState == 0) return;
    sys.backend()->DropHazard(sys.owner().pos, /*proximity=*/true);
    sys.backend()->SfxByName("drop mine", 0.8f);
    s.subState = 0;
}
void PMine_Tick(PowerupSystem&, float) {}    // 0x4582f0

// ============================== R_FLAME (16) ===============================
// arm 0x45a7c0 / fire 0x45a850 / canfire 0x45a890 / deact 0x45a8b0 / tick 0x45ae80
// Flamethrower jet: burns while held, depletes after a short duration.
void RFlame_Arm(PowerupSystem&, Slot& s) {   // orig FX FUN_00465e80(0x16)
    s.charge = tune::kFlameDuration; s.jetState = 0;
}
bool RFlame_CanFire(PowerupSystem&, Slot& s) {  // 0x45a890: pool[0]==0 (depleted)
    return s.charge <= 0.f;
}
void RFlame_Deact(PowerupSystem& sys, Slot&) {  // 0x45a8b0: jet off + stop FX 0x16
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->FlameJet(sys.owner().pos, fwd, /*on=*/false);
}
void RFlame_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x45a850
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    if (s.charge <= 0.f) {                    // orig: +0x14>4 -> shut off, clear pool
        sys.backend()->FlameJet(sys.owner().pos, fwd, false);
        return;
    }
    if (mode != kFireSecondary) {             // orig: param_2 != 1 -> jet on
        sys.backend()->FlameJet(sys.owner().pos, fwd, true);
        s.jetState = 1;
    }
}
void RFlame_Tick(PowerupSystem& sys, float dt) {  // 0x45ae80: spark sim + deplete
    if (sys.slot().activeCode == kRFlame && sys.slot().jetState)
        sys.slot().charge -= dt;
}

// ============================== SHOTGUN (17) ===============================
// arm 0x45b200 / fire 0x45b6e0 / canfire 0x45b260 / deact 0x45b290 / tick 0x45b700
// Short-range spread: a small number of pellet bursts then disarms.
void Shotgun_Arm(PowerupSystem&, Slot& s) { s.subState = tune::kShotgunPellets; }  // +0x10=4
bool Shotgun_CanFire(PowerupSystem&, Slot& s) { return s.subState == 0; }           // 0x45b260
void Shotgun_Deact(PowerupSystem&, Slot&) {}                                        // 0x45b290
void Shotgun_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x45b6e0: mode==2 & +0x10!=0
    if (mode != kFirePrimary || s.subState == 0) return;
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->SpreadCone(sys.owner().pos, fwd);
    sys.backend()->SfxByName("shotgun", 0.85f);
    s.subState = 0;                          // single discharge (FUN_0045b390)
}
void Shotgun_Tick(PowerupSystem&, float) {}  // 0x45b700

// ============================== FLASH (18) =================================
// arm 0x454a40 / fire 0x454db0 / canfire 0x454a90 / deact 0x454ab0 / tick 0x454e00
// Blind flash: one screen burst, then disarms.
void Flash_Arm(PowerupSystem&, Slot& s) { s.subState = 1; }    // armed (+0xc == 1)
bool Flash_CanFire(PowerupSystem&, Slot& s) { return s.subState == 4; }  // 0x454a90: +0xc==4
void Flash_Deact(PowerupSystem&, Slot&) {}                     // 0x454ab0: +0xc=5
void Flash_Fire(PowerupSystem& sys, Slot& s, int mode) {       // 0x454db0: mode==2 & +0xc==1
    if (mode != kFirePrimary || s.subState != 1) return;
    sys.backend()->BlindFlash(sys.owner().pos);   // orig FUN_00454c10 + FX 0x18
    sys.backend()->SfxByName("flash", 0.8f);
    s.subState = 4;                          // -> canfire true -> deactivate
}
void Flash_Tick(PowerupSystem&, float) {}    // 0x454e00

// ============================== OIL (19) ===================================
// arm 0x456d80 / fire 0x457800 / canfire 0x456dd0 / deact 0x456e00 / tick 0x4577b0
// Oil slick: drips a slick onto the ground at a distance cadence while held,
// consuming a supply meter.
void Oil_Arm(PowerupSystem&, Slot& s) { s.charge = 1.0f; }     // supply +0x00 = 1.0
bool Oil_CanFire(PowerupSystem&, Slot& s) { return s.charge < 0.f; }  // 0x456dd0: supply<0
void Oil_Deact(PowerupSystem&, Slot&) {}                       // 0x456e00
void Oil_Fire(PowerupSystem& sys, Slot& s, int /*mode*/) {     // 0x457800 (ignores mode)
    // orig: drop only when the car has moved >= _DAT_005cc56c since the last drop.
    if (!sys.backend()->OilDropDue(sys.owner().owner, sys.owner().pos)) return;
    sys.backend()->DropOilSlick(sys.owner().pos);
    s.charge -= tune::kOilDrop;              // orig DAT_0068a250 -= _DAT_005cc56c
}
void Oil_Tick(PowerupSystem&, float) {}      // 0x4577b0

// ============================== MORTAR (7) =================================
// arm 0x453350 / fire 0x4533b0 / canfire 0x453610 / deact 0x453630 / tick 0x453b80
// Lobbed mortar: fire on SECONDARY (mode==1), rate-gated, ammo-limited.
void Mortar_Arm(PowerupSystem&, Slot& s) {   // orig ammo=3, cooldown=-1.0
    s.ammo = tune::kMortarAmmo; s.cooldown = 0.f;
}
bool Mortar_CanFire(PowerupSystem&, Slot& s) { return s.ammo < 1; }  // 0x453610: ammo<1
void Mortar_Deact(PowerupSystem&, Slot&) {}  // 0x453630
void Mortar_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x4533b0: mode==1, ammo>=0, cd<=0
    if (mode != kFireSecondary || s.ammo < 1 || s.cooldown > 0.f) return;
    s.ammo--;
    s.cooldown = tune::kMortarRate;
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    const float* v = sys.owner().vel;
    // orig (no lock): vel = fwd*_DAT_005cd02c + ownerVel*_DAT_005cc9c8 (lob).
    float vel[3] = { fwd[0] * tune::kMortarSpeed + v[0] * 0.5f,
                     0.4f,                                   // upward arc component
                     fwd[2] * tune::kMortarSpeed + v[2] * 0.5f };
    sys.backend()->SpawnMortar(sys.owner().pos, vel, /*target=*/-1);
    sys.backend()->SfxByName("explosion1", 0.6f);
}
void Mortar_Tick(PowerupSystem& sys, float dt) {  // 0x453b80: projectiles + cooldown
    if (sys.slot().activeCode == kMortar && sys.slot().cooldown > 0.f)
        sys.slot().cooldown -= dt;
}

}  // namespace Effect
}  // namespace Powerup
}  // namespace mashed_re
