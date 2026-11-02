// MATCH-S06: port body of 0x00429620 = the byte-matched source re/match/s06/00429620.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00429300 -> HudOverlayFloatGet (symmap swap-batch2 sw5)

#define MATCH_PORT 1
#define FUN_00429620 FUN_00429620
#define FUN_00429300 HudOverlayFloatGet
#pragma comment(linker, "/EXPORT:FUN_00429620=_FUN_00429620")
extern "C" {
#include "../../../../re/match/s06/00429620.c"
}
