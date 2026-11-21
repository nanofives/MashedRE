// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004cac10 = re/match/rwtb/004cac10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004cac10 PortTop_Bound_004cac10
#define FUN_004c9ad0 RwCore_004c9ad0   // exe body (RwCore/RwCore_004c9ad0.cpp)
#define FUN_004d1e30 RwCore_004d1e30   // exe body (RwCore/RwCore_004d1e30.cpp)
#define FUN_004d6200 RwCore_004d6200   // exe body (RwCore/RwCore_004d6200.cpp)
#define FUN_004db3e0 RwCore_004db3e0   // exe body (RwCore/RwCore_004db3e0.cpp)
#define FUN_004dc8a0 RwCore_004dc8a0   // exe body (RwCore/RwCore_004dc8a0.cpp)
#define FUN_004dc970 RwCore_004dc970   // exe body (RwCore/RwCore_004dc970.cpp)
#define FUN_004e0920 RwCore_004e0920   // exe body (RwCore/RwCore_004e0920.cpp)
#include "../../../../re/match/rwtb/004cac10.c"
#endif // MASHED_STANDALONE
