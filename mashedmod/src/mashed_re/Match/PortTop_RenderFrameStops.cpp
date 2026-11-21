// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): named stops of the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe).
// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.
// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it
// names the next body to bind. MASHED_OT_RR_STOPS=count: a stop logs its first call and returns 0 (eax), so the frame render
// runs on past it with that callee's effects missing; stops declared float / double by a caller always exit.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#include <cstdio>
#include <cstdlib>
namespace {
int RlStopCount() {
    static int s = -1;
    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RR_STOPS"); s = (v && v[0] == 'c') ? 1 : 0; }
    return s;
}
unsigned g_rlStopHits = 0;
void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: frame-render stop (PORT-TOP-RENDERFRAME-492E90), call %u, callers %s, return %p, %s\n", rva, n, callers, ret,
                     exits ? "exits" : "returns 0 (MASHED_OT_RR_STOPS=count)");
        std::fclose(f);
    }
}
} // namespace
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
unsigned PortTop_RR_StopHits(void) { return g_rlStopHits; }
// stop rva 00404320: declared void by 0x00492e90; no matched or admitted source
int PortTop_RRStop_00404320(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00404320u, "0x00492e90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00404320u, 0u, _ReturnAddress()); return 0; }
// stop rva 00406130: declared void by 0x004072e0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RRStop_00406130(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00406130u, "0x004072e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00406130u, 0u, _ReturnAddress()); return 0; }
// stop rva 0040df20: declared void by 0x00492e90; cpp-unclean: C2664 'int RwCore_004c1c80(int,undefined4 *)': el argumento 2 no puede convertirse de 'undefined1 *' a 'undefined4 *
int PortTop_RRStop_0040df20(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0040df20u, "0x00492e90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0040df20u, 0u, _ReturnAddress()); return 0; }
// stop rva 00412100: declared void by 0x0040bde0; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'undefined4 *' a 'int *'
int PortTop_RRStop_00412100(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00412100u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00412100u, 0u, _ReturnAddress()); return 0; }
// stop rva 00412130: declared int by 0x00401f10; cpp-unclean: C3861 'GetRaceSubMode': no se encontrÂ¢ el identificador
int PortTop_RRStop_00412130(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00412130u, "0x00401f10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00412130u, 0u, _ReturnAddress()); return 0; }
// stop rva 00413a00: declared void by 0x00410b30; cpp-unclean: C2065 'DAT_00828320': identificador no declarado
int PortTop_RRStop_00413a00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00413a00u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00413a00u, 0u, _ReturnAddress()); return 0; }
// stop rva 00413a40: declared void by 0x00410b30; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RRStop_00413a40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00413a40u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00413a40u, 0u, _ReturnAddress()); return 0; }
// stop rva 00413f50: declared void by 0x0040bde0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RRStop_00413f50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00413f50u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00413f50u, 0u, _ReturnAddress()); return 0; }
// stop rva 004173e0: declared char by 0x00444ff0; cpp-unclean: C2065 'DAT_007f1a9c': identificador no declarado
int PortTop_RRStop_004173e0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004173e0u, "0x00444ff0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004173e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004189c0: declared void by 0x00410b30; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RRStop_004189c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004189c0u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004189c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a3e0: declared void by 0x0040dfc0; cpp-unclean: C3861 'PortTop_Bound_0042a5d0': no se encontrÂ¢ el identificador
int PortTop_RRStop_0041a3e0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a3e0u, "0x0040dfc0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a3e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a960: declared void by 0x00411ce0; cpp-unclean: C2632 'char' seguido de 'bool' no es vÂ lido
int PortTop_RRStop_0041a960(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a960u, "0x00411ce0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a960u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041b630: declared void by 0x0040dfc0; cpp-unclean: C3861 'Orig_004b3fc0': no se encontrÂ¢ el identificador
int PortTop_RRStop_0041b630(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041b630u, "0x0040dfc0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041b630u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041db80: declared void by 0x0040dfc0; no matched or admitted source
int PortTop_RRStop_0041db80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041db80u, "0x0040dfc0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041db80u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041ded0: declared void by 0x0040dfc0; no matched or admitted source
int PortTop_RRStop_0041ded0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041ded0u, "0x0040dfc0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041ded0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041e9b0: declared int by 0x00426640 0x004270f0; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_0041e9b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041e9b0u, "0x00426640 0x004270f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041e9b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041ebb0: declared void by 0x00410b30; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0041ebb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041ebb0u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041ebb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041f290: declared int by 0x00404650; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_0041f290(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041f290u, "0x00404650", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041f290u, 0u, _ReturnAddress()); return 0; }
// stop rva 00421560: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_00421560(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00421560u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00421560u, 0u, _ReturnAddress()); return 0; }
// stop rva 004228f0: declared void by 0x00420050; no matched or admitted source
int PortTop_RRStop_004228f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004228f0u, "0x00420050", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004228f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00426030: declared void by 0x00492e90; cpp-unclean: C2660 'PortTop_Bound_0041e8f0': la funciÂ¢n no acepta 1 argumentos
int PortTop_RRStop_00426030(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00426030u, "0x00492e90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00426030u, 0u, _ReturnAddress()); return 0; }
// stop rva 00443080: declared int by 0x00429e10; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_00443080(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00443080u, "0x00429e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00443080u, 0u, _ReturnAddress()); return 0; }
// stop rva 00444b60: declared void by 0x00444ff0; no matched or admitted source
int PortTop_RRStop_00444b60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00444b60u, "0x00444ff0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00444b60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00457610: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_00457610(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00457610u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00457610u, 0u, _ReturnAddress()); return 0; }
// stop rva 0045b990: declared void by 0x00410b30; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'void **' a 'undefined **'
int PortTop_RRStop_0045b990(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0045b990u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0045b990u, 0u, _ReturnAddress()); return 0; }
// stop rva 00467210: declared int by 0x004270f0; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_00467210(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00467210u, "0x004270f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00467210u, 0u, _ReturnAddress()); return 0; }
// stop rva 004725c0: declared void by 0x00410b30; cpp-unclean: C2440 '=': no se puede realizar la conversiÂ¢n de 'undefined4 *' a 'int *'
int PortTop_RRStop_004725c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004725c0u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004725c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00475e50: declared void by 0x0040bde0 0x00410b30; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_00475e50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00475e50u, "0x0040bde0 0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00475e50u, 0u, _ReturnAddress()); return 0; }
// stop rva 00484310: declared void by 0x00484580; static in re/match/s14fu/00484310.c:69 (file-local there): no linkable exe body
int PortTop_RRStop_00484310(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00484310u, "0x00484580", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00484310u, 0u, _ReturnAddress()); return 0; }
// stop rva 00484360: declared int by 0x00484580; no matched or admitted source
int PortTop_RRStop_00484360(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00484360u, "0x00484580", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00484360u, 0u, _ReturnAddress()); return 0; }
// stop rva 00486f50: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_00486f50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00486f50u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00486f50u, 0u, _ReturnAddress()); return 0; }
// stop rva 00487de0: declared void by 0x0040bde0; cpp-unclean: C2660 'PortTop_Bound_00487d70': la funciÂ¢n no acepta 1 argumentos
int PortTop_RRStop_00487de0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00487de0u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00487de0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004891f0: declared void by 0x00410b30; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_004891f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004891f0u, "0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004891f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048ad50: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048ad50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048ad50u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048ad50u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048baf0: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048baf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048baf0u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048baf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048fce0: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048fce0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048fce0u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048fce0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048fd10: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048fd10(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048fd10u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048fd10u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048fd40: declared void by 0x0040bde0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048fd40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048fd40u, "0x0040bde0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048fd40u, 0u, _ReturnAddress()); return 0; }
// stop rva 0048fd70: declared void by 0x0040bde0 0x00410b30; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_0048fd70(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0048fd70u, "0x0040bde0 0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0048fd70u, 0u, _ReturnAddress()); return 0; }
// stop rva 00490490: declared void by 0x0040bde0 0x00410b30; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_00490490(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00490490u, "0x0040bde0 0x00410b30", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00490490u, 0u, _ReturnAddress()); return 0; }
// stop rva 004914b0: declared void by 0x004270f0; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RRStop_004914b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004914b0u, "0x004270f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004914b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b42c0: declared void by 0x004011f0 0x00420050; no matched or admitted source
int PortTop_RRStop_004b42c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b42c0u, "0x004011f0 0x00420050", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b42c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b4a80: declared int by 0x004b4ad0; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_004b4a80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b4a80u, "0x004b4ad0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b4a80u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b5af0: declared void by 0x004270f0 0x00483a70; no matched or admitted source
int PortTop_RRStop_004b5af0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b5af0u, "0x004270f0 0x00483a70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b5af0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b5d90: declared void by 0x004b60c0; no matched or admitted source
int PortTop_RRStop_004b5d90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b5d90u, "0x004b60c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b5d90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c1b40: declared int by 0x00420050 0x004219c0 0x00425e40 0x00477a10 0x004e5680; bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound
int PortTop_RRStop_004c1b40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c1b40u, "0x00420050 0x004219c0 0x00425e40 0x00477a10 0x004e5680", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c1b40u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c3d90: declared V3_570 * / triangle goes through by 0x00422570; owned by RACE rows (run 53, race_owned.txt)
int PortTop_RRStop_004c3d90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c3d90u, "0x00422570", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c3d90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c52f0: declared RwMatrix * by 0x00420050; no matched or admitted source
int PortTop_RRStop_004c52f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c52f0u, "0x00420050", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c52f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004e14a0: declared void * by 0x004cd430; no matched or admitted source
int PortTop_RRStop_004e14a0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004e14a0u, "0x004cd430", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004e14a0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004e6680: declared int / void by 0x004013f0 0x00401f10 0x00404650 0x00420050 0x00426d90; bound by RACE-BIND-C4-BULK cp1 (orch/RACE-BIND-C4-BULK @ d4fda5c93, unmerged): listed, not rebound
int PortTop_RRStop_004e6680(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004e6680u, "0x004013f0 0x00401f10 0x00404650 0x00420050 0x00426d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004e6680u, 0u, _ReturnAddress()); return 0; }
// stop rva 005459c0: declared V3_004729b0 * by 0x004729b0; no matched or admitted source
int PortTop_RRStop_005459c0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x005459c0u, "0x004729b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005459c0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005586f0: declared int by 0x00444ff0; no matched or admitted source
int PortTop_RRStop_005586f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x005586f0u, "0x00444ff0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005586f0u, 0u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
