// PORT-OT-RACE (2026-11-16): named stops of the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py).
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
int RlStopCount() {
    static int s = -1;
    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RL_STOPS"); s = (v && v[0] == 'c') ? 1 : 0; }
    return s;
}
unsigned g_rlStopHits = 0;
void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-load stop (PORT-OT-RACE), call %u, callers %s, return %p, %s\n", rva, n, callers, ret,
                     exits ? "exits" : "returns 0 (MASHED_OT_RL_STOPS=count)");
        std::fclose(f);
    }
}
} // namespace
extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
unsigned PortTop_RL_StopHits(void) { return g_rlStopHits; }
// stop rva 00405730: declared void by 0x0047b7d0; no matched or admitted source
int PortTop_RLStop_00405730(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00405730u, "0x0047b7d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00405730u, 0u, _ReturnAddress()); return 0; }
// stop rva 00409680: declared void by 0x00426e10; cpp-unclean: C2632 'char' seguido de 'bool' no es v lido
int PortTop_RLStop_00409680(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00409680u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00409680u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a5b0: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s03/raw_0041a5b0.c
int PortTop_RLStop_0041a5b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a5b0u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a5b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a5d0: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s03/h3_0041a5d0.c
int PortTop_RLStop_0041a5d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a5d0u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a5d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00425bf0: declared void by 0x004262f0; cpp-unclean: C2664 'void PortTop_PizFs_Mount_00495280(undefined4)': el argumento 1 no puede convertirse de 'const char [28]' a 'u
int PortTop_RLStop_00425bf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00425bf0u, "0x004262f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00425bf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00430250: declared int * by 0x0047b280; no matched or admitted source
int PortTop_RLStop_00430250(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00430250u, "0x0047b280", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00430250u, 0u, _ReturnAddress()); return 0; }
// stop rva 00451730: declared void by 0x00485460; cpp-unclean: C2664 'float *RwCore_004c1740(float *,float *,int)': el argumento 1 no puede convertirse de 'undefined4' a 'float *'
int PortTop_RLStop_00451730(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00451730u, "0x00485460", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00451730u, 0u, _ReturnAddress()); return 0; }
// stop rva 0045de80: declared void by 0x00462950; no matched or admitted source
int PortTop_RLStop_0045de80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0045de80u, "0x00462950", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0045de80u, 0u, _ReturnAddress()); return 0; }
// stop rva 004783f0: declared void by 0x00478660; not bound here (--roots-only): matched source re/match/s12/004783f0.c
int PortTop_RLStop_004783f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004783f0u, "0x00478660", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004783f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004785e0: declared static void / void by 0x00478660; not bound here (--roots-only): matched source re/match/s12/004785e0.c
int PortTop_RLStop_004785e0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004785e0u, "0x00478660", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004785e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004790e0: declared void by 0x00479030 0x00479330; register-arg: world pointer in EAX (re/match/s12/004790e0.c header; caller 0x00479330 loads [ebx+0x105d4] into EAX at 0x00479e2e), declared void(void) by re/match/s12/00479330.c under MATCH_PORT and static in its own source; no exe body takes EAX
int PortTop_RLStop_004790e0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004790e0u, "0x00479030 0x00479330", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004790e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047be80: declared static void / void by 0x0047bf70; no matched or admitted source
int PortTop_RLStop_0047be80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047be80u, "0x0047bf70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047be80u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047f7d0: declared int by 0x0047f840; no matched or admitted source
int PortTop_RLStop_0047f7d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047f7d0u, "0x0047f840", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047f7d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004854e0: declared void by 0x004858c0; no matched or admitted source
int PortTop_RLStop_004854e0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004854e0u, "0x004858c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004854e0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00487280: declared int by 0x00480100; no matched or admitted source
int PortTop_RLStop_00487280(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00487280u, "0x00480100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00487280u, 0u, _ReturnAddress()); return 0; }
// stop rva 00491590: declared undefined4 by 0x00491780; no matched or admitted source
int PortTop_RLStop_00491590(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00491590u, "0x00491780", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00491590u, 0u, _ReturnAddress()); return 0; }
// stop rva 004a332b: declared (none found) by 0x00478200 0x00490e50; no matched or admitted source
int PortTop_RLStop_004a332b(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004a332bu, "0x00478200 0x00490e50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004a332bu, 0u, _ReturnAddress()); return 0; }
// stop rva 004b3f50: declared int by 0x00480340; no matched or admitted source
int PortTop_RLStop_004b3f50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b3f50u, "0x00480340", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b3f50u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b4550: declared void by 0x00478660 0x00479030; no matched or admitted source
int PortTop_RLStop_004b4550(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b4550u, "0x00478660 0x00479030", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b4550u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b4eb0: declared void * by 0x004b5030; no matched or admitted source
int PortTop_RLStop_004b4eb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b4eb0u, "0x004b5030", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b4eb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b53b0: declared void by 0x00479030 0x0047bf70; not bound here (--roots-only): matched source re/match/s18/004b53b0.c
int PortTop_RLStop_004b53b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b53b0u, "0x00479030 0x0047bf70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b53b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c3d90: declared RwV3d * / int / void by 0x00478660 0x0047bf70 0x00485460; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RLStop_004c3d90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c3d90u, "0x00478660 0x0047bf70 0x00485460", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c3d90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c52f0: declared void * by 0x004c1480; no matched or admitted source
int PortTop_RLStop_004c52f0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c52f0u, "0x004c1480", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c52f0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004cf2b0: declared int by 0x00478110; no matched or admitted source
int PortTop_RLStop_004cf2b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004cf2b0u, "0x00478110", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004cf2b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004cf7d0: declared int by 0x004b3d20; no matched or admitted source
int PortTop_RLStop_004cf7d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004cf7d0u, "0x004b3d20", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004cf7d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00545260: declared undefined4 by 0x004b3cc0; no matched or admitted source
int PortTop_RLStop_00545260(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00545260u, "0x004b3cc0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00545260u, 0u, _ReturnAddress()); return 0; }
// stop rva 00545340: declared int by 0x00546380; no matched or admitted source
int PortTop_RLStop_00545340(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00545340u, "0x00546380", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00545340u, 0u, _ReturnAddress()); return 0; }
// stop rva 00551ad0: declared int * by 0x005518f0; no matched or admitted source
int PortTop_RLStop_00551ad0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00551ad0u, "0x005518f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00551ad0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00557a60: declared int by 0x00557d90; no matched or admitted source
int PortTop_RLStop_00557a60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00557a60u, "0x00557d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00557a60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00558cb0: declared undefined4 by 0x005518f0; not bound here (--roots-only): matched source re/match/rwl05/00558cb0.c
int PortTop_RLStop_00558cb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00558cb0u, "0x005518f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00558cb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559560: declared int by 0x00559b50; not bound here (--roots-only): matched source re/match/rwl05/00559560.c
int PortTop_RLStop_00559560(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00559560u, "0x00559b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00559560u, 0u, _ReturnAddress()); return 0; }
// stop rva 005595d0: declared int * by 0x00559b50; no matched or admitted source
int PortTop_RLStop_005595d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x005595d0u, "0x00559b50", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005595d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559c40: declared int / void by 0x0047d080 0x0047f940 0x0047fad0 0x0047fc40 0x0047fe00 0x0047ff70; no matched or admitted source
int PortTop_RLStop_00559c40(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00559c40u, "0x0047d080 0x0047f940 0x0047fad0 0x0047fc40 0x0047fe00 0x0047ff70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00559c40u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559ee0: declared int by 0x0047d080; no matched or admitted source
int PortTop_RLStop_00559ee0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00559ee0u, "0x0047d080", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00559ee0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055ab30: declared int by 0x00480100; no matched or admitted source
int PortTop_RLStop_0055ab30(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055ab30u, "0x00480100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055ab30u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055ac00: declared void by 0x00480100; no matched or admitted source
int PortTop_RLStop_0055ac00(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055ac00u, "0x00480100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055ac00u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055b940: declared float by 0x0047f940 0x0047fad0 0x0047fc40 0x0047fe00 0x0047ff70; no matched or admitted source
int PortTop_RLStop_0055b940(void) { RlStopLog(0x0055b940u, "0x0047f940 0x0047fad0 0x0047fc40 0x0047fe00 0x0047ff70", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x0055b940u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055ded0: declared int by 0x0057c300; no matched or admitted source
int PortTop_RLStop_0055ded0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055ded0u, "0x0057c300", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055ded0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055dff0: declared void by 0x00480100 0x0057c220; no matched or admitted source
int PortTop_RLStop_0055dff0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055dff0u, "0x00480100 0x0057c220", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055dff0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0055f800: declared int by 0x0055f450; no matched or admitted source
int PortTop_RLStop_0055f800(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0055f800u, "0x0055f450", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0055f800u, 0u, _ReturnAddress()); return 0; }
// stop rva 00562ac0: declared int by 0x00562600; no matched or admitted source
int PortTop_RLStop_00562ac0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00562ac0u, "0x00562600", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00562ac0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0057c210: declared int by 0x0047d080 0x00480100; no matched or admitted source
int PortTop_RLStop_0057c210(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0057c210u, "0x0047d080 0x00480100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0057c210u, 0u, _ReturnAddress()); return 0; }
// stop rva 0057c4b0: declared void by 0x0047f4c0; no matched or admitted source
int PortTop_RLStop_0057c4b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0057c4b0u, "0x0047f4c0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0057c4b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 005c1d63: declared int by 0x004b67e0; no matched or admitted source
int PortTop_RLStop_005c1d63(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x005c1d63u, "0x004b67e0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x005c1d63u, 0u, _ReturnAddress()); return 0; }
}
#endif // MASHED_STANDALONE
