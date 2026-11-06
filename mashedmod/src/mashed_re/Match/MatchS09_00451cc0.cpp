// MATCH-S09: port body of 0x00451cc0 = the byte-matched source re/match/s09/00451cc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/WarzonePontoon.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Exe only (WarzonePontoon.cpp is). +0xc4 holds Track/WarzonePontoon.cpp ResetCb (set by InitFrom).
// Callers: Track/WarzonePontoon.cpp Deactivate (StateMachine states 3 / 5, Pontoon_Reset).


#define MATCH_PORT 1
#define FUN_00451cc0 WarzonePontoonDeactivate
#pragma comment(linker, "/EXPORT:WarzonePontoonDeactivate=_WarzonePontoonDeactivate")
extern "C" {
#include "../../../../re/match/s09/00451cc0.c"
}
