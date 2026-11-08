// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055de20: hand-matched (h3). 100%. unsigned `> 0` guard gives jbe. */
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
typedef int (*Cb)(int, int);
// MATCH: 0x0055de20 FUN_0055de20
int FUN_0055de20(int param_1, Cb param_2, int param_3)
{
  int iVar1;
  unsigned int uVar2;

  uVar2 = 0;
  if (*(unsigned int *)(param_1 + 0xc) > 0) {
    do {
      iVar1 = *(int *)(*(int *)(param_1 + 8) + uVar2 * 4);
      if ((iVar1 != 0) && (iVar1 = (*param_2)(iVar1, param_3), iVar1 == 0)) {
        return param_1;
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < *(unsigned int *)(param_1 + 0xc));
  }
  return param_1;
}
