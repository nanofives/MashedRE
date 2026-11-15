// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005adc10 (321 B). Best 78.9% (regs 89.8%), residual class (b). ops 96.9%: Ghidra's FUN_005b3620/FUN_005ada40 arities were wrong (real: 005b3620(a,b) cdecl, 005ada40(x,1)); nested call gives push-1-first order; residual = hoisting of lea esi,[param+0x1c] into the guard and orig's separate rollback counter (mov edi,ebx) which cl folds. */
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
MGLOBAL(unsigned char, DAT_009146f0, 0x009146f0);
int FUN_005a61b0(int p0);
int FUN_005ad770(int p0, int p1, int p2, int p3, int p4);
int FUN_005ad910(unsigned int p0, int p1, int p2, int p3, int p4);
int FUN_005ad980(int p0, int p1, int p2);
int FUN_005ad9d0(int p0, int p1, int p2, int p3, int p4);
void FUN_005ada40(int p0, int p1);
int FUN_005adfe0(int p0, int p1);
int FUN_005ae010(int p0, int p1);
int FUN_005b3620(int p0, int p1);
// MATCH: 0x005adc10 FUN_005adc10
int FUN_005adc10(int param_1,unsigned int param_2)
{
  int uVar1;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int *puVar5;

  uVar4 = 0;
  if (uVar4 < param_2) {
   puVar5 = (int *)(param_1 + 0x1c);
   do {
    iVar2 = puVar5[-6];
    if (iVar2 == 0x2000) {
      uVar1 = FUN_005b3620((int)&DAT_009146f0, puVar5[-2]);
      iVar2 = FUN_005ad910(0x2000, uVar1, puVar5[-5], puVar5[-1], *puVar5);
    }
    else if (iVar2 == 0x4000) {
      iVar2 = FUN_005b3620((int)&DAT_009146f0, puVar5[-2]);
      iVar3 = FUN_005a61b0(*(int *)(iVar2 + 0x14));
      iVar2 = FUN_005ad9d0(puVar5[-4], iVar3 + puVar5[-4], iVar2, puVar5[-1], *puVar5);
    }
    else if (iVar2 == 0x1000) {
      if (puVar5[-2] == 0) {
        uVar1 = 0;
      }
      else {
        uVar1 = FUN_005b3620((int)&DAT_009146f0, puVar5[-2]);
      }
      iVar2 = FUN_005ad980(uVar1, puVar5[-1], *puVar5);
    }
    else {
      iVar2 = FUN_005ad770(iVar2, puVar5[-4], puVar5[-5] + puVar5[-4], puVar5[-1], *puVar5);
    }
    if (iVar2 == 0) {
      if (uVar4 > 0) {
        unsigned int c = uVar4;
        puVar5 = (int *)(uVar4 * 0x20 + -0x10 + param_1);
        do {
          FUN_005ada40(FUN_005b3620((int)&DAT_009146f0, *puVar5), 1);
          puVar5 = puVar5 - 8;
          c = c - 1;
        } while (c != 0);
      }
      return 0;
    }
    FUN_005ae010(iVar2, puVar5[-3]);
    FUN_005adfe0(iVar2, puVar5[-7]);
    uVar4 = uVar4 + 1;
    puVar5 = puVar5 + 8;
   } while (uVar4 < param_2);
  }
  return param_1;
}
