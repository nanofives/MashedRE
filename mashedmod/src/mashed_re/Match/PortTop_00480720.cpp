// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00480720 = re/match/s13/00480720.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00480720 PortTop_Bound_00480720
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
#define FUN_004c3df0 RwV3dTransformPoints   // exe body (Match/MatchS19_004c3df0.cpp)
#define FUN_004c45f0 RwCore_004c45f0   // exe body (RwCore/RwCore_004c45f0.cpp)
#define FUN_0055ac00 PortTop_RFStop_0055ac00   // named stop
#define FUN_0055ad30 PortTop_Bound_0055ad30
#define FUN_0055ae70 PortTop_Bound_0055ae70
#define FUN_0055dff0 PortTop_RFStop_0055dff0   // named stop
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/00480720.c"
}
#endif // MASHED_STANDALONE
