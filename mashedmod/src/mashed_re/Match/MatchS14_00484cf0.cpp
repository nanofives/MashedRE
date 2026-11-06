// MATCH-S14: port body of 0x00484cf0 = the byte-matched source re/match/s14/00484cf0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Race/WorldRegistry_naked.inc).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

// Callees bind to the port bodies the old naked body called (RG_4847d0 / RG_484ac0, verbatim naked in Race/WorldRegistry_naked.inc,
// exe-only TU Race/WorldRegistry.cpp), so this TU links only into mashed_re.exe. No RH_ScopedInstall: the old body had none.
#define MATCH_PORT 1
#define FUN_00484cf0 RG_484cf0
#define FUN_004847d0 RG_4847d0
#define FUN_00484ac0 RG_484ac0
#pragma comment(linker, "/EXPORT:RG_484cf0=_RG_484cf0")
extern "C" {
#include "../../../../re/match/s14/00484cf0.c"
}
