// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x00566420: best 31.1% (regs 34.1%, ops 72.6%), class (b). rewritten with proper float *poly / *dpoly parameters; the original keeps x in the FPU across the loop, double locals tried and are worse. */
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
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005e5470, 0x005e5470);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, _DAT_005e5470, 0x005e5470);
// MATCH: 0x00566420 FUN_00566420
void FUN_00566420(float param_1,float param_2,float param_3,float param_4,int param_5,float *param_6,
                 float *param_7)

{
  float x;
  float f;
  float df;
  float nx;
  int i;
  int iter;

  iter = 0x17;
  x = (param_1 + param_2) * _DAT_005cc32c;
  if ((param_1 <= param_2) && (param_3 * param_4 < DAT_005d757c)) {
    while (1) {
      f = param_6[param_5];
      i = param_5;
      while (i != 0) {
        i = i - 1;
        f = f * x + param_6[i];
      }
      if (DAT_005d757c <= param_3 * f) {
        param_1 = x;
        param_3 = f;
        if (DAT_005d757c <= param_4 * f) {
          break;
        }
      }
      else {
        param_2 = x;
        param_4 = f;
      }
      df = param_7[param_5 - 1];
      i = param_5 - 1;
      while (i != 0) {
        i = i - 1;
        df = df * x + param_7[i];
      }
      if ((DAT_005d757c < (x - param_1) * df - f == DAT_005d757c < df) &&
          (DAT_005d757c < (param_2 - x) * df + f == DAT_005d757c < df)) {
        nx = x - f / df;
      }
      else {
        nx = (param_1 + param_2) * _DAT_005cc32c;
      }
      if ((nx - x) * (nx - x) < nx * nx * _DAT_005e5470) {
        return;
      }
      iter = iter - 1;
      x = nx;
      if (iter == 0) {
        return;
      }
    }
  }
  return;
}
