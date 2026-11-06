// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047b1b0 (h1): 100% (regs 100%). Lua C function: three float args into 0x006bf1bc/c0/c4. Raw 78.6%.
   What moved it: float-typed globals. Residual: none. */
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
MGLOBAL(float, DAT_006bf1bc, 0x006bf1bc);
MGLOBAL(float, DAT_006bf1c0, 0x006bf1c0);
MGLOBAL(float, DAT_006bf1c4, 0x006bf1c4);
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);

// MATCH: 0x0047b1b0 FUN_0047b1b0
int FUN_0047b1b0(void *L)
{
  int top;

  top = FUN_004b6fc0(L);
  DAT_006bf1bc = (float)FUN_004b7090(L, top - 2);
  DAT_006bf1c0 = (float)FUN_004b7090(L, top - 1);
  DAT_006bf1c4 = (float)FUN_004b7090(L, top);
  return 0;
}
