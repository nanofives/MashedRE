// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00539900: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef signed char sbyte;
typedef long double float10;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007dc5b4, 0x007dc5b4);
float10 FUN_004c3b90(float p0);
undefined4 FUN_00547bf0(float * p0, float * p1, float * p2, float * p3);
// MATCH: 0x00539900 FUN_00539900
void FUN_00539900(int param_1,float *param_2,code *param_3,undefined4 param_4)

{
  int iVar1;
  float *pfVar2;
  float fVar3;
  char cVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  undefined1 *puVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  float *pfVar15;
  uint uVar16;
  byte *pbVar17;
  int iVar18;
  float *pfVar19;
  uint uVar20;
  float10 fVar21;
  undefined1 *local_2bc;
  uint local_2b4;
  float local_2a8;
  float local_2a4;
  float local_2a0;
  float local_29c;
  float local_298;
  float local_294;
  uint local_290;
  float *local_28c;
  float *local_288;
  float *local_284;
  float local_280;
  int local_27c;
  uint local_278;
  float local_274;
  float local_270;
  int local_268;
  float local_264 [12];
  undefined1 local_234;
  undefined1 local_233;
  undefined2 local_232;
  undefined1 *local_22c;
  undefined1 *local_228 [65];
  undefined1 **local_124;
  float local_120 [6];
  int local_108;
  int local_104 [64];
  int *local_4;
  
  pfVar15 = param_2;
  pfVar19 = local_120;
  for (iVar11 = 6; iVar11 != 0; iVar11 = iVar11 + -1) {
    *pfVar19 = *pfVar15;
    pfVar15 = pfVar15 + 1;
    pfVar19 = pfVar19 + 1;
  }
  local_4 = local_104;
  local_108 = *(int *)(param_1 + 0x1c);
  do {
    local_4 = local_4 + -1;
    piVar5 = (int *)*local_4;
    iVar11 = *piVar5;
    while (-1 < iVar11) {
      if ((float)piVar5[5] <= *(float *)((int)local_120 + *piVar5)) {
        *local_4 = piVar5[3];
        fVar3 = *(float *)((int)local_120 + *piVar5 + 0xc);
        if (fVar3 < (float)piVar5[4] != (fVar3 == (float)piVar5[4])) {
          local_4 = local_4 + 1;
          *local_4 = piVar5[2];
        }
      }
      else {
        *local_4 = piVar5[2];
      }
      piVar5 = (int *)*local_4;
      iVar11 = *piVar5;
    }
    local_27c = *local_4;
    iVar11 = *(int *)(DAT_007dc5b4 + local_27c);
    if (iVar11 != 0) {
      iVar6 = *(int *)(local_27c + 4);
      iVar7 = *(int *)(local_27c + 8);
      if (((((*(float *)(iVar11 + 0x10) < *param_2 == (*(float *)(iVar11 + 0x10) == *param_2)) ||
            (*(float *)(iVar11 + 0x14) < param_2[1] == (*(float *)(iVar11 + 0x14) == param_2[1])))
           || (*(float *)(iVar11 + 0x18) < param_2[2] == (*(float *)(iVar11 + 0x18) == param_2[2])))
          || ((*(float *)(iVar11 + 4) < param_2[3] || (*(float *)(iVar11 + 8) < param_2[4])))) ||
         (*(float *)(iVar11 + 0xc) < param_2[5])) {
        local_124 = &local_22c;
      }
      else {
        local_232 = 0;
        local_233 = 0xff;
        if (*(short *)(iVar11 + 0x1e) == 0) {
          local_233 = *(undefined1 *)(iVar11 + 0x1c);
        }
        pfVar15 = param_2;
        pfVar19 = local_264;
        for (iVar12 = 6; iVar12 != 0; iVar12 = iVar12 + -1) {
          *pfVar19 = *pfVar15;
          pfVar15 = pfVar15 + 1;
          pfVar19 = pfVar19 + 1;
        }
        pfVar15 = param_2;
        pfVar19 = local_264 + 6;
        for (iVar12 = 6; iVar12 != 0; iVar12 = iVar12 + -1) {
          *pfVar19 = *pfVar15;
          pfVar15 = pfVar15 + 1;
          pfVar19 = pfVar19 + 1;
        }
        local_22c = &local_234;
        local_2bc = (undefined1 *)(uint)(*(float *)(iVar11 + 0x10) < param_2[3]);
        if (param_2[4] <= *(float *)(iVar11 + 0x14)) {
          uVar14 = 0;
        }
        else {
          uVar14 = 4;
        }
        if (param_2[5] <= *(float *)(iVar11 + 0x18)) {
          uVar20 = 0;
        }
        else {
          uVar20 = 0x10;
        }
        if (*(float *)(iVar11 + 4) <= *param_2) {
          uVar16 = 0;
        }
        else {
          uVar16 = 2;
        }
        if (*(float *)(iVar11 + 8) <= param_2[1]) {
          uVar13 = 0;
        }
        else {
          uVar13 = 8;
        }
        if (*(float *)(iVar11 + 0xc) <= param_2[2]) {
          uVar8 = 0;
        }
        else {
          uVar8 = 0x20;
        }
        local_124 = local_228 + 1;
        local_228[0] = (undefined1 *)(uVar8 | uVar13 | uVar16 | uVar20 | uVar14 | (uint)local_2bc);
        local_268 = iVar11;
        do {
          local_124 = local_124 + -2;
          puVar9 = *local_124;
          if (puVar9[1] == -1) {
LAB_00539b7c:
            if (local_124[1] != (undefined1 *)0x0) goto code_r0x00539b87;
            iVar12 = *(int *)(local_268 + 0x20);
            iVar10 = (uint)*(ushort *)(puVar9 + 2) * 0x10 + iVar12;
            cVar4 = *(char *)(iVar10 + 1);
            iVar18 = iVar10;
            while (cVar4 == -1) {
              iVar18 = (uint)*(ushort *)(iVar18 + 2) * 0x10 + iVar12;
              cVar4 = *(char *)(iVar18 + 1);
            }
            iVar1 = iVar10 + 8;
            cVar4 = *(char *)(iVar10 + 9);
            while (cVar4 == -1) {
              iVar10 = (uint)*(ushort *)(iVar1 + 2) * 0x10;
              iVar1 = iVar10 + 8 + iVar12;
              cVar4 = *(char *)(iVar10 + 9 + iVar12);
            }
            uVar20 = (uint)*(ushort *)(iVar18 + 2);
            local_2bc = (undefined1 *)0x0;
            uVar14 = (ushort)((ushort)*(byte *)(iVar1 + 1) + *(short *)(iVar1 + 2)) - uVar20;
            goto LAB_00539c84;
          }
LAB_00539c74:
          local_2bc = local_124[1];
          uVar20 = (uint)*(ushort *)(puVar9 + 2);
          uVar14 = (uint)(byte)puVar9[1];
LAB_00539c84:
          local_278 = (uVar14 & 0xffff) + uVar20;
          local_2b4 = uVar20;
          if (uVar20 < local_278) {
            do {
              if (*(int *)(iVar11 + 0x24) != 0) {
                uVar20 = (uint)*(ushort *)(*(int *)(iVar11 + 0x24) + uVar20 * 2);
              }
              pfVar15 = (float *)(iVar7 + (uint)*(ushort *)(iVar6 + uVar20 * 8) * 0xc);
              pfVar19 = (float *)(iVar7 + (uint)*(ushort *)(iVar6 + 2 + uVar20 * 8) * 0xc);
              pfVar2 = (float *)(iVar7 + (uint)*(ushort *)(iVar6 + 4 + uVar20 * 8) * 0xc);
              if ((local_2bc == (undefined1 *)0x0) ||
                 (iVar12 = FUN_00547bf0(param_2,pfVar15,pfVar19,pfVar2), iVar12 != 0)) {
                local_29c = *pfVar15;
                local_298 = pfVar15[1];
                local_294 = pfVar15[2];
                local_274 = *pfVar2 - *pfVar15;
                local_270 = pfVar2[1] - pfVar15[1];
                local_2a8 = (pfVar2[2] - pfVar15[2]) * (pfVar19[1] - pfVar15[1]) -
                            local_270 * (pfVar19[2] - pfVar15[2]);
                local_2a4 = local_274 * (pfVar19[2] - pfVar15[2]) -
                            (pfVar2[2] - pfVar15[2]) * (*pfVar19 - *pfVar15);
                local_2a0 = local_270 * (*pfVar19 - *pfVar15) -
                            local_274 * (pfVar19[1] - pfVar15[1]);
                local_280 = local_2a0 * local_2a0 + local_2a8 * local_2a8 + local_2a4 * local_2a4;
                local_290 = uVar20;
                local_28c = pfVar15;
                local_288 = pfVar19;
                local_284 = pfVar2;
                fVar21 = (float10)FUN_004c3b90(local_280);
                local_2a8 = (float)((float10)local_2a8 * fVar21);
                local_2a4 = (float)((float10)local_2a4 * fVar21);
                local_2a0 = (float)(fVar21 * (float10)local_2a0);
                iVar12 = (*param_3)(param_2,local_27c,&local_2a8,0,param_4);
                if (iVar12 == 0) {
                  return;
                }
              }
              local_2b4 = local_2b4 + 1;
              uVar20 = local_2b4 & 0xffff;
            } while ((int)uVar20 < (int)local_278);
          }
        } while (&local_22c < local_124);
      }
    }
    if (local_4 <= &local_108) {
      return;
    }
  } while( true );
code_r0x00539b87:
  pbVar17 = (byte *)((uint)*(ushort *)(puVar9 + 2) * 0x10 + *(int *)(local_268 + 0x20));
  uVar14 = *pbVar17 & 0xc;
  if (*(float *)(pbVar17 + 0xc) <= *(float *)((int)local_264 + uVar14)) {
    *local_124 = pbVar17 + 8;
    fVar3 = *(float *)((int)local_264 + uVar14 + 0xc);
    if (fVar3 < *(float *)(pbVar17 + 4) != (fVar3 == *(float *)(pbVar17 + 4))) {
      local_124 = local_124 + 2;
      *local_124 = pbVar17;
      local_124[1] = local_124[-1];
      if (*(float *)(pbVar17 + 4) <= *(float *)((int)local_264 + uVar14 + 0x18)) {
        local_124[1] = (undefined1 *)((uint)local_124[1] & ~(2 << (sbyte)(uVar14 >> 1)));
      }
      fVar3 = *(float *)((int)local_264 + uVar14 + 0x24);
      if (fVar3 < *(float *)(pbVar17 + 0xc) != (fVar3 == *(float *)(pbVar17 + 0xc))) {
        local_124[-1] = (undefined1 *)((uint)local_124[-1] & ~(1 << (sbyte)(uVar14 >> 1)));
      }
    }
  }
  else {
    *local_124 = pbVar17;
  }
  puVar9 = *local_124;
  if (puVar9[1] != -1) goto LAB_00539c74;
  goto LAB_00539b7c;
}
