// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00430b90 = re/match/s07/00430b90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00430b90 PortTop_Bound_00430b90
#define FUN_0040bb50 RwCore_0040bb50   // exe body (RwCore/RwCore_0040bb50.cpp)
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_004282a0 RwCore_004282a0   // exe body (RwCore/RwCore_004282a0.cpp)
#define FUN_0042ac00 MenuGroupCountBody   // exe body (Match/MatchS06_0042ac00.cpp)
#define FUN_0042ac50 Calc42ac50   // exe body (Match/MatchS06_0042ac50.cpp)
#define FUN_0042b8b0 ScreenWidthGet   // exe body (Match/MatchS07_0042b8b0.cpp)
#define FUN_0042b8c0 ScreenHeightGet   // exe body (Match/MatchS07_0042b8c0.cpp)
#define FUN_00472c60 RwCore_00472c60   // exe body (RwCore/RwCore_00472c60.cpp)
#define FUN_00473870 TextSpriteUVExplicit   // exe body (Match/MatchS11_00473870.cpp)
#define FUN_004739f0 PortTop_Bound_004739f0
extern "C" {
#include "../../../../re/match/s07/00430b90.c"
}
#endif // MASHED_STANDALONE
