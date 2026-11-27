// BIND-GATES-ALLKNOBS (2026-11-21): named stops of the all-knobs gate roots (re/tools/bind/porttop_bind.py --profile gates).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.
// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it
// names the next body to bind. MASHED_OT_GT_STOPS=count: a stop logs its first call and returns 0 (eax), so the caller
// runs on past it with that callee's effects missing; stops declared float / double by a caller always exit.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#include <cstdio>
#include <cstdlib>
namespace {
int RlStopCount() {
    static int s = -1;
    if (s < 0) { const char* const v = std::getenv("MASHED_OT_GT_STOPS"); s = (v && v[0] == 'c') ? 1 : 0; }
    return s;
}
unsigned g_rlStopHits = 0;
void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: gate-root stop (BIND-GATES-ALLKNOBS), call %u, callers %s, return %p, %s\n", rva, n, callers, ret,
                     exits ? "exits" : "returns 0 (MASHED_OT_GT_STOPS=count)");
        std::fclose(f);
    }
}
} // namespace
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
unsigned PortTop_GT_StopHits(void) { return g_rlStopHits; }
// stop rva 00408b00: declared void by 0x00448220; no matched or admitted source
int PortTop_GTStop_00408b00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00408b00u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00408b00u, 0u, _ReturnAddress()); return 0; }
// stop rva 004095a0: declared undefined1 * by 0x00409710; outside the gate roots (RVA ownership, BIND-GATES-ALLKNOBS binds only its roots), source re/match/s01/004095a0.c
int PortTop_GTStop_004095a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004095a0u, "0x00409710", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004095a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00418860: declared void by 0x004103a0; outside the gate roots (RVA ownership, BIND-GATES-ALLKNOBS binds only its roots), source re/match/s03/h3_00418860.c
int PortTop_GTStop_00418860(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00418860u, "0x004103a0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00418860u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041ef60: declared void by 0x00448220; outside the gate roots (RVA ownership, BIND-GATES-ALLKNOBS binds only its roots), source re/match/s04/0041ef60.c
int PortTop_GTStop_0041ef60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041ef60u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041ef60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00434360: declared void by 0x00448220; no matched or admitted source
int PortTop_GTStop_00434360(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00434360u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00434360u, 0u, _ReturnAddress()); return 0; }
// stop rva 00441990: declared void by 0x00448220; outside the gate roots (RVA ownership, BIND-GATES-ALLKNOBS binds only its roots), source re/match/s08/00441990.c
int PortTop_GTStop_00441990(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00441990u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00441990u, 0u, _ReturnAddress()); return 0; }
// stop rva 00442e00: declared void by 0x00448220; no matched or admitted source
int PortTop_GTStop_00442e00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00442e00u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00442e00u, 0u, _ReturnAddress()); return 0; }
// stop rva 00446520: declared void by 0x00448220; outside the gate roots (RVA ownership, BIND-GATES-ALLKNOBS binds only its roots), source re/match/s08/00446520.c
int PortTop_GTStop_00446520(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00446520u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00446520u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046c7b0: declared int by 0x004103a0 0x00448220; no matched or admitted source
int PortTop_GTStop_0046c7b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046c7b0u, "0x004103a0 0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046c7b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046cbb0: declared void by 0x00448220; no matched or admitted source
int PortTop_GTStop_0046cbb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046cbb0u, "0x00448220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046cbb0u, 0u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
