// FLAGS: /O2 /GS /Ob1
/* 0x0047b280 (Lua C function, 7 numeric args -> 5-int record): 100% (regs 100%), relocheck OK.
   From raw 89.9%: lua_tonumber (0x004b7090) typed as returning double and cast (int) inline at each
   read (the __ftol 0x004a2c48 calls). Residual: none. */
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
int *FUN_00430250(int a, int b);
// MATCH: 0x0047b280 FUN_0047b280
int FUN_0047b280(lua_State *L)
{
  int n;
  int a, b, c, d, e, f, g;
  int *p;
  n = FUN_004b6fc0(L);
  a = (int)FUN_004b7090(L, n - 6);
  b = (int)FUN_004b7090(L, n - 5);
  c = (int)FUN_004b7090(L, n - 4);
  d = (int)FUN_004b7090(L, n - 3);
  e = (int)FUN_004b7090(L, n - 2);
  f = (int)FUN_004b7090(L, n - 1);
  g = (int)FUN_004b7090(L, n);
  p = FUN_00430250(a, b);
  if (p != 0) {
    p[0] = c;
    p[1] = d;
    p[2] = e;
    p[3] = f;
    p[4] = g;
  }
  return 0;
}
