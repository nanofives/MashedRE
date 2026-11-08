// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e1670: best 52.8% (regs 84.9%), class (b). Hand-written from the listing: fast path (small remainder, free-list pop) and split path; the original keeps param_3 in edi and the remainder in esi across the compare, ours picks esi/ecx. Tried: a remainder local, permuter (its 69.8% variants read an uninitialised local, discarded). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
// MATCH: 0x004e1670 FUN_004e1670
int FUN_004e1670(int param_1,int *param_2,int param_3)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  iVar4 = *param_2 - param_3;
  if ((uint)iVar4 < 0x100) {
    iVar1 = param_2[1];
    iVar2 = *(int *)(param_1 + 0x14);
    iVar3 = *(int *)(param_1 + 0xc);
    if ((int *)(iVar3 + -8 + iVar2 * 8) != param_2) {
      *param_2 = *(int *)(iVar3 + -8 + iVar2 * 8);
      iVar2 = *(int *)(iVar3 + -4 + iVar2 * 8);
      param_2[1] = iVar2;
      *(int **)(iVar2 + 0xc) = param_2;
    }
    *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + -1;
    *(undefined4 *)(iVar1 + 0xc) = 0;
    return iVar1 + 0x20;
  }
  iVar2 = param_2[1];
  *(int *)(iVar2 + 0x28 + param_3) = iVar4 + -0x20;
  *(int *)(iVar2 + 0x20 + param_3) = iVar2;
  iVar1 = iVar2 + 0x20 + param_3;
  *(undefined4 *)(iVar1 + 4) = *(undefined4 *)(iVar2 + 4);
  *(int **)(iVar1 + 0xc) = param_2;
  *(int *)(iVar2 + 8) = param_3;
  *(int *)(iVar2 + 4) = iVar1;
  if (*(int **)(iVar1 + 4) != (int *)0x0) {
    **(int **)(iVar1 + 4) = iVar1;
  }
  param_2[1] = iVar1;
  *param_2 = *(int *)(iVar1 + 8);
  *(undefined4 *)(iVar2 + 0xc) = 0;
  return iVar2 + 0x20;
}
