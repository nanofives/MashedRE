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
// stop rva 00409680: declared void by 0x00426e10; cpp-unclean: C2632 'char' seguido de 'bool' no es v lido
int PortTop_RLStop_00409680(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00409680u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00409680u, 0u, _ReturnAddress()); return 0; }
// stop rva 00409710: declared int by 0x00426e10; cpp-unclean: C2632 'char' seguido de 'bool' no es v lido
int PortTop_RLStop_00409710(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00409710u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00409710u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a5b0: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s03/raw_0041a5b0.c
int PortTop_RLStop_0041a5b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a5b0u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a5b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041a5d0: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s03/h3_0041a5d0.c
int PortTop_RLStop_0041a5d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041a5d0u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041a5d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0041f260: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s04/0041f260.c
int PortTop_RLStop_0041f260(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0041f260u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0041f260u, 0u, _ReturnAddress()); return 0; }
// stop rva 00425bf0: declared void by 0x004262f0; cpp-unclean: C2664 'void PortTop_PizFs_Mount_00495280(undefined4)': el argumento 1 no puede convertirse de 'const char [28]' a 'u
int PortTop_RLStop_00425bf0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00425bf0u, "0x004262f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00425bf0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00440bc0: declared void by 0x0047a0f0 0x0047a130; cpp-unclean: C2632 'char' seguido de 'bool' no es v lido
int PortTop_RLStop_00440bc0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00440bc0u, "0x0047a0f0 0x0047a130", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00440bc0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00458dd0: declared void by 0x00458e00; cpp-unclean: C2664 'int *Orig_004e8090(int *,int)': el argumento 1 no puede convertirse de 'undefined4' a 'int *'
int PortTop_RLStop_00458dd0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00458dd0u, "0x00458e00", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00458dd0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00462950: declared void by 0x00426e10; no matched or admitted source
int PortTop_RLStop_00462950(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00462950u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00462950u, 0u, _ReturnAddress()); return 0; }
// stop rva 00478660: declared void by 0x00426e10; no matched or admitted source
int PortTop_RLStop_00478660(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00478660u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00478660u, 0u, _ReturnAddress()); return 0; }
// stop rva 00479330: declared int by 0x00426e10; no matched or admitted source
int PortTop_RLStop_00479330(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00479330u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00479330u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047a020: declared int by 0x00426e10; no matched or admitted source
int PortTop_RLStop_0047a020(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047a020u, "0x00426e10", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047a020u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047b860: declared void by 0x0047b9b0; cpp-unclean: C2660 'PortTop_RLStop_004b7330': la funci¢n no acepta 1 argumentos
int PortTop_RLStop_0047b860(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047b860u, "0x0047b9b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047b860u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047b8d0: declared undefined4 by 0x0047b9b0; cpp-unclean: C2664 'void Orig_004b6520(undefined4,undefined4)': el argumento 1 no puede convertirse de 'undefined1 [32768]' a 'un
int PortTop_RLStop_0047b8d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047b8d0u, "0x0047b9b0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047b8d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047be80: declared static void / void by 0x0047bf70; no matched or admitted source
int PortTop_RLStop_0047be80(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047be80u, "0x0047bf70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047be80u, 0u, _ReturnAddress()); return 0; }
// stop rva 0047f7d0: declared int by 0x0047f840; no matched or admitted source
int PortTop_RLStop_0047f7d0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0047f7d0u, "0x0047f840", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0047f7d0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00487280: declared int by 0x00480100; no matched or admitted source
int PortTop_RLStop_00487280(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00487280u, "0x00480100", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00487280u, 0u, _ReturnAddress()); return 0; }
// stop rva 00491590: declared undefined4 by 0x00491780; no matched or admitted source
int PortTop_RLStop_00491590(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00491590u, "0x00491780", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00491590u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b3f50: declared int by 0x00480340; no matched or admitted source
int PortTop_RLStop_004b3f50(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b3f50u, "0x00480340", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b3f50u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b5260: declared void by 0x0041a8d0; not bound here (--roots-only): matched source re/match/s18/004b5260.c
int PortTop_RLStop_004b5260(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b5260u, "0x0041a8d0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b5260u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b53b0: declared void by 0x0047bf70; no matched or admitted source
int PortTop_RLStop_004b53b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b53b0u, "0x0047bf70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b53b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 004b7480: declared void by 0x0047b880; no matched or admitted source
int PortTop_RLStop_004b7480(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004b7480u, "0x0047b880", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004b7480u, 0u, _ReturnAddress()); return 0; }
// stop rva 004c3d90: declared int by 0x0047bf70; cpp-unclean: C2197 'code (__cdecl *)': hay demasiados argumentos para la llamada
int PortTop_RLStop_004c3d90(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004c3d90u, "0x0047bf70", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004c3d90u, 0u, _ReturnAddress()); return 0; }
// stop rva 004cf2b0: declared int by 0x00478110; no matched or admitted source
int PortTop_RLStop_004cf2b0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x004cf2b0u, "0x00478110", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x004cf2b0u, 0u, _ReturnAddress()); return 0; }
// stop rva 0052de60: declared undefined4 by 0x005518f0; not bound here (--roots-only): matched source re/match/s23/0052de60.c
int PortTop_RLStop_0052de60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x0052de60u, "0x005518f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x0052de60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00551ad0: declared int * by 0x005518f0; no matched or admitted source
int PortTop_RLStop_00551ad0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00551ad0u, "0x005518f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00551ad0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00557a60: declared int by 0x00557d90; no matched or admitted source
int PortTop_RLStop_00557a60(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00557a60u, "0x00557d90", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00557a60u, 0u, _ReturnAddress()); return 0; }
// stop rva 00558cb0: declared undefined4 by 0x005518f0; no matched or admitted source
int PortTop_RLStop_00558cb0(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();
    if (n == 1 || ex) RlStopLog(0x00558cb0u, "0x005518f0", n, _ReturnAddress(), ex);
    if (ex) PortTop_AudioCalleeStop(0x00558cb0u, 0u, _ReturnAddress()); return 0; }
// stop rva 00559560: declared int by 0x00559b50; no matched or admitted source
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
}
#endif // MASHED_STANDALONE
