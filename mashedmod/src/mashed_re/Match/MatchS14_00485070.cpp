// MATCH-S14: port body of 0x00485070 = the byte-matched source re/match/s14/00485070.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Race/WorldRegistry_naked.inc).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

// Callees bind to the port bodies the old naked body called (RG_484de0 / RG_484f70, verbatim naked in Race/WorldRegistry_naked.inc,
// exe-only TU Race/WorldRegistry.cpp), so this TU links only into mashed_re.exe. No RH_ScopedInstall: the old body had none.
#define MATCH_PORT 1
#define FUN_00485070 RG_485070
#define FUN_00484de0 RG_484de0
#define FUN_00484f70 RG_484f70
#pragma comment(linker, "/EXPORT:RG_485070=_RG_485070")
extern "C" {
#include "../../../../re/match/s14/00485070.c"
}
