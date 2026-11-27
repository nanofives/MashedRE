// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x005518f0 = re/match/rwtb/005518f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005518f0 PortTop_Bound_005518f0
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004cc790 RwCore_004cc790   // exe body (RwCore/RwCore_004cc790.cpp)
#define FUN_0052ddc0 RwCore_0052ddc0   // exe body (RwCore/RwCore_0052ddc0.cpp)
#define FUN_0052ddf0 RwCore_0052ddf0   // exe body (RwCore/RwCore_0052ddf0.cpp)
#define FUN_0052de60 PortTop_Bound_0052de60   // exe body (Match/PortTop_0052de60.cpp)
#define FUN_00551a50 Orig_00551a50   // exe body (Match/PortTop_00551a50.cpp)
#define FUN_00551ad0 PortTop_RLStop_00551ad0   // named stop
#define FUN_00551fd0 RwCore_00551fd0   // exe body (RwCore/RwCore_00551fd0.cpp)
#define FUN_00558cb0 PortTop_Bound_00558cb0
#define FUN_00562560 RwCore_00562560   // exe body (RwCore/RwCore_00562560.cpp)
#include "../../../../re/match/rwtb/005518f0.c"
#endif // MASHED_STANDALONE
