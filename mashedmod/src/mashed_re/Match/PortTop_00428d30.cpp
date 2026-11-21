// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00428d30 = re/match/s06/00428d30.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00428d30 PortTop_Bound_00428d30
#define FUN_00401ee0 PortTop_Local_00401570_00401ee0   // exe body (Match/PortTop_00401570.cpp)
#define FUN_0040d250 DoubleDerefIndexedGetter   // exe body (Match/MatchS02_0040d250.cpp)
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_00428450 RwCore_00428450   // exe body (RwCore/RwCore_00428450.cpp)
#define FUN_00428760 RwCore_00428760   // exe body (RwCore/RwCore_00428760.cpp)
#define FUN_0042e590 RwCore_0042e590   // exe body (RwCore/RwCore_0042e590.cpp)
#define FUN_0042f0b0 GetFrameCounterPlus73   // exe body (Match/MatchS07_0042f0b0.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_00472c60 RwCore_00472c60   // exe body (RwCore/RwCore_00472c60.cpp)
#define FUN_00472f40 RwCore_00472f40   // exe body (RwCore/RwCore_00472f40.cpp)
#define FUN_004730b0 RwCore_004730b0   // exe body (RwCore/RwCore_004730b0.cpp)
#define FUN_00473220 RwCore_00473220   // exe body (RwCore/RwCore_00473220.cpp)
#define FUN_004744a0 PortTop_Bound_004744a0   // exe body (Match/PortTop_004744a0.cpp)
#define FUN_004c19f0 RwCore_004c19f0   // exe body (RwCore/RwCore_004c19f0.cpp)
#define FUN_004c1a00 RwCore_004c1a00   // exe body (RwCore/RwCore_004c1a00.cpp)
#define FUN_004c1bb0 RwCore_004c1bb0   // exe body (RwCore/RwCore_004c1bb0.cpp)
#define FUN_004c1be0 Orig_004c1be0   // exe body (Match/PortTop_004c1be0.cpp)
#define FUN_004c1c80 RwCore_004c1c80   // exe body (RwCore/RwCore_004c1c80.cpp)
extern "C" {
#include "../../../../re/match/s06/00428d30.c"
}
#endif // MASHED_STANDALONE
