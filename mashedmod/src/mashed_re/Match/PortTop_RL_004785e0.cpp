// RVA-LINT: ORIGTOP-ONLY
// RACE-LOAD-004E99B0 (run 57, L-184): stack-argument entry for 0x004785e0, a register-argument function in the original
// (p in EAX, out in ECX, norm on the stack; re/match/s12/004785e0.c header). This TU compiles that source as is (its
// FUN_004785e0 is static, hooks.csv C4 c4=frida-replay) and exposes it as PortTop_RL_004785e0(p, out, norm), the form
// re/match/s12/00478660.c declares under MATCH_PORT. The raceload profile binds 0x004785e0 to it (`alias` in
// re/tools/bind/state/raceload/rules.tsv). Reached only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD.
// Evidence: verify/raceload_99b0_20261127/RESULT_RACE-LOAD-004E99B0.md.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c39b0 RwCore_004c39b0                      // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004785e0_caller PortTop_RL_004785e0_StandIn   // the source's stand-in caller, unused here
extern "C" {
#include "../../../../re/match/s12/004785e0.c"
// 0x004785e0
void PortTop_RL_004785e0(float *p, float *out, int norm) { FUN_004785e0(p, out, norm); }
}
#endif // MASHED_STANDALONE
