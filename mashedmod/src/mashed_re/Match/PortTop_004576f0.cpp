// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004576f0 = the matched source re/match/s29/004576f0.c
// (POWERUPINIT-B, BYTE-IDENTICAL). "oil" / "oilShine" particle batches 0x0068a2c0 / 0x0068a210, four
// oilslicker.dff clones in the 16-byte records at 0x0068a250 (+0xc), each through 0x0040cda0 and the static 0x00456d00.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 7 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp)
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x0040cda0 = PortTop_PWI_StandIn_0040cda0 (stand-in below)
//  - 0x00474d60 = Orig_00474d60 (Match/PortTop_00474d60.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// 0x0040cda0 (re/match/s02/0040cda0.c, BYTE-IDENTICAL) runs only when DAT_00636ad0 == 0, then calls 0x004672d0 (the
// camera at DAT_006905b0 +0x04) and RpClumpForAllAtomics(clump, 0x0040bb10, args). 0x0040bb10 has no matched source,
// so 0x0040cda0 is a logging stand-in here. The static 0x00456d00 (record in EAX) is in the source, and calls 0x00474d60.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_004576f0 Orig_004576f0
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_0040cda0 PortTop_PWI_StandIn_0040cda0
#define FUN_00474d60 Orig_00474d60
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/004576f0.c"
// Stand-in for 0x0040cda0 (reason in the header): logs and returns.
void PortTop_PWI_StandIn_0040cda0(int clump, float f, int tex) {
    static int s_n = 0;
    if (s_n++ < 4) {
        if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
            std::fprintf(fp, "[ORIG_TOP] rva 0x0040cda0(clump 0x%08X, %g, %d): stand-in, not run: its RpClumpForAllAtomics "
                             "callback 0x0040bb10 has no matched source (POWERUPINIT-C)\n", static_cast<unsigned>(clump), f, tex);
            std::fclose(fp);
        }
    }
}
}  // extern "C"
#endif  // MASHED_STANDALONE
