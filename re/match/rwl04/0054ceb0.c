// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054ceb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined1;
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0054ceb0 FUN_0054ceb0
/* [C1 2026-05-19] 4-param `(out_buf, stride, src8_floats, levels)`. */

void FUN_0054ceb0(float *param_1,int param_2,float *param_3,int param_4)
{
  float d;
  float a;
  float b;
  float c;
  float e;
  float v;
  float u;
  float x0;
  float y0;
  float xs;
  float ys;
  int i;
  int j;
  d = _DAT_005cc320 / (float)param_4;
  a = (param_3[5] - param_3[1]) * d;
  b = (param_3[7] - param_3[3]) * d;
  c = (param_3[4] - param_3[0]) * d;
  e = (param_3[6] - param_3[2]) * d;
  i = 0;
  if (-1 < param_4) {
    do {
      v = (float)i;
      x0 = c * v + param_3[0];
      y0 = a * v + param_3[1];
      ys = ((b * v + param_3[3]) - y0) * d;
      xs = ((e * v + param_3[2]) - x0) * d;
      j = 0;
      do {
        u = (float)j;
        param_1[0] = u * xs + x0;
        param_1[1] = u * ys + y0;
        param_1 = (float *)((int)param_1 + param_2);
        j = j + 1;
      } while (j <= param_4);
      i = i + 1;
    } while (i <= param_4);
  }
}
