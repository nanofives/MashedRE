// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 58.6% (regs 66.7%, ops 86.9%) after match_permute (two rounds, 1200 compiles); raw Ghidra form 32.8%. Mesh builder: three de-dup index loops (uint counters with ushort compares), FUN_004f0c50 takes seven ushort args pushed zero-extended. Residual: register allocation (original keeps the object in ebx and a shared zero in edi, ours swaps them) and the frame 0x28 vs 0x24. Class (b).
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

#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef unsigned char byte;
typedef struct { ushort a; ushort b; ushort c; ushort d; } Tri;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004f0ae0(int p0);
undefined4 FUN_004f0bd0(int p0);
undefined4 FUN_004f0c50(int p0, int *p1, ushort p2, ushort p3, ushort p4, ushort p5, ushort p6, ushort p7, ushort p8);
int FUN_004f3b00(int p0, int p1);
int *FUN_004f3bd0(int *p0, int p1);
// MATCH: 0x004e8a10 FUN_004e8a10
int FUN_004e8a10(int param_1)
{
  Tri *tri;
  int *piVar2;
  uint n0 = 0;
  uint n1 = 0;
  uint n2 = 0;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iStack_18;

  if (*(int *)(param_1 + 0x54) != 0) {
    return param_1;
  }
  iVar6 = FUN_004f0ae0(*(int *)(param_1 + 0x10));
  if (iVar6 != 0) {
    if (0 < *(int *)(param_1 + 0x24)) {
      iVar7 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar9, 0x10006);
      iVar8 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar9, 0x10503);
      iVar9 = *(int *)(param_1 + 0x24) * 4;
      iVar9 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar9, 0x10503);
      iStack_18 = 0;
      if (0 < *(int *)(param_1 + 0x10)) {
        do {
          iVar13 = 0;
          tri = (Tri *)(*(int *)(param_1 + 0x2c) + iStack_18 * 8);
          piVar10 = FUN_004f3bd0((int *)(param_1 + 0x20), tri->d);
          piVar2 = (int *)*piVar10;
          uVar14 = 0;
          if ((ushort)n0 != 0) {
            do {
              if (*(int **)(iVar7 + (uVar14 & 0xffff) * 4) == piVar2) break;
              uVar14++;
            } while ((ushort)uVar14 < (ushort)n0);
          }
          if ((ushort)uVar14 == (ushort)n0) {
            *(int **)(iVar7 + (uVar14 & 0xffff) * 4) = piVar2;
            n0++;
          }
          if (piVar2 != 0) {
            iVar13 = *piVar2;
          }
          uVar12 = 0;
          if ((ushort)0 != n1) {
            do {
              if (*(int *)(iVar8 + (uVar12 & 0xffff) * 4) == iVar13) break;
              uVar12++;
            } while ((ushort)uVar12 < (ushort)n1);
          }
          if ((ushort)uVar12 == (ushort)n1) {
            *(int *)(iVar8 + (uVar12 & 0xffff) * 4) = iVar13;
            n1++;
          }
          uVar11 = 0;
          if ((ushort)0 != n2) {
            do {
              if (*(int *)(iVar9 + (uVar11 & 0xffff) * 4) == piVar10[2]) break;
              uVar11++;
            } while ((ushort)uVar11 < (ushort)n2);
          }
          if ((ushort)uVar11 == (ushort)n2) {
            *(int *)(iVar9 + (uVar11 & 0xffff) * 4) = piVar10[2];
            n2++;
          }
          FUN_004f0c50(iVar6, piVar10, tri->a, tri->b, tri->c, tri->d, (ushort)uVar14, (ushort)uVar12, (ushort)uVar11);
          iStack_18++;
        } while (iStack_18 < *(int *)(param_1 + 0x10));
      }
      (**(code **)(DAT_007d3ff8 + 0x10c))(iVar7);
      (**(code **)(DAT_007d3ff8 + 0x10c))(iVar8);
      (**(code **)(DAT_007d3ff8 + 0x10c))(iVar9);
    }
    iVar9 = FUN_004f3b00(iVar6, (*(byte *)(param_1 + 8) & 1) != 0);
    if (iVar9 != 0) {
      *(int *)(param_1 + 0x54) = iVar9;
      return param_1;
    }
    FUN_004f0bd0(iVar6);
  }
  return 0;
}
