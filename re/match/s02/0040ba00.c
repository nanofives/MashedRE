// FLAGS: /O2 /GS /Ob1
/* 0x0040ba00 (91 B): 100.0% exact, regs 100.0%.
   Raw g2s 96.8% (unsigned compare against 99 from undefined4). 0x008a94e0..0x008a94ec is one int[4];
   min over it from 99, then an indexed loop marks the entries equal to the min. Residual: none. */
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
MGLOBALA(int, DAT_008a94e0, 4, 0x008a94e0);

// MATCH: 0x0040ba00 FUN_0040ba00
void FUN_0040ba00(int *param_1)
{
  int best;
  int i;

  best = 99;
  if (DAT_008a94e0[0] < best) {
    best = DAT_008a94e0[0];
  }
  if (DAT_008a94e0[1] < best) {
    best = DAT_008a94e0[1];
  }
  if (DAT_008a94e0[2] < best) {
    best = DAT_008a94e0[2];
  }
  if (DAT_008a94e0[3] < best) {
    best = DAT_008a94e0[3];
  }
  for (i = 0; i < 4; i++) {
    param_1[i] = 0;
    if (DAT_008a94e0[i] == best) {
      param_1[i] = 1;
    }
  }
}
