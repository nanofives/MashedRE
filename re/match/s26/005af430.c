// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af430: 100% by hand: switch lists the dead cases 2 and 6 as their own return 0 (plus default) so the DDK builds the 5-entry jump table instead of range compares. cxx pending. */
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
undefined4 FUN_005b1110(undefined4 p0, undefined4 p1, undefined4 p2);
// MATCH: 0x005af430 FUN_005af430
undefined4 FUN_005af430(int param_1)
{
  switch(*(int *)(param_1 + 8)) {
  case 2:
    return 0;
  case 3: case 4: case 5:
    return FUN_005b1110(*(int *)(param_1 + 0x14),*(undefined4 *)(*(int *)(param_1 + 0x14) + 0x58),3);
  case 6:
    return 0;
  default:
    return 0;
  }
}
