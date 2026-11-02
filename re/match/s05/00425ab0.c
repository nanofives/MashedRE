// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425ab0 (88 B), h9 by hand: 100.0% (regs 100.0%). Raw 94.4%: the first store is an immediate 0 while
   the other 15 reuse xor eax,eax. Typing DAT_008992a0 as float (store of 0.0f) gives the immediate; every other
   global is int. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(float, DAT_008992a0, 0x008992a0);
MGLOBAL(int, DAT_008992a4, 0x008992a4);
MGLOBAL(int, DAT_008992ec, 0x008992ec);
MGLOBAL(int, DAT_008992f0, 0x008992f0);
MGLOBAL(int, DAT_0089933c, 0x0089933c);
MGLOBAL(int, DAT_00899388, 0x00899388);
MGLOBAL(int, DAT_008993d4, 0x008993d4);
MGLOBAL(int, DAT_00899420, 0x00899420);
MGLOBAL(int, DAT_0089946c, 0x0089946c);
MGLOBAL(int, DAT_008994b8, 0x008994b8);
MGLOBAL(int, DAT_00899338, 0x00899338);
MGLOBAL(int, DAT_00899384, 0x00899384);
MGLOBAL(int, DAT_008993d0, 0x008993d0);
MGLOBAL(int, DAT_0089941c, 0x0089941c);
MGLOBAL(int, DAT_00899468, 0x00899468);
MGLOBAL(int, DAT_008994b4, 0x008994b4);
// MATCH: 0x00425ab0 FUN_00425ab0
/* [C2 2026-05-17] No args, void return. Hand-unrolled clear of 8 entries × 2 dwords each, at
   stride 0x4c, in the array
 */

void FUN_00425ab0(void)

{
  DAT_008992a0 = 0;
  DAT_008992a4 = 0;
  DAT_008992ec = 0;
  DAT_008992f0 = 0;
  DAT_00899338 = 0;
  DAT_0089933c = 0;
  DAT_00899384 = 0;
  DAT_00899388 = 0;
  DAT_008993d0 = 0;
  DAT_008993d4 = 0;
  DAT_0089941c = 0;
  DAT_00899420 = 0;
  DAT_00899468 = 0;
  DAT_0089946c = 0;
  DAT_008994b4 = 0;
  DAT_008994b8 = 0;
  return;
}
