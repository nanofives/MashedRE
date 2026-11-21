// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00440bc0 = re/match/tlr/00440bc0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00440bc0 PortTop_Bound_00440bc0
#define FUN_0047a180 PortTop_RLStop_0047a180   // named stop
#define FUN_0047a1b0 PortTop_RLStop_0047a1b0   // named stop
#define FUN_0047a1e0 PortTop_RLStop_0047a1e0   // named stop
#define FUN_0047a280 PortTop_RLStop_0047a280   // named stop
#define FUN_0047a320 PortTop_RLStop_0047a320   // named stop
#define FUN_0047a3a0 PortTop_RLStop_0047a3a0   // named stop
#define FUN_0047a440 PortTop_RLStop_0047a440   // named stop
#define FUN_0047a4a0 PortTop_RLStop_0047a4a0   // named stop
#define FUN_0047a540 PortTop_RLStop_0047a540   // named stop
#define FUN_0047a580 PortTop_RLStop_0047a580   // named stop
#define FUN_0047a5b0 PortTop_RLStop_0047a5b0   // named stop
#define FUN_0047a5e0 PortTop_RLStop_0047a5e0   // named stop
#define FUN_0047a610 PortTop_RLStop_0047a610   // named stop
#define FUN_0047a6b0 PortTop_RLStop_0047a6b0   // named stop
#define FUN_0047a6f0 PortTop_RLStop_0047a6f0   // named stop
#define FUN_0047a720 PortTop_RLStop_0047a720   // named stop
#define FUN_0047a750 PortTop_RLStop_0047a750   // named stop
#define FUN_0047a790 PortTop_RLStop_0047a790   // named stop
#define FUN_0047a7d0 PortTop_RLStop_0047a7d0   // named stop
#define FUN_0047a7f0 PortTop_RLStop_0047a7f0   // named stop
#define FUN_0047a810 PortTop_RLStop_0047a810   // named stop
#define FUN_0047a880 PortTop_RLStop_0047a880   // named stop
#define FUN_0047a8b0 PortTop_RLStop_0047a8b0   // named stop
#define FUN_0047a950 PortTop_RLStop_0047a950   // named stop
#define FUN_0047a9b0 PortTop_RLStop_0047a9b0   // named stop
#define FUN_0047aa20 PortTop_RLStop_0047aa20   // named stop
#define FUN_0047aa50 PortTop_RLStop_0047aa50   // named stop
#define FUN_0047aaa0 PortTop_RLStop_0047aaa0   // named stop
#define FUN_0047aad0 PortTop_RLStop_0047aad0   // named stop
#define FUN_0047ab30 PortTop_RLStop_0047ab30   // named stop
#define FUN_0047abd0 PortTop_RLStop_0047abd0   // named stop
#define FUN_0047ac80 PortTop_RLStop_0047ac80   // named stop
#define FUN_0047acd0 PortTop_RLStop_0047acd0   // named stop
#define FUN_0047ad00 PortTop_RLStop_0047ad00   // named stop
#define FUN_0047ad40 PortTop_RLStop_0047ad40   // named stop
#define FUN_0047ade0 PortTop_RLStop_0047ade0   // named stop
#define FUN_0047ae80 PortTop_RLStop_0047ae80   // named stop
#define FUN_0047aef0 PortTop_RLStop_0047aef0   // named stop
#define FUN_0047af50 PortTop_RLStop_0047af50   // named stop
#define FUN_0047afa0 PortTop_RLStop_0047afa0   // named stop
#define FUN_0047b000 PortTop_RLStop_0047b000   // named stop
#define FUN_0047b0a0 PortTop_RLStop_0047b0a0   // named stop
#define FUN_0047b110 PortTop_RLStop_0047b110   // named stop
#define FUN_0047b160 PortTop_RLStop_0047b160   // named stop
#define FUN_0047b1b0 PortTop_RLStop_0047b1b0   // named stop
#define FUN_0047b200 PortTop_RLStop_0047b200   // named stop
#define FUN_0047b230 PortTop_RLStop_0047b230   // named stop
#define FUN_0047b250 PortTop_RLStop_0047b250   // named stop
#define FUN_0047b280 PortTop_RLStop_0047b280   // named stop
#define FUN_0047b350 PortTop_RLStop_0047b350   // named stop
#define FUN_0047b3c0 PortTop_RLStop_0047b3c0   // named stop
#define FUN_0047b420 PortTop_RLStop_0047b420   // named stop
#define FUN_0047b480 PortTop_RLStop_0047b480   // named stop
#define FUN_0047b500 PortTop_RLStop_0047b500   // named stop
#define FUN_0047b5b0 PortTop_RLStop_0047b5b0   // named stop
#define FUN_0047b630 PortTop_RLStop_0047b630   // named stop
#define FUN_0047b6b0 PortTop_RLStop_0047b6b0   // named stop
#define FUN_0047b720 PortTop_RLStop_0047b720   // named stop
#define FUN_0047b7d0 PortTop_RLStop_0047b7d0   // named stop
#define FUN_0047b980 PortTop_RLStop_0047b980   // named stop
#define FUN_004917c0 PortTop_RLStop_004917c0   // named stop
#define FUN_00491860 PortTop_RLStop_00491860   // named stop
#define FUN_00491900 PortTop_RLStop_00491900   // named stop
#define FUN_004919b0 PortTop_RLStop_004919b0   // named stop
#define FUN_00491a10 PortTop_RLStop_00491a10   // named stop
#define FUN_00491a70 PortTop_RLStop_00491a70   // named stop
#define FUN_005c9d00 GetRaceEndTrigger   // exe body (Match/PortBatch_GAMEPLAY_005c9d00.cpp)
#include "../../../../re/match/tlr/00440bc0.c"
#endif // MASHED_STANDALONE
