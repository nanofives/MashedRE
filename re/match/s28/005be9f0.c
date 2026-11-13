// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005be9f0: 80.0% (regs 82.2%). Loop rewritten to if(n)do{..}while(i!=lim && n) with n copied from param_2 at function top (64.4->80.0), stdcall vtbl call. Residual: ebp/ebx save placement (ours pushes ebp lazily inside the branch) and load order before xor edi,edi; permuter 153 compiles no gain. Class: b. */
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
typedef int (__stdcall *vfn1)(int *);
// MATCH: 0x005be9f0 FUN_005be9f0
int __stdcall FUN_005be9f0(int param_1, unsigned int param_2, int param_3, unsigned int *param_4)
{
  int iVar1;
  int *piVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;

  uVar5 = 0;
  uVar4 = param_2;
  if (*(unsigned int *)(param_1 + 0x10) < *(unsigned int *)(*(int *)(param_1 + 0xc) + 0x120)) {
    iVar1 = *(int *)(*(int *)(param_1 + 0xc) + 0x11c);
    if (uVar4 != 0) {
      do {
        uVar4 = uVar4 - 1;
        piVar2 = *(int **)(iVar1 + *(int *)(param_1 + 0x10) * 4);
        *(int **)(param_3 + uVar5 * 4) = piVar2;
        ((vfn1)*(int *)(*piVar2 + 4))(piVar2);
        uVar5 = uVar5 + 1;
        iVar3 = *(int *)(param_1 + 0x10) + 1;
        *(int *)(param_1 + 0x10) = iVar3;
      } while (iVar3 != *(int *)(*(int *)(param_1 + 0xc) + 0x120) && uVar4 != 0);
    }
  }
  if (param_4 != (unsigned int *)0) {
    *param_4 = uVar5;
  }
  return uVar5 < param_2;
}

