// MATCH-S08: port body of 0x00443300 = the byte-matched source re/match/s08/00443300.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/AiSplineEval.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_00443300 AiSplineEval_x87
#pragma comment(linker, "/EXPORT:AiSplineEval_x87=_AiSplineEval_x87")
extern "C" {
#include "../../../../re/match/s08/00443300.c"
}
