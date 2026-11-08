// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc4f0 (h2): 100% (regs 100%), match.py --bytes BYTE-IDENTICAL (132 B incl. the dword jump table
   past Ghidra's 28 B). Every case 1..26 written out with its own return 0/1: the original's direct dword table (no
   byte index table) needs the many distinct case bodies that are merged only after table layout. Raw 54.5% (13
   cases + default). Residual: none. relocheck OK. */
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
// MATCH: 0x004cc4f0 FUN_004cc4f0
undefined4 FUN_004cc4f0(undefined4 *param_1)
{
  switch (*param_1) {
  case 1:
    return 0;
  case 2:
    return 0;
  case 3:
    return 0;
  case 4:
    return 0;
  case 5:
    return 1;
  case 6:
    return 1;
  case 7:
    return 1;
  case 8:
    return 1;
  case 9:
    return 1;
  case 10:
    return 1;
  case 11:
    return 1;
  case 12:
    return 0;
  case 13:
    return 0;
  case 14:
    return 1;
  case 15:
    return 1;
  case 16:
    return 1;
  case 17:
    return 0;
  case 18:
    return 1;
  case 19:
    return 0;
  case 20:
    return 1;
  case 21:
    return 0;
  case 22:
    return 0;
  case 23:
    return 0;
  case 24:
    return 0;
  case 25:
    return 0;
  case 26:
    return 1;
  }
  return 0;
}
