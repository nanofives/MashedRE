// MATCH-S06: port body of 0x004288a0 = the byte-matched source re/match/s06/004288a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x004282a0 -> MenuMenusBA (symmap swap-batch2 sw4)
//   0x00428760 -> ViewportScaledRectDraw_Arg7Zero (symmap swap-batch2 sw5)

#define MATCH_PORT 1
#define FUN_004288a0 FUN_004288a0
#define FUN_00427e00 FUN_00427e00
#define FUN_004282a0 MenuMenusBA
#define FUN_00428760 ViewportScaledRectDraw_Arg7Zero
#pragma comment(linker, "/EXPORT:FUN_004288a0=_FUN_004288a0")
extern "C" {
#include "../../../../re/match/s06/004288a0.c"
}
