// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004270f0 = re/match/s06/004270f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004270f0 PortTop_Bound_004270f0
#define FUN_0041e8d0 PortTop_Bound_0041e8d0
#define FUN_0041e8e0 PortTop_Bound_0041e8e0
#define FUN_0041e9b0 PortTop_RRStop_0041e9b0   // named stop
#define FUN_0041e9f0 PortTop_Bound_0041e9f0
#define FUN_0041ea00 PortTop_Bound_0041ea00
#define FUN_00426d20 PortTop_Bound_00426d20
#define FUN_00426d90 PortTop_Bound_00426d90
#define FUN_004292c0 UtilWritePtr   // exe body (Match/MatchS06_004292c0.cpp)
#define FUN_004292d0 FUN_004292d0   // exe body (Match/MatchS06_004292d0.cpp)
#define FUN_00450b10 RwCore_00450b10   // exe body (RwCore/RwCore_00450b10.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_00467210 PortTop_RRStop_00467210   // named stop
#define FUN_00472ad0 PortTop_Bound_00472ad0
#define FUN_00478220 PortTop_Bound_00478220
#define FUN_00478cd0 PortTop_Bound_00478cd0
#define FUN_004914b0 PortTop_RRStop_004914b0   // named stop
#define FUN_004924e0 PortTop_Bound_004924e0
#define FUN_004b5af0 PortTop_RRStop_004b5af0   // named stop
#define FUN_004e5660 PortTop_Bound_004e5660
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
extern "C" {
#include "../../../../re/match/s06/004270f0.c"
}
#endif // MASHED_STANDALONE
