// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055c3e0 (h2): hand, 65.1%, class (c): original converts the callee result with the C++ bool pattern neg/sbb/neg then je; no C spelling gives it ((x != 0) & 1 gets closest); local float arrays a[3], b[3]. */
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
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
typedef int (*FN)(int,float *,float *);
// MATCH: 0x0055c3e0 FUN_0055c3e0
void FUN_0055c3e0(int param_1,float *param_2,float *param_3,float *param_4)
{
  float a[3];
  float b[3];
  int ok;

  ok = (((*(FN *)(*(int *)(param_1 + 0x5c) + 0x14))(param_1,param_2,a)) != 0) & 1;
  if (ok) {
    *param_4 = param_2[1] * a[1] + a[0] * param_2[0] + param_2[2] * a[2];
    b[0] = param_2[0] * DAT_005cc33c;
    b[1] = param_2[1] * DAT_005cc33c;
    b[2] = param_2[2] * DAT_005cc33c;
    ok = (((*(FN *)(*(int *)(param_1 + 0x5c) + 0x14))(param_1,b,a)) != 0) & 1;
    if (ok) {
      *param_3 = param_2[1] * a[1] + a[0] * param_2[0] + param_2[2] * a[2];
    }
  }
}
