// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c3c0 (h7): 100%. Raw g2s 16.1% (unaff_retaddr cookie, goto web). Hand: /GS cookie comes from the
   char buf[8] local (no source for it), swap of param_1/param_2 written param_2-first, two goto labels shared by
   the early-outs and the float tests (f == 1.0f at 0x005cc320, f <= 0.009999f at 0x005cd788, same -> 0.75f at
   0x005cc950). FUN_004a2b60 is sprintf-shaped (buf, "%.2f" at 0x005cd78c, double). Residual: none. */
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
MGLOBAL(float, DAT_005cc950, 0x005cc950);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cd788, 0x005cd788);
MGLOBALA(char, s__2f_005cd78c, 5, 0x005cd78c);
short *FUN_00427780(int p0);
int FUN_004a2b60(char *buf, const char *fmt, ...);
// MATCH: 0x0042c3c0 FUN_0042c3c0
void FUN_0042c3c0(int param_1, int param_2, int param_3, int param_4)
{
  short *p;
  int same;
  int tmp;
  float f;
  char buf[8];

  p = FUN_00427780(param_3);
  same = 0;
  if (param_4 != 0) {
    if (param_1 == 0) {
      if (param_2 == 0) {
        goto one;
      }
      param_1 = 1;
    }
    if (param_2 == 0) {
      param_2 = 1;
    }
    tmp = param_2;
    param_2 = param_1;
    param_1 = tmp;
    if (param_1 == 1 && param_2 == param_1) {
      same = param_1;
    }
  }
  if (param_1 == 0) {
one:
    p[0] = 4;
    p[1] = 0x31;
    p[3] = 0x30;
    p[4] = 0x20;
    return;
  }
  if (param_2 == 0) {
zero:
    p[0] = 4;
    p[1] = 0x30;
    p[3] = 0x30;
    p[4] = 0x20;
    return;
  }
  f = (float)param_2 / (float)param_1;
  if (same) {
    f = DAT_005cc950;
  } else {
    if (f == DAT_005cc320) {
      goto one;
    }
    if (f <= DAT_005cd788) {
      goto zero;
    }
  }
  FUN_004a2b60(buf, s__2f_005cd78c, (double)f);
  p[0] = 5;
  p[1] = 0x30;
  p[3] = (short)buf[2];
  p[4] = (short)buf[3];
  p[5] = 0x20;
}
