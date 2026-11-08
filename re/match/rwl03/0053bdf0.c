// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053bdf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005e4578, 0x005e4578);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float10 FUN_004c3b90(float p0);
undefined4 FUN_00547450(float * p0, float * p1, float * p2, float * p3, float * p4, float * p5);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005e4578, 0x005e4578);
// MATCH: 0x0053bdf0 FUN_0053bdf0
/* [C1 2026-05-20] Sphere-vs-atomic counterpart (mirrors FUN_00539ec0). */

void FUN_0053bdf0(int param_1,float *param_2,undefined4 param_3,code *param_4,undefined4 param_5)

{
  int iVar1;
  float *pfVar2;
  float fVar3;
  char cVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  uint uVar12;
  undefined1 *puVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  undefined2 uVar21;
  byte *pbVar19;
  int iVar20;
  int iVar22;
  float *pfVar23;
  uint uVar24;
  float *pfVar25;
  uint uVar26;
  ushort *puVar27;
  float10 fVar28;
  undefined8 uVar29;
  uint local_1ac;
  float local_1a8;
  float local_1a4 [4];
  float local_194;
  float local_190;
  uint local_18c;
  float *local_188;
  float *local_184;
  float *local_180;
  float local_17c [4];
  float local_16c;
  float local_168;
  float local_164;
  uint local_160;
  int local_15c;
  uint local_158;
  undefined1 *local_154;
  int local_150;
  float fStack_14c;
  int local_148;
  float local_144 [12];
  undefined1 local_114;
  undefined1 local_113;
  undefined2 local_112;
  undefined1 *local_10c;
  undefined1 *local_108 [65];
  undefined1 **local_4;
  
  iVar15 = *(int *)(DAT_007dc5b0 + param_1);
  if (iVar15 == 0) {
    local_164 = _DAT_005cc320 / param_2[3];
    local_160 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
    puVar27 = *(ushort **)(param_1 + 0x2c);
    local_1ac = 0;
    if (0 < *(int *)(param_1 + 0x10)) {
      do {
        pfVar23 = (float *)(local_160 + (uint)*puVar27 * 0xc);
        pfVar25 = (float *)(local_160 + (uint)puVar27[1] * 0xc);
        pfVar2 = (float *)(local_160 + (uint)puVar27[2] * 0xc);
        iVar15 = FUN_00547450(param_2,pfVar23,pfVar25,pfVar2,local_1a4,&local_1a8);
        if (iVar15 != 0) {
          local_1a8 = local_1a8 * local_164;
          local_1a4[3] = *pfVar23;
          local_194 = pfVar23[1];
          local_190 = pfVar23[2];
          local_18c = local_1ac;
          local_188 = pfVar23;
          local_184 = pfVar25;
          local_180 = pfVar2;
          iVar15 = (*param_4)(param_3,local_1a4,local_1a8,param_5);
          if (iVar15 == 0) {
            return;
          }
        }
        local_1ac = local_1ac + 1;
        puVar27 = puVar27 + 4;
      } while ((int)local_1ac < *(int *)(param_1 + 0x10));
    }
  }
  else {
    if (*(float *)(iVar15 + 0x10) <= *param_2) {
      fVar5 = DAT_005d757c;
      if (*(float *)(iVar15 + 4) < *param_2) {
        fVar5 = *param_2 - *(float *)(iVar15 + 4);
      }
    }
    else {
      fVar5 = *(float *)(iVar15 + 0x10) - *param_2;
    }
    if (*(float *)(iVar15 + 0x14) <= param_2[1]) {
      fVar6 = DAT_005d757c;
      if (*(float *)(iVar15 + 8) < param_2[1]) {
        fVar6 = param_2[1] - *(float *)(iVar15 + 8);
      }
    }
    else {
      fVar6 = *(float *)(iVar15 + 0x14) - param_2[1];
    }
    if (*(float *)(iVar15 + 0x18) <= param_2[2]) {
      fVar7 = DAT_005d757c;
      if (*(float *)(iVar15 + 0xc) < param_2[2]) {
        fVar7 = param_2[2] - *(float *)(iVar15 + 0xc);
      }
    }
    else {
      fVar7 = *(float *)(iVar15 + 0x18) - param_2[2];
    }
    if (fVar7 * fVar7 + fVar6 * fVar6 + fVar5 * fVar5 <= param_2[3] * param_2[3]) {
      fVar5 = *param_2;
      fVar6 = param_2[1];
      fVar7 = param_2[2];
      local_164 = _DAT_005cc320 / param_2[3];
      fVar11 = param_2[3] * _DAT_005e4578;
      local_17c[3] = *param_2 - param_2[3];
      local_16c = param_2[1] - param_2[3];
      local_168 = param_2[2] - param_2[3];
      local_17c[0] = param_2[3] + *param_2;
      local_17c[1] = param_2[3] + param_2[1];
      fVar8 = *param_2;
      local_17c[2] = param_2[3] + param_2[2];
      fVar9 = param_2[1];
      fVar10 = param_2[2];
      iVar22 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
      local_15c = iVar22;
      local_150 = *(int *)(param_1 + 0x2c);
      local_1a4[3] = fVar8 - fVar11;
      local_194 = fVar9 - fVar11;
      local_190 = fVar10 - fVar11;
      local_1a4[0] = fVar5 + fVar11;
      local_1a4[1] = fVar6 + fVar11;
      local_1a4[2] = fVar7 + fVar11;
      if ((((*(float *)(iVar15 + 0x10) < local_17c[0] != (*(float *)(iVar15 + 0x10) == local_17c[0])
            ) && (*(float *)(iVar15 + 0x14) < local_17c[1] !=
                  (*(float *)(iVar15 + 0x14) == local_17c[1]))) &&
          (*(float *)(iVar15 + 0x18) < local_17c[2] != (*(float *)(iVar15 + 0x18) == local_17c[2])))
         && (((*param_2 - param_2[3] <= *(float *)(iVar15 + 4) &&
              (param_2[1] - param_2[3] <= *(float *)(iVar15 + 8))) &&
             (param_2[2] - param_2[3] <= *(float *)(iVar15 + 0xc))))) {
        local_148 = iVar15;
        local_112 = 0;
        local_113 = 0xff;
        if (*(short *)(iVar15 + 0x1e) == 0) {
          local_113 = *(undefined1 *)(iVar15 + 0x1c);
        }
        fVar3 = *(float *)(iVar15 + 0x10);
        pfVar23 = local_17c;
        pfVar25 = local_144;
        for (iVar16 = 6; iVar16 != 0; iVar16 = iVar16 + -1) {
          *pfVar25 = *pfVar23;
          pfVar23 = pfVar23 + 1;
          pfVar25 = pfVar25 + 1;
        }
        pfVar23 = local_1a4;
        pfVar25 = local_144 + 6;
        for (iVar16 = 6; iVar16 != 0; iVar16 = iVar16 + -1) {
          *pfVar25 = *pfVar23;
          pfVar23 = pfVar23 + 1;
          pfVar25 = pfVar25 + 1;
        }
        local_10c = &local_114;
        if (fVar9 - fVar11 <= *(float *)(iVar15 + 0x14)) {
          uVar26 = 0;
        }
        else {
          uVar26 = 4;
        }
        if (fVar10 - fVar11 <= *(float *)(iVar15 + 0x18)) {
          uVar24 = 0;
        }
        else {
          uVar24 = 0x10;
        }
        if (*(float *)(iVar15 + 4) <= fVar5 + fVar11) {
          uVar18 = 0;
        }
        else {
          uVar18 = 2;
        }
        if (*(float *)(iVar15 + 8) <= fVar6 + fVar11) {
          uVar17 = 0;
        }
        else {
          uVar17 = 8;
        }
        if (*(float *)(iVar15 + 0xc) <= fVar7 + fVar11) {
          uVar12 = 0;
        }
        else {
          uVar12 = 0x20;
        }
        local_4 = local_108 + 1;
        local_108[0] = (undefined1 *)
                       (uVar12 | uVar17 | uVar18 | uVar24 | uVar26 | (uint)(fVar3 < fVar8 - fVar11))
        ;
        do {
          uVar21 = (undefined2)(uVar18 >> 0x10);
          local_4 = local_4 + -2;
          puVar13 = *local_4;
          if (puVar13[1] == -1) {
LAB_0053c16e:
            iVar16 = *(int *)(local_148 + 0x20);
            if (local_4[1] != (undefined1 *)0x0) goto code_r0x0053c186;
            iVar14 = (uint)*(ushort *)(puVar13 + 2) * 0x10 + iVar16;
            cVar4 = *(char *)(iVar14 + 1);
            iVar20 = iVar14;
            while (cVar4 == -1) {
              iVar20 = (uint)*(ushort *)(iVar20 + 2) * 0x10 + iVar16;
              cVar4 = *(char *)(iVar20 + 1);
            }
            iVar1 = iVar14 + 8;
            cVar4 = *(char *)(iVar14 + 9);
            while (cVar4 == -1) {
              iVar14 = (uint)*(ushort *)(iVar1 + 2) * 0x10;
              iVar1 = iVar14 + 8 + iVar16;
              cVar4 = *(char *)(iVar14 + 9 + iVar16);
            }
            uVar18 = CONCAT22((short)((uint)iVar20 >> 0x10),*(undefined2 *)(iVar20 + 2));
            local_154 = (undefined1 *)0x0;
            uVar26 = (ushort)((ushort)*(byte *)(iVar1 + 1) + *(short *)(iVar1 + 2)) - uVar18;
            goto LAB_0053c268;
          }
LAB_0053c258:
          uVar18 = CONCAT22(uVar21,*(undefined2 *)(puVar13 + 2));
          uVar26 = (uint)(byte)puVar13[1];
          local_154 = local_4[1];
LAB_0053c268:
          uVar24 = uVar18 & 0xffff;
          local_158 = uVar18;
          local_160 = (uVar26 & 0xffff) + uVar24;
          if (uVar24 < local_160) {
            do {
              if (*(int *)(iVar15 + 0x24) != 0) {
                uVar24 = (uint)*(ushort *)(*(int *)(iVar15 + 0x24) + uVar24 * 2);
              }
              pfVar23 = (float *)(iVar22 + (uint)*(ushort *)(local_150 + uVar24 * 8) * 0xc);
              pfVar25 = (float *)(iVar22 + (uint)*(ushort *)(local_150 + 2 + uVar24 * 8) * 0xc);
              pfVar2 = (float *)(iVar22 + (uint)*(ushort *)(local_150 + 4 + uVar24 * 8) * 0xc);
              if (local_154 == (undefined1 *)0x0) {
                local_17c[0] = *pfVar2 - *pfVar23;
                local_17c[1] = pfVar2[1] - pfVar23[1];
                local_1a4[0] = (pfVar2[2] - pfVar23[2]) * (pfVar25[1] - pfVar23[1]) -
                               local_17c[1] * (pfVar25[2] - pfVar23[2]);
                local_1a4[1] = local_17c[0] * (pfVar25[2] - pfVar23[2]) -
                               (pfVar2[2] - pfVar23[2]) * (*pfVar25 - *pfVar23);
                local_1a4[2] = local_17c[1] * (*pfVar25 - *pfVar23) -
                               local_17c[0] * (pfVar25[1] - pfVar23[1]);
                fStack_14c = local_1a4[2] * local_1a4[2] +
                             local_1a4[0] * local_1a4[0] + local_1a4[1] * local_1a4[1];
                fVar28 = (float10)FUN_004c3b90(fStack_14c);
                local_1a4[0] = (float)((float10)local_1a4[0] * fVar28);
                local_1a4[1] = (float)((float10)local_1a4[1] * fVar28);
                local_1a4[2] = (float)(fVar28 * (float10)local_1a4[2]);
                local_1a8 = (param_2[2] - pfVar23[2]) * local_1a4[2] +
                            (*param_2 - *pfVar23) * local_1a4[0] +
                            (param_2[1] - pfVar23[1]) * local_1a4[1];
LAB_0053c42f:
                local_1a8 = local_1a8 * local_164;
                local_1a4[3] = *pfVar23;
                local_194 = pfVar23[1];
                local_190 = pfVar23[2];
                local_18c = uVar24;
                local_188 = pfVar23;
                local_184 = pfVar25;
                local_180 = pfVar2;
                uVar29 = (*param_4)(param_3,local_1a4,local_1a8,param_5);
                uVar18 = (uint)((ulonglong)uVar29 >> 0x20);
                if ((int)uVar29 == 0) {
                  return;
                }
              }
              else {
                uVar29 = FUN_00547450(param_2,pfVar23,pfVar25,pfVar2,local_1a4,&local_1a8);
                uVar18 = (uint)((ulonglong)uVar29 >> 0x20);
                if ((int)uVar29 != 0) goto LAB_0053c42f;
              }
              local_158 = local_158 + 1;
              uVar24 = local_158 & 0xffff;
              iVar22 = local_15c;
            } while ((int)uVar24 < (int)local_160);
          }
          if (local_4 <= &local_10c) {
            return;
          }
        } while( true );
      }
    }
  }
  return;
code_r0x0053c186:
  pbVar19 = (byte *)((uint)*(ushort *)(puVar13 + 2) * 0x10 + iVar16);
  uVar26 = *pbVar19 & 0xc;
  if (*(float *)(pbVar19 + 0xc) <= *(float *)((int)local_144 + uVar26)) {
    *local_4 = pbVar19 + 8;
    fVar5 = *(float *)((int)local_144 + uVar26 + 0xc);
    if (fVar5 < *(float *)(pbVar19 + 4) != (fVar5 == *(float *)(pbVar19 + 4))) {
      local_4 = local_4 + 2;
      *local_4 = pbVar19;
      local_4[1] = local_4[-1];
      if (*(float *)(pbVar19 + 4) <= *(float *)((int)local_144 + uVar26 + 0x18)) {
        local_4[1] = (undefined1 *)((uint)local_4[1] & ~(2 << (sbyte)(uVar26 >> 1)));
      }
      fVar5 = *(float *)((int)local_144 + uVar26 + 0x24);
      if (fVar5 < *(float *)(pbVar19 + 0xc) != (fVar5 == *(float *)(pbVar19 + 0xc))) {
        pbVar19 = (byte *)~(1 << (sbyte)(uVar26 >> 1));
        local_4[-1] = (undefined1 *)((uint)local_4[-1] & (uint)pbVar19);
      }
    }
  }
  else {
    *local_4 = pbVar19;
  }
  uVar21 = (undefined2)((uint)pbVar19 >> 0x10);
  puVar13 = *local_4;
  if (puVar13[1] != -1) goto LAB_0053c258;
  goto LAB_0053c16e;
}
