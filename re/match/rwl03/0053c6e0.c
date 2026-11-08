// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053c6e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned short ushort;
typedef long double float10;
typedef int code();
MGLOBAL(undefined1, DAT_005e4574, 0x005e4574);
MGLOBAL(undefined1, DAT_005cea1c, 0x005cea1c);
float10 FUN_004c3b90(float p0);
MGLOBAL(int, _DAT_005e4574, 0x005e4574);
MGLOBAL(int, _DAT_005cea1c, 0x005cea1c);
// MATCH: 0x0053c6e0 FUN_0053c6e0
/* [C1 2026-05-20] Ray-vs-morphed-atomic (vertex-interpolated triangles). */

void FUN_0053c6e0(int param_1,int param_2,float *param_3,undefined4 param_4,code *param_5,
                 undefined4 param_6)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  float *pfVar12;
  ushort *puVar13;
  float10 fVar14;
  float local_ac;
  float fStack_a8;
  float fStack_a4;
  float local_a0;
  float fStack_9c;
  float fStack_98;
  float local_94;
  float fStack_90;
  float fStack_8c;
  int local_88;
  int local_84;
  float local_80;
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  int local_58;
  float local_54;
  float local_50;
  float local_4c;
  float *local_48;
  float local_44;
  float local_40;
  float local_3c;
  float *local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  int local_1c;
  float *local_18;
  float *local_14;
  float *local_10;
  float local_c;
  float local_8;
  
  local_58 = 0;
  local_54 = param_3[3] - *param_3;
  local_50 = param_3[4] - param_3[1];
  local_4c = param_3[5] - param_3[2];
  iVar10 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14 + *(short *)(param_2 + 4) * 0x1c);
  local_88 = iVar10;
  local_48 = &local_a0;
  local_38 = &local_94;
  iVar11 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14 + *(short *)(param_2 + 6) * 0x1c);
  puVar13 = *(ushort **)(param_1 + 0x2c);
  local_80 = *(float *)(param_2 + 0x10) * *(float *)(param_2 + 0xc);
  local_84 = iVar11;
  pfVar12 = &local_ac;
  if (0 < *(int *)(param_1 + 0x10)) {
    do {
      if (iVar10 == iVar11) {
        pfVar12 = (float *)(iVar10 + (uint)*puVar13 * 0xc);
        local_48 = (float *)(iVar10 + (uint)puVar13[1] * 0xc);
        local_38 = (float *)(iVar10 + (uint)puVar13[2] * 0xc);
      }
      else {
        iVar9 = (uint)*puVar13 * 0xc;
        pfVar1 = (float *)(iVar9 + 4 + iVar10);
        pfVar2 = (float *)(iVar9 + 8 + iVar10);
        local_ac = (*(float *)(iVar9 + iVar11) - *(float *)(iVar9 + iVar10)) * local_80 +
                   *(float *)(iVar9 + iVar10);
        fStack_a8 = *pfVar1 + (*(float *)(iVar9 + 4 + iVar11) - *pfVar1) * local_80;
        fStack_a4 = *pfVar2 + (*(float *)(iVar9 + 8 + iVar11) - *pfVar2) * local_80;
        iVar9 = (uint)puVar13[1] * 0xc;
        pfVar1 = (float *)(iVar9 + 4 + iVar10);
        pfVar2 = (float *)(iVar9 + 8 + iVar10);
        local_a0 = (*(float *)(iVar9 + iVar11) - *(float *)(iVar9 + iVar10)) * local_80 +
                   *(float *)(iVar9 + iVar10);
        fStack_9c = *pfVar1 + (*(float *)(iVar9 + 4 + iVar11) - *pfVar1) * local_80;
        fStack_98 = *pfVar2 + (*(float *)(iVar9 + 8 + iVar11) - *pfVar2) * local_80;
        iVar9 = (uint)puVar13[2] * 0xc;
        pfVar1 = (float *)(iVar9 + 4 + iVar10);
        pfVar2 = (float *)(iVar9 + 8 + iVar10);
        local_94 = (*(float *)(iVar9 + iVar11) - *(float *)(iVar9 + iVar10)) * local_80 +
                   *(float *)(iVar9 + iVar10);
        fStack_90 = *pfVar1 + (*(float *)(iVar9 + 4 + iVar11) - *pfVar1) * local_80;
        fStack_8c = *pfVar2 + (*(float *)(iVar9 + 8 + iVar11) - *pfVar2) * local_80;
      }
      local_64 = *local_48 - *pfVar12;
      local_60 = local_48[1] - pfVar12[1];
      local_5c = local_48[2] - pfVar12[2];
      local_70 = *local_38 - *pfVar12;
      local_6c = local_38[1] - pfVar12[1];
      local_68 = local_38[2] - pfVar12[2];
      fVar5 = local_68 * local_50 - local_6c * local_4c;
      fVar7 = local_70 * local_4c - local_68 * local_54;
      fVar4 = local_6c * local_54 - local_70 * local_50;
      fVar6 = fVar4 * local_5c + fVar5 * local_64 + fVar7 * local_60;
      if (_DAT_005e4574 < fVar6) {
        fVar8 = fVar6 * _DAT_005cea1c;
        fVar3 = fVar6 - fVar8;
        local_7c = *param_3 - *pfVar12;
        local_78 = param_3[1] - pfVar12[1];
        local_74 = param_3[2] - pfVar12[2];
        fVar5 = local_74 * fVar4 + local_7c * fVar5 + local_78 * fVar7;
        if ((fVar8 <= fVar5) && (fVar5 < fVar3 != (fVar5 == fVar3))) {
          local_44 = local_78 * local_5c - local_74 * local_60;
          local_40 = local_74 * local_64 - local_7c * local_5c;
          local_3c = local_7c * local_60 - local_78 * local_64;
          fVar4 = local_3c * local_4c + local_44 * local_54 + local_40 * local_50;
          if ((fVar8 <= fVar4) &&
             (((fVar5 = fVar5 + fVar4, fVar5 < fVar3 != (fVar5 == fVar3) &&
               (fVar5 = local_3c * local_68 + local_44 * local_70 + local_40 * local_6c,
               fVar8 <= fVar5)) && (fVar5 < fVar3 != (fVar5 == fVar3))))) {
            local_28 = *pfVar12;
            local_24 = pfVar12[1];
            local_1c = local_58;
            local_20 = pfVar12[2];
            local_18 = pfVar12;
            local_14 = local_48;
            local_10 = local_38;
            local_c = *local_38 - *pfVar12;
            local_8 = local_38[1] - pfVar12[1];
            local_34 = (local_38[2] - pfVar12[2]) * (local_48[1] - pfVar12[1]) -
                       local_8 * (local_48[2] - pfVar12[2]);
            local_30 = local_c * (local_48[2] - pfVar12[2]) -
                       (local_38[2] - pfVar12[2]) * (*local_48 - *pfVar12);
            local_2c = local_8 * (*local_48 - *pfVar12) - local_c * (local_48[1] - pfVar12[1]);
            fVar14 = (float10)FUN_004c3b90(local_2c * local_2c +
                                           local_34 * local_34 + local_30 * local_30);
            local_34 = (float)((float10)local_34 * fVar14);
            local_30 = (float)((float10)local_30 * fVar14);
            local_2c = (float)(fVar14 * (float10)local_2c);
            iVar9 = (*param_5)(param_4,&local_34,fVar5 / fVar6,param_6);
            iVar10 = local_88;
            iVar11 = local_84;
            if (iVar9 == 0) {
              return;
            }
          }
        }
      }
      local_58 = local_58 + 1;
      puVar13 = puVar13 + 4;
    } while (local_58 < *(int *)(param_1 + 0x10));
  }
  return;
}
