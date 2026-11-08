// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005538a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_00912be8, 0x00912be8);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
float10 FUN_004c3b30(float p0);
float10 FUN_004c3b90(float p0);
undefined4 FUN_0055deb0(int p0);
int FUN_005c4bb0(int p0, undefined4 p1);
MGLOBAL(int, _DAT_00912be8, 0x00912be8);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
// MATCH: 0x005538a0 FUN_005538a0
/* [C1 2026-05-19] 3-param `(dst_path, src_4_points[8], depth)`. Recursive cubic-Bezier subdivision
   (De Casteljau). */

void FUN_005538a0(undefined4 *param_1,float *param_2,int param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  undefined4 *puVar8;
  float10 fVar9;
  float10 fVar10;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  
  param_3 = param_3 + -1;
  if (-1 < param_3) {
    fVar1 = param_2[6] - *param_2;
    fVar2 = param_2[7] - param_2[1];
    fVar3 = fVar1 * fVar1 + fVar2 * fVar2;
    fVar6 = param_2[2] - *param_2;
    fVar5 = param_2[3] - param_2[1];
    fVar4 = fVar6 * fVar1 + fVar2 * fVar5;
    if (((fVar6 * fVar6 + fVar5 * fVar5) - _DAT_00912be8) * fVar3 < fVar4 * fVar4) {
      fVar5 = param_2[4] - *param_2;
      fVar4 = param_2[5] - param_2[1];
      fVar1 = fVar5 * fVar1 + fVar2 * fVar4;
      if (((fVar5 * fVar5 + fVar4 * fVar4) - _DAT_00912be8) * fVar3 < fVar1 * fVar1) {
        iVar7 = FUN_0055deb0(*param_1);
        puVar8 = (undefined4 *)FUN_005c4bb0(*param_1,0x30190);
        *puVar8 = 1;
        puVar8[1] = param_2[6];
        puVar8[2] = param_2[7];
        fVar1 = param_2[7] - (float)puVar8[-4];
        fVar2 = (float)puVar8[-5] - param_2[6];
        fVar3 = fVar1 * fVar1 + fVar2 * fVar2;
        fVar9 = (float10)FUN_004c3b30(fVar3);
        fVar10 = (float10)FUN_004c3b90(fVar3);
        puVar8[3] = (float)((float10)fVar1 * fVar10);
        puVar8[4] = (float)(fVar10 * (float10)fVar2);
        puVar8[5] = (float)fVar9 + (float)puVar8[-1];
        if (iVar7 < 2) {
          puVar8[-3] = puVar8[3];
          puVar8[-2] = puVar8[4];
          return;
        }
        fVar1 = (float)puVar8[-3] + (float)((float10)fVar1 * fVar10);
        fVar3 = (float)puVar8[-2] + (float)(fVar10 * (float10)fVar2);
        fVar2 = _DAT_005cc320 / ((float)puVar8[-3] * fVar1 + (float)puVar8[-2] * fVar3);
        puVar8[-3] = fVar2 * fVar1;
        puVar8[-2] = fVar2 * fVar3;
        return;
      }
    }
    fVar1 = (param_2[4] + param_2[2]) * _DAT_005cc32c;
    fVar2 = (param_2[5] + param_2[3]) * _DAT_005cc32c;
    local_40 = (param_2[2] + *param_2) * _DAT_005cc32c;
    local_3c = (param_2[1] + param_2[3]) * _DAT_005cc32c;
    local_18 = (param_2[4] + param_2[6]) * _DAT_005cc32c;
    local_14 = (param_2[7] + param_2[5]) * _DAT_005cc32c;
    local_38 = (local_40 + fVar1) * _DAT_005cc32c;
    local_34 = (local_3c + fVar2) * _DAT_005cc32c;
    local_20 = (local_18 + fVar1) * _DAT_005cc32c;
    local_1c = (local_14 + fVar2) * _DAT_005cc32c;
    local_30 = (local_20 + local_38) * _DAT_005cc32c;
    local_48 = *param_2;
    local_44 = param_2[1];
    local_10 = param_2[6];
    local_2c = (local_1c + local_34) * _DAT_005cc32c;
    local_c = param_2[7];
    local_28 = local_30;
    local_24 = local_2c;
    FUN_005538a0(param_1,&local_48,param_3);
    FUN_005538a0(param_1,&local_28,param_3);
    return;
  }
  iVar7 = FUN_0055deb0(*param_1);
  puVar8 = (undefined4 *)FUN_005c4bb0(*param_1,0x30190);
  *puVar8 = 1;
  puVar8[1] = param_2[6];
  puVar8[2] = param_2[7];
  fVar1 = param_2[7] - (float)puVar8[-4];
  fVar2 = (float)puVar8[-5] - param_2[6];
  fVar3 = fVar1 * fVar1 + fVar2 * fVar2;
  fVar9 = (float10)FUN_004c3b30(fVar3);
  fVar10 = (float10)FUN_004c3b90(fVar3);
  puVar8[3] = (float)((float10)fVar1 * fVar10);
  puVar8[4] = (float)(fVar10 * (float10)fVar2);
  puVar8[5] = (float)fVar9 + (float)puVar8[-1];
  if (iVar7 < 2) {
    puVar8[-3] = puVar8[3];
    puVar8[-2] = puVar8[4];
    return;
  }
  fVar1 = (float)puVar8[-3] + (float)((float10)fVar1 * fVar10);
  fVar3 = (float)puVar8[-2] + (float)(fVar10 * (float10)fVar2);
  fVar2 = _DAT_005cc320 / ((float)puVar8[-3] * fVar1 + (float)puVar8[-2] * fVar3);
  puVar8[-3] = fVar2 * fVar1;
  puVar8[-2] = fVar2 * fVar3;
  return;
}
