// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f36c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
void FUN_004f2420(void);
void FUN_004f38c0(void);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
void * FUN_004f0970(undefined4 p0);
int FUN_004f0d80(void * p0, void * p1, undefined4 p2);
void FUN_005c24e0(void * p0, uint p1, uint p2, void * p3);
// MATCH: 0x004f36c0 FUN_004f36c0
undefined4 * FUN_004f36c0(int param_1)
{
  undefined2 uVar1;
  undefined4 uVar2;
  int *piVar3;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined4 *puVar9;
  int iVar10;
  uint uVar11;
  undefined4 uStack_8;
  undefined4 uStack_4;

  piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(*(int *)(param_1 + 4) << 2,0x10502);
  if (piVar3 != (int *)0x0) {
    uVar4 = 0;
    if (0 < *(uint *)(param_1 + 4)) {
      iVar8 = 0;
      do {
        iVar10 = *(int *)(param_1 + 8) + iVar8;
        iVar8 = iVar8 + 0x14;
        piVar3[uVar4] = iVar10;
        uVar4 = uVar4 + 1;
      } while (uVar4 < *(uint *)(param_1 + 4));
    }
    FUN_005c24e0(piVar3,*(undefined4 *)(param_1 + 4),4,(void *)FUN_004f2420);
    uVar4 = *(uint *)(param_1 + 4);
    iVar8 = 1;
    if (1 < uVar4) {
      iVar10 = *(int *)(*piVar3 + 8);
      uVar11 = 1;
      if (1 < uVar4) {
        do {
          if (*(int *)(piVar3[uVar11] + 8) != iVar10) {
            iVar8 = iVar8 + 1;
            iVar10 = *(int *)(piVar3[uVar11] + 8);
          }
          uVar11 = uVar11 + 1;
        } while (uVar11 < uVar4);
      }
    }
    iVar10 = (uVar4 + iVar8 * 2) * 6 + 0x10;
    puVar5 = (undefined4 *)FUN_004f0970(iVar10);
    uVar4 = 0;
    if (puVar5 != (undefined4 *)0x0) {
      *puVar5 = 0;
      *(undefined2 *)(puVar5 + 1) = 1;
      uVar1 = *(undefined2 *)(DAT_00912118 + DAT_007d3ff8);
      puVar5[3] = 0;
      *(undefined2 *)((int)puVar5 + 6) = uVar1;
      puVar5[2] = *(int *)(param_1 + 4) * 3;
      *(short *)(DAT_00912118 + DAT_007d3ff8) = *(short *)(DAT_00912118 + DAT_007d3ff8) + 1;
      puVar6 = puVar5 + 4;
      puVar5[5] = 0;
      uVar2 = *(undefined4 *)(*piVar3 + 8);
      *puVar6 = (undefined4)(puVar6 + iVar8 * 3);
      puVar5[6] = uVar2;
      puVar9 = puVar6 + iVar8 * 3;
      do {
        puVar7 = puVar6;
        if (*(int *)(piVar3[uVar4] + 8) != puVar6[2]) {
          puVar7 = puVar6 + 3;
          puVar6[4] = 0;
          uVar2 = *(undefined4 *)(piVar3[uVar4] + 8);
          *puVar7 = (undefined4)puVar9;
          puVar6[5] = uVar2;
          *(short *)(puVar5 + 1) = *(short *)(puVar5 + 1) + 1;
        }
        *(undefined2 *)puVar9 = *(undefined2 *)piVar3[uVar4];
        *(undefined2 *)((int)puVar9 + 2) = *(undefined2 *)(piVar3[uVar4] + 2);
        *(undefined2 *)(puVar9 + 1) = *(undefined2 *)(piVar3[uVar4] + 4);
        uVar4 = uVar4 + 1;
        puVar7[1] = puVar7[1] + 3;
        puVar6 = puVar7;
        puVar9 = (undefined4 *)((int)puVar9 + 6);
      } while (uVar4 < *(uint *)(param_1 + 4));
      FUN_004f0d80(puVar5,(void *)FUN_004f38c0,0);
      (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
      return puVar5;
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
    uStack_8 = 2;
    uStack_4 = FUN_004d7ff0(0x80000013,iVar10);
    FUN_004d8480(&uStack_8);
    return (undefined4 *)0x0;
  }
  uStack_8 = 2;
  uStack_4 = FUN_004d7ff0(0x80000013,*(int *)(param_1 + 4) << 2);
  FUN_004d8480(&uStack_8);
  return (undefined4 *)0x0;
}
