// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x00578bd0: 41.7% (regs 68.1%), class b. strength-reduced pointer lea edi/esi, param_2 copy reg. Tried: struct copy, alias placement, permuter. cxx:ok. */
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
typedef struct V3_ { int x, y, z; } V3_;
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
void FUN_0055c2d0(int param_1, int param_2, float *param_3, float *param_4, float *param_5);
// MATCH: 0x00578bd0 FUN_00578bd0
float10 FUN_00578bd0(int *param_1,float *param_2,float *param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  int iVar3;
  float fVar4;
  float *pfVar5;
  float d;

  iVar3 = *(int *)(*param_1 + 0x10);
  fVar4 = *(float *)(iVar3 + 0x38) * param_2[2] +
          *(float *)(iVar3 + 0x30) * *param_2 + *(float *)(iVar3 + 0x34) * param_2[1];
  fVar1 = fVar4 + param_2[3];
  fVar2 = fVar4 + param_2[4];
  pfVar5 = param_2;
  FUN_0055c2d0(*(int *)(param_1[1] + 8),*(int *)(param_1[1] + 0x10),param_2,(float *)&param_2,(float *)&param_1);
  d = fVar1 - *(float *)&param_1;
  fVar1 = *(float *)&param_2 - fVar2;
  if (d > fVar1) {
    *param_4 = *(float *)&param_1;
    *(V3_ *)param_3 = *(V3_ *)pfVar5;
    return d;
  }
  *param_3 = *pfVar5 * DAT_005cc33c;
  param_3[1] = DAT_005cc33c * pfVar5[1];
  param_3[2] = DAT_005cc33c * pfVar5[2];
  *param_4 = -*(float *)&param_2;
  return fVar1;
}
