// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566b40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef long double float10;
MGLOBAL(undefined1, DAT_005dae94, 0x005dae94);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
double FUN_005667c0(float * p0, float * p1);
void FUN_00566830(float * p0, float * p1);
MGLOBAL(float, _DAT_005dae94, 0x005dae94);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
double sqrt(double);
#pragma intrinsic(sqrt)
// MATCH: 0x00566b40 FUN_00566b40
double FUN_00566b40(float *param_1,float *param_2,float *param_3,float param_4)

{
  double fVar1;
  float fVar2;

  fVar2 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  if (fVar2 > _DAT_005dae94) {
    fVar1 = _DAT_005cc320 / sqrt(fVar2);
    *param_1 = (float)(fVar1 * *param_2);
    param_1[1] = (float)(param_2[1] * fVar1);
    param_1[2] = (float)(param_2[2] * fVar1);
    return fVar1 * fVar2;
  }
  if (param_4 == DAT_005d757c) {
    FUN_00566830(param_1,param_3);
    FUN_005667c0(param_1,param_1);
    return DAT_005d757c;
  }
  *param_1 = param_4 * *param_3;
  param_1[1] = param_3[1] * param_4;
  param_1[2] = param_3[2] * param_4;
  FUN_005667c0(param_1,param_1);
  return DAT_005d757c;
}
