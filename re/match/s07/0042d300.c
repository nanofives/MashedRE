// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d300 (h5, by hand): 100.0% (regs 100.0%). Signed difference split into sign code / minutes (6000) / seconds (100) / remainder; remainder recomputed from *param_4 (re-read after the store). Residual: none. */
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
// MATCH: 0x0042d300 FUN_0042d300
void FUN_0042d300(int param_1, int param_2, int *param_3, int *param_4, int *param_5, float *param_6)
{
  int diff;

  diff = param_1 - param_2;
  *param_3 = 0;
  if (diff == 0) {
    *param_3 = 2;
  }
  else if (diff < 0) {
    diff = -diff;
    *param_3 = 1;
  }
  *param_4 = diff / 6000;
  *param_5 = (diff - *param_4 * 6000) / 100;
  *param_6 = (float)(diff - (*param_4 * 60 + *param_5) * 100);
}
