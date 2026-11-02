// MATCH-S07: port bodies of 0x00431d00 and 0x00431b80 = the byte-matched source re/match/s07/00431b80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The file holds both functions (0x00431b80 is a static callee the DDK passes arguments to in registers), so this TU
// is the only body of each in the targets that compile it. 0x00431d00 moved out of Frontend/GameModeCarSelect.cpp
// (MATCH-S07-FU, 2026-11-01); 0x00431b80 had no port body before.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Under VS2022 the
// static FUN_00431b80 keeps internal linkage and an ordinary convention; nothing outside this TU calls it.
//
// What the swap changes: the old CarSelectReset called the cursor mover through CallCursorMover with (0, 0), (r, 0),
// (0, 0) (a misread of the register arguments, and in the exe a stand-in that skips the mover); the original calls
// FUN_00431b80(0, 1, 0), (1, 1, 0), (2, 1, 0). No exe caller reaches CarSelectReset, and it stays uninstalled in the
// .asi as before (the old install was commented out), so neither target runs new code by default.
// CallCursorMover (Frontend/GameModeCarSelect.cpp) stays as CarSelectCycleColour's call helper.

#define MATCH_PORT 1
#define FUN_00431d00 CarSelectReset
#pragma comment(linker, "/EXPORT:CarSelectReset=_CarSelectReset")
extern "C" {
#include "../../../../re/match/s07/00431b80.c"
}
