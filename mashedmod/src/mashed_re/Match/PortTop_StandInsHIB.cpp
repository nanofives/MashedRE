// HUDINIT-B (L-111 step 1, chain 2 of 0x00427ca0): named stand-ins for two callees of the matched 0x00427ca0 / 0x00556d70.
// Each logs its first call and returns 0. MASHED_ORIG_TOP with MASHED_ORIG_TOP_RWMOD=1 only.
//  - 0x00556e90 (re/match/s24/00556e90.c, 97.5%): the brush colour setter. It runs (from 0x00556d70 and four times from
//    0x00427ca0). Not byte-identical: 35 differing bytes, all integer moves scheduled across x87 instructions
//    (0x00556f47, 0x00556f90..0x00556f9d, 0x0055700a, 0x0055701c..0x00557025). That is outside the L-115 class (SIB
//    base/index order), so the lead's rule (L0, 2026-11-10) is a logging stand-in. What it cuts: the brushes' RGBAReal
//    colours (+0x00 / +0x18 / +0x30 / +0x48), the raw RGBA8 copy at +0x60 and +0x64 bit 0 stay as 0x00556d70 left them
//    (zero).
//  - 0x00427880 (re/match/s06/00427880.c, 100%): the font-file loader. Kept a stand-in in this row (lead scope): its
//    callee 0x00554390 is chain C (33.5%). DAT_0067d838 stays 0, so 0x00555830 still gets 0 at AppDestroy.
#ifdef MASHED_STANDALONE
#include <cstdio>

namespace {
void HibStandInLog(const char* rva, const char* job, int* once) {
    if (*once) return;
    *once = 1;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: stand-in, not run (%s) (HUDINIT-B)\n", rva, job);
        std::fclose(f);
    }
}
}  // namespace

extern "C" {
int PortTop_StandInHIB_00556e90() { static int once; HibStandInLog("0x00556e90", "97.5% source, residual outside L-115", &once); return 0; }
int PortTop_StandInHIB_00427880() { static int once; HibStandInLog("0x00427880", "font-file loader, chain C", &once); return 0; }
}  // extern "C"
#endif  // MASHED_STANDALONE
