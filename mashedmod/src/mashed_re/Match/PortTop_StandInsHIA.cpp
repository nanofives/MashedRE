// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0): named stand-ins in the Rt2d open chain. Each logs its first call and returns 0. MASHED_ORIG_TOP with MASHED_ORIG_TOP_RWMOD=1 only.
//  - 0x00555360 (re/match/s24/00555360.c, 94.4%) and 0x005c4ad0 (re/match/s28/005c4ad0.c, 96.7%): these two DO run.
//    Neither source is byte-identical, and each residual is outside the L-115 class (SIB base/index order), so the
//    lead's rule (L0, 2026-11-10) is a logging stand-in until L0 rules on the proofs in RESULT_HUDINIT-A.md. Each
//    returns 0, the result both sources give when their first allocation fails.
//  - 0x005c4c60 (re/match/s28/005c4c60.c, 93.0%, not run): called by 0x00553f40 only for a node with a non-zero count at
//    +0x4. The only nodes on this path come from 0x005540d0 with +0x4 = 0.
//  - 0x0055deb0 (no matched source): called by 0x00553f40 only for +0x4 == -1 inside a test that needs +0x4 != -1.
#ifdef MASHED_STANDALONE
#include <cstdio>

namespace {
void HiaStandInLog(const char* rva, const char* job, int* once) {
    if (*once) return;
    *once = 1;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: stand-in, not run (%s) (HUDINIT-A)\n", rva, job);
        std::fclose(f);
    }
}
}  // namespace

extern "C" {
int PortTop_StandInHIA_00555360() { static int once; HiaStandInLog("0x00555360", "94.4% source, residual outside L-115", &once); return 0; }
int PortTop_StandInHIA_005c4ad0() { static int once; HiaStandInLog("0x005c4ad0", "96.7% source, residual outside L-115", &once); return 0; }
int PortTop_StandInHIA_005c4c60() { static int once; HiaStandInLog("0x005c4c60", "0x00553f40 node with a non-zero count", &once); return 0; }
int PortTop_StandInHIA_0055deb0() { static int once; HiaStandInLog("0x0055deb0", "0x00553f40 count -1 read", &once); return 0; }
}  // extern "C"
#endif  // MASHED_STANDALONE
