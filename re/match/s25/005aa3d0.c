// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aa3d0 (h2): best 93.0% (regs 96.5%, ops 96.5%, 57/57 ins), class b. PORT-TOP-AUDIO-5AA3D0-STOPS: the loop pointer is computed from field +0x10 / +0x14 before the ushort count load, so the base load is hoisted above the ushort test as in the original (both arms: the then arm and the else arm at 0x005aa430); was 86.0%. Residual: the original keeps the base in ecx and forms the pointer with lea esi,[ecx+0x10], ours loads it into esi and adds 0x10. equiv.py EQUIV-IF-CALLERS-IGNORE(ECX), callers 0x005a9c5e / 0x005a9c66 ignore. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
MGLOBAL(undefined1, DAT_009146f0, 0x009146f0);
void FUN_004d7ff0(void);
void FUN_005ae0b0(undefined4 * p0);
int * FUN_005b36b0(int * p0, int p1);
// MATCH: 0x005aa3d0 FUN_005aa3d0
void FUN_005aa3d0(int param_1,int param_2)

{
  ushort uVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;

  uVar2 = (undefined4)FUN_005b36b0((int *)&DAT_009146f0,0);
  if (param_2 == 1) {
    iVar5 = *(int *)(param_1 + 0x4c);
    puVar4 = (undefined4 *)(*(int *)(iVar5 + 0x10) + 0x10);
    uVar1 = *(ushort *)(iVar5 + 0xc);
    if (uVar1 > 0) {
      uVar3 = (uint)uVar1;
      do {
        puVar4[-1] = uVar2;
        *puVar4 = 0;
        puVar4[1] = (undefined4)FUN_004d7ff0;
        FUN_005ae0b0(puVar4 + -4);
        puVar4 = puVar4 + 6;
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
      return;
    }
  }
  else {
    iVar5 = *(int *)(param_1 + 0x4c);
    puVar4 = (undefined4 *)(*(int *)(iVar5 + 0x14) + 0x10);
    uVar1 = *(ushort *)(iVar5 + 0xe);
    if (uVar1 > 0) {
      uVar3 = (uint)uVar1;
      do {
        puVar4[-1] = uVar2;
        *puVar4 = 0;
        puVar4[1] = (undefined4)FUN_004d7ff0;
        FUN_005ae0b0(puVar4 + -4);
        puVar4 = puVar4 + 6;
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
    }
  }
  return;
}
