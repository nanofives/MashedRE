// MATCH-S06: port body of 0x0042a530 = the byte-matched source re/match/s06/0042a530.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a470 = FUN_0042a470 (Match/MatchS06_0042a470.cpp), 0x00496400 = ConfigLogDebug
// (Match/PortSave01_Config.cpp, the matched body re/match/s16/00496400.c in both targets, by name; PORT-SAVE-01).

#define MATCH_PORT 1
#define FUN_00496400 ConfigLogDebug
#pragma comment(linker, "/EXPORT:FUN_0042a530=_FUN_0042a530")
extern "C" {
#include "../../../../re/match/s06/0042a530.c"
}
