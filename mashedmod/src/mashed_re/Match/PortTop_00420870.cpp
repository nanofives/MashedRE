// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00420870 = re/match/s05/00420870.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00420870 PortTop_Bound_00420870
#define FUN_0041f3d0 PortTop_Bound_0041f3d0
#define FUN_00420290 PortTop_RFStop_00420290   // named stop
#define FUN_00422470 PortTop_Bound_00422470
#define FUN_00422aa0 SlotFieldSet   // exe body (Match/MatchS05_00422aa0.cpp)
#define FUN_004260a0 GetDat00657438   // exe body (Match/MatchS06_004260a0.cpp)
#define FUN_004260b0 GetDat0065743c   // exe body (Match/MatchS06_004260b0.cpp)
#define FUN_00426dc0 PortTop_Bound_00426dc0
#define FUN_0046bce0 PortTop_Bound_0046bce0
#define FUN_0046bd20 PortTop_Bound_0046bd20
#define FUN_0046bda0 PortTop_Bound_0046bda0
#define FUN_0046cb30 PortTop_Bound_0046cb30
#define FUN_00484cf0 PortTop_Bound_00484cf0
#define FUN_004b5240 Orig_004b5240   // exe body (Match/PortTop_004b5240.cpp)
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004c1520 PortTop_Bound_004c1520   // exe body (Match/PortTop_004c1520.cpp)
#define FUN_00540080 PortTop_Bound_00540080
#define FUN_00540100 PortTop_Bound_00540100
#define FUN_00540160 PortTop_Bound_00540160
#define FUN_00544bf0 PortTop_RFStop_00544bf0   // named stop
#define FUN_0041f360 PortTop_Local_00420870_0041f360   // file-local in this source
#define FUN_0041f4f0 PortTop_Local_00420870_0041f4f0   // file-local in this source
#define FUN_0041f590 PortTop_Local_00420870_0041f590   // file-local in this source
#define FUN_0041f710 PortTop_Local_00420870_0041f710   // file-local in this source
#define FUN_0041f770 PortTop_Local_00420870_0041f770   // file-local in this source
#define FUN_0041f880 PortTop_Local_00420870_0041f880   // file-local in this source
#define FUN_00420340 PortTop_Local_00420870_00420340   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00420870.c"
}
#endif // MASHED_STANDALONE
