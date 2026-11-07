// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a6030: by hand. 100% from the Ghidra form (globals as MGLOBAL, DAT_007dc950/94c as 4-byte). */
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
MGLOBAL(undefined4, DAT_007dc950, 0x007dc950);
MGLOBAL(undefined4, DAT_007dc94c, 0x007dc94c);
MGLOBAL(int, DAT_007dc95c, 0x007dc95c);
MGLOBAL(undefined1, PTR_LAB_00632ba8, 0x00632ba8);
void FUN_005a6010(void);
void FUN_005a60e0(void);
void FUN_005a6070(undefined1 * p0, undefined4 p1, void (*p2)(void));
// MATCH: 0x005a6030 FUN_005a6030
void FUN_005a6030(void)
{
  DAT_007dc950 = 1;
  if (DAT_007dc95c == 1) {
    FUN_005a60e0();
  }
  FUN_005a6070(&PTR_LAB_00632ba8,0x10,FUN_005a6010);
  DAT_007dc94c = 0;
  DAT_007dc950 = 0;
}
