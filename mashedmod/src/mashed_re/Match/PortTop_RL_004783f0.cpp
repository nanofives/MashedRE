// RVA-LINT: ORIGTOP-ONLY
// RACE-LOAD-004E99B0 fix round (run 57, L-184): exe body of 0x004783f0 compiled from the source the replay verified,
// re/match/pilot/a4_004783f0.c (verify/provinsitu_ref1_20261121/PROMOTIONS.tsv line 41: IDENTICAL-FOR-600, run
// b1_023950_016_4, link 90.8%; hooks.csv C4 c4=frida-replay). The binder's default source for a replay row is the one
// re/match/*/004783f0.c file, re/match/s12/004783f0.c, a different unit, so the raceload profile binds 0x004783f0 to
// this TU (`alias` in re/tools/bind/state/raceload/rules.tsv). Callees bind to the same exe bodies the generated
// wrapper named. Reached only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD.
// Evidence: verify/raceload_99b0_20261127/RESULT_RACE-LOAD-004E99B0.md.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004783f0 PortTop_RL_004783f0
#define FUN_00409300 PortTop_Bound_00409300
#define FUN_00409330 PortTop_Bound_00409330
#define FUN_004a3384 MashedCrt_acos_004a3384   // exe body (Match/PortTop_CrtMath.cpp)
#define FUN_004c39b0 RwCore_004c39b0           // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004c3b30 RwCore_004c3b30           // exe body (RwCore/RwCore_004c3b30.cpp)
extern "C" {
#include "../../../../re/match/pilot/a4_004783f0.c"
}
#endif // MASHED_STANDALONE
