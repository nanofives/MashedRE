// Powerup/PowerupSystem.h — verbatim-structured port of the MASHED power-up
// effect cluster: dispatcher FUN_0045bba0 + 9-entry type table @0x005f9998
// (stride 0x40) + slot lifecycle @0x0088fbe0 (stride 0xb4).
//
// Maps:  re/analysis/structs/powerup_system.md         (D1 — architecture)
//        re/analysis/structs/powerup_effects_decomp.md (D2 — split + verbatim decomp)
//
// [PORT vs STUB — read before trusting any C-level]
//   PORTED VERBATIM (faithful to the decompilation):
//     - the dispatch architecture (3-pass per-frame loop, FUN_0045bba0),
//     - the slot lifecycle: lookup FUN_0045baa0, activate FUN_0045bfa0,
//       deactivate FUN_0045bac0, pickup-wrapper FUN_0045c010,
//     - the fire_mode decode (primary->2, secondary->1, both->3, none->pickup),
//     - each per-type effect's DECISION LOGIC (ammo/cooldown/fire-mode gating,
//       charge/jet state transitions, target-acquisition branch structure).
//   STUBBED (subsystems NOT yet landed standalone — see SESSION_VERIFICATION_AUDIT
//   _2026-06-16): every LEAF the original effects reach into —
//     - RW scene-graph (WS-E):  RwFrameAddChild / RwFrameRemoveChild / FUN_004c0ed0…
//     - contact system (WS-B):  FUN_0045bfe0 -> FUN_004b4b60/4cd0/4d10 -> FUN_0045c350
//     - per-type projectile pools (0x006883xx.., strides in the D2 doc) — unmapped
//     - the DAT_005c*/005d* tuning band — unharvested
//   is routed through IPowerupBackend, which the host (TrackRenderer) realises on
//   its existing in-race scaffold visuals. Therefore this module is C2-grade
//   (faithful structure, un-diffable leaves), NOT C4. NO overclaim.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Powerup {

// Real MASHED power-up type codes (POWERUPS_GOLD.LUA + the 9-entry table; the 3
// absent codes 6/8/21 are handled upstream — see powerup_system.md U-WSD-3).
enum Code {
    kCodeNone = -1,
    kMortar  = 7,   // table entry 8
    kGun     = 9,   // entry 0
    kDrum    = 10,  // entry 1
    kMissile = 11,  // entry 2
    kPMine   = 12,  // entry 3
    kRFlame  = 16,  // entry 4
    kShotgun = 17,  // entry 5
    kFlash   = 18,  // entry 6
    kOil     = 19,  // entry 7
};

// fire_mode the dispatcher passes to the type's FIRE slot (entry+0x08). Decoded
// in FUN_0045bba0 from two controller buttons: primary set -> 2, secondary set
// -> 1, both -> 3, neither -> the pickup/auto-deactivate branch.
enum FireMode { kFireNone = 0, kFireSecondary = 1, kFirePrimary = 2, kFireBoth = 3 };

// What an effect reads about a car. Mirrors the owner vehicle record fields the
// original effects touch (WS-A1):
//   owner = +0xb0 (controller/player index)
//   pos   = world matrix +0x30..0x38 (read via FUN_004c0ed0(frame))
//   fwd   = +0x20..0x28 (forward/orientation vec)
//   vel   = +0x90..0x98 (velocity vec)
struct HostCar {
    float pos[3] = {0, 0, 0};
    float fwd[3] = {0, 0, 0};
    float vel[3] = {0, 0, 0};
    float yaw    = 0.f;
    int   owner  = 0;
    bool  alive  = true;
};

// --- Stub boundary --------------------------------------------------------
// The leaf operations the real per-type effects perform against subsystems that
// are not yet landed standalone. The host implements these on its scaffold
// visuals; each method names the original RVA(s) it stands in for so a future
// session can swap in the verbatim subsystem and diff-original it.
class IPowerupBackend {
public:
    virtual ~IPowerupBackend() {}

    // The firing player + the AI field (so area/target effects can reach them).
    virtual const HostCar& Player() const = 0;
    virtual int            AiCount() const = 0;
    virtual HostCar&       Ai(int i) = 0;

    // MISSILE FIRE (FUN_00455150): launch a homing/straight projectile from pos
    // along vel; target = AI index or -1. (orig: projectile pool DAT_006883bc
    // stride 0x6c + RwFrameAddChild + FX FUN_00465e80(0x12/0x14)).
    virtual void SpawnMissile(const float pos[3], const float vel[3], int target) = 0;
    // MORTAR FIRE (FUN_004533b0): lob a projectile (arced if no lock).
    // (orig: pool DAT_00684ea8 stride 0x110 + FX FUN_00465ca0(0x12/0x1e)).
    virtual void SpawnMortar(const float pos[3], const float vel[3], int target) = 0;
    // DRUM/P_MINE FIRE (FUN_00454740 / FUN_00457ef0): drop a hazard behind the car.
    // (orig: FUN_004541e0 / FUN_00457c10 + FX FUN_00465ca0(0x12)).
    virtual void DropHazard(const float pos[3], bool proximity) = 0;
    // GUN FIRE (FUN_004561c0): forward hitscan tracer + impact on the car ahead.
    // (orig: FUN_004c0ed0 target, tracer FUN_0048aa20, FX FUN_00465ca0(0x13)).
    virtual void HitscanForward(const float pos[3], const float fwd[3]) = 0;
    // SHOTGUN FIRE (FUN_0045b6e0 -> FUN_0045b390): short-range spread cone.
    virtual void SpreadCone(const float pos[3], const float fwd[3]) = 0;
    // R_FLAME jet on/off (FUN_0045a850 sets +0x1c): continuous forward burner.
    virtual void FlameJet(const float pos[3], const float fwd[3], bool on) = 0;
    // FLASH FIRE (FUN_00454db0): blind/screen flash centred on the car.
    // (orig: FUN_00454c10 + FX FUN_00465ca0(0x18)).
    virtual void BlindFlash(const float pos[3]) = 0;
    // OIL FIRE (FUN_00457800): place a slick on the ground under the car.
    // (orig: contact FUN_004b4cd0 + FUN_004b4650/5080 + FX FUN_00465e80(0x17)).
    virtual void DropOilSlick(const float pos[3]) = 0;

    // Owner self-effects some types apply (boost-on-fire / brief shield).
    virtual void SfxByName(const char* name, float vol) = 0;

    // WS-B stand-in: did the car move far enough since the last oil drop? (the
    // original gates OIL on |pos - lastDrop|^2 >= _DAT_005cc56c, trail per owner).
    // The host keeps the trail; this stub returns true at a fixed cadence.
    virtual bool OilDropDue(int owner, const float pos[3]) = 0;
};

// One per-player power-up slot. Mirrors the real slot struct @0x0088fbe0:
//   +0xa8 -> activeCode (which type entry is active; kCodeNone = none held)
//   +0xac -> armed      (the ARM-result handle; != 0 means armed)
//   +0xb0 -> owner
// plus a compact per-type instance state standing in for the per-type pool
// record the original ARM (entry+0x04) allocates.
struct Slot {
    int   owner      = 0;          // +0xb0
    int   activeCode = kCodeNone;  // +0xa8
    bool  armed      = false;      // +0xac != 0
    // compact instance state (per-type pool stand-in)
    int   ammo       = 0;          // GUN/MISSILE/MORTAR shots remaining
    float cooldown   = 0.f;        // MORTAR/GUN fire-rate gate
    float charge     = 0.f;        // GUN spin-up charge (+0x18)
    int   jetState   = 0;          // R_FLAME on/off counter (+0x14/+0x1c)
    int   subState   = 0;          // DRUM/P_MINE/SHOTGUN/FLASH armed sub-state
    // input bytes (orig DAT_007f103c+3 / DAT_007f14ff / DAT_007f1055)
    bool  firePrimary   = false;
    bool  fireSecondary = false;
    bool  pickupFlag    = false;
};

// The 9-entry type table mirror. Each entry carries the real RVAs (for citation/
// diffing) and pointers to the ported effect fns. (orig table @0x005f9998.)
class PowerupSystem;
struct TypeEntry {
    int         code;               // entry +0x00
    const char* name;
    std::uint32_t armRva, fireRva, canfireRva, deactRva, tickRva;  // for diff/cite
    void (*arm)   (PowerupSystem&, Slot&);
    bool (*canfire)(PowerupSystem&, Slot&);
    void (*deact) (PowerupSystem&, Slot&);
    void (*fire)  (PowerupSystem&, Slot&, int mode);
    void (*tick)  (PowerupSystem&, float dt);
};

class PowerupSystem {
public:
    void Init(IPowerupBackend* be) { be_ = be; slot_ = Slot(); }
    IPowerupBackend* backend() const { return be_; }

    // --- lifecycle (verbatim FUN_0045baa0 / FUN_0045bfa0 / FUN_0045bac0) ---
    // 0x0045baa0 — linear lookup of the 9 entries for `code`; returns index/-1.
    static int Lookup(int code);
    // 0x0045bfa0 (via pickup wrapper 0x0045c010) — activate a held type: set the
    // active entry then call its ARM.
    void Activate(int code);
    // 0x0045bac0 — deactivate: call the active type's DEACT then clear the slot.
    void Deactivate();

    bool Armed()      const { return slot_.armed; }
    int  ActiveCode() const { return slot_.activeCode; }
    Slot&       slot()       { return slot_; }
    const Slot& slot() const { return slot_; }

    // Controller bridge (orig DAT_007f103c+3 primary, DAT_007f14ff secondary).
    void SetFireButtons(bool primary, bool secondary) {
        slot_.firePrimary = primary; slot_.fireSecondary = secondary;
    }
    // fire_mode decode exactly as FUN_0045bba0 (primary->2, secondary->1, both->3).
    static int DecodeFireMode(bool primary, bool secondary);

    // --- dispatcher (verbatim FUN_0045bba0, player-slot pass + per-type TICK) ---
    // gameMode is FUN_0040e350()'s value; the original only runs when == 6.
    void Tick(float dt, const HostCar& player, int gameMode);

    // expose the static table (also used by re-classify / diff citation).
    static const TypeEntry* Table();
    static int TableCount();   // DAT_005f9bd8 == 9

private:
    IPowerupBackend* be_ = nullptr;
    Slot             slot_;
    HostCar          owner_;       // last player snapshot the dispatcher saw
    friend struct TypeEntry;
public:
    // accessor used by the effect fns (they read the firing car from here).
    const HostCar& owner() const { return owner_; }
};

}  // namespace Powerup
}  // namespace mashed_re
