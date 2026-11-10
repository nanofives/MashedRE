// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x00411f30 = the matched source re/match/s02/00411f30.c
// (MATCH-S02, 100%): "lazer.dff" read into a zeroed 0x400-byte buffer (0x004b65a0, then 0x004b3b70 with the size
// 0x004b65b0 returns), DAT_0063bc40 = 0x004781b0("LaserTower.dff", 0), then six clumps parsed from the buffer
// (0x004b3e40), each given 0x004b5320(clump, 0x40, 1) and 0x004b5580(clump, &{ff, 0, 0, ff}), and stored as
// DAT_0063bb38[i].tex = 0x004b3f90(clump) with +0x04 = 0 (six 0x2c-byte records).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 (Match/PortTop_0045bae0.cpp). These are the records the
// matched teardown 0x00412010 reads (Match/PortTop_00412010.cpp).
// Callees bind by name, no raw-address call:
//  - 0x004b6520 = Orig_004b6520, 0x004b65a0 / 0x004b65b0 (the source's thunk_FUN_004b68e0 / thunk_FUN_004b68f0,
//    call targets 0x00411f6e / 0x00411f78) = Orig_004b65a0 / Orig_004b65b0, 0x004b3b70 = Orig_004b3b70, 0x004b3e40 =
//    Orig_004b3e40, 0x004781b0 = Orig_004781b0 (this row's TUs).
//  - 0x004b5320 = Orig_004b5320 (Match/PortTop_004b5320.cpp).
//  - 0x004b3f90 = MashedRw_ClumpFirstAtomic_004b3f90 (LibRw/RwApiBind.cpp: the RW-order first atomic as an RW-layout
//    view, +0x3c = the clump).
//  - 0x004b5580 = stand-in PortTop_PWI_StandIn_004b5580 (below). Its matched source (re/match/s18/004b5580.c) is
//    RpClumpForAllAtomics(clump, 0x004b5560, colour), and the callback 0x004b5560 has no hooks.csv row and no matched
//    source, so its effect on the atomic is not known.
// lazer.dff is 0x3cc bytes in Powerups.piz (piz_extract.py list), inside the 0x400-byte buffer.
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_00411f30 Orig_00411f30
#define FUN_004b6520 Orig_004b6520
#define thunk_FUN_004b68e0 Orig_004b65a0
#define thunk_FUN_004b68f0 Orig_004b65b0
#define FUN_004b3b70 Orig_004b3b70
#define FUN_004781b0 Orig_004781b0
#define FUN_004b3e40 Orig_004b3e40
#define FUN_004b5320 Orig_004b5320
#define FUN_004b5580 PortTop_PWI_StandIn_004b5580
#define FUN_004b3f90 MashedRw_ClumpFirstAtomic_004b3f90
extern "C" {
#include "../../../../re/match/s02/00411f30.c"
// Stand-in for 0x004b5580 (reason in the header), with the source's own parameter types.
void PortTop_PWI_StandIn_004b5580(undefined4 p0, RGBA* p1) {
    static int s_n = 0;
    if (s_n++ < 6) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x004b5580(clump 0x%08X, colour %02X%02X%02X%02X): stand-in, not run: its "
                            "RpClumpForAllAtomics callback 0x004b5560 has no matched source (POWERUPINIT-A)\n",
                         p0, p1->r, p1->g, p1->b, p1->a);
            std::fclose(f);
        }
    }
}
}  // extern "C"
#endif  // MASHED_STANDALONE
