// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00426e10 = re/match/s06/00426e10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00426e10 PortTop_Bound_00426e10
#define FUN_004053d0 Orig_004053d0   // exe body (Match/PortTop_00405400.cpp)
#define FUN_00409680 PortTop_RLStop_00409680   // named stop
#define FUN_00409710 PortTop_Bound_00409710   // exe body (Match/PortTop_00409710.c)
#define FUN_00412050 PortTop_Bound_00412050
#define FUN_0041e870 TrackNodeRecordScan   // exe body (Match/MatchS04_0041e870.cpp)
#define FUN_0041e8b0 PortTop_Bound_0041e8b0
#define FUN_0041e970 PortTop_Bound_0041e970
#define FUN_0041e980 PortTop_Bound_0041e980
#define FUN_0041e9d0 PortTop_Bound_0041e9d0
#define FUN_0041ea90 PortTop_Bound_0041ea90
#define FUN_004235b0 PortTop_Bound_004235b0
#define FUN_00423630 PortTop_Bound_00423630
#define FUN_00425ab0 PortTop_Bound_00425ab0
#define FUN_004260e0 PortTop_Bound_004260e0
#define FUN_004262f0 PortTop_Bound_004262f0
#define FUN_00426cd0 PortTop_Bound_00426cd0
#define FUN_0042a530 PortTop_Bound_0042a530
#define FUN_0042a8d0 PortTop_Bound_0042a8d0
#define FUN_0042f510 Vehicle0HandleGet   // exe body (Match/MatchS07_0042f510.cpp)
#define FUN_00462950 PortTop_Bound_00462950
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_004715a0 PortTop_Bound_004715a0
#define FUN_00478660 PortTop_Bound_00478660
#define FUN_00479330 PortTop_Bound_00479330
#define FUN_0047a020 PortTop_Bound_0047a020
#define FUN_0047a0f0 PortTop_Bound_0047a0f0
#define FUN_0047c0b0 PortTop_Bound_0047c0b0
#define FUN_0047c0f0 PortTop_Bound_0047c0f0
#define FUN_00480340 PortTop_Bound_00480340
#define FUN_00491780 PortTop_Bound_00491780
#define FUN_004924c0 PortTop_Bound_004924c0
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
#define FUN_004c1b10 RwCore_004c1b10   // exe body (RwCore/RwCore_004c1b10.cpp)
extern "C" {
#include "../../../../re/match/s06/00426e10.c"
}
#endif // MASHED_STANDALONE
