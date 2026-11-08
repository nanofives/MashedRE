// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053b7b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned __int64 undefined8;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned __int64 ulonglong;
typedef signed char sbyte;
typedef long double float10;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(int, DAT_007dc5b0, 0x007dc5b0);
float10 FUN_004c3b90(float p0);
undefined4 FUN_00547bf0(float * p0, float * p1, float * p2, float * p3);
// MATCH: 0x0053b7b0 FUN_0053b7b0
void FUN_0053b7b0(int param_1,float *param_2,code *param_3,undefined4 param_4)

{
  int iVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  float fVar5;
  char cVar6;
  uint uVar7;
  undefined1 *puVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  undefined2 uVar16;
  byte *pbVar13;
  float *pfVar14;
  int iVar15;
  uint uVar17;
  ushort *puVar18;
  uint uVar19;
  float *pfVar20;
  uint uVar21;
  float10 fVar22;
  undefined8 uVar23;
  uint local_198;
  float local_194;
  float local_190;
  float local_18c;
  float local_188;
  float local_184;
  float local_180;
  uint local_17c;
  float *local_178;
  float *local_174;
  float *local_170;
  float local_16c;
  float local_168;
  float local_164;
  uint local_15c;
  float *local_158;
  undefined1 *local_154;
  int local_150;
  int local_14c;
  int local_148;
  float local_144 [12];
  undefined1 local_114;
  undefined1 local_113;
  undefined2 local_112;
  undefined1 *local_10c;
  undefined1 *local_108 [65];
  undefined1 **local_4;
  
  iVar10 = *(int *)(DAT_007dc5b0 + param_1);
  if (iVar10 == 0) {
    local_15c = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
    puVar18 = *(ushort **)(param_1 + 0x2c);
    local_198 = 0;
    if (0 < *(int *)(param_1 + 0x10)) {
      do {
        pfVar14 = (float *)(local_15c + (uint)*puVar18 * 0xc);
        pfVar20 = (float *)(local_15c + (uint)puVar18[1] * 0xc);
        pfVar2 = (float *)(local_15c + (uint)puVar18[2] * 0xc);
        iVar10 = FUN_00547bf0(param_2,pfVar14,pfVar20,pfVar2);
        if (iVar10 != 0) {
          local_168 = *pfVar2 - *pfVar14;
          local_164 = pfVar2[1] - pfVar14[1];
          local_188 = *pfVar14;
          local_184 = pfVar14[1];
          local_17c = local_198;
          local_180 = pfVar14[2];
          local_194 = (pfVar2[2] - pfVar14[2]) * (pfVar20[1] - pfVar14[1]) -
                      local_164 * (pfVar20[2] - pfVar14[2]);
          local_190 = local_168 * (pfVar20[2] - pfVar14[2]) -
                      (pfVar2[2] - pfVar14[2]) * (*pfVar20 - *pfVar14);
          local_18c = local_164 * (*pfVar20 - *pfVar14) - local_168 * (pfVar20[1] - pfVar14[1]);
          local_16c = local_18c * local_18c + local_194 * local_194 + local_190 * local_190;
          local_178 = pfVar14;
          local_174 = pfVar20;
          local_170 = pfVar2;
          fVar22 = (float10)FUN_004c3b90(local_16c);
          local_194 = (float)((float10)local_194 * fVar22);
          local_190 = (float)((float10)local_190 * fVar22);
          local_18c = (float)(fVar22 * (float10)local_18c);
          iVar10 = (*param_3)(param_2,&local_194,0,param_4);
          if (iVar10 == 0) {
            return;
          }
        }
        local_198 = local_198 + 1;
        puVar18 = puVar18 + 4;
      } while ((int)local_198 < *(int *)(param_1 + 0x10));
    }
  }
  else {
    local_150 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
    local_14c = *(int *)(param_1 + 0x2c);
    if ((((*(float *)(iVar10 + 0x10) < *param_2 != (*(float *)(iVar10 + 0x10) == *param_2)) &&
         (*(float *)(iVar10 + 0x14) < param_2[1] != (*(float *)(iVar10 + 0x14) == param_2[1]))) &&
        (*(float *)(iVar10 + 0x18) < param_2[2] != (*(float *)(iVar10 + 0x18) == param_2[2]))) &&
       (((param_2[3] <= *(float *)(iVar10 + 4) && (param_2[4] <= *(float *)(iVar10 + 8))) &&
        (param_2[5] <= *(float *)(iVar10 + 0xc))))) {
      local_112 = 0;
      local_113 = 0xff;
      if (*(short *)(iVar10 + 0x1e) == 0) {
        local_113 = *(undefined1 *)(iVar10 + 0x1c);
      }
      fVar4 = *(float *)(iVar10 + 0x10);
      fVar5 = param_2[3];
      pfVar14 = param_2;
      pfVar20 = local_144;
      for (iVar11 = 6; iVar11 != 0; iVar11 = iVar11 + -1) {
        *pfVar20 = *pfVar14;
        pfVar14 = pfVar14 + 1;
        pfVar20 = pfVar20 + 1;
      }
      pfVar14 = param_2;
      pfVar20 = local_144 + 6;
      for (iVar11 = 6; iVar11 != 0; iVar11 = iVar11 + -1) {
        *pfVar20 = *pfVar14;
        pfVar14 = pfVar14 + 1;
        pfVar20 = pfVar20 + 1;
      }
      local_10c = &local_114;
      if (param_2[4] <= *(float *)(iVar10 + 0x14)) {
        uVar17 = 0;
      }
      else {
        uVar17 = 4;
      }
      if (param_2[5] <= *(float *)(iVar10 + 0x18)) {
        uVar21 = 0;
      }
      else {
        uVar21 = 0x10;
      }
      if (*(float *)(iVar10 + 4) <= *param_2) {
        uVar19 = 0;
      }
      else {
        uVar19 = 2;
      }
      if (*(float *)(iVar10 + 8) <= param_2[1]) {
        uVar12 = 0;
      }
      else {
        uVar12 = 8;
      }
      if (*(float *)(iVar10 + 0xc) <= param_2[2]) {
        uVar7 = 0;
      }
      else {
        uVar7 = 0x20;
      }
      local_4 = local_108 + 1;
      local_108[0] = (undefined1 *)
                     (uVar7 | uVar12 | uVar19 | uVar21 | uVar17 | (uint)(fVar4 < fVar5));
      pfVar14 = param_2;
      local_148 = iVar10;
      do {
        uVar16 = (undefined2)((uint)pfVar14 >> 0x10);
        local_4 = local_4 + -2;
        puVar8 = *local_4;
        if (puVar8[1] == -1) {
LAB_0053b966:
          iVar11 = *(int *)(local_148 + 0x20);
          if (local_4[1] != (undefined1 *)0x0) goto code_r0x0053b97e;
          iVar9 = (uint)*(ushort *)(puVar8 + 2) * 0x10 + iVar11;
          cVar6 = *(char *)(iVar9 + 1);
          iVar15 = iVar9;
          while (cVar6 == -1) {
            iVar15 = (uint)*(ushort *)(iVar15 + 2) * 0x10 + iVar11;
            cVar6 = *(char *)(iVar15 + 1);
          }
          iVar1 = iVar9 + 8;
          cVar6 = *(char *)(iVar9 + 9);
          while (cVar6 == -1) {
            iVar9 = (uint)*(ushort *)(iVar1 + 2) * 0x10;
            iVar1 = iVar9 + 8 + iVar11;
            cVar6 = *(char *)(iVar9 + 9 + iVar11);
          }
          pfVar14 = (float *)CONCAT22((short)((uint)iVar15 >> 0x10),*(undefined2 *)(iVar15 + 2));
          local_154 = (undefined1 *)0x0;
          uVar17 = (uint)(ushort)((ushort)*(byte *)(iVar1 + 1) + *(short *)(iVar1 + 2)) -
                   (int)pfVar14;
          goto LAB_0053ba56;
        }
LAB_0053ba46:
        local_154 = local_4[1];
        pfVar14 = (float *)CONCAT22(uVar16,*(undefined2 *)(puVar8 + 2));
        uVar17 = (uint)(byte)puVar8[1];
LAB_0053ba56:
        uVar21 = (uint)pfVar14 & 0xffff;
        local_15c = (uVar17 & 0xffff) + uVar21;
        local_158 = pfVar14;
        if (uVar21 < local_15c) {
          do {
            if (*(int *)(iVar10 + 0x24) != 0) {
              uVar21 = (uint)*(ushort *)(*(int *)(iVar10 + 0x24) + uVar21 * 2);
            }
            pfVar20 = (float *)(local_150 + (uint)*(ushort *)(local_14c + uVar21 * 8) * 0xc);
            pfVar2 = (float *)(local_150 + (uint)*(ushort *)(local_14c + 2 + uVar21 * 8) * 0xc);
            pfVar3 = (float *)(local_150 + (uint)*(ushort *)(local_14c + 4 + uVar21 * 8) * 0xc);
            if (local_154 == (undefined1 *)0x0) {
LAB_0053bb38:
              local_168 = *pfVar3 - *pfVar20;
              local_164 = pfVar3[1] - pfVar20[1];
              local_188 = *pfVar20;
              local_184 = pfVar20[1];
              local_180 = pfVar20[2];
              local_194 = (pfVar3[2] - pfVar20[2]) * (pfVar2[1] - pfVar20[1]) -
                          local_164 * (pfVar2[2] - pfVar20[2]);
              local_190 = local_168 * (pfVar2[2] - pfVar20[2]) -
                          (pfVar3[2] - pfVar20[2]) * (*pfVar2 - *pfVar20);
              local_18c = local_164 * (*pfVar2 - *pfVar20) - local_168 * (pfVar2[1] - pfVar20[1]);
              local_16c = local_18c * local_18c + local_194 * local_194 + local_190 * local_190;
              local_17c = uVar21;
              local_178 = pfVar20;
              local_174 = pfVar2;
              local_170 = pfVar3;
              fVar22 = (float10)FUN_004c3b90(local_16c);
              local_194 = (float)((float10)local_194 * fVar22);
              local_190 = (float)((float10)local_190 * fVar22);
              local_18c = (float)(fVar22 * (float10)local_18c);
              uVar23 = (*param_3)(param_2,&local_194,0,param_4);
              pfVar14 = (float *)((ulonglong)uVar23 >> 0x20);
              if ((int)uVar23 == 0) {
                return;
              }
            }
            else {
              uVar23 = FUN_00547bf0(param_2,pfVar20,pfVar2,pfVar3);
              pfVar14 = (float *)((ulonglong)uVar23 >> 0x20);
              if ((int)uVar23 != 0) goto LAB_0053bb38;
            }
            local_158 = (float *)((int)local_158 + 1);
            uVar21 = (uint)local_158 & 0xffff;
          } while ((int)uVar21 < (int)local_15c);
        }
        if (local_4 <= &local_10c) {
          return;
        }
      } while( true );
    }
  }
  return;
code_r0x0053b97e:
  pbVar13 = (byte *)((uint)*(ushort *)(puVar8 + 2) * 0x10 + iVar11);
  uVar17 = *pbVar13 & 0xc;
  if (*(float *)(pbVar13 + 0xc) <= *(float *)((int)local_144 + uVar17)) {
    *local_4 = pbVar13 + 8;
    fVar4 = *(float *)((int)local_144 + uVar17 + 0xc);
    if (fVar4 < *(float *)(pbVar13 + 4) != (fVar4 == *(float *)(pbVar13 + 4))) {
      local_4 = local_4 + 2;
      *local_4 = pbVar13;
      local_4[1] = local_4[-1];
      if (*(float *)(pbVar13 + 4) <= *(float *)((int)local_144 + uVar17 + 0x18)) {
        local_4[1] = (undefined1 *)((uint)local_4[1] & ~(2 << (sbyte)(uVar17 >> 1)));
      }
      fVar4 = *(float *)((int)local_144 + uVar17 + 0x24);
      if (fVar4 < *(float *)(pbVar13 + 0xc) != (fVar4 == *(float *)(pbVar13 + 0xc))) {
        pbVar13 = (byte *)~(1 << (sbyte)(uVar17 >> 1));
        local_4[-1] = (undefined1 *)((uint)local_4[-1] & (uint)pbVar13);
      }
    }
  }
  else {
    *local_4 = pbVar13;
  }
  uVar16 = (undefined2)((uint)pbVar13 >> 0x10);
  puVar8 = *local_4;
  if (puVar8[1] != -1) goto LAB_0053ba46;
  goto LAB_0053b966;
}
