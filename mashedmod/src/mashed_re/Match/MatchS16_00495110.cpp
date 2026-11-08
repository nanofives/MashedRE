// PORT-TOP-01: port body of 0x00495110 = the byte-matched source re/match/s16/00495110.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// New body: the port had no body of this RVA. MainLoopInit 0x00492770 (Match/MatchS15_00492770.cpp) calls it by name.
// Compiled here as C++ with MATCH_PORT; the global 0x007f1030 binds to its original address (preamble MGLOBAL).
//
// Callee FUN_004950b0 (QPC * 3000000 / freq, signed __int64, no zero guard; Match/MatchS16_004950b0.cpp):
//   .asi : the matched body QpcTimeScaledTo3Mhz by name. MASHED.exe stored freq at 0x00771e70 earlier in its boot.
//   .exe : STAND-IN Call004950b0 below. The exe maps the original data range zero-filled and nothing in the exe
//          runs the original's QPF store 0x00495120 (TimerQpfStore, original caller 0x00493540), so freq is 0 and the
//          matched body would raise #DE. The stand-in runs the matched TimerQpfStore (Match/MatchS16_00495120.cpp)
//          once while freq is 0, then the matched QpcTimeScaledTo3Mhz. Both bodies are the matched sources; only
//          the call order is the exe's (the original runs 0x00495120 from 0x00493540 before 0x00492770).
//          Listed in verify/porttop_20261108/BINDINGS.tsv.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00495110 Store7f1030_Qpc3Mhz
#ifdef MASHED_STANDALONE
extern "C" unsigned int __cdecl QpcTimeScaledTo3Mhz(void);   // 0x004950b0, Match/MatchS16_004950b0.cpp
extern "C" unsigned int __cdecl TimerQpfStore(void);         // 0x00495120, Match/MatchS16_00495120.cpp
extern "C" unsigned int MatchS16_00495110_Call004950b0(void) {
    if (*reinterpret_cast<volatile unsigned int*>(0x00771e70u) == 0u &&
        *reinterpret_cast<volatile unsigned int*>(0x00771e74u) == 0u) {
        TimerQpfStore();
    }
    return QpcTimeScaledTo3Mhz();
}
#define FUN_004950b0 MatchS16_00495110_Call004950b0
#else
#define FUN_004950b0 QpcTimeScaledTo3Mhz
#endif
#pragma comment(linker, "/EXPORT:Store7f1030_Qpc3Mhz=_Store7f1030_Qpc3Mhz")
extern "C" {
#include "../../../../re/match/s16/00495110.c"
}
RH_ScopedInstall(Store7f1030_Qpc3Mhz, 0x00495110);
