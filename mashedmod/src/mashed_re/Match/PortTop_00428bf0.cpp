// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00428bf0 = re/match/s06/00428bf0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00428bf0 PortTop_Bound_00428bf0
#define FUN_00427c90 GetDat0067d84c   // exe body (Match/MatchS06_00427c90.cpp)
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_00428320 PortTop_Bound_00428320
#define FUN_00428450 RwCore_00428450   // exe body (RwCore/RwCore_00428450.cpp)
#define FUN_00428760 RwCore_00428760   // exe body (RwCore/RwCore_00428760.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_004a2b60 MashedCrt_sprintf_004a2b60   // exe body (Match/PortTop_CrtStdio.cpp)
#define FUN_004c19f0 RwCore_004c19f0   // exe body (RwCore/RwCore_004c19f0.cpp)
#define FUN_004c1a00 RwCore_004c1a00   // exe body (RwCore/RwCore_004c1a00.cpp)
#define FUN_004c1bb0 RwCore_004c1bb0   // exe body (RwCore/RwCore_004c1bb0.cpp)
#define FUN_004c1be0 Orig_004c1be0   // exe body (Match/PortTop_004c1be0.cpp)
extern "C" {
#include "../../../../re/match/s06/00428bf0.c"
}
#endif // MASHED_STANDALONE
