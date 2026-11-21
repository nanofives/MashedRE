// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047b8d0 = re/match/s13/0047b8d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2664 (state/raceload/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047b8d0 PortTop_Bound_0047b8d0
#define FUN_0047b8a0 PortTop_RLStop_0047b8a0   // named stop
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_005507b0 RwCore_005507b0   // exe body (RwCore/RwCore_005507b0.cpp)
#define FUN_00550910 RwCore_00550910   // exe body (RwCore/RwCore_00550910.cpp)
#define FUN_00550950 RwCore_00550950   // exe body (RwCore/RwCore_00550950.cpp)
#define FUN_005509b0 RwCore_005509b0   // exe body (RwCore/RwCore_005509b0.cpp)
#define FUN_00550bc0 RwCore_00550bc0   // exe body (RwCore/RwCore_00550bc0.cpp)
#define MATCH_PREAMBLE 1   // C unit: the source preamble is replaced by these lines (its MGLOBAL is `extern T n` in C, unresolved in the exe)
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef unsigned char bool;
#define MGLOBAL(T, n, a) enum { mglobal_##n }   // the name is a #define at its original address below
#define MGLOBALA(T, n, N, a) enum { mglobala_##n }
#define DAT_00616038 (*(uint *)0x00616038u)
#define DAT_005cf010 (*(undefined1 *)0x005cf010u)   // 0x0047b905 push 0x5cf010
#define thunk_FUN_005507b0 RwCore_005507b0   // 0x0047b90b call 0x4b5360; 0x004b5360 jmp 0x5507b0
#define thunk_FUN_005509b0 RwCore_005509b0   // 0x0047b917 / 0x0047b929 call 0x4b5390; 0x004b5390 jmp 0x5509b0
#define thunk_FUN_00550bc0 RwCore_00550bc0   // 0x0047b91d call 0x4b53a0; 0x004b53a0 jmp 0x550bc0
#define thunk_FUN_00550950 RwCore_00550950   // 0x0047b937 call 0x4b5380; 0x004b5380 jmp 0x550950
#define thunk_FUN_00550910 RwCore_00550910   // 0x0047b93f call 0x4b5370; 0x004b5370 jmp 0x550910
#include "../../../../re/match/s13/0047b8d0.c"
#endif // MASHED_STANDALONE
