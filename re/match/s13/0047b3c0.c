// FLAGS: /O2 /GS /Ob1
/* 0x0047b3c0 (Lua C function, list of ints to FUN_00471490 between 0x00471430/0x00471450): 100% (regs 100%),
   relocheck OK. From raw 68.6%: for loop with i initialised before gettop, tonumber as double cast (int).
   Residual: none. */
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
int FUN_004b6fc0(lua_State *L);
double FUN_004b7090(lua_State *L, int idx);
void FUN_00471430(void);
void FUN_00471450(void);
void FUN_00471490(int id);
// MATCH: 0x0047b3c0 FUN_0047b3c0
int FUN_0047b3c0(lua_State *L)
{
  int buf[64];
  int i;
  int n;
  i = 0;
  n = FUN_004b6fc0(L);
  FUN_00471430();
  for (; i < n; i++) {
    buf[i] = (int)FUN_004b7090(L, i + 1);
    FUN_00471490(buf[i]);
  }
  FUN_00471450();
  return 0;
}
