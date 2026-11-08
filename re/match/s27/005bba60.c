// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bba60: 64.9% (regs 100.0% renamed). Hand: 4-arg cdecl callees, args forwarded. Residual (a): original loads b,c,d then pushes d before computing [esi+0x4c]/lea (arg schedule + register choice); struct form, r= merge, return-a-first, permuter 84 compiles did not move it. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef unsigned char byte;
int FUN_005abd30(int p0, int p1, int p2, int p3);
int FUN_005bc050(int p0, int p1, int p2, int p3);
// MATCH: 0x005bba60 FUN_005bba60
int FUN_005bba60(int a, int b, int c, int d)
{
  if (*(byte *)(*(int *)(*(int *)(a + 0x58) + 0x20) + 0x50) & 8) {
    if (FUN_005abd30(*(int *)(a + 0x4c), b, c, d) == 0) return 0;
  } else {
    if (FUN_005bc050(a + 0x5c, b, c, d) == 0) return 0;
  }
  return a;
}
