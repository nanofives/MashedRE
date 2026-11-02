// MATCH-S09-FU: port body of 0x00451ce0 = the byte-matched source re/match/s09/00451ce0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/WarzonePontoon.cpp).
// Compiled here as C++ with MATCH_PORT. Exe only (WarzonePontoon.cpp is).
// Struct offsets read: +0x38, +0x50, +0x84, +0x94, +0xa8 of the object at 0x00684d18 (WarzonePontoon.cpp Obj: pos, hier,
// m54[12], m94, mov; its static_assert pins the layout). Callee: 0x00550bc0 = VfsStreamGetType
// (Save/VfsStream.cpp, its one port body: returns *(p + 8) = WarzonePontoon.cpp Hier::matArray = &mats[0], so + 0x40 is
// node 1 matrix, the one the old Sample read).
// Callers: Track/WarzonePontoon.cpp Sample (StateMachine states 0..4).


#define MATCH_PORT 1
#define FUN_00451ce0 WarzonePontoonSample
#define FUN_00550bc0 VfsStreamGetType
#pragma comment(linker, "/EXPORT:WarzonePontoonSample=_WarzonePontoonSample")
extern "C" {
#include "../../../../re/match/s09/00451ce0.c"
}
