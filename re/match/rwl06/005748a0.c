// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005748a0: hand-matched (h2). 100.0% regs 100.0%. Each element reads p2[i] into a local float first (t = p2[i]; p1[i] += t * p3): that is what makes the compiler load the memory operand before the parameter (fld p2[i]; fmul p3). Same trick as the raw spelling p2[i]*p3 does NOT give it. */
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
// MATCH: 0x005748a0 FUN_005748a0
void FUN_005748a0(float *param_1,float *param_2,float param_3)
{
  { float t = param_2[0]; param_1[0] += t * param_3; }
  { float t = param_2[1]; param_1[1] += t * param_3; }
  { float t = param_2[2]; param_1[2] += t * param_3; }
  { float t = param_2[3]; param_1[3] += t * param_3; }
  { float t = param_2[4]; param_1[4] += t * param_3; }
  { float t = param_2[5]; param_1[5] += t * param_3; }
  { float t = param_2[6]; param_1[6] += t * param_3; }
  { float t = param_2[7]; param_1[7] += t * param_3; }
  { float t = param_2[8]; param_1[8] += t * param_3; }
}
