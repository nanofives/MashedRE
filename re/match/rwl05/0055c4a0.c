// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055c4a0: hand-matched (h1). 100%. Count field as unsigned with '> 0' gives jbe. */
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
// MATCH: 0x0055c4a0 FUN_0055c4a0
void FUN_0055c4a0(int param_1,int param_2)
{
  unsigned int *piVar1;
  unsigned int uVar2;
  int iVar3;

  if (**(short **)(param_1 + 0x5c) == 8) {
    piVar1 = *(unsigned int **)(param_1 + 0x40);
    uVar2 = 0;
    if (piVar1[1] > 0) {
      iVar3 = 0;
      do {
        FUN_0055c4a0(*piVar1 + iVar3,param_2);
        piVar1 = *(unsigned int **)(param_1 + 0x40);
        uVar2 = uVar2 + 1;
        iVar3 = iVar3 + 0x60;
      } while (uVar2 < piVar1[1]);
      return;
    }
  }
  else {
    *(int *)(param_1 + 0x54) = param_2;
  }
  return;
}
