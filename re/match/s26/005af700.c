// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005af700 (63 B). Best 100.0% (regs 100.0%), exact. if(idx<n/2){forward walk}else{backward walk} branch order was the fix. */
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
// MATCH: 0x005af700 FUN_005af700
int * FUN_005af700(int param_1,int param_2,unsigned int param_3)
{
  int *p;
  unsigned int n = *(unsigned int *)(param_2 + 8);
  if (param_3 < (n >> 1)) {
    p = *(int **)(param_1 + 0x20);
    for (; param_3 != 0; param_3 = param_3 - 1) {
      p = (int *)*p;
    }
    return p - 0xb;
  } else {
    unsigned int u = n - 1;
    int q = *(int *)(param_1 + 0x24);
    if (u != param_3) {
      int c = u - param_3;
      do {
        q = *(int *)(q + 4);
        c = c - 1;
      } while (c != 0);
    }
    return (int *)(q - 0x2c);
  }
}
