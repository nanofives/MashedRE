// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0057c0d0: hand-matched (h2). 98.7% regs 98.7%. Re-derived from the x87 stack trace: three edge vectors d=p4-p3, a=p1-p3, b=p2-p3 and two cross products ca (3 stored floats), cb (cb.z kept in a register), dot = cb[2]*ca[2] + (cb[1]*ca[1] + cb[0]*ca[0]), result >= DAT_005d757c. Local float arrays reproduce the stack layout. Residual: one instruction (mov ecx,[esp+0x2c] loading p2) is scheduled one slot later than the original; pointer copies and the permuter (177 compiles) did not move it. Class a (scheduling). */
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
typedef unsigned int undefined4;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x0057c0d0 FUN_0057c0d0
undefined4 FUN_0057c0d0(float *param_1,float *param_2,float *param_3,float *param_4)
{
  float d[3];
  float a[3];
  float b[3];
  float ca[3];
  float cb[3];

  d[0] = *param_4 - *param_3;
  d[1] = param_4[1] - param_3[1];
  d[2] = param_4[2] - param_3[2];
  a[0] = *param_1 - *param_3;
  a[1] = param_1[1] - param_3[1];
  a[2] = param_1[2] - param_3[2];
  ca[0] = a[2] * d[1] - a[1] * d[2];
  ca[1] = a[0] * d[2] - a[2] * d[0];
  ca[2] = a[1] * d[0] - a[0] * d[1];
  b[0] = *param_2 - *param_3;
  b[1] = param_2[1] - param_3[1];
  b[2] = param_2[2] - param_3[2];
  cb[0] = b[2] * d[1] - b[1] * d[2];
  cb[1] = b[0] * d[2] - b[2] * d[0];
  cb[2] = d[0] * b[1] - b[0] * d[1];
  if (cb[2] * ca[2] + (cb[1] * ca[1] + cb[0] * ca[0]) >= DAT_005d757c) {
    return 1;
  }
  return 0;
}
