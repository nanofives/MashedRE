// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00562380 (h2): hand, 74.0%, class (b): term order of the three cross products and fld DAT vs fld x order; permuter 240 s reached 74.0%. */
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
MGLOBAL(float, DAT_005cc56c, 0x005cc56c);
MGLOBAL(float, DAT_005cd110, 0x005cd110);
MGLOBAL(float, DAT_005cc9a0, 0x005cc9a0);
float FUN_00565d50(int p0, float * p1, float * p2, float * p3);
float FUN_005667c0(float * p0, float * p1);
// MATCH: 0x00562380 FUN_00562380
void FUN_00562380(int param_1,int param_2)
{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  float fVar5;
  float fVar6;

  pfVar2 = *(float **)(param_1 + 0xc);
  pfVar3 = *(float **)(param_1 + 0x10);
  pfVar1 = (float *)(param_1 + 0x1c);
  pfVar4 = *(float **)(param_1 + 0x14);
  *pfVar1 = (pfVar4[2] - pfVar2[2]) * pfVar3[1] +
            (pfVar3[2] - pfVar4[2]) * pfVar2[1] + (pfVar2[2] - pfVar3[2]) * pfVar4[1];
  *(float *)(param_1 + 0x20) =
       (*pfVar3 - *pfVar4) * pfVar2[2] +
       (*pfVar2 - *pfVar3) * pfVar4[2] + (*pfVar4 - *pfVar2) * pfVar3[2];
  *(float *)(param_1 + 0x24) =
       (pfVar2[1] - pfVar3[1]) * *pfVar4 +
       (pfVar4[1] - pfVar2[1]) * *pfVar3 + (pfVar3[1] - pfVar4[1]) * *pfVar2;
  FUN_005667c0(pfVar1,pfVar1);
  fVar6 = FUN_00565d50(param_2,*(float **)(param_1 + 0xc),
                                *(float **)(param_1 + 0x10),*(float **)(param_1 + 0x14));
  *(float *)(param_1 + 0x18) = *(float *)(param_2 + 0xc) * DAT_005cc56c;
  fVar5 = *(float *)(param_2 + 0xc) * DAT_005cd110;
  *(int *)(param_2 + 0x18) = 3;
  *(float *)(param_2 + 0xc) = fVar5;
  if (fVar5 < fVar6 * DAT_005cc9a0) {
    *(float *)(param_2 + 0xc) = fVar6 * DAT_005cc9a0;
  }
}
