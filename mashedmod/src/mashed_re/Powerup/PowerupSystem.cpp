// Powerup/PowerupSystem.cpp — dispatcher + lifecycle + 9-entry type table.
// Verbatim port of FUN_0045bba0 (dispatch), FUN_0045baa0 (lookup), FUN_0045bfa0
// (activate), FUN_0045bac0 (deactivate). See PowerupSystem.h header for the
// PORT-vs-STUB ledger and powerup_effects_decomp.md for the decompilation.
#include "PowerupSystem.h"
#include "PowerupEffects.h"
#include "PowerupContact.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

// NON-DEGENERACY CONTROL (harness only; unset in every real run).
//   MASHED_PU_FORCE=nobox -> the box-state gate at 0x0045bc67..0x0045bca5 is
//                            skipped, i.e. exactly the build that predates it.
// MEASURED 2026-09-28: with it set, verify/d3_contact_20260928b/s2 goes back to
// 181 decision mismatches and its SWEEP row back to 152 vs 185 -- so the gate is
// what carries the fix, not some incidental change alongside it.
static bool NoBoxGate() {
    static int m = -1;
    if (m < 0) {
        const char* e = std::getenv("MASHED_PU_FORCE");
        m = (e && std::strcmp(e, "nobox") == 0) ? 1 : 0;
    }
    return m != 0;
}

// ---- MASHED_PU_STEPDUMP (D3 WS-D measurement, 2026-09-26) -----------------
// Same columns as scenario_launch.py --statediff-puhook so re/tools/pu_diff.py
// reads both. rec_pre/rec_post carry the compact slot state instead of the raw
// pool record: ammo|cooldown|charge|jet|sub|counter|life.
static std::FILE* s_dump = nullptr;
static bool       s_dumpInit = false;
static std::FILE* DumpFile() {
    if (!s_dumpInit) {
        s_dumpInit = true;
        const char* p = std::getenv("MASHED_PU_STEPDUMP");
        if (p && *p) {
            s_dump = std::fopen(p, "w");
            if (s_dump)
                std::fprintf(s_dump, "frame,call,state,slot,ctrl,cur3,prev3,cur4,prev4,boxstate,"
                                     "dt,act,code_pre,h_pre,code_post,h_post,fire_modes,"
                                     "canfire_rets,deact_ra,rec_pre,rec_post\n");
        }
    }
    return s_dump;
}
static void SlotState(const Slot& k, char* out, std::size_t n) {
    std::snprintf(out, n, "%d|%.9g|%.9g|%d|%d|%d|%.9g", k.ammo, k.cooldown, k.charge,
                  k.jetState, k.subState, k.counter, k.life);
}

void PowerupSystem::Init(IPowerupBackend* be) {
    be_ = be;
    for (int s = 0; s < kSlots; ++s) { slots_[s] = Slot(); slots_[s].owner = s; }
    cur_ = 0; frame_ = 0;
}

// 0x0045baa0 PowerupTypeLookup — linear search of the 9 entries for code==entry+0.
//   for(i=0;i<DAT_005f9bd8;i++) if(ESI==*(0x005f9998+i*0x40)) return entry; return 0;
int PowerupSystem::Lookup(int code) {
    for (int i = 0; i < s_count; ++i)
        if (s_table[i].code == code) return i;
    return -1;
}

// 0x0045c010(idx, code): slot = 0x0088fbe0 + idx*0xb4 (IMUL 0x0045c014, ADD
// 0x0045c01a), JMP 0x0045bfa0, which reads the type from the 2nd stack arg
// (MOV ESI,[ESP+0xc] at 0x0045bfa1) and does
//   slot+0xa8 = lookup(code);  slot+0xac = (*(entry+0x04))(slot)   // ARM
// The original has NO null-guard (absent codes 6/8/21 would deref NULL+4) and no
// already-armed guard; its callers only activate an empty slot (dispatcher
// pickup branch gated on [slot+0xac]==0 at 0x0045bcab; box collect 0x00458a1b).
// We guard both because the standalone's callers are not yet all ported.
void PowerupSystem::Activate(int s, int code) {
    Slot& k = slots_[s];
    if (k.armed) return;
    int e = Lookup(code);
    if (e < 0) return;
    k.activeCode = code;                 // +0xa8
    k.armed      = true;                 // +0xac (the ARM-result handle)
    const int save = cur_; cur_ = s;
    s_table[e].arm(*this, k);            // (entry+0x04)(slot)
    cur_ = save;
    pendingAct_[s] = s_table[e].name;
}

// 0x0045bac0 PowerupSlotDeactivate (slot in ESI):
//   (*( *(slot+0xa8) +0x10))(slot);  slot+0xa8 = 0;  slot+0xac = 0
void PowerupSystem::Deactivate(int s) {
    Slot& k = slots_[s];
    if (!k.armed) return;
    const int save = cur_; cur_ = s;
    int e = Lookup(k.activeCode);
    if (e >= 0) s_table[e].deact(*this, k);   // (entry+0x10)(slot)
    cur_ = save;
    k.activeCode = kCodeNone;            // +0xa8 = 0
    k.armed      = false;                // +0xac = 0
    k.ammo = 0; k.cooldown = 0.f; k.charge = 0.f;
    k.jetState = 0; k.subState = 0; k.counter = 0; k.life = 0.f;
}

// fire_mode decode, exactly the FUN_0045bba0 branch lattice:
//   0x0045bd72 cl=cur;  if(cl) m=3 (0x0045bd7c)
//   0x0045bd84 dl=prev; if(dl){ if(!cl) m=1 (0x0045bd92); -> fire with m }
//   else { if(!cl) -> no fire (0x0045bda4); m=2 (0x0045bdaa) }
int PowerupSystem::DecodeFireMode(bool cur, bool prev) {
    if (cur && !prev) return kFirePrimary;     // 2 (press edge)
    if (cur &&  prev) return kFireBoth;        // 3 (held)
    if (!cur && prev) return kFireSecondary;   // 1 (release edge)
    return kFireNone;
}

void PowerupSystem::DumpRow(int s, int state, int codePre, int mode, int canf, int deact,
                            float dt, const char* act) {
    std::FILE* f = DumpFile();
    if (!f) return;
    const Slot& k = slots_[s];
    if (codePre == kCodeNone && k.activeCode == kCodeNone && mode == kFireNone && !deact && !act)
        return;
    char post[160]; SlotState(k, post, sizeof post);
    char modes[8] = "";
    if (mode != kFireNone) std::snprintf(modes, sizeof modes, "%d", mode);
    char cf[8] = "";
    if (canf >= 0) std::snprintf(cf, sizeof cf, "%d", canf);
    std::fprintf(f, "%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%.9g,%s%s,%d,0,%d,0,%s,%s,%s,%s,%s\n",
                 frame_, frame_, state, s, s, k.fireCur ? 255 : 0, k.firePrev ? 255 : 0,
                 k.discCur ? 255 : 0, k.discPrev ? 255 : 0, boxState_[s], dt, act ? "A" : "",
                 act ? act : "", codePre, k.activeCode, modes, cf,
                 deact == 1 ? "0x45bd67" : deact == 2 ? "0x45be52"
                            : deact == 3 ? "0x45bcfc" : deact == 4 ? "0x45bc9d" : "",
                 dumpPre_[s], post);
}

// 0x0045bba0 dispatcher. Per-slot pass over the 4 slots (0x0088fbe0 stride
// 0xb4), then the per-type TICK pass (entry+0x1c for all 9, 0x0045bdf2).
// PORTED per slot, only when raceState == 6 (CMP EAX,6 / JNE at 0x0045bc2b):
//   armed ([slot+0xac], 0x0045bcab):
//     CANFIRE (entry+0x0c)(slot) 0x0045bd58; nonzero -> FUN_0045bac0 0x0045bd62
//     else fire_mode from cur/prev ctrl byte +7 (0x0045bd72/0x0045bd84);
//       mode != 0 -> FIRE (entry+0x08)(slot,mode) 0x0045bdbb
//       mode == 0 && discCur && !discPrev (0x0045be31/0x0045be3f) -> FUN_0045bac0
//       0x0045be4d
// PORTED 2026-09-28 (D3 criterion (c)): the armed contact sweep FUN_0045bfe0 ->
// FUN_004b4b60 -> FUN_0045c350 (0x0045bcb2..0x0045bd11), including the
// deactivation branch at 0x0045bcf7 the capture reports as deact_ra 0x45bcfc.
// Its two leaves live in Powerup/PowerupContact.cpp; the sphere they query
// (slot+0x80..0x8c) has no writer in the ported lifecycle, so in the shipping
// build the query returns 0 and the branch is inert. MEASURED under injection of
// the original's own verdicts: verify/d3_contact_20260928 c2/c3/g2/g3, sweep
// counts 280/309/207/309 exact and the one c2 deactivation reproduced.
// PORTED 2026-09-28 (D3 powerups): the per-slot BOX-STATE gate DAT_0068d1f0[slot]
// (0x0045bc67..0x0045bca5; states 2/3/4 each short-circuit the pass). Its five
// PRODUCERS are still not ported -- see SetBoxState in the header for the caller
// list -- so the host supplies the value (pu_replay feeds the capture column,
// TickPowerupDispatch mirrors FUN_004111c0's 1-or-4).
// NOT PORTED (recorded, not faked): the unarmed pickup branch (0x0045bd24..0x0045bd47, FUN_0045c010(slot,0x10)).
// Pickups arrive through Activate() from the host instead. Per-type mode pass
// (entry+0x38/+0x3c, 0x0045be11) and the tail FUN_0045a190/FUN_00459000 likewise.
void PowerupSystem::Tick(float dt, const HostCar cars[kSlots], int raceState) {
    ++frame_;
    // MASHED_PU_CONTACTDUMP rows carry the same (frame, call) pair the stepdump
    // rows do, so re/tools/pu_contact_report.py pairs the two standalone CSVs the
    // same way it pairs the two Frida ones.
    Contact::SetCallCounter(frame_, frame_);
    for (int s = 0; s < kSlots; ++s) {
        owners_[s] = cars[s];
        slots_[s].owner = s;
        if (DumpFile()) SlotState(slots_[s], dumpPre_[s], sizeof dumpPre_[s]);
    }
    int codePre[kSlots], modeOut[kSlots], canfOut[kSlots], deactOut[kSlots];
    for (int s = 0; s < kSlots; ++s) {
        Slot& k = slots_[s];
        codePre[s] = k.activeCode; modeOut[s] = kFireNone; canfOut[s] = -1; deactOut[s] = 0;
        if (raceState != 6) continue;             // 0x0045bc2b CMP EAX,6 / JNE 0x45bdc1
        // BOX-STATE GATE, 0x0045bc67..0x0045bca5 (D3 powerups, 2026-09-28).
        // Disassembled from the pinned anchor; EDI = slot+0x90, so [EDI+0x18] is
        // the active ENTRY (+0xa8) and [EDI+0x1c] the armed handle (+0xac):
        //   0x0045bc6b  MOV EAX,[ECX*4+0x68d1f0]  ; ECX = slot index
        //   0x0045bc72  CMP EAX,4 / JE  0x45bdc1  ; 4 -> whole pass skipped
        //   0x0045bc7b  CMP EAX,2 / JNE 0x45bca2
        //   0x0045bc80  MOV EAX,[EDI+0x18]        ; the +0xa8 ENTRY, not +0xac
        //   0x0045bc85  MOV [ECX*4+0x68d1f0],3    ; unconditional in this arm
        //   0x0045bc90  JE  0x45bdc1              ; entry == 0 -> out
        //   0x0045bc98  CALL 0x45bac0             ; deactivate, RA 0x45bc9d
        //   0x0045bca2  CMP EAX,3 / JE  0x45bdc1  ; 3 -> whole pass skipped
        // A skipped pass calls neither CANFIRE nor FIRE, which is exactly the
        // empty canfire_rets/fire_modes the capture shows.
        if (!NoBoxGate()) {
            const int bs = boxState_[s];
            if (bs == 4) continue;                                 // 0x0045bc75
            if (bs == 2) {                                         // 0x0045bc7e
                boxState_[s] = 3;                                  // 0x0045bc85
                if (k.activeCode != kCodeNone) {                   // 0x0045bc83
                    cur_ = s; Deactivate(s); deactOut[s] = 4;      // 0x0045bc98
                }
                continue;                                          // 0x0045bc9d
            }
            if (bs == 3) continue;                                 // 0x0045bca5
        }
        if (!k.armed) continue;                   // 0x0045bcab MOV EAX,[EDI+0x1c] / JE 0x45bd1b
        // ARMED SWEEP, 0x0045bcb2..0x0045bd11 (D3 criterion (c), 2026-09-28).
        // Order matters: the original runs it BEFORE CANFIRE and RE-TESTS the
        // armed flag afterwards (0x0045bd14 `MOV EAX,[EDI+0x1c]` / `JNE 0x45bd4e`),
        // because the sweep can deactivate the slot.
        //   sweep_query != 0  AND  sweep_confirm == 0  ->  FUN_0045bac0 @0x0045bcf7
        // The deactivation the capture records as deact_ra 0x45bcfc.
        {
            Contact::WorldHit sw;
            cur_ = s;
            if (Contact::SweepQuery(s, owners_[s].pos, &sw, 0x0045bcd8) != 0) {  // 0x0045bcdd
                if (Contact::SweepConfirm(s, &sw, 0x0045bcea) == 0) {            // 0x0045bcef
                    Deactivate(s); deactOut[s] = 3;
                    continue;                      // 0x0045bd14 re-test: not armed
                }
            }
        }
        const int e = Lookup(k.activeCode);
        if (e < 0) continue;
        cur_ = s;
        const bool cf = s_table[e].canfire(*this, k);        // (entry+0x0c)(slot)
        canfOut[s] = cf ? 1 : 0;
        if (cf) { Deactivate(s); deactOut[s] = 1; continue; } // 0x0045bd62
        const int mode = DecodeFireMode(k.fireCur, k.firePrev);
        if (mode != kFireNone) {
            modeOut[s] = mode;
            s_table[e].fire(*this, k, mode);                  // (entry+0x08)(slot,mode)
        } else if (k.discCur && !k.discPrev) {
            Deactivate(s); deactOut[s] = 2;                    // 0x0045be4d
        }
    }
    // Per-type TICK pass (entry+0x1c) for all 9 types. The original's tick walks
    // the type's whole per-owner pool; here it runs once per slot with that slot
    // current, and each tick body acts only on a slot holding its type.
    for (int i = 0; i < s_count; ++i)
        for (int s = 0; s < kSlots; ++s) { cur_ = s; s_table[i].tick(*this, dt); }
    cur_ = 0;
    for (int s = 0; s < kSlots; ++s) {
        DumpRow(s, raceState, codePre[s], modeOut[s], canfOut[s], deactOut[s], dt,
                pendingAct_[s]);
        pendingAct_[s] = nullptr;
    }
    if (s_dump) std::fflush(s_dump);
}

}  // namespace Powerup
}  // namespace mashed_re
