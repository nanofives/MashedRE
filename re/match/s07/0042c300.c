// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c300 (h7): 100%. Raw g2s 78.8% (goto shape), permuter on it: no gain. Hand: three digit blocks
   (tens / units / trailing space), each tried in 4 condition forms (t||h, h||t, if-h-else-if-t, negated inner);
   the matching set is tens `if (t||h) { if (h==0) .. else .. }`, units `if (h||t) { if (h) .. else .. } else ..`,
   space `if (h) .. else if (t) .. else ..`. FUN_00427780 returns short*. Residual: none. */
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
short *FUN_00427780(int p0);
// MATCH: 0x0042c300 FUN_0042c300
void FUN_0042c300(int param_1, int param_2)
{
  short *p;
  int h;
  int t;
  int u;

  p = FUN_00427780(param_2);
  if (param_1 > 999) {
    param_1 = 999;
  }
  h = param_1 / 100;
  if (h != 0) {
    p[0] = 4;
    p[1] = (short)h + 0x30;
  }
  t = (param_1 % 100) / 10;
  u = (param_1 % 100) % 10;
  if (t != 0 || h != 0) {
    if (h == 0) {
      p[0] = 3;
      p[1] = (short)t + 0x30;
    } else {
      p[2] = (short)t + 0x30;
    }
  }
  if (h != 0 || t != 0) {
    if (h != 0) {
      p[3] = (short)u + 0x30;
    } else {
      p[2] = (short)u + 0x30;
    }
  } else {
    p[0] = 2;
    p[1] = (short)u + 0x30;
  }
  if (h != 0) {
    p[4] = 0x20;
  } else if (t != 0) {
    p[3] = 0x20;
  } else {
    p[2] = 0x20;
  }
}
