// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_004c45f0(float *p0);
void FUN_0056bb80(float * p0, float * p1, float p2);
void FUN_0056bce0(float * p0, float * p1, float p2);
// MATCH: 0x0056be80 FUN_0056be80
void FUN_0056be80(int *param_1,int param_2,float param_3)
{
  float fVar4;
  float fVar1;
  float *pfVar5;
  float *pfVar6;
  float fVar2;
  float *pfVar7;
  float fVar3;
  float local_3c;
  float *pfVar8;
  float local_34;
  float local_38;
  float local_28;
  float local_30;
  float local_24;
  float local_20;
  float local_2c;
  float local_1c;
  float local_14;
  float local_c;

  local_3c = pfVar5[4];
  pfVar8 = (float *)(param_2 * 0x40 + *param_1);
  local_38 = pfVar5[5];
  pfVar5 = (float *)(param_2 * 0x20 + param_1[2]);
  local_34 = pfVar5[6];
  pfVar7 = (float *)(param_1[4] + param_2 * 0x10);
  if ((*(unsigned char *)(param_1[6] + 4 * param_2) & 1) != 0) {
    pfVar6 = (float *)(param_1[0x14] + param_2 * 0x10);
    fVar1 = pfVar6[1] * local_38 + local_3c * *pfVar6 + pfVar6[2] * local_34;
    local_38 = pfVar6[1] * fVar1 + local_38;
    FUN_0056bb80(pfVar7,pfVar6,fVar1 * param_3);
    fVar1 = -fVar1;
    local_3c = fVar1 * *pfVar6 + local_3c;
    local_34 = pfVar6[2] * fVar1 + local_34;
  }
  FUN_0056bce0(pfVar7,&local_3c,param_3);
  local_24 = param_3 * *pfVar5 + pfVar8[0xc];
  local_20 = param_3 * pfVar5[1] + pfVar8[0xd];
  local_1c = param_3 * pfVar5[2] + pfVar8[0xe];
  local_c = DAT_005cc574 /
            (pfVar7[1] * pfVar7[1] + *pfVar7 * *pfVar7 +
            pfVar7[2] * pfVar7[2] + pfVar7[3] * pfVar7[3]);
  local_14 = pfVar7[1] * local_c;
  local_c = pfVar7[2] * local_c;
  local_3c = pfVar7[3] * fVar3;
  local_38 = pfVar7[3] * local_14;
  fVar3 = local_c * *pfVar7;
  local_34 = pfVar7[3] * local_c;
  local_2c = local_14 * pfVar7[1];
  local_28 = local_c * pfVar7[2];
  local_c = local_c * pfVar7[1];
  local_30 = fVar3 * *pfVar7;
  fVar1 = pfVar7[2];
  fVar2 = *pfVar7;
  pfVar8[0xc] = 0.0f;
  pfVar8[0xd] = 0.0f;
  pfVar8[0xe] = 0.0f;
  pfVar8[0xc] = local_24;
  pfVar8[0xd] = local_20;
  pfVar8[0xe] = local_1c;
  pfVar8[2] = fVar3 * fVar1 - local_38;
  *(int *)&pfVar8[3] = 3;
  fVar4 = DAT_005cc320 - (local_28 + local_2c);
  *pfVar8 = fVar4;
  pfVar8[1] = local_34 + local_14 * fVar2;
  pfVar8[4] = local_14 * fVar2 - local_34;
  pfVar8[5] = DAT_005cc320 - (local_28 + local_30);
  pfVar8[6] = local_3c + local_c;
  pfVar8[8] = fVar3 * fVar1 + local_38;
  pfVar8[9] = local_c - local_3c;
  pfVar8[10] = DAT_005cc320 - (local_2c + local_30);
  FUN_004c45f0(pfVar8);
}
