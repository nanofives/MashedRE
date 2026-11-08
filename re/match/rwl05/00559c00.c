// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 83.3%. Returns piVar1 (Ghidra typed it void); the return 0 reuses eax. Supersedes the header below. */
/* MATCH-RWL05 0x00559c00: hand-matched (h3). 83.3%. early-return form; the original lays out `jne L; ret; L:` with the ret inline, ours shares the final ret. Tried if/else, != 0, returns, permuter. class (a). */
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
// MATCH: 0x00559c00 FUN_00559c00
int *FUN_00559c00(int *param_1)
{
  int *piVar1;
  int n = param_1[0xf];

  if (n == 0) {
    return 0;
  }
  n--;
  param_1[0xf] = n;
  piVar1 = *(int **)(param_1[0xe] + n * 4);
  *piVar1 = (int)param_1;
  *(unsigned short *)(piVar1 + 1) = 0xffff;
  return piVar1;
}
