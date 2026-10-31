// FLAGS: /O2 /GS /Ob1
/* 0x0040b8e0 FUN_0040b8e0 (66 B): best 100.0% exact, regs 100.0%.
   First hand pass: ternary (f()==4 ? 10 : 7) gives the sub/neg/sbb/and/add form; two separate
   if (g == 1) / if (g == 2) returns. No residual. */
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);

int FUN_0040e340(void);
int FUN_0042f500(void);

// MATCH: 0x0040b8e0 FUN_0040b8e0
int FUN_0040b8e0(void)
{
  int n;

  n = (FUN_0040e340() == 4) ? 10 : 7;
  if (FUN_0042f500() != 0) {
    n = 7;
  }
  if (DAT_007f0fd0 == 1) {
    return 7;
  }
  if (DAT_007f0fd0 == 2) {
    return 7;
  }
  return n;
}
