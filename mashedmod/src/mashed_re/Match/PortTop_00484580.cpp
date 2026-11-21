// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00484580 = re/match/racepath/004844a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00484580 PortTop_Bound_00484580
#define FUN_004292c0 UtilWritePtr   // exe body (Match/MatchS06_004292c0.cpp)
#define FUN_004292d0 FUN_004292d0   // exe body (Match/MatchS06_004292d0.cpp)
#define FUN_004842b0 PortTop_Bound_004842b0
#define FUN_00484310 PortTop_RRStop_00484310   // named stop
#define FUN_00484360 PortTop_RRStop_00484360   // named stop
#define FUN_004b41b0 PortTop_Bound_004b41b0
#define FUN_004b60c0 PortTop_Bound_004b60c0
#define FUN_004b61c0 PortTop_Bound_004b61c0
#define FUN_004e5660 PortTop_Bound_004e5660
#define FUN_004e5820 RwCore_004e5820   // exe body (RwCore/RwCore_004e5820.cpp)
#define FUN_004e5840 PortTop_Bound_004e5840
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_004844a0 PortTop_Local_00484580_004844a0   // file-local in this source
extern "C" {
#include "../../../../re/match/racepath/004844a0.c"
}
#endif // MASHED_STANDALONE
