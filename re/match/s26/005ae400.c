// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae400: 100% from the raw converter body: (param_2 >= end) spelling for the second range compare (cmp operand order), FUN_005ae590 prototyped int (test eax,eax), thunk call renamed FUN_005ae4b0 (the jmp thunk to 0x004522d0 that the call really targets) with a pointer arg. cxx pending. */
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
void FUN_005ae550(int *p0, int * p1);
int FUN_005ae590(int *p0);
void FUN_005ae4b0(int *p0);
// MATCH: 0x005ae400 FUN_005ae400
void FUN_005ae400(int param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  
  piVar3 = *(int **)(param_1 + 0xc);
  piVar1 = (int *)(param_1 + 0xc);
  if (piVar3 != piVar1) {
    while ((param_2 <= piVar3 || (param_2 >= (int *)(*(int *)(param_1 + 4) + (int)piVar3)))) {
      piVar3 = (int *)*piVar3;
      if (piVar3 == piVar1) {
        return;
      }
    }
    FUN_005ae550(piVar3,param_2);
    iVar2 = FUN_005ae590(piVar3);
    if (iVar2 != 0) {
      *(int *)piVar3[1] = *piVar3;
      *(int *)(*piVar3 + 4) = piVar3[1];
      if ((int *)*piVar1 == piVar1) {
        *piVar3 = *piVar1;
        piVar3[1] = (int)piVar1;
        *(int **)(*piVar1 + 4) = piVar3;
        *piVar1 = (int)piVar3;
        return;
      }
      FUN_005ae4b0(piVar3);
    }
  }
  return;
}
