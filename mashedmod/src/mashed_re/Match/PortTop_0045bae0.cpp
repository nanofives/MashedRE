// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x0045bae0 = the matched source re/match/s10/0045bae0.c
// (MATCH-S10, bytes identical over 181 B): two slot passes (0x0045ba10 on the four 0xb4-byte slots at 0x0088fbe0 and
// 0x0068d200, DAT_0068d4d0[i] = 0), DAT_0068d4e0 = 0x0042a6b0("PowerUpModels.txd", 0, 0), DAT_005f9998[i].init() for
// i < DAT_005f9bd8, then 0x00459290, 0x004587a0, 0x00453f30, 0x0047b9b0("powerups_all.lua", &tmp, 0x00440bc0),
// 0x00411f30 and the tail call 0x004039c0. Caller: the matched 0x00402750 (Match/PortTop_00402750.cpp) through
// PortTop_PWI_Call0045bae0 below, on the MASHED_ORIG_TOP path only.
//
// The source file also holds 0x0045ba10 and 0x0045bac0 (static), 0x0045bf30 and 0x0045bba0. They compile here as
// Orig_0045bf30 / Orig_0045bba0 and nothing in the exe calls them (0x0040cfd0 keeps the 0x0045bf30 stand-in: it calls
// DAT_005f9998[i].clear(), slot +0x30, which is 0 in the exe). The linker needs every callee of 0x0045bba0 resolved, so
// each one binds to a logging trap, PortTop_PWI_NotRun_<rva> (below), that no exe path reaches.
//
// Callees of 0x0045bae0, bound by name, no raw-address call:
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp). 0x0042a6b0 = FUN_0042a6b0 (Match/MatchS06_0042a6b0.cpp).
//  - 0x00459290, 0x00453f30, 0x00411f30, 0x004039c0 = Orig_<rva> (this row's Match/PortTop_<rva>.cpp).
//  - 0x004587a0 = PortTop_PWI_Call004587a0 below: Orig_004587a0 (Match/PortTop_004587a0.cpp), then
//    PickupsOrig_SetPoolBuilt (Gameplay/PickupTick.cpp). From then on the 25 pool clumps belong to the builder and the
//    matched teardown 0x00458880. The hand tick borrows each +0x00 word for its fake object, and the teardown forwarder
//    puts the clump back first (the reason is in PickupTick.cpp).
//  - 0x0047b9b0: declared deviation L-114 (decision (b), 2026-11-10). The Lua 4.0 VM is not ported: 0x0047b860 opens a
//    lua-4.0 state (0x004b7330, 0x004c0510), 0x0047b8d0 runs the chunk through the stream layer 0x005507b0..0x00550bc0,
//    and the registrar 0x00440bc0 has no hooks.csv row. In its place the hand runner Save::RunTrackPowerupsScript
//    (Save/GameSaveFormat.h) reads powerups_all.lua from the piz mount stack (PortTop_Fs_ReadAll) and applies its
//    Set_Track_Powerups calls (0x0047b280) to the span at 0x007f0a40, rows at 0x007f0cb0, which is what the script does
//    in the original. The callback argument (0x00440bc0) is not called. The out argument is not written. Porting the
//    Lua library is row LUA-PORT.
//  - The type table: DAT_005f9998 (9 x 0x40 bytes, count DAT_005f9bd8 = 9 seeded by Compat/PortStateSeed.inc) holds the
//    original's .text addresses. The exe's image pad holds that range zero-filled (Track/CourseTableState.cpp, U-9783),
//    so every +0x14 init slot is 0 and the matched loop would call address 0. PortTop_PWI_Call0045bae0 first stores a
//    logging stand-in in each +0x14 slot that is 0, on this path only. Read from original\MASHED.exe.unpatched:
//      i type init        status
//      0 0x09 0x00456760  matched source re/match/s09/00456760.c, callees not on this path (0x004b3fc0 has no source)
//      1 0x0a 0x004548e0  matched source re/match/s09/004548e0.c, same
//      2 0x0b 0x00455400  no hooks.csv row, no matched source
//      3 0x0c 0x004583a0  no hooks.csv row, no matched source
//      4 0x10 0x0045a6b0  no hooks.csv row, no matched source
//      5 0x11 0x0045b830  no hooks.csv row, no matched source
//      6 0x12 0x00454e70  no hooks.csv row, no matched source
//      7 0x13 0x004576f0  no hooks.csv row, no matched source
//      8 0x07 0x00452f70  no hooks.csv row, no matched source
//    Rows POWERUPINIT-B (match the seven) and POWERUPINIT-C (port the nine onto the table). No other exe code reads the
//    +0x14 slots (0x0045b930 calls the +0x18 slots, Match/PortTop_0045b930.cpp, which stay 0).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "../Save/GameSaveFormat.h"

extern "C" void* PortTop_Fs_ReadAll(const char* name, unsigned* len, const char** from);
namespace mashed_re { namespace Gameplay { int PickupsOrig_SetPoolBuilt(); } }

static void PortTop_PWI_Log(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] ");
        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(f, fmt, ap);
        va_end(ap);
        std::fprintf(f, " (POWERUPINIT-A)\n");
        std::fclose(f);
    }
}

// Call site of rva 0x0047b9b0 (L-114): C++ overload taking the source's argument types (the string, &tmp, the registrar).
static void PortTop_PWI_Call0047b9b0(const char* name, int* out, void (*registrar)(void)) {
    (void)out;
    (void)registrar;
    unsigned len = 0;
    const char* from = "none";
    void* const text = PortTop_Fs_ReadAll(name, &len, &from);
    int skipped = 0, calls = -1;
    if (text) {
        calls = mashed_re::Save::RunTrackPowerupsScript(reinterpret_cast<unsigned char*>(static_cast<unsigned long>(0x007f0a40u)),
                                                        static_cast<const char*>(text), len, &skipped);
        std::free(text);
    }
    PortTop_PWI_Log("rva 0x0047b9b0: deviation L-114, hand runner Save::RunTrackPowerupsScript on the span 0x007f0a40: "
                    "%u bytes, %d Set_Track_Powerups calls, %d statements skipped, from %s", len, calls, skipped, from);
}

#define MATCH_PORT 1
#define FUN_0045bae0 Orig_0045bae0
#define FUN_0045bf30 Orig_0045bf30
#define FUN_0045bba0 Orig_0045bba0
#define FUN_004b6520 Orig_004b6520
#define FUN_00459290 Orig_00459290
#define FUN_004587a0 PortTop_PWI_Call004587a0
#define FUN_00453f30 Orig_00453f30
#define FUN_0047b9b0 PortTop_PWI_Call0047b9b0
#define FUN_00440bc0 PortTop_PWI_NotRun_00440bc0
#define FUN_00411f30 Orig_00411f30
#define FUN_004039c0 Orig_004039c0
// callees of the uncalled Orig_0045bba0 (traps)
#define FUN_0040e350 PortTop_PWI_NotRun_0040e350
#define FUN_0041f060 PortTop_PWI_NotRun_0041f060
#define FUN_0045bfe0 PortTop_PWI_NotRun_0045bfe0
#define FUN_004b4b60 PortTop_PWI_NotRun_004b4b60
#define FUN_0045c350 PortTop_PWI_NotRun_0045c350
#define FUN_00476880 PortTop_PWI_NotRun_00476880
#define FUN_0045c010 PortTop_PWI_NotRun_0045c010
#define FUN_00426c00 PortTop_PWI_NotRun_00426c00
#define FUN_0045a190 PortTop_PWI_NotRun_0045a190
#define FUN_00459000 PortTop_PWI_NotRun_00459000
#define FUN_00453f60 PortTop_PWI_NotRun_00453f60
extern "C" {
#include "../../../../re/match/s10/0045bae0.c"

void Orig_004587a0(void);   // matched, Match/PortTop_004587a0.cpp

static void PortTop_PWI_Trap(const char* rva) {
    PortTop_PWI_Log("rva %s: reached from the uncalled Orig_0045bba0, no exe path calls it", rva);
}
int PortTop_PWI_NotRun_0040e350(void) { PortTop_PWI_Trap("0x0040e350"); return 0; }
void PortTop_PWI_NotRun_0041f060(int i, Mat0045ba10* m) { (void)i; (void)m; PortTop_PWI_Trap("0x0041f060"); }
void* PortTop_PWI_NotRun_0045bfe0(void) { PortTop_PWI_Trap("0x0045bfe0"); return 0; }
int PortTop_PWI_NotRun_004b4b60(void* world, V3_0045ba10* pos, int* hit) {
    (void)world; (void)pos; (void)hit; PortTop_PWI_Trap("0x004b4b60"); return 0;
}
int PortTop_PWI_NotRun_0045c350(int* hit, V3_0045ba10* pos) { (void)hit; (void)pos; PortTop_PWI_Trap("0x0045c350"); return 0; }
void PortTop_PWI_NotRun_00476880(V3_0045ba10* pos, float* col, float a, float b) {
    (void)pos; (void)col; (void)a; (void)b; PortTop_PWI_Trap("0x00476880");
}
void PortTop_PWI_NotRun_0045c010(int slot, int type) { (void)slot; (void)type; PortTop_PWI_Trap("0x0045c010"); }
int PortTop_PWI_NotRun_00426c00(void) { PortTop_PWI_Trap("0x00426c00"); return 0; }
void PortTop_PWI_NotRun_0045a190(void) { PortTop_PWI_Trap("0x0045a190"); }
void PortTop_PWI_NotRun_00459000(void) { PortTop_PWI_Trap("0x00459000"); }
void PortTop_PWI_NotRun_00453f60(void) { PortTop_PWI_Trap("0x00453f60"); }
// The registrar's address is passed to 0x0047b9b0 and the L-114 binding does not call it.
void PortTop_PWI_NotRun_00440bc0(void) { PortTop_PWI_Trap("0x00440bc0"); }

// Call site of rva 0x004587a0: the matched builder, then the pool's ownership rule (header).
void PortTop_PWI_Call004587a0(void) {
    Orig_004587a0();
    const int clumps = mashed_re::Gameplay::PickupsOrig_SetPoolBuilt();
    PortTop_PWI_Log("rva 0x004587a0: matched body, %u of 25 pool entries hold a clump, record 0x0068b968 +0x04 = %08X, "
                    "DAT_0068b9ac = %08X", static_cast<unsigned>(clumps),
                    *reinterpret_cast<const unsigned*>(static_cast<unsigned long>(0x0068b96cu)),
                    *reinterpret_cast<const unsigned*>(static_cast<unsigned long>(0x0068b9acu)));
}

// The nine type-table inits (+0x14). POWERUPINIT-C: seven run their matched bodies (Match/PortTop_<rva>.cpp), each
// through a forwarder that logs the call and the records the init wrote. One stays a stand-in (entry 1 was the second until ATOMIC-VIEW-2):
//  - entry 1, 0x004548e0 (DepthCharge): runs matched since ATOMIC-VIEW-2 (librw clone / set-frame / add-child bindings
//    on the 0x004b3f90 view, Match/PortTop_004548e0.cpp).
//  - entry 2, 0x00455400 (Missile): no byte match (94.0%, POWERUPINIT-B). Row POWERUPINIT-B2.
void Orig_00456760(void);
void Orig_004548e0(void);
void Orig_004583a0(void);
void Orig_0045a6b0(void);
void Orig_0045b830(void);
void Orig_00454e70(void);
void Orig_004576f0(void);
void Orig_00452f70(void);
#define PWI_INIT(i, rva, why)                                                                                          static void PortTop_PWI_InitStandIn_##i(void) {                                                                        PortTop_PWI_Log("type table entry %u (+0x14 = 0x%08X in the original): init stand-in, not run: " why,                               static_cast<unsigned>(i), 0x##rva##u);                                                        }
PWI_INIT(2, 00455400, "no byte match (94.0 percent), row POWERUPINIT-B2")
#undef PWI_INIT
static unsigned PortTop_PWI_Dw(unsigned a);
#define PWI_MATCHED(i, rva, rec, what)                                                                                 static void PortTop_PWI_Init_##i(void) {                                                                               Orig_##rva();                                                                                                      PortTop_PWI_Log("type table entry %u: rva 0x" #rva ": matched body returned, " what " = %08X",                                     static_cast<unsigned>(i), PortTop_PWI_Dw(rec));                                                }
PWI_MATCHED(0, 00456760, 0x006886b8u, "first clone (0x006886b8)")
PWI_MATCHED(1, 004548e0, 0x00688260u, "first DepthCharge clone (0x00688260)")
PWI_MATCHED(3, 004583a0, 0x0068a368u, "first mine clone (0x0068a368)")
PWI_MATCHED(4, 0045a6b0, 0x0068bb80u, "first flame clone (0x0068bb80)")
PWI_MATCHED(5, 0045b830, 0x0068d174u, "first shotgun clone (0x0068d174)")
PWI_MATCHED(6, 00454e70, 0x00688368u, "first flashunit clone (0x00688368)")
PWI_MATCHED(7, 004576f0, 0x0068a25cu, "first oil clone (0x0068a25c)")
PWI_MATCHED(8, 00452f70, 0x00684e4cu, "first mortar clone (0x00684e4c)")
#undef PWI_MATCHED
static void (*const s_pwiInit[9])(void) = {
    PortTop_PWI_Init_0, PortTop_PWI_Init_1, PortTop_PWI_InitStandIn_2,
    PortTop_PWI_Init_3, PortTop_PWI_Init_4, PortTop_PWI_Init_5,
    PortTop_PWI_Init_6, PortTop_PWI_Init_7, PortTop_PWI_Init_8,
};

static unsigned PortTop_PWI_Dw(unsigned a) { return *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(a)); }

// Call site of rva 0x0045bae0 (from the matched 0x00402750): fill the type table's +0x14 slots, then the matched body.
void PortTop_PWI_Call0045bae0(void) {
    const int n = static_cast<int>(PortTop_PWI_Dw(0x005f9bd8u));
    if (n != 9) {
        PortTop_PWI_Log("rva 0x0045bae0: stand-in, not run: DAT_005f9bd8 = %d, the table holds 9 entries", n);
        return;
    }
    unsigned seeded = 0, kept = 0;
    for (int i = 0; i < 9; ++i) {
        unsigned* const slot = reinterpret_cast<unsigned*>(static_cast<unsigned long>(0x005f9998u + 0x40u * i + 0x14u));
        if (*slot == 0) {
            *slot = static_cast<unsigned>(reinterpret_cast<unsigned long>(s_pwiInit[i]));
            ++seeded;
        } else {
            ++kept;
        }
    }
    PortTop_PWI_Log("rva 0x0045bae0: type table +0x14 slots: %u init pointers stored (8 matched, 1 stand-in), %u already non-zero, of 9", seeded, kept);
    Orig_0045bae0();
    PortTop_PWI_Log("rva 0x0045bae0: matched body returned: DAT_0068d4e0 = %08X, DAT_0068bb58 = %08X, record 0x0068b9b8 +0x04 = %08X",
                    PortTop_PWI_Dw(0x0068d4e0u), PortTop_PWI_Dw(0x0068bb58u), PortTop_PWI_Dw(0x0068b9bcu));
    unsigned laser = 0;
    for (int i = 0; i < 6; ++i) if (PortTop_PWI_Dw(0x0063bb38u + 0x2cu * i)) ++laser;
    PortTop_PWI_Log("rva 0x0045bae0: DAT_00636c00 (Bomb.dff) = %08X, DAT_0063bc40 (LaserTower.dff) = %08X, %u of 6 laser records",
                    PortTop_PWI_Dw(0x00636c00u), PortTop_PWI_Dw(0x0063bc40u), laser);
}
}  // extern "C"
#endif  // MASHED_STANDALONE
