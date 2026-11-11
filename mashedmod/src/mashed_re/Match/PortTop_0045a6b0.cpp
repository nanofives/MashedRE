// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x0045a6b0 = the matched source re/match/s29/0045a6b0.c
// (POWERUPINIT-B, BYTE-IDENTICAL). FlameThrower.dff clones in four 0x68-byte records from 0x0068bb60, the
// part-code-1 atom's frame LTM copied in, 0x0045a3a0(record), an "exp_cloud2" particle pool per record.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 4 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004c0ed0 = MashedRw_FrameGetLTM_004c0ed0 (LibRw/RwApiBind.cpp)
//  - 0x0045a3a0 = PortTop_PWI_Call0045a3a0 -> Orig_0045a3a0 (near-match body below)
//  - 0x004892c0 = Orig_004892c0 (Match/PortTop_004892c0.cpp)
//  - 0x004893a0 = Orig_004893a0 (Match/PortTop_004893a0.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// 0x0045a3a0 (hooks.csv C2) runs its unedited near-match source re/match/s09/0045a3a0.c (best 81.0%, residual = register
// allocation: param_1 kept in EBP from entry vs reloaded into EAX after the loop), admitted per function by L-117 under
// L-115 (NEARMATCH-45A3A0; byte table verify/portorigtop_20261109/RESULT_L115-REGCHOICE-45A3A0.md). It zeroes fields
// 0..6, 8, 11, 12 of the 4 x 5 x 5 0x34-byte table at 0x0068bd00 and sets record +0x00 / +0x14 / +0x18 = 0, +0x0c = 1.0f.
// The s09 source takes int*, 0045a6b0.c declares Rec_0045a6b0*: PortTop_PWI_Call0045a3a0 forwards the pointer
// (two extern "C" prototypes of one name with different types do not compile in one C++ TU).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdlib>
#define MATCH_PORT 1
#define FUN_0045a6b0 Orig_0045a6b0
#define FUN_0040bb30 Search40bb30
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004c0ed0 MashedRw_FrameGetLTM_004c0ed0
#define FUN_004892c0 Orig_004892c0
#define FUN_004893a0 Orig_004893a0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#define FUN_0045a3a0 Orig_0045a3a0
#include "../../../../re/match/s09/0045a3a0.c"
#undef FUN_0045a3a0
#define FUN_0045a3a0 PortTop_PWI_Call0045a3a0
#include "../../../../re/match/s29/0045a6b0.c"
// 0x0045a3a0 call site @0x0045a7d9 inside 0x0045a6b0: the record pointer passed to the near-match body.
// Evidence only: MASHED_DBG_45A3A0_POISON=1 fills the ten cleared fields of all 100 table entries with 0xCDCDCDCD before
// the call (the table is zero-filled at boot, so the clear is otherwise invisible); the log line counts the nonzero
// cleared fields before and after. Default off, no effect on the default path.
static unsigned PortTop_PWI_CountNz0045a3a0(void) {
    const unsigned *t = reinterpret_cast<const unsigned *>(0x0068bd00u);
    static const int k_f[10] = {0, 1, 2, 3, 4, 5, 6, 8, 11, 12};
    unsigned nz = 0;
    for (int e = 0; e < 100; ++e)
        for (int f = 0; f < 10; ++f) nz += t[e * 13 + k_f[f]] != 0u;
    return nz;
}
void PortTop_PWI_Call0045a3a0(Rec_0045a6b0 *r) {
    static int s_n = 0;
    unsigned nz0 = 0;
    const bool poison = s_n < 4 && std::getenv("MASHED_DBG_45A3A0_POISON") != nullptr;
    if (poison) {
        unsigned *t = reinterpret_cast<unsigned *>(0x0068bd00u);
        static const int k_f[10] = {0, 1, 2, 3, 4, 5, 6, 8, 11, 12};
        for (int e = 0; e < 100; ++e)
            for (int f = 0; f < 10; ++f) t[e * 13 + k_f[f]] = 0xCDCDCDCDu;
    }
    if (s_n < 4) nz0 = PortTop_PWI_CountNz0045a3a0();
    Orig_0045a3a0(reinterpret_cast<int *>(r));
    if (s_n++ < 4) {
        if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
            const unsigned *w = reinterpret_cast<const unsigned *>(r);
            std::fprintf(fp, "[ORIG_TOP] rva 0x0045a3a0(record 0x%08X): near-match body ran (L-115/L-117): table 0x0068bd00 "
                             "cleared-field nonzero %u -> %u (poison=%d), +0x00=%08X +0x0c=%08X +0x14=%08X +0x18=%08X "
                             "(NEARMATCH-45A3A0)\n",
                         static_cast<unsigned>(reinterpret_cast<unsigned long>(r)), nz0, PortTop_PWI_CountNz0045a3a0(),
                         poison ? 1 : 0, w[0], w[3], w[5], w[6]);
            std::fclose(fp);
        }
    }
}
}  // extern "C"
#endif  // MASHED_STANDALONE
