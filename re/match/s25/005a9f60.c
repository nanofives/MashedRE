// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a9f60: by hand, Ghidra structure with true callee arity (005ae920/005ae400 take 2 args: iVar4, piVar5). Best 58.8% (regs 66.2%, ops 82.4%, 68/64 ins). Residual (b): original keeps param_1 in esi and tests the flag byte via a push ebx / mov bl,[esi+0x1c] / test bl,0x20 / pop ebx and loads [edi+0xc] as a dword into edx; ours takes &param_1 (stack slot) and lea ebx,[ecx+0x1c], CSEs the address and reloads bytes. Tried a dword temp for [iVar2+0xc] and an (int)piVar3+0x1c byte address: no change. */
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
typedef void (*S25_fn2)(int *, undefined4);
int FUN_005aa7e0(int p0, uint p1, int ** p2);
void FUN_005ae920(int p0, int * p1);
void FUN_005ae400(int p0, int * p1);
void FUN_004522d0(int * p0);
// MATCH: 0x005a9f60 FUN_005a9f60
void FUN_005a9f60(int *param_1,S25_fn2 param_2,undefined4 param_3)
{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  piVar3 = param_1;
  uVar1 = param_1[7];
  iVar2 = *param_1;
  if ((uVar1 & 8) == 0) {
    piVar5 = param_1;
    if ((uVar1 & 0x10) != 0) {
      iVar4 = FUN_005aa7e0(iVar2,uVar1 & 7,&param_1);
      piVar5 = *(int **)(iVar4 + (int)piVar3);
    }
  }
  else {
    piVar5 = (int *)param_1[-1];
  }
  iVar4 = *(int *)(iVar2 + 0x48);
  if ((iVar4 == 0) || ((param_2 != (S25_fn2)0 && ((*(byte *)((int)piVar3 + 0x1c) & 0x20) != 0)))) {
    if (param_2 != (S25_fn2)0) {
      (*param_2)(piVar5,param_3);
    }
    return;
  }
  uVar6 = *(uint *)(iVar2 + 0xc);
  if ((uVar6 & 0x40) != 0) {
    FUN_005ae920(iVar4,piVar5);
    return;
  }
  if ((char)*(uint *)(iVar2 + 0xc) < '\0') {
    FUN_005ae400(iVar4,piVar5);
    return;
  }
  FUN_004522d0(piVar5);
  return;
}
