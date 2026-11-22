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
#define FUN_0047a180 PortTop_Bound_0047a180
#define FUN_0047a1b0 PortTop_Bound_0047a1b0
#define FUN_0047a1e0 PortTop_Bound_0047a1e0
#define FUN_0047a280 PortTop_Bound_0047a280
#define FUN_0047a320 PortTop_Bound_0047a320
#define FUN_0047a3a0 PortTop_Bound_0047a3a0
#define FUN_0047a440 PortTop_Bound_0047a440
#define FUN_0047a4a0 PortTop_Bound_0047a4a0
#define FUN_0047a540 PortTop_Bound_0047a540
#define FUN_0047a580 PortTop_Bound_0047a580
#define FUN_0047a5b0 PortTop_Bound_0047a5b0
#define FUN_0047a5e0 PortTop_Bound_0047a5e0
#define FUN_0047a610 PortTop_Bound_0047a610
#define FUN_0047a6b0 PortTop_Bound_0047a6b0
#define FUN_0047a6f0 PortTop_Bound_0047a6f0
#define FUN_0047a720 PortTop_Bound_0047a720
#define FUN_0047a750 PortTop_Bound_0047a750
#define FUN_0047a790 PortTop_Bound_0047a790
#define FUN_0047a7d0 PortTop_Bound_0047a7d0
#define FUN_0047a7f0 PortTop_Bound_0047a7f0
#define FUN_0047a810 PortTop_Bound_0047a810
#define FUN_0047a880 PortTop_Bound_0047a880
#define FUN_0047a8b0 PortTop_Bound_0047a8b0
#define FUN_0047a950 PortTop_Bound_0047a950
#define FUN_0047a9b0 PortTop_Bound_0047a9b0
#define FUN_0047aa20 PortTop_Bound_0047aa20
#define FUN_0047aa50 PortTop_Bound_0047aa50
#define FUN_0047aaa0 PortTop_Bound_0047aaa0
#define FUN_0047aad0 PortTop_Bound_0047aad0
#define FUN_0047ab30 PortTop_Bound_0047ab30
#define FUN_0047abd0 PortTop_Bound_0047abd0
#define FUN_0047ac80 PortTop_Bound_0047ac80
#define FUN_0047acd0 PortTop_Bound_0047acd0
#define FUN_0047ad00 PortTop_Bound_0047ad00
#define FUN_0047ad40 PortTop_Bound_0047ad40
#define FUN_0047ade0 PortTop_Bound_0047ade0
#define FUN_0047ae80 PortTop_Bound_0047ae80
#define FUN_0047aef0 PortTop_Bound_0047aef0
#define FUN_0047af50 PortTop_Bound_0047af50
#define FUN_0047afa0 PortTop_Bound_0047afa0
#define FUN_0047b000 PortTop_Bound_0047b000
#define FUN_0047b0a0 PortTop_Bound_0047b0a0
#define FUN_0047b110 PortTop_Bound_0047b110
#define FUN_0047b160 PortTop_Bound_0047b160
#define FUN_0047b1b0 PortTop_Bound_0047b1b0
#define FUN_0047b200 PortTop_Bound_0047b200
#define FUN_0047b230 PortTop_Bound_0047b230
#define FUN_0047b250 PortTop_Bound_0047b250
#define FUN_0047b280 PortTop_Bound_0047b280
#define FUN_0047b350 PortTop_Bound_0047b350
#define FUN_0047b3c0 PortTop_Bound_0047b3c0
#define FUN_0047b420 PortTop_Bound_0047b420
#define FUN_0047b480 PortTop_Bound_0047b480
#define FUN_0047b500 PortTop_Bound_0047b500
#define FUN_0047b5b0 PortTop_Bound_0047b5b0
#define FUN_0047b630 PortTop_Bound_0047b630
#define FUN_0047b6b0 PortTop_Bound_0047b6b0
#define FUN_0047b720 PortTop_Bound_0047b720
#define FUN_0047b7d0 PortTop_Bound_0047b7d0
#define FUN_0047b980 PortTop_Bound_0047b980
#define FUN_004917c0 PortTop_Bound_004917c0
#define FUN_00491860 PortTop_Bound_00491860
#define FUN_00491900 PortTop_Bound_00491900
#define FUN_004919b0 PortTop_Bound_004919b0
#define FUN_00491a10 PortTop_Bound_00491a10
#define FUN_00491a70 PortTop_Bound_00491a70
#define FUN_005c9d00 GetRaceEndTrigger   // exe body (Match/PortBatch_GAMEPLAY_005c9d00.cpp)
#include "../../../../re/match/tlr/00440bc0.c"
#endif // MASHED_STANDALONE
