// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042bb60 (h6, by hand): 95.9% on Ghidra's 314 B size, whole obj symbol BYTE-IDENTICAL (323 B, relocations masked): Ghidra's size stops before the add eax,2 / return -1 tail. Raw 66.9%. Rewritten: two 4-entry loops over the 16-byte table (unrolled by the compiler), idx > -1 (>= 0 gave test/jl), if-chain on the count with OR-ed (n0 && n1) pairs per count (nested ifs and a switch both lost the shared cmp esi,2 block), shared n0 == 0 / (n1 != 0) + 2 tail. Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { int idx; int b; int c; int d; } S10;
typedef struct { int a; int b; int c; } S0C;
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(S0C, DAT_0067e938, 12, 0x0067e938);
// MATCH: 0x0042bb60 FUN_0042bb60
int FUN_0042bb60(void)
{
  int i;
  int n;
  int n0;
  int n1;
  for (i = 0; i < 4; i++) {
    DAT_007f1a14[i].b = -1;
  }
  n = 0;
  n1 = 0;
  n0 = 0;
  for (i = 0; i < 4; i++) {
    if (DAT_007f1a14[i].idx > -1) {
      DAT_007f1a14[i].b = DAT_0067e938[DAT_007f1a14[i].idx].a - 1;
      if (DAT_007f1a14[i].b == 0) {
        n0++;
      }
      else if (DAT_007f1a14[i].b == 1) {
        n1++;
      }
      n++;
    }
  }
  if (n == 2) {
    if (n0 == 1 && n1 == 1) {
      return 0x1000;
    }
    return 0;
  }
  if (n == 3) {
    if ((n0 == 1 && n1 == 2) || (n0 == 2 && n1 == 1)) {
      return 0x1000;
    }
  }
  else if (n == 4) {
    if ((n0 == 2 && n1 == 2) || (n0 == 1 && n1 == 3) || (n0 == 3 && n1 == 1)) {
      return 0x1000;
    }
  }
  else {
    return -1;
  }
  if (n0 == 0) {
    return 1;
  }
  return (n1 != 0) + 2;
}
