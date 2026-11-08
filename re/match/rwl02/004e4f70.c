// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e4f70: best 81.8% (regs 81.8%), residual class b. orig lays out the -1 (leaf) branch out of line and falls into the sibling loop from cmp -1; ours puts the leaf case inline; tried loop forms and permuter, 81.8. */
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

MGLOBAL(undefined4, DAT_006189e4, 0x006189e4);
int FUN_004d8060(undefined4 *p0, int *p1);
undefined4 FUN_004d8bd0(int p0);
undefined4 FUN_004e33c0(int *p0);
undefined4 FUN_004e3410(int *p0);
undefined4 FUN_004f0c10(int p0);
void FUN_004e4f70(int *param_1);
// MATCH: 0x004e4f70 FUN_004e4f70
void FUN_004e4f70(int *param_1)
{
  int *piVar1;
  int *piVar2;
  int iVar3;
  iVar3 = *param_1;
  if (iVar3 == -2) return;
  while (iVar3 != -1) {
    if (param_1[2] != 0) {
      FUN_004e4f70((int *)param_1[2]);
    }
    param_1 = (int *)param_1[3];
    if (param_1 == (int *)0) return;
    iVar3 = *param_1;
    if (iVar3 == -2) return;
  }
  if (param_1[0xd] != 0) {
    FUN_004d8bd0(param_1[0xd]);
  }
  piVar1 = (int *)param_1[0xe];
  while (piVar1 != param_1 + 0xe) {
    piVar2 = piVar1;
    piVar1 = (int *)*piVar1;
    FUN_004e3410(piVar2);
  }
  piVar1 = (int *)param_1[0x10];
  while (piVar1 != param_1 + 0x10) {
    piVar2 = piVar1;
    piVar1 = (int *)*piVar1;
    FUN_004e33c0(piVar2);
  }
  FUN_004d8060(&DAT_006189e4,param_1);
  if (param_1[0x1e] == 0) {
    return;
  }
  FUN_004f0c10(param_1[0x1e]);
  param_1[0x1e] = 0;
}
