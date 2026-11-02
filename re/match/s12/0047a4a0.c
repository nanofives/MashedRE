// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a4a0: 100% (regs 100%) by hand (h6). Raw converter 96.9%: FUN_004b7090 returns an x87 value
   that the original converts with __ftol (Ghidra showed the __ftol call FUN_004a2c48 as a separate no-arg call).
   Fix: declare FUN_004b7090 as returning double and write iVar4 = (int)FUN_004b7090(...). Residual: none. */
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
typedef unsigned __int64 ulonglong;
typedef long double float10;
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
MGLOBAL(int, DAT_006bf1c8, 0x006bf1c8);

int FUN_004b6fc0(undefined4 p0);
double FUN_004b7090(undefined4 p0, int p1);
int FUN_004b70d0(undefined4 p0, undefined4 p1);
// MATCH: 0x0047a4a0 FUN_0047a4a0
undefined4 FUN_0047a4a0(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  char *pcVar5;
  
  iVar2 = FUN_004b6fc0(param_1);
  if (iVar2 == 0) {
    pcVar3 = (char *)FUN_004b70d0(param_1,0);
    pcVar5 = (char *)((*(int *)(DAT_006bf1cc + 0x10) + 0x78) * 0x40 + DAT_006bf1c8);
    *(int *)(DAT_006bf1cc + 0x10) = *(int *)(DAT_006bf1cc + 0x10) + 1;
    do {
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
      *pcVar5 = cVar1;
      pcVar5 = pcVar5 + 1;
    } while (cVar1 != '\0');
    return 0;
  }
  iVar4 = (int)FUN_004b7090(param_1,iVar2 + -1);
  pcVar3 = (char *)FUN_004b70d0(param_1,iVar2);
  pcVar5 = (char *)((iVar4 + 0x78) * 0x40 + DAT_006bf1c8);
  do {
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
    *pcVar5 = cVar1;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  *(int *)(DAT_006bf1cc + 0x10) = *(int *)(DAT_006bf1cc + 0x10) + 1;
  return 0;
}
