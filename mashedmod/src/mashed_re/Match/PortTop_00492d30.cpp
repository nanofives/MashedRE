// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x00492d30 = the matched source re/match/s15/00492d30.c
// (match.py 100%), the 7-case frame machine on DAT_00771968 that the run function 0x00492290 calls once per frame.
// Reached only on the MASHED_ORIG_TOP path (Match/PortTop_00492370.cpp, PortTop_Bound_00492d30). Every callee binds by
// name, no raw-address call:
//   0x004950b0  QpcTimeScaledTo3Mhz        matched exe body, Match/MatchS16_004950b0.cpp
//   0x0045b350  RwInitNullStub             matched exe body, Match/MatchS09_0045b350.cpp
//   0x005c9d00  GetRaceEndTrigger          exe body, Util/GameStateGetters.cpp
//   0x0042b930  MenuAlphaGet               matched exe body, Match/MatchS07_0042b930.cpp
//   0x004030d0  Orig_004030d0              matched, Match/PortTop_004030d0.cpp (PORT-TOP-05)
//   0x004111c0  PortTop_StandIn05_004111c0 stand-in (counts calls): the race tick of the knob path is the hand session,
//                                          run by the wrapper after this body (PortTop_HandFrameGameFlow)
//   0x0043dfd0  PortTop_StandIn05_0043dfd0 stand-in: the hand menu navigation + input globals (HandTop_FrameMenu /
//                                          HandTop_FrameInputGlobals). The provisional body (Match/ProvS08_0043dfd0.cpp,
//                                          PROVISIONAL.tsv) has 63 counting stubs and is not wired here.
//   0x0043d7c0  Orig_0043d7c0              matched, Match/PortTop_0043d7c0.cpp (PORT-TOP-05)
//   0x00496920  TimerTable772ffcGet        matched exe body, Match/MatchS16_00496920.cpp (exe since PORT-TOP-05)
//   0x004671a0  BackBuffRasterOpen         matched exe body, Match/MatchS11_004671a0.cpp
//   0x00498860  PortTop_Bound05_00498860   -> Orig_00498860, matched, Match/PortTop_00498860.cpp (PORT-TOP-05)
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00492d30 Orig_00492d30
#define FUN_004950b0 QpcTimeScaledTo3Mhz
#define FUN_0045b350 RwInitNullStub
#define FUN_005c9d00 GetRaceEndTrigger
#define FUN_0042b930 MenuAlphaGet
#define FUN_004030d0 Orig_004030d0
#define FUN_004111c0 PortTop_StandIn05_004111c0
#define FUN_0043dfd0 PortTop_StandIn05_0043dfd0
#define FUN_0043d7c0 Orig_0043d7c0
#define FUN_00496920 TimerTable772ffcGet
#define FUN_004671a0 BackBuffRasterOpen
#define FUN_00498860 PortTop_Bound05_00498860
extern "C" {
#include "../../../../re/match/s15/00492d30.c"
}
#endif // MASHED_STANDALONE
