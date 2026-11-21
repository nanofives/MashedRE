// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00544e00 = re/match/rwtb/00544e00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00544e00 PortTop_Bound_00544e00
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#include "../../../../re/match/rwtb/00544e00.c"
void *PortTop_Keep_00544e00 = (void *)PortTop_Bound_00544e00;   // anchor: Match/PortTop_BulkRefs.cpp /include (no bound caller yet)
#endif // MASHED_STANDALONE
