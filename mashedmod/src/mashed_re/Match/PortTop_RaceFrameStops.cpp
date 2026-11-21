// PORT-TOP-06 (2026-11-21): named stops of the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.
// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it
// names the next body to bind. MASHED_OT_RF_STOPS=count: a stop logs its first call and returns 0 (eax), so the frame
// runs on past it with that callee's effects missing; stops declared float / double by a caller always exit.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#include <cstdio>
#include <cstdlib>
namespace {
int RlStopCount() {
    static int s = -1;
    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RF_STOPS"); s = (v && v[0] == 'c') ? 1 : 0; }
    return s;
}
unsigned g_rlStopHits = 0;
void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-frame stop (PORT-TOP-06), call %u, callers %s, return %p, %s\n", rva, n, callers, ret,
                     exits ? "exits" : "returns 0 (MASHED_OT_RF_STOPS=count)");
        std::fclose(f);
    }
}
} // namespace
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
unsigned PortTop_RF_StopHits(void) { return g_rlStopHits; }
// stop rva 004019d0: declared void by 0x0040dba0; no matched or admitted source
int PortTop_RFStop_004019d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004019d0u, "0x0040dba0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004019d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00401da0: declared void by 0x00401570; no matched or admitted source
int PortTop_RFStop_00401da0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00401da0u, "0x00401570", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00401da0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004039f0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_004039f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004039f0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004039f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004046a0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_004046a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004046a0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004046a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00404fa0: declared void by 0x00405460; no matched or admitted source
int PortTop_RFStop_00404fa0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00404fa0u, "0x00405460", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00404fa0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00405430: declared int by 0x0040d470; no matched or admitted source
int PortTop_RFStop_00405430(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00405430u, "0x0040d470", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00405430u, 0u, _ReturnAddress()); return 0; }
// stop rva 00406370: declared void by 0x004073b0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_00406370(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00406370u, "0x004073b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00406370u, 0u, _ReturnAddress()); return 0; }
// stop rva 00406ce0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_00406ce0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00406ce0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00406ce0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00407a40: declared int by 0x004252c0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_00407a40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00407a40u, "0x004252c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00407a40u, 0u, _ReturnAddress()); return 0; }
// stop rva 00407a90: declared int by 0x00408610; no matched or admitted source
int PortTop_RFStop_00407a90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00407a90u, "0x00408610", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00407a90u, 0u, _ReturnAddress()); return 0; }
// stop rva 00407e20: declared void by 0x00408610; no matched or admitted source
int PortTop_RFStop_00407e20(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00407e20u, "0x00408610", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00407e20u, 0u, _ReturnAddress()); return 0; }
// stop rva 00408b00: declared void by 0x00423670; no matched or admitted source
int PortTop_RFStop_00408b00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00408b00u, "0x00423670", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00408b00u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040aef0: declared void by 0x0040b090; no matched or admitted source
int PortTop_RFStop_0040aef0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040aef0u, "0x0040b090", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040aef0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040dd80: declared static void / void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0040dd80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040dd80u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040dd80u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040ddb0: declared void by 0x004111c0; cpp-unclean: C2660 'PortTop_Bound_00426c50': la funciÂ¢n no acepta 1 argumentos
int PortTop_RFStop_0040ddb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040ddb0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040ddb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040ddf0: declared static void / void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0040ddf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040ddf0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040ddf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040e180: declared void by 0x00410d10 0x00471ac0; no matched or admitted source
int PortTop_RFStop_0040e180(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040e180u, "0x00410d10 0x00471ac0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040e180u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040e590: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0040e590(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040e590u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040e590u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040eee0: declared int by 0x00410d10; no matched or admitted source
int PortTop_RFStop_0040eee0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040eee0u, "0x00410d10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040eee0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040feb0: declared static void / void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0040feb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040feb0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040feb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00410130: declared static void / void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_00410130(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00410130u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00410130u, 0u, _ReturnAddress()); return 0; }
// stop rva 004103a0: declared static void / void by 0x004111c0; static in re/match/s02/004103a0.c:59 (file-local there): no linkable exe body
int PortTop_RFStop_004103a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004103a0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004103a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00410ab0: declared static void / void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_00410ab0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00410ab0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00410ab0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00411580: declared undefined4 / void by 0x00429310 0x004298e0; cpp-unclean: C2664 'void PortTop_Bound_00411530(int,int,undefined4 *,undefined4 *,undefined4)': el argumento 3 no puede convertir
int PortTop_RFStop_00411580(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00411580u, "0x00429310 0x004298e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00411580u, 0u, _ReturnAddress()); return 0; }
// stop rva 004117b0: declared void by 0x0040de00; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RFStop_004117b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004117b0u, "0x0040de00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004117b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00411870: declared void by 0x0040e560 0x00429310; no matched or admitted source
int PortTop_RFStop_00411870(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00411870u, "0x0040e560 0x00429310", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00411870u, 0u, _ReturnAddress()); return 0; }
// stop rva 00412190: declared (none found) by 0x0040bd80; no matched or admitted source
int PortTop_RFStop_00412190(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00412190u, "0x0040bd80", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00412190u, 0u, _ReturnAddress()); return 0; }
// stop rva 00412620: declared void by 0x0040ce00; no matched or admitted source
int PortTop_RFStop_00412620(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00412620u, "0x0040ce00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00412620u, 0u, _ReturnAddress()); return 0; }
// stop rva 00412f30: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_00412f30(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00412f30u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00412f30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00413cb0: declared void by 0x0040ce00; no matched or admitted source
int PortTop_RFStop_00413cb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00413cb0u, "0x0040ce00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00413cb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00413f20: declared void by 0x0040bd80 0x004111c0; cpp-unclean: C2065 'DAT_0063bc60': identificador no declarado
int PortTop_RFStop_00413f20(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00413f20u, "0x0040bd80 0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00413f20u, 0u, _ReturnAddress()); return 0; }
// stop rva 00414060: declared void by 0x004111c0; cpp-unclean: C2065 'DAT_0089a384': identificador no declarado
int PortTop_RFStop_00414060(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00414060u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00414060u, 0u, _ReturnAddress()); return 0; }
// stop rva 00414180: declared void by 0x004111c0; cpp-unclean: C2065 'DAT_0089a368': identificador no declarado
int PortTop_RFStop_00414180(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00414180u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00414180u, 0u, _ReturnAddress()); return 0; }
// stop rva 00414220: declared void by 0x004111c0; cpp-unclean: C3861 'PortTop_Bound_0040e4b0': no se encontrÂ¢ el identificador
int PortTop_RFStop_00414220(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00414220u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00414220u, 0u, _ReturnAddress()); return 0; }
// stop rva 00415860: declared void by 0x004252c0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_00415860(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00415860u, "0x004252c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00415860u, 0u, _ReturnAddress()); return 0; }
// stop rva 00417370: declared void by 0x00422ba0; cpp-unclean: C2065 'DAT_0089a4fc': identificador no declarado
int PortTop_RFStop_00417370(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00417370u, "0x00422ba0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00417370u, 0u, _ReturnAddress()); return 0; }
// stop rva 00417450: declared void by 0x00423040; cpp-unclean: C2065 'DAT_007f1a9c': identificador no declarado
int PortTop_RFStop_00417450(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00417450u, "0x00423040", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00417450u, 0u, _ReturnAddress()); return 0; }
// stop rva 00417530: declared void by 0x00423040; cpp-unclean: C2065 'DAT_007f1a9c': identificador no declarado
int PortTop_RFStop_00417530(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00417530u, "0x00423040", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00417530u, 0u, _ReturnAddress()); return 0; }
// stop rva 00418860: declared void by 0x004111c0 0x00425a40; cpp-unclean: C2065 'DAT_00801ca0': identificador no declarado
int PortTop_RFStop_00418860(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00418860u, "0x004111c0 0x00425a40", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00418860u, 0u, _ReturnAddress()); return 0; }
// stop rva 00418990: declared void by 0x004111c0; cpp-unclean: C3861 'PortTop_Bound_004194f0': no se encontrÂ¢ el identificador
int PortTop_RFStop_00418990(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00418990u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00418990u, 0u, _ReturnAddress()); return 0; }
// stop rva 004189a0: declared void by 0x004111c0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_004189a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004189a0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004189a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004194f0: declared (none found) by 0x00411170; cpp-unclean: C3861 'Orig_004b6520': no se encontrÂ¢ el identificador
int PortTop_RFStop_004194f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004194f0u, "0x00411170", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004194f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00419760: declared void by 0x004111c0 0x00422fd0; cpp-unclean: C3861 'Orig_004b6520': no se encontrÂ¢ el identificador
int PortTop_RFStop_00419760(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00419760u, "0x004111c0 0x00422fd0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00419760u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a250: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0041a250(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a250u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a250u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a9b0: declared void by 0x00411ae0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_0041a9b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a9b0u, "0x00411ae0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a9b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041ad00: declared void by 0x00411ae0; cpp-unclean: C2065 'DAT_0063c630': identificador no declarado
int PortTop_RFStop_0041ad00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041ad00u, "0x00411ae0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041ad00u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041b4d0: declared void by 0x004111c0; cpp-unclean: C3861 'Orig_004b3fc0': no se encontrÂ¢ el identificador
int PortTop_RFStop_0041b4d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041b4d0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041b4d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041b510: declared void by 0x004111c0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RFStop_0041b510(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041b510u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041b510u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041b540: declared void by 0x004111c0; link: re/match/s04/0041b540.c calls FUN_0041af50_body (:48), which has no exe body (LNK2019)
int PortTop_RFStop_0041b540(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041b540u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041b540u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041c180: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0041c180(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041c180u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041c180u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041dca0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0041dca0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041dca0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041dca0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041e6c0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0041e6c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041e6c0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041e6c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00420870: declared void by 0x0040da50; no matched or admitted source
int PortTop_RFStop_00420870(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00420870u, "0x0040da50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00420870u, 0u, _ReturnAddress()); return 0; }
// stop rva 00421980: declared void by 0x00421c10; static in re/match/s05/004219f0.c:38 (file-local there): no linkable exe body
int PortTop_RFStop_00421980(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00421980u, "0x00421c10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00421980u, 0u, _ReturnAddress()); return 0; }
// stop rva 00423540: declared int by 0x00423670; cpp-unclean: C2664 'undefined4 *RwCore_004cc230(undefined4,int,undefined4 *)': el argumento 3 no puede convertirse de 'undefined1
int PortTop_RFStop_00423540(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00423540u, "0x00423670", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00423540u, 0u, _ReturnAddress()); return 0; }
// stop rva 00424b80: declared void by 0x00410860; no matched or admitted source
int PortTop_RFStop_00424b80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00424b80u, "0x00410860", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00424b80u, 0u, _ReturnAddress()); return 0; }
// stop rva 00424eb0: declared undefined4 by 0x00425a40; no matched or admitted source
int PortTop_RFStop_00424eb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00424eb0u, "0x00425a40", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00424eb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00426810: declared void by 0x00426ab0; no matched or admitted source
int PortTop_RFStop_00426810(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00426810u, "0x00426ab0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00426810u, 0u, _ReturnAddress()); return 0; }
// stop rva 00430290: declared void by 0x00411170; no matched or admitted source
int PortTop_RFStop_00430290(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00430290u, "0x00411170", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00430290u, 0u, _ReturnAddress()); return 0; }
// stop rva 004307a0: declared undefined4 by 0x0043aee0; no matched or admitted source
int PortTop_RFStop_004307a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004307a0u, "0x0043aee0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004307a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004414b0: declared void by 0x0040d470; no matched or admitted source
int PortTop_RFStop_004414b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004414b0u, "0x0040d470", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004414b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004464c0: declared void by 0x00448700; no matched or admitted source
int PortTop_RFStop_004464c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004464c0u, "0x00448700", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004464c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00448220: declared void by 0x0040d470; no matched or admitted source
int PortTop_RFStop_00448220(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00448220u, "0x0040d470", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00448220u, 0u, _ReturnAddress()); return 0; }
// stop rva 00452140: declared int / void by 0x0045c350 0x00486830; cpp-unclean: C2664 'undefined4 PortTop_Bound_00451990(int,undefined4 *,undefined4 *)': el argumento 1 no puede convertirse de 'un
int PortTop_RFStop_00452140(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00452140u, "0x0045c350 0x00486830", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00452140u, 0u, _ReturnAddress()); return 0; }
// stop rva 0045b9d0: declared void by 0x004111c0; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'void **' a 'undefined **'
int PortTop_RFStop_0045b9d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0045b9d0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0045b9d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0045be90: declared void by 0x004111c0; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'void **' a 'undefined **'
int PortTop_RFStop_0045be90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0045be90u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0045be90u, 0u, _ReturnAddress()); return 0; }
// stop rva 0045bed0: declared void by 0x00403250 0x00410510 0x004111c0; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'void **' a 'undefined **'
int PortTop_RFStop_0045bed0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0045bed0u, "0x00403250 0x00410510 0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0045bed0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046b1c0: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0046b1c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046b1c0u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046b1c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046b540: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_0046b540(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046b540u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046b540u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046c7b0: declared int by 0x0040b9a0 0x0040da50 0x00410510 0x00410860 0x00410d10 0x004111c0 0x004252c0; no matched or admitted source
int PortTop_RFStop_0046c7b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046c7b0u, "0x0040b9a0 0x0040da50 0x00410510 0x00410860 0x00410d10 0x004111c0 0x004252c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046c7b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046cbb0: declared int / void by 0x00408610 0x00410510 0x00410d10 0x00422ba0 0x004252c0; no matched or admitted source
int PortTop_RFStop_0046cbb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046cbb0u, "0x00408610 0x00410510 0x00410d10 0x00422ba0 0x004252c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046cbb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046d3a0: declared void * by 0x0046d400; no matched or admitted source
int PortTop_RFStop_0046d3a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046d3a0u, "0x0046d400", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046d3a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0046f6c0: declared static void / void by 0x004704c0; no matched or admitted source
int PortTop_RFStop_0046f6c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0046f6c0u, "0x004704c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0046f6c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00470c70: declared undefined4 by 0x00425a40; no matched or admitted source
int PortTop_RFStop_00470c70(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00470c70u, "0x00425a40", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00470c70u, 0u, _ReturnAddress()); return 0; }
// stop rva 00471ec0: declared void by 0x00426ab0; no matched or admitted source
int PortTop_RFStop_00471ec0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00471ec0u, "0x00426ab0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00471ec0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00474f50: declared void * by 0x00475010; no matched or admitted source
int PortTop_RFStop_00474f50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00474f50u, "0x00475010", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00474f50u, 0u, _ReturnAddress()); return 0; }
// stop rva 004752f0: declared void by 0x00425d50 0x004756e0; no matched or admitted source
int PortTop_RFStop_004752f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004752f0u, "0x00425d50 0x004756e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004752f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00475f90: declared void by 0x00476440; no matched or admitted source
int PortTop_RFStop_00475f90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00475f90u, "0x00476440", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00475f90u, 0u, _ReturnAddress()); return 0; }
// stop rva 00476880: declared void by 0x00421d20; cpp-unclean: C2660 'PortTop_Bound_00476710': la funciÂ¢n no acepta 5 argumentos
int PortTop_RFStop_00476880(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00476880u, "0x00421d20", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00476880u, 0u, _ReturnAddress()); return 0; }
// stop rva 00477be0: declared int by 0x0040d8f0; no matched or admitted source
int PortTop_RFStop_00477be0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00477be0u, "0x0040d8f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00477be0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047bb10: declared void by 0x0047c160; no matched or admitted source
int PortTop_RFStop_0047bb10(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047bb10u, "0x0047c160", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047bb10u, 0u, _ReturnAddress()); return 0; }
// stop rva 00480d60: declared void by 0x00422ba0; no matched or admitted source
int PortTop_RFStop_00480d60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00480d60u, "0x00422ba0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00480d60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00481a30: declared void by 0x0040ce00; no matched or admitted source
int PortTop_RFStop_00481a30(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00481a30u, "0x0040ce00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00481a30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00482c10: declared void by 0x00411ae0; no matched or admitted source
int PortTop_RFStop_00482c10(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00482c10u, "0x00411ae0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00482c10u, 0u, _ReturnAddress()); return 0; }
// stop rva 004847d0: declared void by 0x00484cf0; no matched or admitted source
int PortTop_RFStop_004847d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004847d0u, "0x00484cf0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004847d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00484ac0: declared int by 0x00484cf0; no matched or admitted source
int PortTop_RFStop_00484ac0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00484ac0u, "0x00484cf0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00484ac0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00486630: declared void by 0x00486830; no matched or admitted source
int PortTop_RFStop_00486630(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00486630u, "0x00486830", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00486630u, 0u, _ReturnAddress()); return 0; }
// stop rva 00487df0: declared void by 0x0040bd80 0x0040ce00; no matched or admitted source
int PortTop_RFStop_00487df0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00487df0u, "0x0040bd80 0x0040ce00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00487df0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00488e70: declared void by 0x004111c0; no matched or admitted source
int PortTop_RFStop_00488e70(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00488e70u, "0x004111c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00488e70u, 0u, _ReturnAddress()); return 0; }
// stop rva 00489500: declared void by 0x00489890; no matched or admitted source
int PortTop_RFStop_00489500(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00489500u, "0x00489890", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00489500u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048a780: declared void by 0x0040ce00; no matched or admitted source
int PortTop_RFStop_0048a780(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048a780u, "0x0040ce00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048a780u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048d140: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048d140(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048d140u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048d140u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048d720: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048d720(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048d720u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048d720u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048d900: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048d900(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048d900u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048d900u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048db20: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048db20(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048db20u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048db20u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048dd40: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048dd40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048dd40u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048dd40u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048df60: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048df60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048df60u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048df60u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048e180: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048e180(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048e180u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048e180u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048e360: declared void by 0x0048e5f0; no matched or admitted source
int PortTop_RFStop_0048e360(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048e360u, "0x0048e5f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048e360u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048ebf0: declared void by 0x00490500; no matched or admitted source
int PortTop_RFStop_0048ebf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048ebf0u, "0x00490500", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048ebf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048fe30: declared void * by 0x0048fe70; no matched or admitted source
int PortTop_RFStop_0048fe30(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048fe30u, "0x0048fe70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048fe30u, 0u, _ReturnAddress()); return 0; }
// stop rva 00490020: declared void by 0x004215a0; no matched or admitted source
int PortTop_RFStop_00490020(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00490020u, "0x004215a0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00490020u, 0u, _ReturnAddress()); return 0; }
// stop rva 004910c0: declared void by 0x00491490; no matched or admitted source
int PortTop_RFStop_004910c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004910c0u, "0x00491490", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004910c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00491340: declared void by 0x00491490; no matched or admitted source
int PortTop_RFStop_00491340(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00491340u, "0x00491490", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00491340u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a3384: declared double by 0x004252c0 0x0046d570; no matched or admitted source
int PortTop_RFStop_004a3384(void) { RlStopLog(0x004a3384u, "0x004252c0 0x0046d570", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x004a3384u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b4550: declared void by 0x0044e0a0; no matched or admitted source
int PortTop_RFStop_004b4550(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b4550u, "0x0044e0a0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b4550u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b49b0: declared void * by 0x004b4a80; no matched or admitted source
int PortTop_RFStop_004b49b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b49b0u, "0x004b4a80", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b49b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b5080: declared void by 0x00487020; no matched or admitted source
int PortTop_RFStop_004b5080(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b5080u, "0x00487020", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b5080u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c3d90: declared void by 0x0040c690; owned by RACE rows (run 53, race_owned.txt)
int PortTop_RFStop_004c3d90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c3d90u, "0x0040c690", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c3d90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c52f0: declared int by 0x0047d180; no matched or admitted source
int PortTop_RFStop_004c52f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c52f0u, "0x0047d180", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c52f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004e6680: declared void by 0x00401570; bound by RACE-BIND-C4-BULK cp1 (orch/RACE-BIND-C4-BULK @ d4fda5c93, unmerged): listed, not rebound
int PortTop_RFStop_004e6680(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004e6680u, "0x00401570", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004e6680u, 0u, _ReturnAddress()); return 0; }
// stop rva 004e89a0: declared int by 0x004b40f0; owned by RACE rows (run 53, race_owned.txt)
int PortTop_RFStop_004e89a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004e89a0u, "0x004b40f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004e89a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00532b80: declared void by 0x004b47e0; no matched or admitted source
int PortTop_RFStop_00532b80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00532b80u, "0x004b47e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00532b80u, 0u, _ReturnAddress()); return 0; }
// stop rva 00535700: declared int by 0x00476df0; no matched or admitted source
int PortTop_RFStop_00535700(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00535700u, "0x00476df0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00535700u, 0u, _ReturnAddress()); return 0; }
// stop rva 00538c80: declared int / void / void * by 0x00408610 0x0046d400 0x0048fe70 0x004b4a80; owned by RACE rows (run 53, race_owned.txt)
int PortTop_RFStop_00538c80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00538c80u, "0x00408610 0x0046d400 0x0048fe70 0x004b4a80", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00538c80u, 0u, _ReturnAddress()); return 0; }
// stop rva 00544bf0: declared int by 0x0048b440 0x0048b960; no matched or admitted source
int PortTop_RFStop_00544bf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00544bf0u, "0x0048b440 0x0048b960", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00544bf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00546b10: declared int by 0x004829d0; owned by RACE rows (run 53, race_owned.txt)
int PortTop_RFStop_00546b10(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00546b10u, "0x004829d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00546b10u, 0u, _ReturnAddress()); return 0; }
// stop rva 00547230: declared int by 0x0044d5e0; no matched or admitted source
int PortTop_RFStop_00547230(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00547230u, "0x0044d5e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00547230u, 0u, _ReturnAddress()); return 0; }
// stop rva 00557b70: declared int by 0x00558100; no matched or admitted source
int PortTop_RFStop_00557b70(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00557b70u, "0x00558100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00557b70u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559c40: declared void by 0x00421d20; no matched or admitted source
int PortTop_RFStop_00559c40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00559c40u, "0x00421d20", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00559c40u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559ee0: declared void by 0x004219f0 0x004222d0; no matched or admitted source
int PortTop_RFStop_00559ee0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00559ee0u, "0x004219f0 0x004222d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00559ee0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055ab30: declared int by 0x0047d180; no matched or admitted source
int PortTop_RFStop_0055ab30(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055ab30u, "0x0047d180", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055ab30u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055ac00: declared void by 0x00421750 0x0047cea0 0x0047d180 0x0047ea60 0x00480720; no matched or admitted source
int PortTop_RFStop_0055ac00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055ac00u, "0x00421750 0x0047cea0 0x0047d180 0x0047ea60 0x00480720", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055ac00u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055b650: declared void by 0x00421750 0x0047cea0 0x00481750; no matched or admitted source
int PortTop_RFStop_0055b650(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055b650u, "0x00421750 0x0047cea0 0x00481750", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055b650u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055dff0: declared void by 0x0047d180 0x0047ea60 0x00480720; no matched or admitted source
int PortTop_RFStop_0055dff0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055dff0u, "0x0047d180 0x0047ea60 0x00480720", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055dff0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00564c80: declared int by 0x0055ae70; no matched or admitted source
int PortTop_RFStop_00564c80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00564c80u, "0x0055ae70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00564c80u, 0u, _ReturnAddress()); return 0; }
// stop rva 00565200: declared void by 0x0055ae70; no matched or admitted source
int PortTop_RFStop_00565200(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00565200u, "0x0055ae70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00565200u, 0u, _ReturnAddress()); return 0; }
// stop rva 00565260: declared void by 0x0055ae70; no matched or admitted source
int PortTop_RFStop_00565260(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00565260u, "0x0055ae70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00565260u, 0u, _ReturnAddress()); return 0; }
// stop rva 0057c210: declared F_00421750 * / int / undefined4 by 0x00421750 0x0047cea0 0x0047d180 0x0047ea60 0x0047f1e0 0x00480720 0x00481750 0x00481780; no matched or admitted source
int PortTop_RFStop_0057c210(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0057c210u, "0x00421750 0x0047cea0 0x0047d180 0x0047ea60 0x0047f1e0 0x00480720 0x00481750 0x00481780", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
