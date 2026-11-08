// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e5300: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 27.9% structure only (class b: source shape not found, size above budget). */
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
typedef int (*F1)(int);
typedef int (*F2)(int, int);
typedef unsigned char undefined;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004f0ae0(int p0);
undefined4 FUN_004f0bd0(int p0);
uint * FUN_004f0c50(int p0, void * p1, undefined2 p2, undefined2 p3, undefined2 p4, undefined2 p5, undefined2 p6, undefined2 p7, undefined2 p8);
int FUN_004f3b00(int p0, byte p1);
// MATCH: 0x004e5300 FUN_004e5300
int FUN_004e5300(int param_1)

{
  undefined2 *puVar1;
  ushort uVar2;
  int *piVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  ushort uVar11;
  uint uVar12;
  int *piVar13;
  int *piVar14;
  uint uVar15;
  ushort uVar16;
  int iVar17;
  int local_120;
  int iStack_110;
  int aiStack_100 [64];
  
  local_120 = 0;
  piVar13 = *(int **)(param_1 + 0x1c);
  if (*(int **)(param_1 + 0x1c) == (int *)0x0) {
    return 0;
  }
  do {
    if (*piVar13 < 0) {
      iVar10 = *(int *)(param_1 + 0x10);
      uVar2 = *(ushort *)(piVar13 + 0x20);
      if (piVar13[0x1e] == 0) {
        iVar5 = FUN_004f0ae0((short)piVar13[0x21]);
        if (iVar5 == 0) {
          return 0;
        }
        uVar11 = 0;
        uVar4 = 0;
        uVar16 = 0;
        if (*(int *)(param_1 + 0x14) != 0) {
          iVar8 = *(int *)(param_1 + 0x14) * 4;
          iVar6 = (*(F2 *)(DAT_007d3ff8 + 0x108))(iVar8,0x10006);
          iVar7 = (*(F2 *)(DAT_007d3ff8 + 0x108))(iVar8,0x10507);
          iVar8 = (*(F2 *)(DAT_007d3ff8 + 0x108))(iVar8,0x10507);
          iStack_110 = 0;
          if ((short)piVar13[0x21] != 0) {
            do {
              uVar15 = 0;
              iVar17 = 0;
              puVar1 = (undefined2 *)(piVar13[1] + iStack_110 * 8);
              piVar14 = *(int **)(iVar10 + (uint)uVar2 * 4 + (uint)(ushort)puVar1[3] * 4);
              piVar3 = (int *)*piVar14;
              if (uVar11 != 0) {
                do {
                  if (*(int **)(iVar6 + (uVar15 & 0xffff) * 4) == piVar3) break;
                  uVar15 = uVar15 + 1;
                } while ((ushort)uVar15 < uVar11);
              }
              if ((ushort)uVar15 == uVar11) {
                *(int **)(iVar6 + (uVar15 & 0xffff) * 4) = piVar3;
                uVar11 = uVar11 + 1;
              }
              if (piVar3 != (int *)0x0) {
                iVar17 = *piVar3;
              }
              uVar12 = 0;
              if (uVar4 != 0) {
                do {
                  if (*(int *)(iVar7 + (uVar12 & 0xffff) * 4) == iVar17) break;
                  uVar12 = uVar12 + 1;
                } while ((ushort)uVar12 < uVar4);
              }
              if ((ushort)uVar12 == uVar4) {
                *(int *)(iVar7 + (uVar12 & 0xffff) * 4) = iVar17;
                uVar4 = uVar4 + 1;
              }
              uVar9 = 0;
              if (uVar16 != 0) {
                do {
                  if (*(int *)(iVar8 + (uVar9 & 0xffff) * 4) == piVar14[2]) break;
                  uVar9 = uVar9 + 1;
                } while ((ushort)uVar9 < uVar16);
              }
              if ((ushort)uVar9 == uVar16) {
                *(int *)(iVar8 + (uVar9 & 0xffff) * 4) = piVar14[2];
                uVar16 = uVar16 + 1;
              }
              FUN_004f0c50(iVar5,piVar14,*puVar1,puVar1[1],puVar1[2],
                           CONCAT22((short)(uVar12 >> 0x10),puVar1[3]),uVar15,uVar12,uVar9);
              iStack_110 = iStack_110 + 1;
            } while (iStack_110 < (int)(uint)*(ushort *)(piVar13 + 0x21));
          }
          (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar6);
          (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar7);
          (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar8);
        }
        iVar10 = FUN_004f3b00(iVar5,(*(byte *)(param_1 + 8) & 1) != 0);
        if (iVar10 == 0) {
          FUN_004f0bd0(iVar5);
          return 0;
        }
        piVar13[0x1e] = iVar10;
      }
      piVar14 = (int *)aiStack_100[local_120];
      local_120 = local_120 + -1;
    }
    else {
      piVar14 = (int *)piVar13[2];
      local_120 = local_120 + 1;
      aiStack_100[local_120] = piVar13[3];
    }
    piVar13 = piVar14;
    if (local_120 < 0) {
      return param_1;
    }
  } while( true );
}
