// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00578cb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
void FUN_0055c2d0(int p0, float * p1, float * p2, float * p3, float * p4);
MGLOBAL(float, _DAT_005cc33c, 0x005cc33c);
// MATCH: 0x00578cb0 FUN_00578cb0
float FUN_00578cb0(int *param_1,float *param_2,float *param_3,float *param_4)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float local_8[2];

  FUN_0055c2d0(*(undefined4 *)(*param_1 + 8),*(undefined4 *)(*param_1 + 0x10),param_2,&local_8[0],
               &local_8[1]);
  iVar1 = *(int *)(param_1[1] + 0x10);
  fVar2 = *(float *)(iVar1 + 0x34) * param_2[1] + *(float *)(iVar1 + 0x30) * *param_2 +
          *(float *)(iVar1 + 0x38) * param_2[2];
  fVar3 = fVar2 + param_2[3];
  fVar2 = fVar2 + param_2[4];
  fVar4 = local_8[0] - fVar2;
  local_8[1] = fVar3 - local_8[1];
  if (fVar4 > local_8[1]) {
    *param_4 = fVar2;
    *param_3 = *param_2;
    param_3[1] = param_2[1];
    param_3[2] = param_2[2];
    return fVar4;
  }
  *param_3 = *param_2 * _DAT_005cc33c;
  param_3[1] = param_2[1] * _DAT_005cc33c;
  param_3[2] = param_2[2] * _DAT_005cc33c;
  *param_4 = -fVar3;
  return local_8[1];
}
