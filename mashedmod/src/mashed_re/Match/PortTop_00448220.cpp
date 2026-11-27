// BIND-GATES-ALLKNOBS (2026-11-21): exe-only port body of rva 0x00448220 = re/match/s08/00448220.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP, from the bound caller named in
// re/tools/bind/state/gates/profile.json (re/tools/bind/porttop_bind.py --profile gates). Callees bind to exe bodies by
// name; a callee with no matched or admitted body, or outside the gate roots, is a named stop (Match/PortTop_GatesStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00448220 PortTop_Bound_00448220
#define FUN_00408a50 PerCarRaceProgressGet   // exe body (Match/MatchS01.cpp)
#define FUN_00408b00 PortTop_GTStop_00408b00   // named stop
#define FUN_00409330 PortTop_Bound_00409330   // exe body (Match/PortTop_00409330.cpp)
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e370 IsCarSlotActive   // exe body (Match/MatchS02_0040e370.cpp)
#define FUN_0040e4a0 PortTop_Bound_0040e4a0   // exe body (Match/PortTop_0040e4a0.cpp)
#define FUN_0040e4b0 PortTop_Bound_0040e4b0   // exe body (Match/PortTop_0040e4b0.cpp)
#define FUN_0041ef60 PortTop_GTStop_0041ef60   // named stop
#define FUN_00434360 PortTop_GTStop_00434360   // named stop
#define FUN_00441990 PortTop_GTStop_00441990   // named stop
#define FUN_00442e00 PortTop_GTStop_00442e00   // named stop
#define FUN_00446520 PortTop_GTStop_00446520   // named stop
#define FUN_0046b4f0 UtilTableRead46b4f0   // exe body (Match/MatchS11_0046b4f0.cpp)
#define FUN_0046c7b0 PortTop_GTStop_0046c7b0   // named stop
#define FUN_0046cbb0 PortTop_GTStop_0046cbb0   // named stop
#define FUN_0046cc10 VehicleFloatFieldAsInt   // exe body (Match/MatchS11_0046cc10.cpp)
#define FUN_0046d400 PortTop_Bound_0046d400   // exe body (Match/PortTop_0046d400.cpp)
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_0046d510 PortTop_Bound_0046d510   // exe body (Match/PortTop_0046d510.cpp)
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_004c1c10 RwCore_004c1c10   // exe body (RwCore/RwCore_004c1c10.cpp)
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
extern "C" {
#include "../../../../re/match/s08/00448220.c"
}
#endif // MASHED_STANDALONE
