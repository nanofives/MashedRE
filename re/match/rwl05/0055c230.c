// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055c230 (h2): hand, 93.4%, class (a): first dot product term order/scheduling (fld before mov eax); permuter and term reorders no gain; /O1 worse. */
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
typedef int (*FN)(int,int,float *,int);
void FUN_0055c0f0(int p0, float *p1);
// MATCH: 0x0055c230 FUN_0055c230
int FUN_0055c230(int param_1,float *param_2,int param_3,float *param_4,int param_5)
{
  int iVar1;
  float v[3];

  if (param_2 != 0) {
    v[0] = param_2[1] * param_4[1] + param_2[0] * param_4[0] + param_2[2] * param_4[2];
    v[1] = param_2[5] * param_4[1] + param_2[4] * param_4[0] + param_2[6] * param_4[2];
    v[2] = param_2[9] * param_4[1] + param_2[8] * param_4[0] + param_2[10] * param_4[2];
    param_4 = v;
  }
  iVar1 = (*(FN *)(*(int *)(param_1 + 0x5c) + 0x18))(param_1,param_3,param_4,param_5);
  if (iVar1 != 0 && param_2 != 0) {
    FUN_0055c0f0(iVar1,param_2);
  }
  return iVar1;
}
