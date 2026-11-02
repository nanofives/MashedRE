// MATCH-S05: port body of 0x00423b00 = the byte-matched source re/match/s05/00423b00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00423b00 FrontendInputDispatch
#define FUN_00423040 FrontendDirInput
#define FUN_00423270 TabCycler
#define FUN_00423320 CursorMover
#define FUN_00423670 MatchS05_00423b00_Call00423670
// Callees of the matched source: 0x00423040/0x00423270/0x00423320 bind to the port bodies FrontendDirInput (Frontend/SlotZeroers_s1.cpp),
// TabCycler and CursorMover (Match/MatchS05_00423270.cpp, MatchS05_00423320.cpp). 0x00423670 has no port body (its
// matched source calls 0x00408b00, which has none): the old Frontend/MenuLeaves_af5.cpp body called it through its
// original address in both targets, so the forwarder makes that same call.
extern "C" void MatchS05_00423b00_Call00423670(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00423670u)();
}
#pragma comment(linker, "/EXPORT:FrontendInputDispatch=_FrontendInputDispatch")
extern "C" {
#include "../../../../re/match/s05/00423b00.c"
}
RH_ScopedInstall(FrontendInputDispatch, 0x00423b00);
