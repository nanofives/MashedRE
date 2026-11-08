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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cea1c, 0x005cea1c);
// MATCH: 0x00579c00 FUN_00579c00
undefined4 FUN_00579c00(float *param_1,uint param_2,int param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  float *pfVar3;
  float *pfVar4;
  uint uVar5;
  float local_c;
  int local_8;
  float local_4;

  fVar2 = DAT_005d757c;
  uVar5 = 1;
  local_c = 0.0f;
  local_4 = -1.0f;
  param_1[2] = 0.0f;
  param_1[1] = 0.0f;
  local_8 = 0;
  *param_1 = 0.0f;
  pfVar4 = param_4;
  if ((int)uVar5 <= (int)param_2) {
    do {
      if ((param_2 & uVar5) != 0) {
        fVar1 = param_4[local_8 + 0x28 + param_2 * 4];
        if (fVar1 < fVar2) {
          fVar2 = fVar1;
        }
        if (fVar1 > local_4) {
          local_4 = fVar1;
        }
        local_c = fVar1 + local_c;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        *param_1 = fVar1 * *pfVar3 + *param_1;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        pfVar3 = pfVar4 + 3;
        param_1[1] = pfVar3[1] * fVar1 + param_1[1];
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        param_1[2] = pfVar3[2] * fVar1 + param_1[2];
      }
      uVar5 = uVar5 * 2;
      local_8 = local_8 + 1;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)param_2);
    if (local_c != DAT_005d757c) {
      local_c = DAT_005cc320 / local_c;
      *param_1 = local_c * *param_1;
      param_1[1] = param_1[1] * local_c;
      param_1[2] = param_1[2] * local_c;
    }
  }
  if (fVar2 > local_4 * DAT_005cea1c) {
    return 1;
  }
  return 0;
}
