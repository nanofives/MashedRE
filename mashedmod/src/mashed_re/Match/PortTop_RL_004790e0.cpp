// RVA-LINT: ORIGTOP-ONLY
// RACE-LOAD-004E99B0 (run 57, L-184): stack-argument entry for 0x004790e0, a register-argument function in the original
// (world pointer in EAX; its one caller 0x00479330 loads [ebx+0x105d4] into EAX at 0x00479e28..0x00479e2e). This TU
// compiles the C4 source re/match/s12/004790e0.c as is (its FUN_004790e0 is static) and exposes it as
// PortTop_RL_004790e0(world). The raceload profile binds 0x004790e0 to it (`alias` in re/tools/bind/state/raceload/rules.tsv),
// and re/match/s12/00479330.c passes the world pointer under MATCH_PORT. Reached only under MASHED_ORIG_TOP +
// MASHED_OT_RACELOAD. Evidence: verify/raceload_99b0_20261127/RESULT_RACE-LOAD-004E99B0.md.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e5c70 RwCore_004e5c70          // exe body (RwCore/RwCore_004e5c70.cpp)
#define FUN_00479030 PortTop_Bound_00479030   // exe body (Match/PortTop_00479030.cpp)
#define FUN_004790e0_caller PortTop_RL_004790e0_StandIn   // the source's stand-in caller, unused here
extern "C" {
#include "../../../../re/match/s12/004790e0.c"
// 0x004790e0
void PortTop_RL_004790e0(int world) { FUN_004790e0(world); }
}
#endif // MASHED_STANDALONE
