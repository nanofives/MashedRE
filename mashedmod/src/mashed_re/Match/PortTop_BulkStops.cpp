// RACE-BIND-C4-BULK (2026-11-21): named stops of the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.
// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it
// names the next body to bind. MASHED_OT_RL_STOPS=count: a stop logs its first call and returns 0 (eax), so the load
// runs on past it with that callee's effects missing; stops declared float / double by a caller always exit.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#include <cstdio>
#include <cstdlib>
namespace {
int BulkStopCount() {
    static int s = -1;
    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RL_STOPS"); s = (v && v[0] == 'c') ? 1 : 0; }
    return s;
}
unsigned g_bulkStopHits = 0;
void BulkStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-load stop (RACE-BIND-C4-BULK), call %u, callers %s, return %p, %s\n", rva, n, callers, ret,
                     exits ? "exits" : "returns 0 (MASHED_OT_RL_STOPS=count)");
        std::fclose(f);
    }
}
} // namespace
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
unsigned PortTop_Bulk_StopHits(void) { return g_bulkStopHits; }
// stop rva 00401da0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_00401da0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00401da0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00401da0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a8d0: declared void by 0x0040d110; cpp-unclean: C2065 'DAT_0063c630': identificador no declarado
int PortTop_BulkStop_0041a8d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0041a8d0u, "0x0040d110", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a8d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041f330: declared (none found) by ; file-local copy of this rva in the source of 0x00420230 (one body per RVA)
int PortTop_BulkStop_0041f330(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0041f330u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041f330u, 0u, _ReturnAddress()); return 0; }
// stop rva 00420420: declared void by 0x0040d110 0x0041fe10; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s05/00420420.c
int PortTop_BulkStop_00420420(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00420420u, "0x0040d110 0x0041fe10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00420420u, 0u, _ReturnAddress()); return 0; }
// stop rva 004222f0: declared int by 0x004223b0; no matched or admitted source
int PortTop_BulkStop_004222f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004222f0u, "0x004223b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004222f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004299d0: declared void by 0x00411d90; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s06/004299d0.c
int PortTop_BulkStop_004299d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004299d0u, "0x00411d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004299d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047f290: declared void by 0x004074a0; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s13/0047f290.c
int PortTop_BulkStop_0047f290(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0047f290u, "0x004074a0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047f290u, 0u, _ReturnAddress()); return 0; }
// stop rva 00482860: declared void by 0x00411750; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s13/00482860.c
int PortTop_BulkStop_00482860(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00482860u, "0x00411750", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00482860u, 0u, _ReturnAddress()); return 0; }
// stop rva 00482900: declared int by 0x00411d90; no matched or admitted source
int PortTop_BulkStop_00482900(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00482900u, "0x00411d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00482900u, 0u, _ReturnAddress()); return 0; }
// stop rva 00482930: declared int by 0x00411d90; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s13/00482930.c
int PortTop_BulkStop_00482930(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00482930u, "0x00411d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00482930u, 0u, _ReturnAddress()); return 0; }
// stop rva 00483a30: declared void by 0x00411750; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s13/00483a30.c
int PortTop_BulkStop_00483a30(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00483a30u, "0x00411750", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00483a30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00483d10: declared int by 0x00411d90; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s13/00483d10.c
int PortTop_BulkStop_00483d10(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00483d10u, "0x00411d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00483d10u, 0u, _ReturnAddress()); return 0; }
// stop rva 004957a0: declared int by 0x00496530 0x004972b0; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s16/004957a0.c
int PortTop_BulkStop_004957a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004957a0u, "0x00496530 0x004972b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004957a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004967e0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004967e0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004967e0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004967e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c790: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c790(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c790u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c790u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c800: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c800(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c800u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c800u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c810: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c810(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c810u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c810u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c840: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c840(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c840u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c840u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c8c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c8c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c8c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c8c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049c990: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049c990(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049c990u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049c990u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d0a0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d0a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d0a0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d0a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d0c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d0c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d0c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d0c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d1d0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d1d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d1d0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d1d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d240: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d240(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d240u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d240u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d270: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d270(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d270u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d270u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d320: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d320(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d320u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d320u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d490: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d490(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d490u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d490u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049d8f0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049d8f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049d8f0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049d8f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049e0d0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049e0d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049e0d0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049e0d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049e6c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049e6c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049e6c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049e6c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049e850: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049e850(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049e850u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049e850u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049e9d0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049e9d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049e9d0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049e9d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049eae0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049eae0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049eae0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049eae0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049fcc0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049fcc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049fcc0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049fcc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0049ffb0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0049ffb0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0049ffb0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0049ffb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a21a0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a21a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a21a0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a21a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a3907: declared int by 0x00472820; no matched or admitted source
int PortTop_BulkStop_004a3907(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a3907u, "0x00472820", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a3907u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4554: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a4554(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4554u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4554u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a45cf: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a45cf(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a45cfu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a45cfu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4b6e: declared void __cdecl by 0x004a787f; no matched or admitted source
int PortTop_BulkStop_004a4b6e(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4b6eu, "0x004a787f", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4b6eu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4da0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a4da0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4da0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4da0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4ea8: declared int by 0x004a504f; no matched or admitted source
int PortTop_BulkStop_004a4ea8(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4ea8u, "0x004a504f", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4ea8u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4fc1: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a4fc1(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4fc1u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4fc1u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a4ff4: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a4ff4(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a4ff4u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a4ff4u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5018: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5018(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5018u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5018u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a59bf: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a59bf(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a59bfu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a59bfu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5aaf: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5aaf(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5aafu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5aafu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5bee: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5bee(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5beeu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5beeu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5d92: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5d92(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5d92u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5d92u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5f4a: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5f4a(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5f4au, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5f4au, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5f9c: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5f9c(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5f9cu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5f9cu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a5fbf: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a5fbf(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a5fbfu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a5fbfu, 0u, _ReturnAddress()); return 0; }
// stop rva 004a6047: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a6047(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a6047u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a6047u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a7800: declared int __cdecl by 0x004a787f; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s17/004a7800.c
int PortTop_BulkStop_004a7800(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a7800u, "0x004a787f", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a7800u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a842d: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a842d(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a842du, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a842du, 0u, _ReturnAddress()); return 0; }
// stop rva 004a8828: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004a8828(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a8828u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a8828u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a9410: declared unsigned int by 0x004a504f; no matched or admitted source
int PortTop_BulkStop_004a9410(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004a9410u, "0x004a504f", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a9410u, 0u, _ReturnAddress()); return 0; }
// stop rva 004aac76: declared void * __cdecl by 0x004a467e; no matched or admitted source
int PortTop_BulkStop_004aac76(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004aac76u, "0x004a467e", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004aac76u, 0u, _ReturnAddress()); return 0; }
// stop rva 004aaf72: declared int __cdecl by 0x004a467e; no matched or admitted source
int PortTop_BulkStop_004aaf72(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004aaf72u, "0x004a467e", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004aaf72u, 0u, _ReturnAddress()); return 0; }
// stop rva 004aaf90: declared void * __cdecl by 0x004a467e; no matched or admitted source
int PortTop_BulkStop_004aaf90(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004aaf90u, "0x004a467e", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004aaf90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac3ab: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac3ab(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac3abu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac3abu, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac435: declared int by 0x004a504f; no matched or admitted source
int PortTop_BulkStop_004ac435(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac435u, "0x004a504f", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac435u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac777: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac777(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac777u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac777u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac7f0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac7f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac7f0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac7f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac953: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac953(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac953u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac953u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac985: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac985(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac985u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac985u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ac9d2: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ac9d2(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ac9d2u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ac9d2u, 0u, _ReturnAddress()); return 0; }
// stop rva 004aca44: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004aca44(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004aca44u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004aca44u, 0u, _ReturnAddress()); return 0; }
// stop rva 004aca78: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004aca78(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004aca78u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004aca78u, 0u, _ReturnAddress()); return 0; }
// stop rva 004acaf3: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004acaf3(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004acaf3u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004acaf3u, 0u, _ReturnAddress()); return 0; }
// stop rva 004acc4b: declared int by 0x005c31d0; no matched or admitted source
int PortTop_BulkStop_004acc4b(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004acc4bu, "0x005c31d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004acc4bu, 0u, _ReturnAddress()); return 0; }
// stop rva 004ad0b4: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ad0b4(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ad0b4u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ad0b4u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b044b: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b044b(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b044bu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b044bu, 0u, _ReturnAddress()); return 0; }
// stop rva 004b046c: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b046c(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b046cu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b046cu, 0u, _ReturnAddress()); return 0; }
// stop rva 004b04ca: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b04ca(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b04cau, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b04cau, 0u, _ReturnAddress()); return 0; }
// stop rva 004b04f8: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b04f8(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b04f8u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b04f8u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b0609: declared int by 0x005c31d0; no matched or admitted source
int PortTop_BulkStop_004b0609(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b0609u, "0x005c31d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b0609u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b67a0: declared void by 0x004b6590; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s18/004b67a0.c
int PortTop_BulkStop_004b67a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b67a0u, "0x004b6590", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b67a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7210: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7210(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7210u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7210u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7280: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7280(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7280u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7280u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7530: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7530(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7530u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7530u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b75d0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b75d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b75d0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b75d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7960: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7960(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7960u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7960u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b79a0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b79a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b79a0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b79a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7a20: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7a20(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7a20u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7a20u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7ca0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7ca0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7ca0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7ca0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7cc0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7cc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7cc0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7cc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7d20: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b7d20(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7d20u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7d20u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7ff0: declared int by 0x004b7fd0 0x004b8080; no matched or admitted source
int PortTop_BulkStop_004b7ff0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b7ff0u, "0x004b7fd0 0x004b8080", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7ff0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b82a0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b82a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b82a0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b82a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b8340: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b8340(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b8340u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b8340u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9540: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9540(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9540u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9540u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9630: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9630(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9630u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9630u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b97f0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b97f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b97f0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b97f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9850: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9850(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9850u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9850u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9890: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9890(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9890u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9890u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9b00: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9b00(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9b00u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9b00u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9bb0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9bb0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9bb0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9bb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b9eb0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004b9eb0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004b9eb0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b9eb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ba140: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ba140(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ba140u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ba140u, 0u, _ReturnAddress()); return 0; }
// stop rva 004ba9e0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004ba9e0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004ba9e0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004ba9e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bab00: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bab00(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bab00u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bab00u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bc440: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bc440(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bc440u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bc440u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bccc0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bccc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bccc0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bccc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bee20: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bee20(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bee20u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bee20u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bee60: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bee60(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bee60u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bee60u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf080: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf080(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf080u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf080u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf0b0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf0b0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf0b0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf0b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf0f0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf0f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf0f0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf0f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf110: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf110(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf110u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf110u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf180: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf180(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf180u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf180u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf200: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf200(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf200u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf200u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bf280: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bf280(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bf280u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bf280u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bfa40: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bfa40(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bfa40u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bfa40u, 0u, _ReturnAddress()); return 0; }
// stop rva 004bfa60: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004bfa60(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004bfa60u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004bfa60u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c06c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_004c06c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x004c06c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c06c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00515680: declared int by 0x00517da0; no matched or admitted source
int PortTop_BulkStop_00515680(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00515680u, "0x00517da0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00515680u, 0u, _ReturnAddress()); return 0; }
// stop rva 00515710: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_00515710(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00515710u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00515710u, 0u, _ReturnAddress()); return 0; }
// stop rva 00515740: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_00515740(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00515740u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00515740u, 0u, _ReturnAddress()); return 0; }
// stop rva 00515770: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_00515770(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00515770u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00515770u, 0u, _ReturnAddress()); return 0; }
// stop rva 005157c0: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_005157c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005157c0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005157c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005159d0: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_005159d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005159d0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005159d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00515fd0: declared void by 0x00523640; no matched or admitted source
int PortTop_BulkStop_00515fd0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00515fd0u, "0x00523640", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00515fd0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005163a0: declared void by 0x005238c0; no matched or admitted source
int PortTop_BulkStop_005163a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005163a0u, "0x005238c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005163a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00516cc0: declared void * by 0x005173f0; no matched or admitted source
int PortTop_BulkStop_00516cc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00516cc0u, "0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00516cc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00516d30: declared void by 0x00517940 0x00528810; no matched or admitted source
int PortTop_BulkStop_00516d30(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00516d30u, "0x00517940 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00516d30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00517250: declared void by 0x005173f0 0x00517940 0x00519270 0x00523640 0x005238c0 0x00523cd0 0x00528810; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00517250.c
int PortTop_BulkStop_00517250(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00517250u, "0x005173f0 0x00517940 0x00519270 0x00523640 0x005238c0 0x00523cd0 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00517250u, 0u, _ReturnAddress()); return 0; }
// stop rva 00517270: declared void by 0x00519270 0x0051ca10 0x005238c0 0x00523cd0; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00517270.c
int PortTop_BulkStop_00517270(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00517270u, "0x00519270 0x0051ca10 0x005238c0 0x00523cd0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00517270u, 0u, _ReturnAddress()); return 0; }
// stop rva 005172b0: declared void by 0x00523170 0x00523640 0x005238c0 0x00523cd0 0x00528810; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/005172b0.c
int PortTop_BulkStop_005172b0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005172b0u, "0x00523170 0x00523640 0x005238c0 0x00523cd0 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005172b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00517380: declared void by 0x00523170 0x00523640 0x005238c0 0x00523cd0 0x00528810; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00517380.c
int PortTop_BulkStop_00517380(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00517380u, "0x00523170 0x00523640 0x005238c0 0x00523cd0 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00517380u, 0u, _ReturnAddress()); return 0; }
// stop rva 005173d0: declared void by 0x005173f0; no matched or admitted source
int PortTop_BulkStop_005173d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005173d0u, "0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005173d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005175d0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005175d0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005175d0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005175d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00517e00: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_00517e00(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00517e00u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00517e00u, 0u, _ReturnAddress()); return 0; }
// stop rva 005183a0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005183a0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005183a0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005183a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005183c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005183c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005183c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005183c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005184b0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005184b0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005184b0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005184b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005184f0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005184f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005184f0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005184f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005196f0: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/005196f0.c
int PortTop_BulkStop_005196f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005196f0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005196f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00519820: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00519820.c
int PortTop_BulkStop_00519820(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00519820u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00519820u, 0u, _ReturnAddress()); return 0; }
// stop rva 005199f0: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/005199f0.c
int PortTop_BulkStop_005199f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005199f0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005199f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00519af0: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_00519af0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00519af0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00519af0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00519bc0: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00519bc0.c
int PortTop_BulkStop_00519bc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00519bc0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00519bc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00519e30: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_00519e30(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00519e30u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00519e30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00519f70: declared int by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00519f70.c
int PortTop_BulkStop_00519f70(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00519f70u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00519f70u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051a6c0: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/0051a6c0.c
int PortTop_BulkStop_0051a6c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051a6c0u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051a6c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051b750: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/0051b750.c
int PortTop_BulkStop_0051b750(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051b750u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051b750u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051bb40: declared void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/0051bb40.c
int PortTop_BulkStop_0051bb40(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051bb40u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051bb40u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051bd70: declared else / void by 0x00519270; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/0051bd70.c
int PortTop_BulkStop_0051bd70(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051bd70u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051bd70u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051c100: declared void by 0x00519270; no matched or admitted source
int PortTop_BulkStop_0051c100(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051c100u, "0x00519270", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051c100u, 0u, _ReturnAddress()); return 0; }
// stop rva 0051c9f0: declared void by 0x00517940 0x00523170 0x005232c0 0x00523640 0x005238c0 0x00523cd0 0x00528810; no matched or admitted source
int PortTop_BulkStop_0051c9f0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0051c9f0u, "0x00517940 0x00523170 0x005232c0 0x00523640 0x005238c0 0x00523cd0 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0051c9f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005207c0: declared void * by 0x00516da0 0x005173f0; no matched or admitted source
int PortTop_BulkStop_005207c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005207c0u, "0x00516da0 0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005207c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00520850: declared void by 0x005173f0; no matched or admitted source
int PortTop_BulkStop_00520850(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00520850u, "0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00520850u, 0u, _ReturnAddress()); return 0; }
// stop rva 00520870: declared void by 0x00518080; no matched or admitted source
int PortTop_BulkStop_00520870(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00520870u, "0x00518080", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00520870u, 0u, _ReturnAddress()); return 0; }
// stop rva 005208c0: declared void * by 0x005173f0; no matched or admitted source
int PortTop_BulkStop_005208c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005208c0u, "0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005208c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00520930: declared void by 0x00516d20 0x00516de0 0x005171a0 0x005173f0 0x00518140; no matched or admitted source
int PortTop_BulkStop_00520930(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00520930u, "0x00516d20 0x00516de0 0x005171a0 0x005173f0 0x00518140", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00520930u, 0u, _ReturnAddress()); return 0; }
// stop rva 00520960: declared void by 0x00517940; no matched or admitted source
int PortTop_BulkStop_00520960(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00520960u, "0x00517940", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00520960u, 0u, _ReturnAddress()); return 0; }
// stop rva 00520990: declared void * by 0x00528810; no matched or admitted source
int PortTop_BulkStop_00520990(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00520990u, "0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00520990u, 0u, _ReturnAddress()); return 0; }
// stop rva 00522ab0: declared int by 0x00528810; no matched or admitted source
int PortTop_BulkStop_00522ab0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00522ab0u, "0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00522ab0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00522b00: declared int by 0x00518140; no matched or admitted source
int PortTop_BulkStop_00522b00(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00522b00u, "0x00518140", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00522b00u, 0u, _ReturnAddress()); return 0; }
// stop rva 00522cc0: declared int by 0x005173f0; no matched or admitted source
int PortTop_BulkStop_00522cc0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00522cc0u, "0x005173f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00522cc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00522ce0: declared int by 0x00517940 0x00528810; no matched or admitted source
int PortTop_BulkStop_00522ce0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00522ce0u, "0x00517940 0x00528810", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00522ce0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00523110: declared png_uint_32 / unsigned int by 0x00517940 0x005238c0; no matched or admitted source
int PortTop_BulkStop_00523110(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00523110u, "0x00517940 0x005238c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00523110u, 0u, _ReturnAddress()); return 0; }
// stop rva 00523140: declared void by 0x00517940; no matched or admitted source
int PortTop_BulkStop_00523140(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00523140u, "0x00517940", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00523140u, 0u, _ReturnAddress()); return 0; }
// stop rva 00528200: declared void by 0x00517940; no matched or admitted source
int PortTop_BulkStop_00528200(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00528200u, "0x00517940", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00528200u, 0u, _ReturnAddress()); return 0; }
// stop rva 00528630: declared void by 0x00517940; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00528630.c
int PortTop_BulkStop_00528630(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00528630u, "0x00517940", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00528630u, 0u, _ReturnAddress()); return 0; }
// stop rva 00528ba0: declared void by 0x00517940; no matched or admitted source
int PortTop_BulkStop_00528ba0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00528ba0u, "0x00517940", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00528ba0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00528e00: declared voidpf by 0x00522b50; no matched or admitted source
int PortTop_BulkStop_00528e00(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00528e00u, "0x00522b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00528e00u, 0u, _ReturnAddress()); return 0; }
// stop rva 00528e20: declared void by 0x00522b50; outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s23/00528e20.c
int PortTop_BulkStop_00528e20(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00528e20u, "0x00522b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00528e20u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052a890: declared uLong by 0x00522b50; no matched or admitted source
int PortTop_BulkStop_0052a890(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052a890u, "0x00522b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052a890u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052a9c0: declared void by 0x00522b50; no matched or admitted source
int PortTop_BulkStop_0052a9c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052a9c0u, "0x00522b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052a9c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052aa40: declared inflate_blocks_statef * by 0x00522b50; no matched or admitted source
int PortTop_BulkStop_0052aa40(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052aa40u, "0x00522b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052aa40u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052b7e0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0052b7e0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052b7e0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052b7e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052bf20: declared void by 0x0052b750; no matched or admitted source
int PortTop_BulkStop_0052bf20(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052bf20u, "0x0052b750", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052bf20u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052c660: declared int by 0x0052b820; no matched or admitted source
int PortTop_BulkStop_0052c660(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0052c660u, "0x0052b820", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052c660u, 0u, _ReturnAddress()); return 0; }
// stop rva 00535700: declared int / return by 0x00487e20; no matched or admitted source
int PortTop_BulkStop_00535700(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00535700u, "0x00487e20", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00535700u, 0u, _ReturnAddress()); return 0; }
// stop rva 0053d460: declared int by 0x0053d400; no matched or admitted source
int PortTop_BulkStop_0053d460(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0053d460u, "0x0053d400", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0053d460u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055b940: declared float by 0x004218b0; no matched or admitted source
int PortTop_BulkStop_0055b940(void) { BulkStopLog(0x0055b940u, "0x004218b0", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x0055b940u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055dff0: declared void by 0x00421a90; no matched or admitted source
int PortTop_BulkStop_0055dff0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0055dff0u, "0x00421a90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055dff0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00563e70: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_00563e70(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00563e70u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00563e70u, 0u, _ReturnAddress()); return 0; }
// stop rva 00563f60: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_00563f60(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00563f60u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00563f60u, 0u, _ReturnAddress()); return 0; }
// stop rva 005641b0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005641b0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005641b0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005641b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005646c0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005646c0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005646c0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005646c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00565120: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_00565120(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x00565120u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00565120u, 0u, _ReturnAddress()); return 0; }
// stop rva 005651b0: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005651b0(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005651b0u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005651b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0057c210: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_0057c210(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x0057c210u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); return 0; }
// stop rva 005c1f1c: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005c1f1c(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005c1f1cu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005c1f1cu, 0u, _ReturnAddress()); return 0; }
// stop rva 005c229b: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005c229b(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005c229bu, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005c229bu, 0u, _ReturnAddress()); return 0; }
// stop rva 005c2470: declared (none found) by ; no matched or admitted source
int PortTop_BulkStop_005c2470(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();
    if (n == 1 || ex) BulkStopLog(0x005c2470u, "", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005c2470u, 0u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
