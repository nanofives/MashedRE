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
//  - 0x0045a3a0 = PortTop_PWI_StandIn_0045a3a0 (stand-in below)
//  - 0x004892c0 = Orig_004892c0 (Match/PortTop_004892c0.cpp)
//  - 0x004893a0 = Orig_004893a0 (Match/PortTop_004893a0.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// 0x0045a3a0 (hooks.csv C2, re/match/s09/0045a3a0.c) has no byte match: best 81.0%, residual = register allocation
// (param_1 in EBP from entry vs reloaded into EAX), not a SIB base/index order, so L-115 does not admit it here. It is a
// logging stand-in. Left undone per call: zeroing fields 0..6, 8, 11, 12 of the 4 x 5 x 5 0x34-byte table at 0x0068bd00
// (pad zero-filled; no exe code writes 0x0068bd00..0x0068d150), and record +0x00 / +0x14 / +0x18 = 0 (pad zero), +0x0c =
// 1.0f (stays 0). The only reader of either on this path would be the per-type update 0x0045bba0, which is uncalled.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_0045a6b0 Orig_0045a6b0
#define FUN_0040bb30 Search40bb30
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004c0ed0 MashedRw_FrameGetLTM_004c0ed0
#define FUN_0045a3a0 PortTop_PWI_StandIn_0045a3a0
#define FUN_004892c0 Orig_004892c0
#define FUN_004893a0 Orig_004893a0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/0045a6b0.c"
// Stand-in for 0x0045a3a0 (reason in the header): logs and returns.
void PortTop_PWI_StandIn_0045a3a0(Rec_0045a6b0 *r) {
    static int s_n = 0;
    if (s_n++ < 4) {
        if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
            std::fprintf(fp, "[ORIG_TOP] rva 0x0045a3a0(record 0x%08X): stand-in, not run: no byte match (81.0 percent, "
                             "register-allocation residual, not L-115) (POWERUPINIT-C)\n",
                         static_cast<unsigned>(reinterpret_cast<unsigned long>(r)));
            std::fclose(fp);
        }
    }
}
}  // extern "C"
#endif  // MASHED_STANDALONE
