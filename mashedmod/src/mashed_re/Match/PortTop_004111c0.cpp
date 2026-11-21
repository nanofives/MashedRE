// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004111c0 = re/match/t01g/004111c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004111c0 PortTop_Bound_004111c0
#define FUN_00401570 PortTop_Bound_00401570
#define FUN_00403250 PortTop_Bound_00403250
#define FUN_004039f0 PortTop_RFStop_004039f0   // named stop
#define FUN_004046a0 PortTop_RFStop_004046a0   // named stop
#define FUN_00405460 PortTop_Bound_00405460
#define FUN_00406ce0 PortTop_RFStop_00406ce0   // named stop
#define FUN_004073b0 PortTop_Bound_004073b0
#define FUN_00407a60 UtilTableInit8a9620   // exe body (Match/MatchS01.cpp)
#define FUN_0040b180 PortTop_Bound_0040b180
#define FUN_0040bd80 PortTop_Bound_0040bd80
#define FUN_0040ce00 PortTop_Bound_0040ce00
#define FUN_0040d470 PortTop_Bound_0040d470
#define FUN_0040da50 PortTop_Bound_0040da50
#define FUN_0040dba0 PortTop_Bound_0040dba0
#define FUN_0040dbd0 PortTop_Bound_0040dbd0
#define FUN_0040dd80 PortTop_RFStop_0040dd80   // named stop
#define FUN_0040ddb0 PortTop_RFStop_0040ddb0   // named stop
#define FUN_0040ddf0 PortTop_RFStop_0040ddf0   // named stop
#define FUN_0040de00 PortTop_Bound_0040de00
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e460 Flag63b908Set   // exe body (Match/MatchS02_0040e460.cpp)
#define FUN_0040e480 CarSlotStateSet   // exe body (Match/MatchS02_0040e480.cpp)
#define FUN_0040e560 PortTop_Bound_0040e560
#define FUN_0040e590 PortTop_RFStop_0040e590   // named stop
#define FUN_0040feb0 PortTop_RFStop_0040feb0   // named stop
#define FUN_00410130 PortTop_RFStop_00410130   // named stop
#define FUN_004103a0 PortTop_RFStop_004103a0   // named stop
#define FUN_00410860 PortTop_Bound_00410860
#define FUN_00410ab0 PortTop_RFStop_00410ab0   // named stop
#define FUN_00411170 PortTop_Bound_00411170
#define FUN_00411ae0 PortTop_Bound_00411ae0
#define FUN_00411d60 PortTop_Bound_00411d60
#define FUN_00412f30 PortTop_RFStop_00412f30   // named stop
#define FUN_00413f20 PortTop_RFStop_00413f20   // named stop
#define FUN_00414060 PortTop_RFStop_00414060   // named stop
#define FUN_00414180 PortTop_RFStop_00414180   // named stop
#define FUN_00414220 PortTop_RFStop_00414220   // named stop
#define FUN_00418860 PortTop_RFStop_00418860   // named stop
#define FUN_00418990 PortTop_RFStop_00418990   // named stop
#define FUN_004189a0 PortTop_RFStop_004189a0   // named stop
#define FUN_00419760 PortTop_RFStop_00419760   // named stop
#define FUN_0041a250 PortTop_RFStop_0041a250   // named stop
#define FUN_0041b4d0 PortTop_RFStop_0041b4d0   // named stop
#define FUN_0041b510 PortTop_RFStop_0041b510   // named stop
#define FUN_0041b540 PortTop_RFStop_0041b540   // named stop
#define FUN_0041bf20 PortTop_Bound_0041bf20
#define FUN_0041c010 HudConstTableInitAndSweep   // exe body (Match/MatchS04_0041b770.cpp)
#define FUN_0041c090 PortTop_Bound_0041c090
#define FUN_0041c180 PortTop_RFStop_0041c180   // named stop
#define FUN_0041cb80 PortTop_Bound_0041cb80
#define FUN_0041cbc0 FloatTableInit   // exe body (Match/MatchS04_0041cbc0.cpp)
#define FUN_0041cc50 PortTop_Bound_0041cc50
#define FUN_0041d730 PortTop_Bound_0041d730
#define FUN_0041d820 TimerSlotClear   // exe body (Match/MatchS04_0041d820.cpp)
#define FUN_0041d830 PortTop_Bound_0041d830
#define FUN_0041d930 PortTop_Bound_0041d930
#define FUN_0041da90 DeltaTimeOutGet   // exe body (Match/MatchS04_0041da90.cpp)
#define FUN_0041dca0 PortTop_RFStop_0041dca0   // named stop
#define FUN_0041e130 TimerTrackSetter   // exe body (Match/MatchS04_0041e130.cpp)
#define FUN_0041e6c0 PortTop_RFStop_0041e6c0   // named stop
#define FUN_0041eda0 TimerBitFieldSet   // exe body (Match/MatchS04_0041eda0.cpp)
#define FUN_0041f000 TimerSlotDataCopy   // exe body (Match/MatchS04_0041f000.cpp)
#define FUN_00420d40 PortTop_Bound_00420d40
#define FUN_004215a0 PortTop_Bound_004215a0
#define FUN_00422120 PortTop_Bound_00422120
#define FUN_004222c0 PortTop_Bound_004222c0
#define FUN_004222d0 PortTop_Bound_004222d0
#define FUN_00422b10 TimerDwordClear   // exe body (Match/MatchS05_00422b10.cpp)
#define FUN_00425a40 PortTop_Bound_00425a40
#define FUN_00425b10 TimerGlobalZero   // exe body (Match/MatchS05_00425b10.cpp)
#define FUN_00425d50 PortTop_Bound_00425d50
#define FUN_00426630 PitchParamSet   // exe body (Util/TimerSetters.cpp)
#define FUN_004266f0 PitchParam2Set   // exe body (Util/TimerSetters.cpp)
#define FUN_00426ab0 PortTop_Bound_00426ab0
#define FUN_00426ba0 HudDrawEnabled   // exe body (Match/MatchS06_00426ba0.cpp)
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_00426c10 PortTop_Bound_00426c10
#define FUN_00426c30 PortTop_Bound_00426c30
#define FUN_00426c70 PortTop_Bound_00426c70
#define FUN_00426c90 PortTop_Bound_00426c90
#define FUN_004292c0 UtilWritePtr   // exe body (Match/MatchS06_004292c0.cpp)
#define FUN_004292d0 FUN_004292d0   // exe body (Match/MatchS06_004292d0.cpp)
#define FUN_00429310 PortTop_Bound_00429310
#define FUN_00429840 RaceStateLatchSet   // exe body (Match/MatchS06_00429840.cpp)
#define FUN_00429860 PortTop_Bound_00429860
#define FUN_0042aab0 UtilInit898a90   // exe body (Match/MatchS06_0042aab0.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00430820 PortTop_Bound_00430820
#define FUN_004425d0 UtilZeroTable8964c0   // exe body (Match/MatchS08_004425d0.cpp)
#define FUN_004430a0 PortTop_Bound_004430a0
#define FUN_004430b0 PortTop_Bound_004430b0
#define FUN_00448700 PortTop_Bound_00448700
#define FUN_00458bf0 PortTop_Bound_00458bf0
#define FUN_00459560 PortTop_Bound_00459560
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_0045b9d0 PortTop_RFStop_0045b9d0   // named stop
#define FUN_0045ba00 RaceResultIndexedStore   // exe body (Match/MatchS10_0045ba00.cpp)
#define FUN_0045bba0 Orig_0045bba0   // exe body (Match/PortTop_0045bae0.cpp)
#define FUN_0045be90 PortTop_RFStop_0045be90   // named stop
#define FUN_0045bed0 PortTop_RFStop_0045bed0   // named stop
#define FUN_00467210 PortTop_Bound_00467210
#define FUN_0046b1c0 PortTop_RFStop_0046b1c0   // named stop
#define FUN_0046b540 PortTop_RFStop_0046b540   // named stop
#define FUN_0046baa0 VehiclePhysicsStateReset   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c5c0 VehicleSlotInit   // exe body (Match/MatchS11_0046c5c0.cpp)
#define FUN_0046c730 EntityDamageStateGet   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c750 EntityVelocityCounterGet   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_0046d780 PortTop_Bound_0046d780
#define FUN_0046d7f0 BoundedTableSignClamp46d7f0   // exe body (Vehicle/LaunchRevCharge.cpp)
#define FUN_00471ac0 PortTop_Bound_00471ac0
#define FUN_00471cf0 PortTop_Bound_00471cf0
#define FUN_00475a60 PortTop_Bound_00475a60
#define FUN_00475c10 PortTop_Bound_00475c10
#define FUN_00476430 PortTop_Bound_00476430
#define FUN_00476440 PortTop_Bound_00476440
#define FUN_00477730 PortTop_Bound_00477730
#define FUN_004777d0 PortTop_Bound_004777d0
#define FUN_00477920 PortTop_Bound_00477920
#define FUN_00477b40 Orig_00477b40   // exe body (Match/PortTop_00477b40.cpp)
#define FUN_00480100 PortTop_Bound_00480100   // exe body (Match/PortTop_00480100.cpp)
#define FUN_00484c90 Zero484c90   // exe body (Match/MatchS14_00484c90.cpp)
#define FUN_00485070 PortTop_Bound_00485070
#define FUN_00488e70 PortTop_RFStop_00488e70   // named stop
#define FUN_0048f260 PortTop_Bound_0048f260
#define FUN_00490380 PortTop_Bound_00490380
#define FUN_004904d0 UtilZeroTable86a4a0   // exe body (Match/MatchS15_004904d0.cpp)
#define FUN_00490500 PortTop_Bound_00490500
#define FUN_00496930 Table773030Get   // exe body (Match/MatchS16_00496930.cpp)
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_0040fc00 PortTop_Local_004111c0_0040fc00   // file-local in this source
#define FUN_004102f0 PortTop_Local_004111c0_004102f0   // file-local in this source
extern "C" {
#include "../../../../re/match/t01g/004111c0.c"
}
#endif // MASHED_STANDALONE
