// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047aaa0 = re/match/s12/0047aaa0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2664 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047aaa0 PortTop_Bound_0047aaa0
#define FUN_004b6fc0 lua_gettop   // exe body (lib:lua-4.0:src/lapi.c:68)
#define FUN_004b70d0 lua_tostring   // exe body (lib:lua-4.0:src/lapi.c:162)
#define MATCH_PREAMBLE 1   // C unit: the source preamble is replaced by these lines (its MGLOBAL is `extern T n` in C, unresolved in the exe)
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef unsigned char bool;
#define MGLOBAL(T, n, a) enum { mglobal_##n }   // the name is a #define at its original address below
#define MGLOBALA(T, n, N, a) enum { mglobala_##n }
#define DAT_006bf1c8 (*(int *)0x006bf1c8u)   // MGLOBAL re/match/s12/0047aaa0.c:27
#include "../../../../re/match/s12/0047aaa0.c"
#endif // MASHED_STANDALONE
