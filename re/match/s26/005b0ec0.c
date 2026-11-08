// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b0ec0: 100% by hand: the if/else chain is a switch on +0x48 (cases 1,2,3 in order), which gives the dec/je chain. cxx pending. */
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
// MATCH: 0x005b0ec0 FUN_005b0ec0
void FUN_005b0ec0(int param_1)
{
  switch (*(int *)(param_1 + 0x48)) {
  case 1:
    if (*(int *)(param_1 + 0x14) != 0) {
      **(int **)(param_1 + 0x18) = *(int *)(param_1 + 0x14);
      *(int *)(*(int *)(param_1 + 0x14) + 4) = *(int *)(param_1 + 0x18);
      *(int *)(param_1 + 0x18) = 0;
      *(int *)(param_1 + 0x14) = 0;
    }
    *(int *)(param_1 + 0x50) = 3;
    break;
  case 2:
    *(int *)(param_1 + 0x50) = 6;
    break;
  case 3:
    *(int *)(param_1 + 0x50) = 5;
    break;
  }
}
