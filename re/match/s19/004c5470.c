// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c5470: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc98c, 0x005cc98c);
MGLOBAL(undefined1, DAT_005cc328, 0x005cc328);
float10 FUN_004c3ac0(float * p0);
void FUN_004c4eb0(float * p0, float * p1);
float * FUN_004c5680(float * p0, float * p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc98c, 0x005cc98c);
MGLOBAL(int, _DAT_005cc328, 0x005cc328);
// MATCH: 0x004c5470 FUN_004c5470
/* [C1 2026-05-19] Signature `float* f(float* mat, float* axis, float* angle, float* */

float * FUN_004c5470(float *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float10 fVar2;
  float10 fVar3;
  float local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  float local_78;
  uint local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_60;
  float local_5c;
  float local_58;
  float local_50;
  float local_4c;
  float local_48;
  float local_40;
  float local_3c;
  float local_38;
  uint local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  local_74 = 1;
  fVar1 = (*param_1 + param_1[5] + param_1[10]) - _DAT_005cc320;
  local_8c = param_1[6] - param_1[9];
  local_88 = param_1[8] - param_1[2];
  local_84 = param_1[1] - param_1[4];
  fVar2 = (float10)FUN_004c3ac0(&local_8c);
  if (fVar2 <= (float10)DAT_005d757c) {
    *param_2 = 0.0;
    param_2[1] = 0.0;
    param_2[2] = 0.0;
  }
  else {
    fVar3 = (float10)_DAT_005cc320 / fVar2;
    *param_2 = (float)((float10)local_8c * fVar3);
    param_2[1] = (float)((float10)local_88 * fVar3);
    param_2[2] = (float)(fVar3 * (float10)local_84);
  }
  fVar3 = (float10)fpatan(fVar2,(float10)fVar1);
  *param_3 = (float)(fVar3 * (float10)_DAT_005cc98c);
  if ((fVar2 < (float10)_DAT_005cc328 != (fVar2 == (float10)_DAT_005cc328)) &&
     (fVar1 < DAT_005d757c != (fVar1 == DAT_005d757c))) {
    FUN_004c5680(param_1,param_2);
  }
  local_40 = _DAT_005cc320 - *param_1;
  local_74 = local_74 & 0xfffdffff;
  local_8 = 0;
  local_3c = -param_1[1];
  local_38 = -param_1[2];
  local_30 = -param_1[4];
  local_2c = _DAT_005cc320 - param_1[5];
  local_c = 0;
  local_28 = -param_1[6];
  local_20 = -param_1[8];
  local_1c = -param_1[9];
  local_18 = _DAT_005cc320 - param_1[10];
  local_10 = 0;
  local_34 = local_74;
  FUN_004c4eb0(&local_80,&local_40);
  *param_4 = local_50;
  param_4[1] = local_4c;
  param_4[2] = local_48;
  *param_4 = param_1[0xc] * local_80 + *param_4;
  param_4[1] = param_1[0xc] * local_7c + param_4[1];
  param_4[2] = param_1[0xc] * local_78 + param_4[2];
  *param_4 = param_1[0xd] * local_70 + *param_4;
  param_4[1] = param_1[0xd] * local_6c + param_4[1];
  param_4[2] = param_1[0xd] * local_68 + param_4[2];
  *param_4 = param_1[0xe] * local_60 + *param_4;
  param_4[1] = param_1[0xe] * local_5c + param_4[1];
  param_4[2] = param_1[0xe] * local_58 + param_4[2];
  return param_1;
}
