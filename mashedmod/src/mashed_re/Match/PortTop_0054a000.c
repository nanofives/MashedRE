// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0054a000 = re/match/g2sh/0054a000.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0054a000 PortTop_Bound_0054a000
#define FUN_005495b0 RwCore_005495b0   // exe body (RwCore/RwCore_005495b0.cpp)
#include "../../../../re/match/g2sh/0054a000.c"
#endif // MASHED_STANDALONE
