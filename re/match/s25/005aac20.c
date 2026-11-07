// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aac20: by hand + permuter. Best 69.7% (regs 70.7%, 75/74 ins). Residual (b): original computes the node pointers into the callee-saved esi/edi pair and sets up push args in a different order inside both unlink loops (the two list-unlink loops share a FUN_005ae920(&DAT_007dcccc, node) tail); Ghidra structure kept, permuter move (+10%) adopted. Probably struct/typed-list source shape not recovered. */
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
MGLOBAL(undefined1, DAT_007dcccc, 0x007dcccc);
int * FUN_005aa9c0(undefined4 p0);
void FUN_005ae920(undefined1 * p0, int * p1);
// MATCH: 0x005aac20 FUN_005aac20
undefined4 FUN_005aac20(undefined4 param_1,int param_2,int param_3)
{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  piVar2 = (int *)FUN_005aa9c0(param_1);
  if (param_3 == 1) {
    piVar3 = *(int **)(*piVar2 + param_2 * 0xc);
    piVar2 = *(int **)(*piVar2 + 4 + param_2 * 0xc);
    if (piVar3 != piVar2) {
      do {
        piVar1 = (int *)*piVar3;
        *(int *)piVar3[-1] = piVar3[-2];
        *(int *)(piVar3[-2] + 4) = piVar3[-1];
        *(int *)piVar3[1] = *piVar3;
        *(int *)(*piVar3 + 4) = piVar3[1];
        FUN_005ae920(&DAT_007dcccc,piVar3 + -5);
        piVar3 = piVar1;
      } while (piVar1 != piVar2);
      return param_1;
    }
  }
  else {
    piVar3 = *(int **)(piVar2[1] + 4 + param_2 * 8);
    piVar2 = *(int **)(piVar2[1] + param_2 * 8);
    while (piVar2 != piVar3) {
      *(int *)piVar2[1] = *piVar2;
      *(int *)(*piVar2 + 4) = piVar2[1];
      *(int *)piVar2[3] = piVar2[2];
      *(int *)(piVar2[2] + 4) = piVar2[3];
      piVar1 = (int *)*piVar2;
      FUN_005ae920(&DAT_007dcccc,piVar2 + -3);
      piVar2 = piVar1;
    }
  }
  return param_1;
}
