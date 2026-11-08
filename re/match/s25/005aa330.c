// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aa330: by hand. Best 54.7% (regs 96.2%, ops 96.2%, 53/53 ins). Residual (b): branch form now matches ((ushort)0 < uVar1 gives test ax,ax / jbe) but the original loads the array base [eax+0x10] before the ushort count and keeps count in edi, iterator in esi, base in ecx; ours allocates count in esi, iterator in edi and loads base after. Tried hoisting the base load into a temp, uint count with 0 < n guard, Ghidra != 0 form. */
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
void FUN_004522d0(int p0);
void FUN_005ae030(int p0);
// MATCH: 0x005aa330 FUN_005aa330
int FUN_005aa330(int param_1)
{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  if ((*(byte *)(param_1 + 0xc) & 4) != 0) {
    iVar4 = *(int *)(*(int *)(param_1 + 0x4c) + 0x10);
    uVar1 = *(ushort *)(*(int *)(param_1 + 0x4c) + 0xc);
    if ((ushort)0 < uVar1) {
      uVar2 = (uint)uVar1;
      iVar3 = iVar4 + uVar2 * 0x18;
      do {
        iVar3 = iVar3 + -0x18;
        FUN_005ae030(iVar3);
        uVar2 = uVar2 - 1;
      } while (0 < uVar2);
    }
    iVar4 = *(int *)(*(int *)(param_1 + 0x4c) + 0x14);
    uVar1 = *(ushort *)(*(int *)(param_1 + 0x4c) + 0xe);
    if ((ushort)0 < uVar1) {
      uVar2 = (uint)uVar1;
      iVar3 = iVar4 + uVar2 * 0x18;
      do {
        iVar3 = iVar3 + -0x18;
        FUN_005ae030(iVar3);
        uVar2 = uVar2 - 1;
      } while (0 < uVar2);
    }
    if ((*(byte *)(param_1 + 0xc) & 1) == 0) {
      FUN_004522d0(*(int *)(*(int *)(param_1 + 0x4c) + 0x10));
    }
    *(undefined4 *)(*(int *)(param_1 + 0x4c) + 0x10) = 0;
    *(undefined4 *)(*(int *)(param_1 + 0x4c) + 0x14) = 0;
  }
  return param_1;
}
