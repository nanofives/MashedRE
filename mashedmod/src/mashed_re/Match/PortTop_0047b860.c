// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047b860 = re/match/s13/0047b860.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2660 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047b860 PortTop_Bound_0047b860
#define FUN_004b7330(...) lua_open()   // exe body (lib:lua-4.0:src/lstate.c:66); the source passes 1 argument(s), its prototype declares 0
#define FUN_004c0510 lua_mathlibopen   // exe body (lib:lua-4.0:src/lib/lmathlib.c:231)
#define MATCH_PREAMBLE 1   // C unit: the source preamble is replaced by these lines (its MGLOBAL is `extern T n` in C, unresolved in the exe)
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef unsigned char bool;
#define MGLOBAL(T, n, a) enum { mglobal_##n }   // the name is a #define at its original address below
#define MGLOBALA(T, n, N, a) enum { mglobala_##n }
#define DAT_006bf1e0 (*(undefined4 *)0x006bf1e0u)   // 0x0047b868 mov [0x6bf1e0], eax
#undef FUN_004b7330
#define RL_ARG_void   // the source declaration `int FUN_004b7330(void)` becomes the non-prototype `int lua_open()` (VS2022 C refuses a 1-argument call against (void): C2197)
#define RL_ARG_0 0   // the source call `FUN_004b7330(0)` becomes `lua_open(0)`: 0x0047b860 push 0 / 0x0047b862 call 0x4b7330
#define FUN_004b7330(...) lua_open(RL_ARG_##__VA_ARGS__)   // lib:lua-4.0:src/lstate.c:66 lua_open(int stacksize); the argument is kept
#include "../../../../re/match/s13/0047b860.c"
#endif // MASHED_STANDALONE
