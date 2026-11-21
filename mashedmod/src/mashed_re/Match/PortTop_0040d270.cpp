// RVA-LINT: ORIGTOP-ONLY
// PORT-OT-RACE (L-111 step 3): exe-only port body of rva 0x0040d270 = the matched source re/match/top07/0040d270.c
// (match.py 100%, C4 by match, hooks.csv Course::Finish), the course load that 0x0040d440 (case 2 of the game-mode machine
// 0x004929d0) calls. Reached only on the MASHED_ORIG_TOP path with MASHED_OT_RACELOAD=1, through PortTop_Bound_0040d270
// (Match/PortTop_StandInsRL.cpp). Knob off: the PORT-TOP-05 stand-in, as before. PREREG verify/ro_20261116/PREREG_PORT-OT-RACE.md.
// Every callee binds by name, no raw-address call:
//   0x00426ba0  HudDrawEnabled               matched exe body (Match/MatchS06_00426ba0.cpp)
//   0x004950b0  QpcTimeScaledTo3Mhz          matched exe body (Match/MatchS16_004950b0.cpp); the replay knob wraps its QPC
//   0x00462510  Set603868_0                matched, Match/MatchS10_00462510.cpp (the .asi TU, shared with the exe by this row)
//   0x00462500  Set603868_1                matched, Match/MatchS10_00462500.cpp (the .asi TU, shared with the exe by this row)
//   0x00426c00  GameStateFlagGet             matched exe body (Match/MatchS06_00426c00.cpp)
//   0x0042f790  GhostModeIsActive                matched, Match/MatchS07_0042f790.cpp (the .asi TU, shared with the exe by this row)
//   0x0042f7a0  GhostFlagReset                matched, Match/MatchS07_0042f7a0.cpp (the .asi TU, shared with the exe by this row)
//   0x0042fe30  RaceEndFlagIfEndMode         matched exe body (Match/MatchS07_0042fe30.cpp)
//   0x0040d040  PortTop_StandInRL_0040d040   stand-in (counted, logged), the B path only
//   0x0040cf80  Orig_0040cf80                matched, Match/PortTop_0040cf80.cpp (PORT-TOP-04C)
//   0x0040cfd0  Orig_0040cfd0                matched, Match/PortTop_0040cfd0.cpp
//   0x0040d020  Orig_0040d020                matched, Match/PortTop_0040d020.cpp (this row); its 0x00426e10 is a stand-in
//   0x0040d110  PortTop_StandInRL_0040d110   stand-in
//   0x00458c70  PortTop_RL_00458c70          matched, Match/PortTop_00458c70.cpp via Match/PortTop_StandInsRL.cpp (RACE-RWGLOBALS-FRAMELIST)
//   0x00426340  PortTop_StandInRL_00426340   stand-in
//   0x00426020  GlobalDat00646e58Get         matched exe body (Match/MatchS05_00426020.cpp)
//   0x00407800  PortTop_StandInRL_00407800   stand-in (C2)
//   0x004074a0  PortTop_StandInRL_004074a0   stand-in
//   0x004264d0  PortTop_RL_004264d0          matched, Match/PortTop_004264d0.cpp via Match/PortTop_StandInsRL.cpp (checkpoint 3)
//   0x004ccde0  PortTop_RL_004ccde0          RwCore_004ccde0 (RwCore/RwCore_004ccde0.cpp) when its list head 0x007d45cc is
//                                            set, else a logged skip (the list is not initialised on this path)
//   0x00426070  TrackPhysWorld2Get                matched, Match/MatchS06_00426070.cpp (the .asi TU, shared with the exe by this row)
//   0x00411d90  PortTop_StandInRL_00411d90   stand-in
//   0x004987b0  DbgPrintf_004987b0           matched exe body (Match/PortTop_004987b0.cpp)
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040d270 Orig_0040d270
#define FUN_00426ba0 HudDrawEnabled
#define FUN_004950b0 QpcTimeScaledTo3Mhz
#define FUN_00462510 Set603868_0
#define FUN_00462500 Set603868_1
#define FUN_00426c00 GameStateFlagGet
#define FUN_0042f790 GhostModeIsActive
#define FUN_0042f7a0 GhostFlagReset
#define FUN_0042fe30 RaceEndFlagIfEndMode
#define FUN_0040d040 PortTop_StandInRL_0040d040
#define FUN_0040cf80 Orig_0040cf80
#define FUN_0040cfd0 Orig_0040cfd0
#define FUN_0040d020 Orig_0040d020
#define FUN_0040d110 PortTop_StandInRL_0040d110
#define FUN_00458c70 PortTop_RL_00458c70   // Match/PortTop_StandInsRL.cpp: the matched body when the RW core frame list exists (RACE-RWGLOBALS-FRAMELIST)
#define FUN_00426340 PortTop_StandInRL_00426340
#define FUN_00426020 GlobalDat00646e58Get
#define FUN_00407800 PortTop_StandInRL_00407800
#define FUN_004074a0 PortTop_StandInRL_004074a0
#define FUN_004264d0 PortTop_RL_004264d0   // Match/PortTop_StandInsRL.cpp: logs, then the matched body (checkpoint 3)
#define FUN_004ccde0 PortTop_RL_004ccde0
#define FUN_00426070 TrackPhysWorld2Get
#define FUN_00411d90 PortTop_StandInRL_00411d90
#define FUN_004987b0 DbgPrintf_004987b0
extern "C" {
#include "../../../../re/match/top07/0040d270.c"
}
#endif // MASHED_STANDALONE
