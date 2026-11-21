// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00557d90 = re/match/rwta/00557d90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00557d90 PortTop_Bound_00557d90
#define FUN_004e8ea0 RwCore_004e8ea0   // exe body (RwCore/RwCore_004e8ea0.cpp)
#define FUN_00557a60 PortTop_RLStop_00557a60   // named stop
#include "../../../../re/match/rwta/00557d90.c"
#endif // MASHED_STANDALONE
