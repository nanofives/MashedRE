// FLAGS: /O2 /GS /Ob1
/* 0x0047b980 (lua_register shape: FUN_004b7200(L, fn, 0) then tail call FUN_004b7250(L, name),
   L = DAT_006bf1e0): 100% (regs 100%), relocheck OK. From raw 66.7%: the second call (Ghidra lost it as a
   thunk) is the 2-arg tail call, global reloaded for it. Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State *L);
MGLOBAL(lua_State *, DAT_006bf1e0, 0x006bf1e0);
void FUN_004b7200(lua_State *L, lua_CFunction fn, int n);
void FUN_004b7250(lua_State *L, const char *name);
// MATCH: 0x0047b980 FUN_0047b980
void FUN_0047b980(const char *name, lua_CFunction fn)
{
  FUN_004b7200(DAT_006bf1e0, fn, 0);
  FUN_004b7250(DAT_006bf1e0, name);
}
