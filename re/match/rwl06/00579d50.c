// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00579d50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef long double float10;
void FUN_0055c000(int p0, float * p1, float * p2, undefined4 p3);
// MATCH: 0x00579d50 FUN_00579d50
float FUN_00579d50(float *param_1,float *param_2,int param_3,int *param_4,float *param_5)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  float *pfVar4;
  float local_18[3];
  float local_c[3];
  float t0;
  float t1;
  float t2;

  local_18[0] = -*param_2;
  local_18[1] = -param_2[1];
  local_18[2] = -param_2[2];
  iVar1 = *param_4;
  if (param_3 > 0) {
    FUN_0055c000(*(undefined4 *)(iVar1 + 8),*(undefined4 *)(iVar1 + 0x10),param_2,local_c);
    uVar3 = *(undefined4 *)(param_4[1] + 0x10);
    uVar2 = *(undefined4 *)(param_4[1] + 8);
    pfVar4 = local_18;
  }
  else {
    FUN_0055c000(*(undefined4 *)(iVar1 + 8),*(undefined4 *)(iVar1 + 0x10),local_18,local_c);
    uVar3 = *(undefined4 *)(param_4[1] + 0x10);
    uVar2 = *(undefined4 *)(param_4[1] + 8);
    pfVar4 = param_2;
  }
  FUN_0055c000(uVar2,uVar3,pfVar4,param_1 + 3);
  t0 = local_c[0] - param_1[3];
  *param_1 = t0;
  t1 = local_c[1] - param_1[4];
  param_1[1] = t1;
  t2 = local_c[2] - param_1[5];
  param_1[2] = t2;
  if (param_5 != (float *)0x0) {
    *param_1 = t0 + *param_5;
    param_1[1] = param_5[1] + t1;
    param_1[2] = param_5[2] + t2;
  }
  return param_2[2] * param_1[2] + *param_1 * *param_2 + param_2[1] * param_1[1];
}
