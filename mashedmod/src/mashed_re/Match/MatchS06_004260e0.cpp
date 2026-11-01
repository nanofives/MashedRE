// MATCH-S06: port body of 0x004260e0 = the byte-matched source re/match/s06/004260e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x004260e0 had no port body: this is its first body (new body, top-down rule L-72). No install; caller 0x00426e10.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#pragma comment(linker, "/EXPORT:FUN_004260e0=_FUN_004260e0")
extern "C" {
#include "../../../../re/match/s06/004260e0.c"
}
