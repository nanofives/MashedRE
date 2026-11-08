// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005adef0 (h3): 100.0% exact (regs 100.0%). if (cb) { if (cb(..)) n++; } else n++; inside the for loop (the || form rotated the loop). */
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
typedef unsigned int undefined4;
typedef int code();
// MATCH: 0x005adef0 FUN_005adef0
int FUN_005adef0(undefined4 *param_1,int (*param_2)(undefined4 *,undefined4),undefined4 param_3)
{
  undefined4 *p;
  int n = 0;
  for (p = (undefined4 *)*param_1; p != param_1; p = (undefined4 *)*p) {
    if (param_2) {
      if (param_2(p, param_3)) n++;
    } else n++;
  }
  return n;
}
