// FLAGS: /O2 /GS /Ob1
/* 0x0047b7d0 (Lua C function, int + vec3 + int + float -> FUN_00405730): 100% (regs 100%), relocheck OK.
   From raw 78.6%: float v[3] local passed by pointer, tonumber cast (float)/(int) per arg, float temp f for
   the last arg. Residual: none. */
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
void FUN_00405730(int a, float *v, int b, float f);
// MATCH: 0x0047b7d0 FUN_0047b7d0
int FUN_0047b7d0(lua_State *L)
{
  int n;
  int a, b;
  float v[3];
  float f;
  n = FUN_004b6fc0(L);
  a = (int)FUN_004b7090(L, n - 5);
  v[0] = (float)FUN_004b7090(L, n - 4);
  v[1] = (float)FUN_004b7090(L, n - 3);
  v[2] = (float)FUN_004b7090(L, n - 2);
  b = (int)FUN_004b7090(L, n - 1);
  f = (float)FUN_004b7090(L, n);
  FUN_00405730(a, v, b, f);
  return 0;
}
