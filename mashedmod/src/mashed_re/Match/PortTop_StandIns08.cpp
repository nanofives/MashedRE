// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08: named stand-ins for the 0x0042c960 subtree on the MASHED_ORIG_TOP path (exe only). Each logs its first
// call; the reason is in the comment and in verify/porttop08_20261112/RESULT_PORT-TOP-08.md. Not bodies of these RVAs.
#ifdef MASHED_STANDALONE
#include <cstdio>

extern "C" {
// rva 0042c510 (called by 0x0042c960): re/match/s07/0042c510.c is 84.2% and equiv.py says NOT-PROVEN (branch-condition,
// verify/porttop08_20261112/equiv_0042c510.txt), and it is not in PROVISIONAL.tsv, so it has no exe body.
unsigned g_PortTop08_0042c510Calls = 0;
void PortTop_StandIn08_0042c510(void) {
    if (g_PortTop08_0042c510Calls++ == 0) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x0042c510: stand-in (PORT-TOP-08) NOT-PROVEN source, not run\n");
            std::fclose(f);
        }
    }
}
}
#endif // MASHED_STANDALONE
