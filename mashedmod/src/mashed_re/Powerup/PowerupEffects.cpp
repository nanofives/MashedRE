// Powerup/PowerupEffects.cpp — the 9 per-type effect functions.
//
// Each function ports the DECISION LOGIC of its original (ammo/cooldown/
// fire-mode gating, charge/jet state transitions) verbatim from the
// decompilation in re/analysis/structs/powerup_effects_decomp.md, and routes
// every LEAF (projectile spawn, RW model attach, contacts, FX) through
// IPowerupBackend. WS-D-VISUAL (2026-06-17): the per-type DEACT teardown leaves
// (entry+0x10) — originally "// stub" pending the renderer because they did
// RwFrameRemoveChild on the RW scene graph — now route through the new
// IPowerupBackend::EffectEnd, realised on the standalone D3D9 "spike" billboard
// layer (TrackRenderer::parts_) instead of the (retired) RW scene graph. The
// tuning band DAT_005c*/005d* is UNHARVESTED, so the
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
    constexpr float kGunChargeCap = 40.0f; // REAL: _DAT_005cd274 (FCOMP 0x0045623d)
    constexpr float kGunChargeStep= 4.0f;  // REAL: _DAT_005cc35c (FADD 0x00456251)
    constexpr int   kDrumDrops    = 2;     // MEASURED: ARM leaves pool +0x08 = 2 for DRUM and
                                           //   P_MINE (verify/d3_pu_20260926/o1,o2 rec0);
                                           //   writer RVA [UNCERTAIN], see D3_POWERUPS §3
    constexpr int   kShotgunRefire= 8;     // REAL: +0xc = 8 at 0x0045b3b9 (FUN_0045b390)
    constexpr float kMissileLife  = 0.5f;  // MEASURED: flight timer +0x28 starts 0.5 on fire
                                           //   (ARM stores 0x3f000000 at +0x14); writer [UNCERTAIN]
    constexpr int   kMissileAmmo  = 1;     // REAL: DAT_006885d8 = 1 (FUN_00455060)
    constexpr int   kMortarAmmo   = 3;     // REAL: (&DAT_00684e44)[] = 3 (FUN_00453350)
    constexpr float kMortarRate   = 0.4f;  // REAL: (&DAT_00684e48)[] = 0x3ecccccd (FUN_004533b0)
    constexpr int   kShotgunPellets = 4;   // REAL: +0x10 = 4 (FUN_0045b200)
    constexpr float kOilDrop        = 0.1f;// REAL: _DAT_005cc56c = 0.1 -> supply 1.0/0.1 = 10 drops (FUN_00457800)
    // REAL (harvested pool13 2026-07-06, instruction-cited at the R_FLAME /
    // MORTAR sections below):
    constexpr int   kFlameBursts         = 5;     // REAL: +0x14 shutoff JL 5 (0x0045a86c)
    constexpr int   kFlameSparksPerBurst = 5;     // REAL: sub rollover CMP 5 (0x0045aa27)
    constexpr float kFlameSparkPeriod    = 0.02f; // REAL: +0x8 re-arm 0x3ca3d70a (0x0045aa4a)
    constexpr float kFlameMeterStep      = 0.04f; // REAL: DAT_005cd18c (FMUL 0x0045a93c)
    constexpr float kMortarFwdFactor     = 0.13f; // REAL: _DAT_005cd02c (FMUL 0x00453494/4a0/4ac)
    constexpr float kMortarVelFactor     = 0.9f;  // REAL: _DAT_005cc9c8 (FMUL 0x004534bb/4cd/4e5)
    // REAL (harvested pool14 2026-07-10, instruction-cited, WS-D unblocked slice #2):
    constexpr float kMortarCooldownInit  = -1.0f; // REAL: MORTAR arm +0x10 = 0xbf800000 (0x0045337a)
    // STAND-IN (world-scale in the original; host-scaled here):
    constexpr float kMissileSpeed   = 0.6f;// real aim = trig * _DAT_005ce480/005ccad0/005cd988 (FUN_00455150)
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
void Gun_Deact(PowerupSystem& sys, Slot&) {  // 0x4566f0: RwFrameRemoveChild (lock indicator)
    sys.backend()->EffectEnd(kGun, sys.owner().pos);
}
void Gun_Fire(PowerupSystem& sys, Slot& s, int /*mode*/) {  // 0x4561c0 (ignores mode)
    // charge +0x18: FCOMP _DAT_005cd274(40.0) / TEST AH,5 / JP at 0x0045623d..0x00456248,
    // then FADD _DAT_005cc35c(4.0) at 0x00456251 -- i.e. add 4 only while < 40, on
    // EVERY fire call, before the rate gate.
    if (s.charge < tune::kGunChargeCap) s.charge += tune::kGunChargeStep;
    // rate gate: FCOMP 0.0 / TEST AH,0x44 / JP at 0x00456270..0x0045627b fires only when
    // the timer is EXACTLY 0.0 (a negative timer blocks until the tick clamps it);
    // ammo +0x10 > 0 at 0x00456281..0x00456289.
    if (s.cooldown != 0.f || s.ammo < 1) return;
    s.ammo--;
    s.cooldown = tune::kGunRate;              // 0x3d75c28f at 0x0045628f
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->HitscanForward(sys.owner().pos, fwd);
    sys.backend()->SfxByName("shotgun", 0.7f);
}
void Gun_Tick(PowerupSystem& sys, float dt) {  // 0x4568d0 (lock indicator; timers)
    Slot& s = sys.slot();
    if (s.activeCode != kGun) return;
    // charge: FSUB _DAT_005cc320(1.0) at 0x00456bbf, clamp <0 -> 0 at 0x00456bd8.
    s.charge -= 1.0f;
    if (s.charge < 0.f) s.charge = 0.f;
    // timer +0x04: FCOMP 0.0 / TEST AH,0x41 / JNE at 0x00456be7..0x00456bf2:
    // > 0 -> FSUB dt (0x00456bf7); <= 0 -> store 0 (0x00456c02).
    if (s.cooldown > 0.f) s.cooldown -= dt;
    else                  s.cooldown = 0.f;
}

// ============================== DRUM (10) ==================================
// arm 0x453fe0 / fire 0x454740 / canfire 0x457ab0 / deact 0x457ad0 / tick 0x454820
// Depth-charge: dropped once behind the car, then disarms.
void Drum_Arm(PowerupSystem&, Slot& s) { s.ammo = tune::kDrumDrops; }  // pool +0x08 = 2
bool Drum_CanFire(PowerupSystem&, Slot& s) {  // 0x457ab0: SETE (*(rec+8)==0) at 0x00457ac1
    return s.ammo == 0;
}
void Drum_Deact(PowerupSystem& sys, Slot&) {  // 0x457ad0: +0x10=2,+0xc=0 (pool state; dropped drum lives on)
    sys.backend()->EffectEnd(kDrum, sys.owner().pos);
}
void Drum_Fire(PowerupSystem& sys, Slot& s, int mode) {         // 0x454740: CMP 2 at 0x00454747
    if (mode != kFirePrimary || s.ammo == 0) return;
    sys.backend()->DropHazard(sys.owner().pos, /*proximity=*/false);  // FUN_004541e0 0x00454758
    sys.backend()->SfxByName("drop mine", 0.8f);
    s.ammo--;   // MEASURED: +0x08 2->1->0, one per press edge (o1 call 1558 t+1/t+8)
}
void Drum_Tick(PowerupSystem&, float) {}     // 0x454820: host advances the hazard

// ============================== MISSILE (11) ===============================
// arm 0x455060 / fire 0x455150 / canfire 0x455360 / deact 0x455390 / tick 0x455c90
// Homing missile (the §7 worked exemplar): fire on PRIMARY (mode==2), one shot.
void Missile_Arm(PowerupSystem&, Slot& s) { s.ammo = tune::kMissileAmmo; }
bool Missile_CanFire(PowerupSystem&, Slot& s) {  // 0x455360
    if (s.jetState != 0) return false;            // in-flight +0x1c != 0 -> 0 (0x00455372..0x0045537b)
    return s.ammo < 1;                            // then ammo +0x08 (0x0045537c)
}
void Missile_Deact(PowerupSystem& sys, Slot&) {  // 0x455390: detach launcher attachment
    sys.backend()->EffectEnd(kMissile, sys.owner().pos);
}
void Missile_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x455150: only mode==2
    if (mode != kFirePrimary || s.ammo < 1) return;
    s.ammo--;
    s.jetState = 1;                 // MEASURED: in-flight +0x1c 0->1 on the fire frame
    s.life = tune::kMissileLife;    // MEASURED: +0x28 = 0.5 (then ticked the same frame)
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    const float* p = sys.owner().pos;
    float vel[3] = { fwd[0] * tune::kMissileSpeed,
                     0.f,
                     fwd[2] * tune::kMissileSpeed };
    sys.backend()->SpawnMissile(p, vel, /*target=*/-1);  // host picks nearest ahead
    sys.backend()->SfxByName("missile exhaust", 0.8f);
}
void Missile_Tick(PowerupSystem& sys, float dt) {  // 0x455c90 (host flies/homes the missile)
    // Flight TIMEOUT only, as measured (o1 call 1311, o2 call 1033): +0x28 -= dt each
    // frame; the frame it would go <= 0 it is stored 0 and in-flight +0x1c cleared.
    // The IMPACT end (contact FUN_0045bfe0 -> FUN_004b4d10 -> FUN_0045c350, then
    // FUN_00455910/00455100) is NOT ported: it is blocked on Collision/ContactStubs.cpp.
    Slot& s = sys.slot();
    if (s.activeCode != kMissile || s.jetState == 0) return;
    s.life -= dt;
    if (s.life <= 0.f) { s.life = 0.f; s.jetState = 0; }
}

// ============================== P_MINE (12) ================================
// arm 0x457a30 / fire 0x457ef0 / canfire 0x457ab0(shared) / deact 0x457ad0(shared)
// Proximity mine: dropped once behind, arms on the ground.
void PMine_Arm(PowerupSystem&, Slot& s) { s.ammo = tune::kDrumDrops; }  // pool +0x08 = 2
bool PMine_CanFire(PowerupSystem&, Slot& s) { return s.ammo == 0; }       // 0x457ab0 (shared)
void PMine_Deact(PowerupSystem& sys, Slot&) {  // 0x457ad0 (shared w/ DRUM): pool state; dropped mine lives on
    sys.backend()->EffectEnd(kPMine, sys.owner().pos);
}
void PMine_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x457ef0: mode==2
    if (mode != kFirePrimary || s.ammo == 0) return;
    sys.backend()->DropHazard(sys.owner().pos, /*proximity=*/true);  // FUN_00457c10
    sys.backend()->SfxByName("drop mine", 0.8f);
    s.ammo--;   // MEASURED: +0x08 2->1->0 (o2 call 905 t+1/t+8)
}
void PMine_Tick(PowerupSystem&, float) {}    // 0x4582f0

// ============================== R_FLAME (16) ===============================
// arm 0x45a7c0 / fire 0x45a850 / canfire 0x45a890 / deact 0x45a8b0 / tick 0x45ae80
// Rear flame: a BURST weapon, not a continuous timer (verbatim budget decoded
// pool13 2026-07-06, instruction-cited): 5 bursts (major counter +0x14,
// shutoff JL 5 at 0x0045a86c); each burst = 5 spark emissions (sub counter
// +0x18, rollover CMP 5 at 0x0045aa27 inside the emission stepper
// FUN_0045a950 — sole caller: tick FUN_0045ae80 — which also clears the jet
// flag +0x1c and resets sub at 0x0045aa2f..0x0045aa36, i.e. each burst ends
// itself); emissions are cooldown-gated at 0.02 s (+0x8 re-armed with
// 0x3ca3d70a at 0x0045aa4a; stepper gates on +0x1c!=0 at 0x0045a9c0 and on
// +0x8 vs 0.0 at 0x0045a9f0). Charge meter (helper LAB_0045a910):
// ((5 - major)*5 - sub) * DAT_005cd18c(0.04) -> 1.0 .. 0.0.
// Slot mapping: ammo=major(+0x14), subState=sub(+0x18), jetState=+0x1c,
// cooldown=+0x8, charge=derived meter.
void RFlame_Arm(PowerupSystem&, Slot& s) {   // 0x45a7c0: +0x08=0.02 (0x3ca3d70a), FX FUN_00465e80(0x16)
    s.ammo = 0; s.subState = 0; s.jetState = 0;
    s.cooldown = tune::kFlameSparkPeriod;
    s.charge = 1.0f;
}
bool RFlame_CanFire(PowerupSystem&, Slot& s) {  // 0x45a890: pool[0]==0 (depleted latch,
    return s.ammo >= tune::kFlameBursts;        //   set by fire's major>=5 arm 0x0045a875)
}
void RFlame_Deact(PowerupSystem& sys, Slot&) {  // 0x45a8b0: jet off + stop FX FUN_004661a0(0x16)
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->FlameJet(sys.owner().pos, fwd, /*on=*/false);  // continuous burner shutdown
    sys.backend()->EffectEnd(kRFlame, sys.owner().pos);           // stop-FX teardown
}
void RFlame_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x45a850
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    if (s.ammo >= tune::kFlameBursts) {       // +0x14 >= 5 (JL 5, 0x0045a86c): shut off
        sys.backend()->FlameJet(sys.owner().pos, fwd, false);
        s.jetState = 0;                       // +0x1c = 0 (0x0045a86e)
        return;
    }
    if (mode != kFireSecondary) {             // param_2 != 1 -> jet on (+0x1c=1, 0x0045a88c)
        sys.backend()->FlameJet(sys.owner().pos, fwd, true);
        s.jetState = 1;
    }
}
void RFlame_Tick(PowerupSystem& sys, float dt) {  // emission stepper FUN_0045a950 (from tick 0x45ae80)
    Slot& s = sys.slot();
    if (s.activeCode != kRFlame) return;
    // The +0x8 decrement at 0x0045aedd is in tick FUN_0045ae80's OUTER loop, ahead
    // of the stepper's +0x1c gate: MEASURED running while the jet is off (o2 call
    // 945 t+11..t+15: 0.00333 -> -0.0133 -> ... -0.08 with +0x1c == 0), so a relit
    // jet emits on its first frame. (Previously gated on the jet -> 1 frame late.)
    s.cooldown -= dt;
    if (!s.jetState) return;                             // gate +0x1c != 0 (0x0045a9c0)
    // U-9015 RESOLVED (pool14 2026-07-10): tick FUN_0045ae80's outer per-slot
    // loop walks the pool with a record pointer ESI = recordBase+4 (confirmed:
    // loop start &DAT_0068bd04, stride SUB ESI,0x68 at 0x0045aeb3, terminal
    // compare `CMP ESI,0x68bb64` at 0x0045af07 == DAT_0068bb60(pool base)+4).
    // The decrement `FLD [ESI+0x4]; FSUB [_DAT_007f100c]; FSTP [ESI+0x4]` at
    // 0x0045aedd is therefore recordBase+0x8 -- exactly the assumed +0x8
    // cooldown field; per-frame dt decrement confirmed, not assumed.
    if (s.cooldown > 0.f) return;                       // gate +0x8 vs 0.0 (0x0045a9f0)
    s.cooldown = tune::kFlameSparkPeriod;                // re-arm 0.02 (0x0045aa4a)
    ++s.subState;                                        // sub++ (0x0045aa24/0x0045aa2a)
    if (s.subState >= tune::kFlameSparksPerBurst) {      // CMP 5 (0x0045aa27)
        ++s.ammo;                                        // major++ (0x0045aa2f/30)
        s.jetState = 0;                                  // +0x1c = 0 (0x0045aa33): burst self-ends
        s.subState = 0;                                  // +0x18 = 0 (0x0045aa36)
        float fwd[3]; OwnerForward(sys.owner(), fwd);
        sys.backend()->FlameJet(sys.owner().pos, fwd, false);
    }
    s.charge = static_cast<float>((tune::kFlameBursts - s.ammo) * tune::kFlameSparksPerBurst
                                  - s.subState) * tune::kFlameMeterStep;  // meter LAB_0045a910
}

// ============================== SHOTGUN (17) ===============================
// arm 0x45b200 / fire 0x45b6e0 / canfire 0x45b260 / deact 0x45b290 / tick 0x45b700
// Short-range spread: a small number of pellet bursts then disarms.
void Shotgun_Arm(PowerupSystem&, Slot& s) {   // +0x10=4 (FUN_0045b200), +0xc=0
    s.ammo = tune::kShotgunPellets; s.counter = 0;
}
bool Shotgun_CanFire(PowerupSystem&, Slot& s) {  // 0x45b260: +0x10==0 && +0xc==0
    return s.ammo == 0 && s.counter == 0;        //   (0x0045b26a..0x0045b278)
}
void Shotgun_Deact(PowerupSystem& sys, Slot&) {  // 0x45b290: +0x18=2,+0x14=0 (pool state)
    sys.backend()->EffectEnd(kShotgun, sys.owner().pos);
}
void Shotgun_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x45b6e0: mode==2 & +0x10!=0
    if (mode != kFirePrimary || s.ammo == 0) return;          //   (0x0045b6e0..0x0045b6f6)
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    sys.backend()->SpreadCone(sys.owner().pos, fwd);
    sys.backend()->SfxByName("shotgun", 0.85f);
    // FUN_0045b390: one pellet per press edge -- DEC at 0x0045b3b5, +0xc = 8 at
    // 0x0045b3b9. (Previously zeroed all 4 pellets on the first shot: MEASURED
    // wrong, the original fires 4 times -- o1 call 1244 t+1/t+8/t+16/t+28.)
    s.ammo--;
    s.counter = tune::kShotgunRefire;
}
void Shotgun_Tick(PowerupSystem& sys, float) {  // 0x45b700
    // MEASURED: +0xc steps down by 1 per frame to 0 (o1 call 1244 t+1..t+15);
    // decrement site inside 0x45b700 [UNCERTAIN], see D3_POWERUPS §3.
    Slot& s = sys.slot();
    if (s.activeCode == kShotgun && s.counter > 0) --s.counter;
}

// ============================== FLASH (18) =================================
// arm 0x454a40 / fire 0x454db0 / canfire 0x454a90 / deact 0x454ab0 / tick 0x454e00
// Blind flash: one screen burst, then disarms.
void Flash_Arm(PowerupSystem&, Slot& s) { s.subState = 1; }    // armed (+0xc == 1)
bool Flash_CanFire(PowerupSystem&, Slot& s) { return s.subState == 4; }  // 0x454a90: +0xc==4
void Flash_Deact(PowerupSystem& sys, Slot&) {  // 0x454ab0: +0xc=5,+8=0 (pool state)
    sys.backend()->EffectEnd(kFlash, sys.owner().pos);
}
void Flash_Fire(PowerupSystem& sys, Slot& s, int mode) {       // 0x454db0: mode==2 & +0xc==1
    if (mode != kFirePrimary || s.subState != 1) return;       //   (0x00454db7, 0x00454dc7)
    sys.backend()->BlindFlash(sys.owner().pos);   // orig FUN_00454c10 + FX 0x18
    sys.backend()->SfxByName("flash", 0.8f);
    s.subState = 2;                               // FUN_00454c10: +0xc = 2 (0x00454c10)
}
void Flash_Tick(PowerupSystem& sys, float) {      // 0x454e00 -> FUN_00454c60 per record
    // FUN_00454c60: state 2 -> 3 (0x00454c95), state 3 -> 4 (0x00454ca3), one step
    // per frame. (Previously FIRE jumped straight to 4: MEASURED one frame early,
    // o1 call 1016: 1 -> 3 on the fire frame, 4 next, deactivate the frame after.)
    Slot& s = sys.slot();
    if (s.activeCode != kFlash) return;
    if (s.subState == 2)      s.subState = 3;
    else if (s.subState == 3) s.subState = 4;
}

// ============================== OIL (19) ===================================
// arm 0x456d80 / fire 0x457800 / canfire 0x456dd0 / deact 0x456e00 / tick 0x4577b0
// Oil slick: drips a slick onto the ground at a distance cadence while held,
// consuming a supply meter.
void Oil_Arm(PowerupSystem&, Slot& s) { s.charge = 1.0f; }     // supply +0x00 = 1.0
// 0x456dd0: FCOMP 0.0 / TEST AH,0x41 / JP at 0x00456de3..0x00456dee returns 1 for
// supply <= 0.0 (C0 or C3 alone), not < 0. Latent only: 1.0 - 10*0.1f lands on
// -7.45e-8 (b3a00000, o1 call 950 t+34), never exactly 0.
bool Oil_CanFire(PowerupSystem&, Slot& s) { return s.charge <= 0.f; }
void Oil_Deact(PowerupSystem& sys, Slot&) {  // 0x456e00: detach drip attachment
    sys.backend()->EffectEnd(kOil, sys.owner().pos);
}
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
void Mortar_Arm(PowerupSystem&, Slot& s) {   // 0x00453350: +0xc=3 (0x00453373), +0x10=-1.0 (0xbf800000, 0x0045337a)
    s.ammo = tune::kMortarAmmo; s.cooldown = tune::kMortarCooldownInit;
}
bool Mortar_CanFire(PowerupSystem&, Slot& s) { return s.ammo < 1; }  // 0x453610: ammo<1
void Mortar_Deact(PowerupSystem& sys, Slot&) {  // 0x453630: detach mortar tube attachment
    sys.backend()->EffectEnd(kMortar, sys.owner().pos);
}
void Mortar_Fire(PowerupSystem& sys, Slot& s, int mode) {  // 0x4533b0: mode==1, ammo>=0, cd<=0
    if (mode != kFireSecondary || s.ammo < 1 || s.cooldown > 0.f) return;
    s.ammo--;
    s.cooldown = tune::kMortarRate;
    float fwd[3]; OwnerForward(sys.owner(), fwd);
    const float* v = sys.owner().vel;
    // VERBATIM straight branch of 0x4533b0 (no target lock — JNZ 0x0045348b
    // not taken): vel[i] = fwd[i] * _DAT_005cd02c(0.13)  [FMULs 0x00453491..
    // 0x004534b2] + ownerVel[i] * _DAT_005cc9c8(0.9)  [0x004534b5..0x004534f4],
    // ALL THREE components. No separate upward-arc term exists in the original
    // (the previous 0.4f here was invented and is removed; vertical motion
    // comes from the car's own fwd[1]/vel[1]). The other branch is TARGET-
    // AIMED via FUN_0045a110 (not random) — un-ported, target=-1 below.
    float vel[3] = { fwd[0] * tune::kMortarFwdFactor + v[0] * tune::kMortarVelFactor,
                     fwd[1] * tune::kMortarFwdFactor + v[1] * tune::kMortarVelFactor,
                     fwd[2] * tune::kMortarFwdFactor + v[2] * tune::kMortarVelFactor };
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
