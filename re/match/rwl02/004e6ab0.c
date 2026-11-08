// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e6ab0: best 46.1% (regs 67.5%), residual class b. structure only (592 B), Ghidra form; 46.1. */
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

MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004e6d40(void);
void FUN_004c0740(int p0, int p1);
void FUN_004c0790(int p0);
int FUN_004c0870(undefined4 p0);
int FUN_004c0a60(int p0);
undefined4 FUN_004c0de0(int p0);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8060(undefined4 *p0, int p1);
int FUN_004d8090(undefined4 *p0, int p1, int p2);
undefined4 * FUN_004d8480(undefined4 *p0);
undefined4 FUN_004d8bd0(int p0);
undefined4 FUN_004e4d90(int p0);
int FUN_004e6710(int p0, void (*p1)(void), undefined4 p2);
int FUN_004e68a0(int p0, int p1, byte p2);
undefined4 FUN_004e6920(int p0);
undefined1 * FUN_004e69a0(int p0);
int FUN_004e6d00(int p0, int p1);
int FUN_004e6d80(void);
void FUN_004e6f80(int p0, int p1);
undefined4 FUN_004e6fe0(undefined4 p0, int p1);
// MATCH: 0x004e6ab0 FUN_004e6ab0
int FUN_004e6ab0(int param_1)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  undefined4 uStack_14;
  undefined4 uStack_10;
  int local_8;
  int local_4;
  iVar4 = FUN_004e6d80();
  if (iVar4 != 0) {
    local_8 = iVar4;
    iVar5 = FUN_004c0870(*(undefined4 *)(param_1 + 4));
    *(int *)(iVar4 + 4) = iVar5;
    if (iVar5 == 0) {
      FUN_004d8060(&DAT_0061867c,iVar4);
      puVar3 = *(undefined4 **)(iVar4 + 8);
      do {
        if (puVar3 == (undefined4 *)(iVar4 + 8)) break;
        puVar1 = (undefined4 *)*puVar3;
        puVar2 = puVar3 + -0x10;
        FUN_004e6920((int)puVar2);
        puVar3 = puVar1;
      } while (puVar2 != (undefined4 *)0);
      puVar3 = *(undefined4 **)(iVar4 + 0x10);
      do {
        if (puVar3 == (undefined4 *)(iVar4 + 0x10)) break;
        puVar2 = (undefined4 *)*puVar3;
        iVar5 = (int)puVar3 + (-4 - DAT_007d7230);
        FUN_004e6fe0(puVar3[-1],iVar5);
        FUN_004e4d90(iVar5);
        puVar3 = puVar2;
      } while (iVar5 != 0);
    }
    else {
      local_4 = 1;
      puVar3 = *(undefined4 **)(param_1 + 8);
      while (puVar3 != (undefined4 *)(param_1 + 8)) {
        puVar2 = (undefined4 *)*puVar3;
        iVar4 = (int)FUN_004e69a0((int)(puVar3 + -0x10));
        if (iVar4 == 0) {
          local_4 = 0;
          iVar4 = local_8;
          break;
        }
        if (puVar3[-0xf] == 0) {
          FUN_004d8060(&DAT_00618664,iVar4);
          if (*(int *)(iVar4 + 0x14) != 0) {
            FUN_004d8bd0(*(int *)(iVar4 + 0x14));
          }
          FUN_004e68a0(iVar4,0,0);
          FUN_004c0790(iVar4);
          (**(void (**)(undefined4,int))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + DAT_007d3ff8),iVar4);
          uStack_14 = 2;
          uStack_10 = FUN_004d7ff0(5);
          FUN_004d8480(&uStack_14);
          iVar4 = local_8;
          break;
        }
        FUN_004c0740(iVar4,*(int *)(puVar3[-0xf] + 0xa0));
        *(byte *)(iVar4 + 3) = *(byte *)(iVar4 + 3) | 1;
        FUN_004e6f80(local_8,iVar4);
        puVar3 = puVar2;
        iVar4 = local_8;
      }
      FUN_004c0a60(*(int *)(param_1 + 4));
      if (local_4 != 0) {
        *(undefined4 *)(iVar4 + 0x28) = *(undefined4 *)(param_1 + 0x28);
        FUN_004d8090(&DAT_0061867c,iVar4,param_1);
        return iVar4;
      }
      FUN_004d8060(&DAT_0061867c,iVar4);
      puVar3 = *(undefined4 **)(iVar4 + 8);
      do {
        if (puVar3 == (undefined4 *)(iVar4 + 8)) break;
        puVar1 = (undefined4 *)*puVar3;
        puVar2 = puVar3 + -0x10;
        FUN_004e6920((int)puVar2);
        puVar3 = puVar1;
      } while (puVar2 != (undefined4 *)0);
      puVar3 = *(undefined4 **)(iVar4 + 0x10);
      do {
        if (puVar3 == (undefined4 *)(iVar4 + 0x10)) break;
        puVar2 = (undefined4 *)*puVar3;
        iVar5 = FUN_004e6d00((int)puVar3 + (-4 - DAT_007d7230),0);
        puVar3 = puVar2;
      } while (iVar5 != 0);
    }
    FUN_004e6710(iVar4,FUN_004e6d40,0);
    if (*(int *)(iVar4 + 4) != 0) {
      FUN_004c0de0(*(int *)(iVar4 + 4));
    }
    (**(void (**)(undefined4,int))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),iVar4);
  }
  return 0;
}
