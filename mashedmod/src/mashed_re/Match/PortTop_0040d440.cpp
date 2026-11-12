// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x0040d440 = the matched source re/match/s02/0040d440.c
// (0x0040d270(PTR_PTR_005f2770, DAT_0063ba7c), then DAT_0063ba78 = DAT_0063ba7c), case 2 of the game-mode machine
// 0x004929d0. Reached only on the MASHED_ORIG_TOP path. Its callee 0x0040d270 (the original's course load, source
// re/match/top07/0040d270.c, no exe body) is the stand-in PortTop_StandIn05_0040d270 (Match/PortTop_StandIns05.cpp): the
// hand path loads the course at the menu launch (exe_main MenuLaunchRace, g_track.Load) before the phase word reaches 2.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040d440 Orig_0040d440
#define FUN_0040d270 PortTop_StandIn05_0040d270
// The source declares 0x0040d270 (int, int) and passes the PTR_PTR_005f2770 pointer as the first argument (C under the
// DDK); this C++ overload converts it.
extern "C" void PortTop_StandIn05_0040d270(int p0, int p1);
static inline void PortTop_StandIn05_0040d270(unsigned char* p0, int p1) {
    PortTop_StandIn05_0040d270(static_cast<int>(reinterpret_cast<unsigned int>(p0)), p1);
}
extern "C" {
#include "../../../../re/match/s02/0040d440.c"
}
#endif // MASHED_STANDALONE
