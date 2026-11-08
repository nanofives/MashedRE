// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL04 0x0054a120 h1: 12.1% (regs 29.3%, ops 51.7%) raw converter body with /O2 /GS /Ob1 (best of sweep); structure only (2 KB+, 45 min cap not spent). C++ fixed with pointer casts and float* prototypes. Class (b). */
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
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_0054ba20(float * p0, int p1, float * p2, int p3);
undefined1 * FUN_0054bda0(undefined1 * p0, int p1, float * p2, int p3);
float * FUN_0054c400(float * p0, int p1, float * p2, int p3);
void FUN_0054c4f0(float * p0, int p1, float * p2, int p3);
undefined1 * FUN_0054c870(undefined1 * p0, int p1, float * p2, int p3);
void FUN_0054ceb0(float * p0, int p1, float * p2, int p3);
short * FUN_0054cfa0(short * p0, short p1, uint p2);
short * FUN_0054d090(short * p0, short p1, int p2);
// MATCH: 0x0054a120 FUN_0054a120
undefined4 FUN_0054a120(int param_1,int param_2,uint param_3)

{
  float *pfVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  undefined4 uVar10;
  int iVar11;
  bool bVar12;
  bool bVar13;
  float *pfVar14;
  uint uVar15;
  float *pfVar16;
  uint uVar17;
  int iVar18;
  int iVar19;
  ushort *puVar20;
  ushort *puVar21;
  ushort *puVar22;
  int iVar23;
  undefined4 *puVar24;
  int *piVar25;
  float *local_19c;
  int local_198;
  undefined4 *local_190;
  int local_18c;
  int local_188;
  int local_184;
  undefined2 *local_17c;
  uint local_174;
  int local_170;
  int local_16c;
  int local_168;
  undefined4 *local_15c;
  int local_154;
  uint local_14c;
  int local_138;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_124;
  undefined4 local_120;
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  int local_10c;
  float local_108 [65];
  
  iVar3 = *(int *)(*(int *)(param_2 + 0x5c) + 0x14);
  iVar4 = *(int *)(*(int *)(param_2 + 0x5c) + 0x18);
  iVar5 = *(int *)(param_2 + 0x54);
  iVar6 = *(int *)(param_2 + 0x30);
  local_174 = (uint)*(ushort *)(iVar5 + 4);
  local_15c = (undefined4 *)(*(int *)(iVar5 + 0xc) + 0x10 + iVar5);
  iVar7 = *(int *)(param_1 + 0xc);
  uVar8 = *(uint *)(param_1 + 8);
  local_190 = (undefined4 *)(param_1 + 0x24);
  local_17c = *(undefined2 **)(param_1 + 4);
  iVar5 = param_3 + 1;
  iVar11 = iVar5 * param_3 * 2;
  local_170 = *(int *)(param_1 + 0x10);
  local_10c = iVar5 * iVar5;
  uVar17 = uVar8 >> 0x10 & 0xff;
  local_154 = (param_3 + 2) * param_3;
  local_168 = 0;
  iVar9 = *(int *)(param_2 + 0x1c);
  if ((param_3 & 1) != 0) {
    local_154 = local_154 + 1;
  }
  local_14c = local_174;
  if (local_174 == 0) {
    *(undefined4 *)(param_1 + 0x14) = 0;
    return 1;
  }
  do {
    bVar12 = true;
    *local_190 = (undefined4)local_17c;
    local_16c = 0;
    uVar10 = local_15c[2];
    local_190[5] = (undefined4)local_15c;
    local_190[4] = uVar10;
    local_190[1] = 0;
    local_190[2] = (undefined4)local_168;
    local_190[3] = 0;
    local_138 = local_15c[1];
    if (local_138 == 0) {
      local_174 = local_174 - 1;
    }
    else {
      puVar22 = (ushort *)*local_15c;
      if (0 < local_138) {
        puVar24 = local_190;
        do {
          iVar18 = 0;
          bVar13 = false;
          iVar19 = iVar11;
          if ((iVar9 != 0) &&
             (*(float *)(*(int *)(param_2 + 0x34) + (uint)puVar22[5] * 8) != DAT_005d757c)) {
            bVar13 = true;
            iVar19 = local_154;
          }
          if (((uint)(puVar24[1] + iVar19) < 0x10000) ||
             (*(int *)(param_1 + 0x14) <= (int)local_174)) {
            if (!bVar12) {
              *local_17c = local_17c[-1];
              local_17c[1] = (undefined2)local_16c;
              local_17c = local_17c + 2;
              puVar24[1] = puVar24[1] + 2;
            }
          }
          else {
            local_190 = puVar24 + 6;
            local_16c = 0;
            *local_190 = (undefined4)local_17c;
            uVar10 = local_15c[2];
            puVar24[0xb] = (undefined4)local_15c;
            puVar24[8] = (undefined4)local_168;
            local_174 = local_174 + 1;
            puVar24[10] = uVar10;
            puVar24[7] = 0;
            puVar24[9] = 0;
            puVar24 = local_190;
          }
          pfVar16 = local_108 + 1;
          if (bVar13) {
            local_18c = 0;
            local_198 = 4;
            do {
              pfVar14 = pfVar16;
              iVar19 = 0;
              iVar23 = local_198;
              if (local_198 < 1) {
LAB_0054a36b:
                iVar23 = 4 - iVar19;
                pfVar14 = local_108 + (local_18c + iVar19 * 4) * 4 + 1;
                do {
                  pfVar14[-1] = 0.0;
                  *pfVar14 = 0.0;
                  pfVar14[1] = 0.0;
                  pfVar14 = pfVar14 + 0x10;
                  iVar23 = iVar23 + -1;
                } while (iVar23 != 0);
              }
              else {
                do {
                  puVar20 = puVar22 + iVar18;
                  iVar18 = iVar18 + 1;
                  iVar23 = iVar23 + -1;
                  pfVar1 = (float *)(iVar3 + (uint)*puVar20 * 0xc);
                  pfVar14[-1] = *pfVar1;
                  *pfVar14 = pfVar1[1];
                  pfVar14[1] = pfVar1[2];
                  pfVar14 = pfVar14 + 0x10;
                } while (iVar23 != 0);
                iVar19 = local_198;
                puVar24 = local_190;
                if (local_198 < 4) goto LAB_0054a36b;
              }
              local_18c = local_18c + 1;
              local_198 = local_198 + -1;
              pfVar16 = pfVar16 + 4;
            } while (0 < local_198);
            FUN_0054ba20((float *)local_170,iVar7,local_108,param_3);
            iVar19 = 0;
            local_198 = local_170 + 0xc;
            if ((uVar8 & 2) != 0) {
              if (iVar4 != 0) {
                pfVar16 = local_108 + 1;
                local_18c = 0;
                local_184 = 4;
                do {
                  pfVar14 = pfVar16;
                  iVar18 = 0;
                  iVar23 = local_184;
                  if (local_184 < 1) {
LAB_0054a452:
                    iVar23 = 4 - iVar18;
                    pfVar14 = local_108 + (local_18c + iVar18 * 4) * 4 + 1;
                    do {
                      pfVar14[-1] = 0.0;
                      *pfVar14 = 0.0;
                      pfVar14[1] = 0.0;
                      pfVar14 = pfVar14 + 0x10;
                      iVar23 = iVar23 + -1;
                    } while (iVar23 != 0);
                  }
                  else {
                    do {
                      puVar20 = puVar22 + iVar19;
                      iVar19 = iVar19 + 1;
                      iVar23 = iVar23 + -1;
                      pfVar1 = (float *)(iVar4 + (uint)*puVar20 * 0xc);
                      pfVar14[-1] = *pfVar1;
                      *pfVar14 = pfVar1[1];
                      pfVar14[1] = pfVar1[2];
                      pfVar14 = pfVar14 + 0x10;
                    } while (iVar23 != 0);
                    iVar18 = local_184;
                    puVar24 = local_190;
                    if (local_184 < 4) goto LAB_0054a452;
                  }
                  local_18c = local_18c + 1;
                  local_184 = local_184 + -1;
                  pfVar16 = pfVar16 + 4;
                } while (0 < local_184);
                FUN_0054ba20((float *)local_198,iVar7,local_108,param_3);
              }
              local_198 = local_170 + 0x18;
            }
            if ((uVar8 & 4) != 0) {
              if (iVar6 != 0) {
                local_184 = 0;
                local_18c = 0;
                local_19c = local_108 + 1;
                local_188 = 4;
                do {
                  pfVar16 = local_19c;
                  iVar19 = 0;
                  iVar18 = local_188;
                  if (local_188 < 1) {
LAB_0054a575:
                    pfVar16 = local_108 + (local_18c + iVar19 * 4) * 4 + 1;
                    iVar19 = 4 - iVar19;
                    do {
                      pfVar16[-1] = 0.0;
                      *pfVar16 = 0.0;
                      pfVar16[1] = 0.0;
                      pfVar16[2] = 0.0;
                      pfVar16 = pfVar16 + 0x10;
                      iVar19 = iVar19 + -1;
                    } while (iVar19 != 0);
                  }
                  else {
                    do {
                      uVar15 = (uint)puVar22[local_184];
                      pfVar16[1] = (float)*(byte *)(iVar6 + uVar15 * 4);
                      *pfVar16 = (float)*(byte *)(iVar6 + 1 + uVar15 * 4);
                      pfVar16[-1] = (float)*(byte *)(iVar6 + 2 + uVar15 * 4);
                      local_184 = local_184 + 1;
                      iVar18 = iVar18 + -1;
                      pfVar16[2] = (float)*(byte *)(iVar6 + 3 + uVar15 * 4);
                      pfVar16 = pfVar16 + 0x10;
                    } while (iVar18 != 0);
                    iVar19 = local_188;
                    puVar24 = local_190;
                    if (local_188 < 4) goto LAB_0054a575;
                  }
                  local_18c = local_18c + 1;
                  local_188 = local_188 + -1;
                  local_19c = local_19c + 4;
                } while (0 < local_188);
                FUN_0054bda0((undefined1 *)local_198,iVar7,local_108,param_3);
              }
              local_198 = local_198 + 4;
            }
            iVar19 = 0;
            if (0 < iVar9) {
              local_19c = (float *)(param_2 + 0x34);
              do {
                if ((int)uVar17 <= iVar19) break;
                iVar18 = (int)*local_19c;
                local_12c = *(undefined4 *)(iVar18 + (uint)*puVar22 * 8);
                local_128 = *(undefined4 *)(iVar18 + 4 + (uint)*puVar22 * 8);
                local_124 = *(undefined4 *)(iVar18 + (uint)puVar22[9] * 8);
                local_120 = *(undefined4 *)(iVar18 + 4 + (uint)puVar22[9] * 8);
                local_11c = *(undefined4 *)(iVar18 + (uint)puVar22[3] * 8);
                local_118 = *(undefined4 *)(iVar18 + 4 + (uint)puVar22[3] * 8);
                FUN_0054c400((float *)local_198,iVar7,(float *)&local_12c,param_3);
                local_198 = local_198 + 8;
                iVar19 = iVar19 + 1;
                local_19c = local_19c + 1;
              } while (iVar19 < iVar9);
            }
            FUN_0054cfa0((short *)local_17c,local_16c,param_3);
            iVar19 = local_154 * 2;
            iVar18 = (int)((param_3 + 2) * iVar5) / 2;
            iVar23 = local_154;
          }
          else {
            local_19c = (float *)0x4;
            puVar20 = puVar22;
            do {
              iVar19 = 4;
              pfVar14 = pfVar16;
              puVar21 = puVar20;
              do {
                uVar2 = *puVar21;
                pfVar16 = pfVar14 + 4;
                puVar21 = puVar21 + 4;
                iVar19 = iVar19 + -1;
                pfVar1 = (float *)(iVar3 + (uint)uVar2 * 0xc);
                pfVar14[-1] = *pfVar1;
                *pfVar14 = pfVar1[1];
                pfVar14[1] = pfVar1[2];
                pfVar14 = pfVar16;
              } while (iVar19 != 0);
              puVar20 = puVar20 + 1;
              local_19c = (float *)((int)local_19c + -1);
            } while (local_19c != (float *)0x0);
            FUN_0054c4f0((float *)local_170,iVar7,local_108,param_3);
            local_198 = local_170 + 0xc;
            if ((uVar8 & 2) != 0) {
              if (iVar4 != 0) {
                pfVar16 = local_108 + 1;
                local_19c = (float *)0x4;
                puVar20 = puVar22;
                do {
                  iVar19 = 4;
                  pfVar14 = pfVar16;
                  puVar21 = puVar20;
                  do {
                    uVar2 = *puVar21;
                    pfVar16 = pfVar14 + 4;
                    puVar21 = puVar21 + 4;
                    iVar19 = iVar19 + -1;
                    pfVar1 = (float *)(iVar4 + (uint)uVar2 * 0xc);
                    pfVar14[-1] = *pfVar1;
                    *pfVar14 = pfVar1[1];
                    pfVar14[1] = pfVar1[2];
                    pfVar14 = pfVar16;
                  } while (iVar19 != 0);
                  puVar20 = puVar20 + 1;
                  local_19c = (float *)((int)local_19c + -1);
                } while (local_19c != (float *)0x0);
                FUN_0054c4f0((float *)local_198,iVar7,local_108,param_3);
              }
              local_198 = local_170 + 0x18;
            }
            if ((uVar8 & 4) != 0) {
              if (iVar6 != 0) {
                pfVar16 = local_108 + 1;
                local_19c = (float *)0x4;
                puVar20 = puVar22;
                do {
                  iVar19 = 4;
                  pfVar14 = pfVar16;
                  puVar21 = puVar20;
                  do {
                    uVar15 = (uint)*puVar21;
                    pfVar16 = pfVar14 + 4;
                    puVar21 = puVar21 + 4;
                    pfVar14[1] = (float)*(byte *)(iVar6 + uVar15 * 4);
                    *pfVar14 = (float)*(byte *)(iVar6 + 1 + uVar15 * 4);
                    iVar19 = iVar19 + -1;
                    pfVar14[-1] = (float)*(byte *)(iVar6 + 2 + uVar15 * 4);
                    pfVar14[2] = (float)*(byte *)(iVar6 + 3 + uVar15 * 4);
                    pfVar14 = pfVar16;
                  } while (iVar19 != 0);
                  puVar20 = puVar20 + 1;
                  local_19c = (float *)((int)local_19c + -1);
                } while (local_19c != (float *)0x0);
                FUN_0054c870((undefined1 *)local_198,iVar7,local_108,param_3);
              }
              local_198 = local_198 + 4;
            }
            iVar19 = 0;
            if (0 < iVar9) {
              piVar25 = (int *)(param_2 + 0x34);
              do {
                if ((int)uVar17 <= iVar19) break;
                iVar18 = *piVar25;
                local_12c = *(undefined4 *)(iVar18 + (uint)*puVar22 * 8);
                local_128 = *(undefined4 *)(iVar18 + 4 + (uint)*puVar22 * 8);
                local_124 = *(undefined4 *)(iVar18 + (uint)puVar22[3] * 8);
                local_120 = *(undefined4 *)(iVar18 + 4 + (uint)puVar22[3] * 8);
                local_11c = *(undefined4 *)(iVar18 + (uint)puVar22[0xc] * 8);
                local_118 = *(undefined4 *)(iVar18 + 4 + (uint)puVar22[0xc] * 8);
                local_114 = *(undefined4 *)(iVar18 + (uint)puVar22[0xf] * 8);
                local_110 = *(undefined4 *)(iVar18 + 4 + (uint)puVar22[0xf] * 8);
                FUN_0054ceb0((float *)local_198,iVar7,(float *)&local_12c,param_3);
                local_198 = local_198 + 8;
                iVar19 = iVar19 + 1;
                piVar25 = piVar25 + 1;
              } while (iVar19 < iVar9);
            }
            FUN_0054d090((short *)local_17c,local_16c,param_3);
            iVar19 = iVar5 * param_3 * 4;
            iVar18 = local_10c;
            iVar23 = iVar11;
            puVar24 = local_190;
          }
          local_16c = local_16c + iVar18;
          local_170 = local_170 + iVar18 * iVar7;
          local_17c = (undefined2 *)((int)local_17c + iVar19);
          local_168 = local_168 + iVar18;
          local_138 = local_138 + -0x10;
          bVar12 = false;
          puVar22 = puVar22 + 0x10;
          puVar24[3] = puVar24[3] + iVar18;
          puVar24[1] = puVar24[1] + iVar23;
        } while (0 < local_138);
      }
      local_190 = local_190 + 6;
    }
    local_15c = local_15c + 3;
    local_14c = local_14c - 1;
    if (local_14c == 0) {
      *(uint *)(param_1 + 0x14) = local_174;
      return 1;
    }
  } while( true );
}
