// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00479330 = re/match/s12/00479330.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00479330 PortTop_Bound_00479330
#define FUN_0042a5d0 PortTop_Bound_0042a5d0
#define FUN_0042a640 PortTop_Bound_0042a640
#define FUN_0042a6b0 PortTop_Bound_0042a6b0
#define FUN_0042a740 PortTop_Bound_0042a740
#define FUN_0042a7f0 PortTop_Bound_0042a7f0
#define FUN_0042a860 PortTop_Bound_0042a860
#define FUN_00474fd0 PortTop_Bound_00474fd0
#define FUN_00478110 PortTop_Bound_00478110
#define FUN_00478200 PortTop_Bound_00478200
#define FUN_00478cc0 RenderWorldStateZeroFill   // exe body (Match/MatchS12_00478cc0.cpp)
#define FUN_00479030 PortTop_Bound_00479030
#define FUN_004790e0 PortTop_RLStop_004790e0   // named stop
#define FUN_004850e0 PortTop_Bound_004850e0
#define FUN_00485460 PortTop_Bound_00485460
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
#define FUN_004b3f90 RwCore_004b3f90   // exe body (RwCore/RwCore_004b3f90.cpp)
#define FUN_004b4000 PortTop_Bound_004b4000
#define FUN_004b4010 PortTop_Bound_004b4010
#define FUN_004b41f0 PortTop_Bound_004b41f0
#define FUN_004b4280 PortTop_Bound_004b4280
#define FUN_004c0740 RwCore_004c0740   // exe body (RwCore/RwCore_004c0740.cpp)
#define FUN_004c0b30 RwCore_004c0b30   // exe body (RwCore/RwCore_004c0b30.cpp)
#define FUN_004c1040 RwCore_004c1040   // exe body (RwCore/RwCore_004c1040.cpp)
#define FUN_004c1340 PortTop_Bound_004c1340
#define FUN_004c1480 PortTop_Bound_004c1480
#define FUN_004c1520 PortTop_Bound_004c1520
#define FUN_004c5c00 RwCore_004c5c00   // exe body (RwCore/RwCore_004c5c00.cpp)
#define FUN_004c5c80 RwCore_004c5c80   // exe body (RwCore/RwCore_004c5c80.cpp)
#define FUN_004e4450 RwCore_004e4450   // exe body (RwCore/RwCore_004e4450.cpp)
#define FUN_004e4810 RwCore_004e4810   // exe body (RwCore/RwCore_004e4810.cpp)
#define FUN_004e4900 Orig_004e4900   // exe body (Match/PortTop_004e4900.cpp)
#define FUN_004e4dd0 RwCore_004e4dd0   // exe body (RwCore/RwCore_004e4dd0.cpp)
#define FUN_004e5c70 RwCore_004e5c70   // exe body (RwCore/RwCore_004e5c70.cpp)
#define FUN_004e6650 PortTop_Bound_004e6650
#define FUN_004e69a0 RwCore_004e69a0   // exe body (RwCore/RwCore_004e69a0.cpp)
#define FUN_004e6f80 RwCore_004e6f80   // exe body (RwCore/RwCore_004e6f80.cpp)
#define FUN_004e7e30 Orig_004e7e30   // exe body (Match/PortTop_004e7e30.cpp)
#define FUN_00538a80 PortTop_Bound_00538a80
#define FUN_0053fbb0 RwCore_0053fbb0   // exe body (RwCore/RwCore_0053fbb0.cpp)
#define FUN_00558ca0 PortTop_Bound_00558ca0
extern "C" {
#include "../../../../re/match/s12/00479330.c"
}
#endif // MASHED_STANDALONE
