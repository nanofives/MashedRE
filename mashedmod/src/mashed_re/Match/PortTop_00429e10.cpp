// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00429e10 = re/match/s06/00429e10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429e10 PortTop_Bound_00429e10
#define FUN_00405890 Pred405890   // exe body (Match/MatchS01.cpp)
#define FUN_0040dc80 PortTop_Bound_0040dc80
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e4a0 PortTop_Bound_0040e4a0
#define FUN_00417730 VehicleRaceAngleGet   // exe body (Vehicle/VehicleState.cpp)
#define FUN_00417740 Float89a870ToInt   // exe body (Match/MatchGFFU_00417740.cpp)
#define FUN_0041e140 PortTop_Bound_0041e140
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_004295a0 HudDualLabelRender   // exe body (Match/MatchS06_004295a0.cpp)
#define FUN_00429660 PortTop_Bound_00429660
#define FUN_00429bd0 PortTop_Bound_00429bd0
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00443080 PortTop_Bound_00443080   // exe body (Match/PortTop_00443080.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_00472c60 RwCore_00472c60   // exe body (RwCore/RwCore_00472c60.cpp)
#define FUN_004c19f0 RwCore_004c19f0   // exe body (RwCore/RwCore_004c19f0.cpp)
#define FUN_004c1a00 RwCore_004c1a00   // exe body (RwCore/RwCore_004c1a00.cpp)
#define FUN_004c1bb0 RwCore_004c1bb0   // exe body (RwCore/RwCore_004c1bb0.cpp)
#define FUN_004c1c80 RwCore_004c1c80   // exe body (RwCore/RwCore_004c1c80.cpp)
extern "C" {
#include "../../../../re/match/s06/00429e10.c"
}
#endif // MASHED_STANDALONE
