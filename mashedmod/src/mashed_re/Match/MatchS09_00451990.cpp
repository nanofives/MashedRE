// MATCH-S09: port body of 0x00451990 = the byte-matched source re/match/s09/00451990.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/WarzonePontoon.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Exe only (WarzonePontoon.cpp is). Callees: 0x004c4dc0 = RwMatrixInvertEntry (Render/RwMatrixInvert.cpp), 0x004c52f0
// = FUN_004c52f0 (Collision/RwpSolverMath2.cpp, by its own name), as the old body's MatInvert / CombinePost.
// Callers: Track/WarzonePontoon.cpp BoxCarry (Coupling, in place on the car matrix).


#define MATCH_PORT 1
#define FUN_00451990 WarzonePontoonBoxCarry
#define FUN_004c4dc0 RwMatrixInvertEntry
#pragma comment(linker, "/EXPORT:WarzonePontoonBoxCarry=_WarzonePontoonBoxCarry")
extern "C" {
#include "../../../../re/match/s09/00451990.c"
}
