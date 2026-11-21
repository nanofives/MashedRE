// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0041a8d0 = re/match/s03/h1_0041a4a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2065 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041a8d0 PortTop_Bound_0041a8d0
#define FUN_0041a5b0 PortTop_RLStop_0041a5b0   // named stop
#define FUN_0041a5d0 PortTop_RLStop_0041a5d0   // named stop
#define FUN_0041f260 PortTop_RLStop_0041f260   // named stop
#define FUN_0042a5d0 PortTop_Bound_0042a5d0
#define FUN_004b51d0 Orig_004b51d0   // exe body (Match/PortTop_004b51d0.cpp)
#define FUN_004b5260 PortTop_RLStop_004b5260   // named stop
#define FUN_004b52f0 RwCore_004b52f0   // exe body (RwCore/RwCore_004b52f0.cpp)
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_004c39b0 RwCore_004c39b0   // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
#define FUN_0041a400 PortTop_Local_0041a8d0_0041a400   // file-local in this source
#define FUN_0041a4a0 PortTop_Local_0041a8d0_0041a4a0   // file-local in this source
#define FUN_0041a500 PortTop_Local_0041a8d0_0041a500   // file-local in this source
#define FUN_0041a550 PortTop_Local_0041a8d0_0041a550   // file-local in this source
#define FUN_0041a840 PortTop_Local_0041a8d0_0041a840   // file-local in this source
#define FUN_0041a9d0 PortTop_Local_0041a8d0_0041a9d0   // file-local in this source
#define FUN_0041aac0 PortTop_Local_0041a8d0_0041aac0   // file-local in this source
#define FUN_0041ac60 PortTop_Local_0041a8d0_0041ac60   // file-local in this source
#define FUN_0041ad00 PortTop_Local_0041a8d0_0041ad00   // file-local in this source
#include "../../../../re/match/s03/h1_0041a4a0.c"
void *PortTop_Keep_0041a8d0 = (void *)PortTop_Bound_0041a8d0;   // anchor: Match/PortTop_BulkRefs.cpp /include (no bound caller yet)
#endif // MASHED_STANDALONE
