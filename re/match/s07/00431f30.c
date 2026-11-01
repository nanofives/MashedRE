// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431f30 (227 B): 100% (regs 100, ops 100), by hand from the raw converter output (81.4%).
   Only change: case bodies written in the order the original lays them out (5, 6/7, 10, 0xc, 0xf, 9/0x10,
   0x11, 8, 0x1f/0x21, 0x1c, 0x1d, 0x12/0x18, 0x13, 0x20, 0x1e, 0x14, 4; the compiler emits bodies in source
   order), case 4 last with break so it falls into the shared ret. Ghidra's size stops at the ret before the
   jump table at 0x00432014; the whole obj symbol is byte-identical (match.py --bytes). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0067e830, 0x0067e830);
MGLOBAL(undefined4, DAT_0067e838, 0x0067e838);
MGLOBAL(undefined4, DAT_0067e820, 0x0067e820);
MGLOBAL(undefined4, DAT_0067e818, 0x0067e818);
MGLOBAL(undefined4, DAT_0067e810, 0x0067e810);
MGLOBAL(undefined4, DAT_0067e7f0, 0x0067e7f0);
MGLOBAL(undefined4, DAT_0067e800, 0x0067e800);
MGLOBAL(undefined4, DAT_0067e7f8, 0x0067e7f8);
MGLOBAL(undefined4, DAT_0067e7e8, 0x0067e7e8);
MGLOBAL(undefined4, DAT_0067e7e0, 0x0067e7e0);
MGLOBAL(undefined4, DAT_0067e7d8, 0x0067e7d8);
MGLOBAL(undefined4, DAT_0067e7d0, 0x0067e7d0);
MGLOBAL(undefined4, DAT_0067e7b0, 0x0067e7b0);
MGLOBAL(undefined4, DAT_0067e808, 0x0067e808);
MGLOBAL(undefined4, DAT_0067e828, 0x0067e828);
MGLOBAL(undefined4, DAT_0067e7b8, 0x0067e7b8);
MGLOBAL(undefined4, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(undefined4, DAT_0067e7a8, 0x0067e7a8);
void FUN_00431d90(void);
// MATCH: 0x00431f30 FUN_00431f30
void FUN_00431f30(undefined4 param_1)

{
  FUN_00431d90();
  switch(param_1) {
  case 5:
    DAT_0067e7c8 = 1;
    return;
  case 6:
  case 7:
    DAT_0067e7b8 = 1;
    return;
  case 10:
    DAT_0067e7b0 = 1;
    return;
  case 0xc:
    DAT_0067e7d0 = 1;
    return;
  case 0xf:
    DAT_0067e7d8 = 1;
    return;
  case 9:
  case 0x10:
    DAT_0067e7e0 = 1;
    return;
  case 0x11:
    DAT_0067e7e8 = 1;
    return;
  case 8:
    DAT_0067e808 = 1;
    DAT_0067e828 = 1;
    return;
  case 0x1f:
  case 0x21:
    DAT_0067e830 = 1;
    return;
  case 0x1c:
    DAT_0067e810 = 1;
    return;
  case 0x1d:
    DAT_0067e818 = 1;
    return;
  case 0x12:
  case 0x18:
    DAT_0067e7f0 = 1;
    return;
  case 0x13:
    DAT_0067e7f8 = 1;
    return;
  case 0x20:
    DAT_0067e838 = 1;
    return;
  case 0x1e:
    DAT_0067e820 = 1;
    return;
  case 0x14:
    DAT_0067e800 = 1;
    return;
  case 4:
    DAT_0067e7a8 = 1;
    break;
  }
  return;
}
