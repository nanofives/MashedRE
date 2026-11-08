// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e62f0: best 53.4% (regs 75.9%), residual class b. structure only (372 B); three list loops, cursor/obj register choices differ, 53.4. */
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

MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004c0790(int *p0);
undefined4 FUN_004c0de0(int p0);
void FUN_004c0e50(int p0);
undefined4 FUN_004c1cf0(int p0);
int FUN_004d8060(undefined4 *p0, int p1);
undefined4 FUN_004d8bd0(int p0);
undefined4 FUN_004e4440(int *p0);
undefined4 FUN_004e4d90(int p0);
undefined4 FUN_004e8ea0(int p0);
// MATCH: 0x004e62f0 FUN_004e62f0
void FUN_004e62f0(int param_1)
{
  int *piVar1;
  undefined4 *puVar2;
  int *piVar3;
  undefined4 *puVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  FUN_004d8060(&DAT_0061867c,param_1);
  piVar5 = *(int **)(param_1 + 8);
  if (*(int **)(param_1 + 8) != (int *)(param_1 + 8)) {
    do {
      piVar3 = (int *)*piVar5;
      piVar1 = piVar5 + -0x10;
      FUN_004d8060(&DAT_00618664,(int)piVar1);
      if (piVar5[-0xb] != 0) {
        FUN_004d8bd0(piVar5[-0xb]);
      }
      if (piVar5[-10] != 0) {
        FUN_004e8ea0(piVar5[-10]);
        iVar7 = piVar5[-0xf];
        piVar5[-10] = 0;
        if ((iVar7 != 0) && (iVar6 = FUN_004e4440(piVar1), iVar6 != 0)) {
          FUN_004c0e50(iVar7);
        }
      }
      FUN_004c0790(piVar1);
      (**(void (**)(int,int *))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d7200 + DAT_007d3ff8),piVar1);
      piVar5 = piVar3;
    } while (piVar3 != (int *)(param_1 + 8));
  }
  puVar2 = *(undefined4 **)(param_1 + 0x10);
  do {
    if (puVar2 == (undefined4 *)(param_1 + 0x10)) break;
    puVar4 = (undefined4 *)*puVar2;
    iVar7 = (int)puVar2 + (-4 - DAT_007d7230);
    puVar2 = (undefined4 *)(DAT_007d7230 + iVar7);
    **(undefined4 **)(DAT_007d7230 + 8 + iVar7) = *(undefined4 *)(DAT_007d7230 + 4 + iVar7);
    *(undefined4 *)(puVar2[1] + 4) = puVar2[2];
    puVar2[2] = 0;
    puVar2[1] = 0;
    *puVar2 = 0;
    FUN_004e4d90(iVar7);
    puVar2 = puVar4;
  } while (iVar7 != 0);
  puVar2 = *(undefined4 **)(param_1 + 0x18);
  do {
    if (puVar2 == (undefined4 *)(param_1 + 0x18)) break;
    puVar4 = (undefined4 *)*puVar2;
    iVar7 = (int)puVar2 + (-4 - DAT_007d722c);
    puVar2 = (undefined4 *)(DAT_007d722c + iVar7);
    **(undefined4 **)(DAT_007d722c + 8 + iVar7) = *(undefined4 *)(DAT_007d722c + 4 + iVar7);
    *(undefined4 *)(puVar2[1] + 4) = puVar2[2];
    puVar2[2] = 0;
    puVar2[1] = 0;
    *puVar2 = 0;
    FUN_004c1cf0(iVar7);
    puVar2 = puVar4;
  } while (iVar7 != 0);
  if (*(int *)(param_1 + 4) != 0) {
    FUN_004c0de0(*(int *)(param_1 + 4));
  }
  (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d7200 + 4 + DAT_007d3ff8),param_1);
}
