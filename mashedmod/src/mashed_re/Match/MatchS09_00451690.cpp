// MATCH-S09-FU: port body of 0x00451690 = the byte-matched source re/match/s09/00451690.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/WarzonePontoon.cpp).
// Compiled here as C++ with MATCH_PORT. Exe only (WarzonePontoon.cpp is).
// Reads obj +0x44 (d44), obj +0x50 (hier) and hier +0x20 (WarzonePontoon.cpp Hier::interp, static_assert 0x20); writes
// obj +0x48 = 5 (state). Callees: 0x0052df70 = WarzonePontoonSubAnimTime, 0x00533ec0 = WarzonePontoonUpdateMatrices
// (Track/WarzonePontoon.cpp forwarders to its SubAnimTime / UpdateMatrices transcriptions).
// Callers: Track/WarzonePontoon.cpp LoopCb, the interpolator's loop callback (InitFrom sets it; AddAnimTime /
// SubAnimTime call it when the animation wraps).


#define MATCH_PORT 1
#define FUN_00451690 WarzonePontoonLoopCb
#define FUN_0052df70 WarzonePontoonSubAnimTime
#define FUN_00533ec0 WarzonePontoonUpdateMatrices
#pragma comment(linker, "/EXPORT:WarzonePontoonLoopCb=_WarzonePontoonLoopCb")
extern "C" {
#include "../../../../re/match/s09/00451690.c"
}
