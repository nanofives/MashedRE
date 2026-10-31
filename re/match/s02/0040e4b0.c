// FLAGS: /O2 /GS /Ob1
/* 0x0040e4b0: for i in 0..3, if ((int *)PTR_PTR_005f2770)[0xd + i] != 0 and FUN_0046c7b0(i) == 1,
   count it and keep i; returns i when exactly one counted, else -1. Typed int* global + index loop:
   100%, 73 B BYTE-IDENTICAL (g2s raw 94.4%). Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int *, PTR_PTR_005f2770, 0x005f2770);

undefined4 FUN_0046c7b0(uint param_1);

// MATCH: 0x0040e4b0 FUN_0040e4b0
int FUN_0040e4b0(void)
{
  int found;
  int count;
  int i;

  found = -1;
  count = 0;
  for (i = 0; i < 4; i++) {
    if (PTR_PTR_005f2770[0xd + i] != 0 && FUN_0046c7b0(i) == 1) {
      found = i;
      count++;
    }
  }
  if (count == 1) {
    return found;
  }
  return -1;
}
