// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00549b20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
void FUN_004522d0(int p0);
int * FUN_004e8640(int p0, void * p1);
int FUN_004e89e0(int p0, ushort p1);
int FUN_004e8a10(int p0);
undefined4 FUN_004f0970(undefined4 p0);
undefined4 FUN_004f1130(void);
int * FUN_004f3b60(int * p0);
int FUN_004f3be0(int * p0, int p1);
undefined4 FUN_00549580(int p0);
int * FUN_0054b340(int * p0, float p1, float p2, int p3, int p4);
// MATCH: 0x00549b20 FUN_00549b20
int * FUN_00549b20(int *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  undefined2 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  uint *puVar14;
  uint uVar15;
  uint uStack_2c;
  undefined2 uStack_28;
  int iStack_24;
  uint *puStack_20;
  int *local_1c;
  uint uStack_18;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar2 = param_1[0xc];
  iVar4 = FUN_00549580((int)param_1);
  uVar15 = param_1[0x10];
  iVar12 = *(int *)(iVar4 + 0x5c);
  iVar4 = FUN_004e89e0(iVar4,(uVar15 >> 0xf & 1) << 0xb | (uVar15 >> 0xe & 1) << 10 |
                             (uVar15 >> 0xd & 1) << 9 | (uVar15 >> 0xc & 1) << 8 |
                             (uVar15 >> 0xb & 1) << 7 | (uVar15 >> 10 & 1) << 6 |
                             (uVar15 >> 9 & 1) << 5 | (uVar15 >> 8 & 1) << 4 |
                             (uVar15 >> 3 & 1) << 3 | (uVar15 >> 2 & 1) << 2 | (uVar15 >> 1 & 1) * 2
                             | uVar15 & 1);
  if ((*(byte *)(param_1 + 0x10) & 1) != 0) {
    if (*(int *)(iVar4 + 0x54) != 0) {
      FUN_004522d0(*(int *)(iVar4 + 0x54));
      *(undefined4 *)(iVar4 + 0x54) = 0;
    }
    FUN_004f3b60((int *)(iVar4 + 0x20));
    uVar15 = param_1[0x13];
    uVar13 = (param_1[0xd] + param_1[0xe]) * 0x10;
    puVar5 = (uint *)FUN_004f0970((uVar13 + uVar15 * 6) * 2 + 0x10);
    *puVar5 = 0;
    puVar5[1] = 0;
    puVar5[2] = 0;
    puVar5[3] = 0;
    puStack_20 = puVar5 + 4;
    uStack_28 = (undefined2)uVar15;
    *puVar5 = (uint)*(byte *)(DAT_00912118 + 0x2d + DAT_007d3ff8) | *puVar5 & 0xffffff00;
    *(undefined2 *)(puVar5 + 1) = uStack_28;
    puVar14 = puStack_20 + uVar15 * 3;
    uVar3 = FUN_004f1130();
    *(undefined2 *)((int)puVar5 + 6) = uVar3;
    puVar5[2] = uVar13;
    uVar13 = 0;
    puVar5[3] = 0;
    if (uVar15 != 0) {
      do {
        uStack_2c = 0;
        uStack_18 = 0;
        *puStack_20 = (uint)puVar14;
        puStack_20[2] = *(uint *)(param_1[0x12] + uVar13 * 4);
        if (param_1[0xe] != 0) {
          iStack_24 = 0;
          do {
            iVar10 = param_1[0x16];
            if (*(uint *)(iVar10 + iStack_24 + 0x40) == uVar13) {
              uVar6 = 0;
              do {
                *(undefined2 *)((int)puVar14 + uVar6 * 2) =
                     *(undefined2 *)(iVar10 + iStack_24 + uVar6 * 4);
                uVar6 = uVar6 + 1;
              } while (uVar6 < 0x10);
              puVar14 = puVar14 + 8;
              uStack_2c = uStack_2c + 0x10;
            }
            uStack_18 = uStack_18 + 1;
            iStack_24 = iStack_24 + 0x44;
          } while (uStack_18 < (uint)param_1[0xe]);
        }
        uVar6 = 0;
        if (param_1[0xd] != 0) {
          iVar10 = 0;
          do {
            puVar7 = (undefined2 *)(param_1[0x17] + iVar10);
            if (*(uint *)(puVar7 + 0x14) == uVar13) {
              *(undefined2 *)puVar14 = *puVar7;
              *(undefined2 *)((int)puVar14 + 2) = puVar7[2];
              *(undefined2 *)(puVar14 + 1) = puVar7[4];
              *(undefined2 *)((int)puVar14 + 6) = puVar7[6];
              *(undefined2 *)(puVar14 + 2) = puVar7[8];
              *(undefined2 *)((int)puVar14 + 10) = puVar7[10];
              *(undefined2 *)(puVar14 + 3) = puVar7[0xc];
              *(undefined2 *)((int)puVar14 + 0xe) = puVar7[0xe];
              *(undefined2 *)(puVar14 + 4) = puVar7[0x10];
              *(undefined2 *)((int)puVar14 + 0x12) = puVar7[0x12];
              *(undefined2 *)(puVar14 + 5) = 0;
              *(undefined2 *)((int)puVar14 + 0x16) = 0;
              *(undefined2 *)(puVar14 + 6) = 0;
              *(undefined2 *)((int)puVar14 + 0x1a) = 0;
              *(undefined2 *)(puVar14 + 7) = 0;
              *(undefined2 *)((int)puVar14 + 0x1e) = 0;
              puVar14 = puVar14 + 8;
              uStack_2c = uStack_2c + 0x10;
            }
            uVar6 = uVar6 + 1;
            iVar10 = iVar10 + 0x2c;
          } while (uVar6 < (uint)param_1[0xd]);
        }
        puStack_20[1] = uStack_2c;
        puStack_20 = puStack_20 + 3;
        uVar13 = uVar13 + 1;
      } while (uVar13 < uVar15);
    }
    uVar15 = 0;
    *(uint **)(iVar4 + 0x54) = puVar5;
    if (param_1[0x13] != 0) {
      do {
        FUN_004f3be0((int *)(iVar4 + 0x20),*(undefined4 *)(param_1[0x12] + uVar15 * 4));
        uVar15 = uVar15 + 1;
      } while (uVar15 < (uint)param_1[0x13]);
    }
  }
  if (((*(byte *)(param_1 + 0x10) & 2) != 0) && ((*(byte *)(param_1 + 0xb) & 1) != 0)) {
    puVar9 = *(undefined4 **)(iVar12 + 0x14);
    iVar10 = *param_1;
    if (iVar2 != 0) {
      puVar8 = puVar9;
      iVar11 = iVar2;
      do {
        puVar1 = (undefined4 *)((iVar10 - (int)puVar9) + (int)puVar8);
        iVar11 = iVar11 + -1;
        *puVar8 = *puVar1;
        puVar8[1] = puVar1[1];
        puVar8[2] = puVar1[2];
        puVar8 = puVar8 + 3;
      } while (iVar11 != 0);
    }
    FUN_004e8640(iVar12,&uStack_10);
    *(undefined4 *)(iVar12 + 4) = uStack_10;
    *(undefined4 *)(iVar12 + 8) = uStack_c;
    *(undefined4 *)(iVar12 + 0xc) = uStack_8;
    *(undefined4 *)(iVar12 + 0x10) = uStack_4;
  }
  if (((*(byte *)(param_1 + 0x10) & 4) != 0) && ((param_1[0xb] & 2U) != 0)) {
    puVar9 = *(undefined4 **)(iVar12 + 0x18);
    iVar12 = param_1[1];
    if ((param_1[0xb] & 0x40U) == 0) {
      if (iVar2 != 0) {
        puVar8 = puVar9;
        iVar10 = iVar2;
        do {
          puVar1 = (undefined4 *)((iVar12 - (int)puVar9) + (int)puVar8);
          iVar10 = iVar10 + -1;
          *puVar8 = *puVar1;
          puVar8[1] = puVar1[1];
          puVar8[2] = puVar1[2];
          puVar8 = puVar8 + 3;
        } while (iVar10 != 0);
      }
    }
    else {
      param_1 = (int *)FUN_0054b340(param_1,1.0e-4f,0.5f,iVar12,(int)puVar9);
    }
  }
  if (((*(byte *)(param_1 + 0x10) & 8) != 0) && ((*(byte *)(param_1 + 0xb) & 4) != 0)) {
    puVar9 = *(undefined4 **)(iVar4 + 0x30);
    if (iVar2 != 0) {
      iVar10 = param_1[2] - (int)puVar9;
      iVar12 = iVar2;
      do {
        *puVar9 = *(undefined4 *)(iVar10 + (int)puVar9);
        puVar9 = puVar9 + 1;
        iVar12 = iVar12 + -1;
      } while (iVar12 != 0);
    }
  }
  if (param_1[0xf] != 0) {
    uVar15 = 0;
    puStack_20 = (uint *)(iVar4 + 0x34);
    local_1c = param_1 + 3;
    do {
      if ((((param_1[0x10] & 0x100 << ((byte)uVar15 & 0x1f)) != 0) && (*local_1c != 0)) &&
         (puVar9 = (undefined4 *)*puStack_20, iVar2 != 0)) {
        iVar10 = *local_1c - (int)puVar9;
        iVar12 = iVar2;
        do {
          *puVar9 = *(undefined4 *)(iVar10 + (int)puVar9);
          puVar9[1] = *(undefined4 *)(iVar10 + 4 + (int)puVar9);
          puVar9 = puVar9 + 2;
          iVar12 = iVar12 + -1;
        } while (iVar12 != 0);
      }
      uVar15 = uVar15 + 1;
      local_1c = local_1c + 1;
      puStack_20 = puStack_20 + 1;
    } while (uVar15 < 8);
  }
  if ((((param_1[0x10] & 1U) != 0) || ((param_1[0x10] & 0x100U) != 0)) &&
     (iVar2 = *(int *)(iVar4 + 0x34), iVar2 != 0)) {
    uVar15 = 0;
    if (param_1[0xd] != 0) {
      iVar12 = 0;
      do {
        uVar15 = uVar15 + 1;
        iVar10 = *(int *)(iVar12 + 0x14 + param_1[0x17]);
        iVar12 = iVar12 + 0x2c;
        *(undefined4 *)(iVar2 + iVar10 * 8) = 0x461c4000;
        *(undefined4 *)(iVar2 + 4 + iVar10 * 8) = 0x461c4000;
      } while (uVar15 < (uint)param_1[0xd]);
    }
    uVar15 = 0;
    if (param_1[0xe] != 0) {
      iVar12 = 0;
      do {
        uVar15 = uVar15 + 1;
        iVar10 = *(int *)(iVar12 + 0x14 + param_1[0x16]);
        iVar12 = iVar12 + 0x44;
        *(undefined4 *)(iVar2 + iVar10 * 8) = 0;
        *(undefined4 *)(iVar2 + 4 + iVar10 * 8) = 0;
      } while (uVar15 < (uint)param_1[0xe]);
    }
  }
  FUN_004e8a10(iVar4);
  return param_1;
}
