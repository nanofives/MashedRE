// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00420420 = re/match/s05/00420420.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00420420 PortTop_Bound_00420420
#define FUN_00420290 PortTop_RFStop_00420290   // named stop
#define FUN_004223b0 PortTop_Bound_004223b0
#define FUN_004224d0 PortTop_Bound_004224d0
#define FUN_004260a0 GetDat00657438   // exe body (Match/MatchS06_004260a0.cpp)
#define FUN_004260b0 GetDat0065743c   // exe body (Match/MatchS06_004260b0.cpp)
#define FUN_0042a5d0 PortTop_Bound_0042a5d0   // exe body (Match/PortTop_0042a5d0.cpp)
#define FUN_0042a6b0 PortTop_Bound_0042a6b0   // exe body (Match/PortTop_0042a6b0.cpp)
#define FUN_004b3fc0 Orig_004b3fc0   // exe body (Match/PortTop_004b3fc0.cpp)
#define FUN_004b5190 RwCore_004b5190   // exe body (RwCore/RwCore_004b5190.cpp)
#define FUN_004b52c0 RwCore_004b52c0   // exe body (RwCore/RwCore_004b52c0.cpp)
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_004c15c0 RwCore_004c15c0   // exe body (RwCore/RwCore_004c15c0.cpp)
#define FUN_004c5c00 RwCore_004c5c00   // exe body (RwCore/RwCore_004c5c00.cpp)
#define FUN_004c5c80 RwCore_004c5c80   // exe body (RwCore/RwCore_004c5c80.cpp)
#define FUN_004e5fc0 RwCore_004e5fc0   // exe body (RwCore/RwCore_004e5fc0.cpp)
#define FUN_004e6920 RwCore_004e6920   // exe body (RwCore/RwCore_004e6920.cpp)
#define FUN_004e6ab0 RwCore_004e6ab0   // exe body (RwCore/RwCore_004e6ab0.cpp)
#define FUN_004e6e00 RwCore_004e6e00   // exe body (RwCore/RwCore_004e6e00.cpp)
#define FUN_004e6fb0 PortTop_Bound_004e6fb0
#define FUN_0053d090 PortTop_Bound_0053d090
#define FUN_0053d400 PortTop_Bound_0053d400
#define FUN_0053fb70 RwCore_0053fb70   // exe body (RwCore/RwCore_0053fb70.cpp)
#define FUN_0053fbb0 RwCore_0053fbb0   // exe body (RwCore/RwCore_0053fbb0.cpp)
#define FUN_0053fd30 RwCore_0053fd30   // exe body (RwCore/RwCore_0053fd30.cpp)
#define FUN_00543b90 PortTop_Bound_00543b90
#define FUN_00543d70 RwCore_00543d70   // exe body (RwCore/RwCore_00543d70.cpp)
#define FUN_00543e10 PortTop_Bound_00543e10
#define FUN_005449f0 PortTop_Bound_005449f0
#define FUN_00544a70 PortTop_BulkStop_00544a70   // named stop
#define FUN_00544ad0 PortTop_BulkStop_00544ad0   // named stop
#define FUN_0041f330 PortTop_Local_00420420_0041f330   // file-local in this source
#define FUN_0041f360 PortTop_Local_00420420_0041f360   // file-local in this source
#define FUN_0041fe10 PortTop_Local_00420420_0041fe10   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00420420.c"
}
extern "C" { void *PortTop_Keep_00420420 = (void *)PortTop_Bound_00420420; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
