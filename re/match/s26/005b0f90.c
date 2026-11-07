// FLAGS: /O1 /G5
/* MATCH-S26 0x005b0f90 (h3): 71.8% exact, regs 76.9%. Residual class (b) switch form + /O1 /G5 (71.8%); /O2 /Ob1 /G5 gives regs 94.7% but ours loads param_1 into ecx where the original uses eax (eax/ecx swap); statement order and struct forms tried. */
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
// MATCH: 0x005b0f90 FUN_005b0f90
void FUN_005b0f90(int param_1,undefined4 param_2,int *param_3)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  
  iVar2 = *param_3;
  iVar3 = *(int *)(param_1 + 0x20);
  switch (iVar2) {
  case 1:
    if (*(int *)(iVar3 + 0x28) != 3) {
      *(undefined4 *)(iVar3 + 0x28) = 8;
    }
    break;
  case 3:
    if (*(int *)(iVar3 + 0x28) != 5) {
      *(undefined4 *)(iVar3 + 0x28) = 4;
    }
    break;
  }
  *(int *)(iVar3 + 0x20) = iVar2;
  piVar1 = (int *)(param_1 + 0x14);
  if (*(int *)(param_1 + 0x14) != 0) {
    **(int **)(param_1 + 0x18) = *(int *)(param_1 + 0x14);
    *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)(param_1 + 0x18);
  }
  piVar4 = (int *)(*(int *)(param_1 + 0x24) + 0xc);
  iVar2 = *piVar4;
  *(int **)(param_1 + 0x18) = piVar4;
  *piVar1 = iVar2;
  *(int **)(*piVar4 + 4) = piVar1;
  *(int **)(*(int *)(param_1 + 0x24) + 0xc) = piVar1;
  return;
}
