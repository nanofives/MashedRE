// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x005584c0: hand-matched (h3). 90.0% (regs 90.0, ops 93.3). only the first `return 0` block layout (original has an inline ret after the first test, ours shares the ret) differs; tried != 0 nesting, return of the global, local result. class (b). */
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
typedef int (*Alloc)(int, int);
typedef int (*Free)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc8b8, 0x007dc8b8);
MGLOBAL(int, DAT_007dc8bc, 0x007dc8bc);
MGLOBAL(int, DAT_007dc8b4, 0x007dc8b4);
MGLOBAL(int, DAT_007dc8b0, 0x007dc8b0);
MGLOBAL(int, DAT_007dc8ac, 0x007dc8ac);
// MATCH: 0x005584c0 FUN_005584c0
int FUN_005584c0(void)
{
  int r;
  r = (*(Alloc *)(DAT_007d3ff8 + 0x108))(0x1c000, 0x40180);
  DAT_007dc8b8 = r;
  if (r == 0) {
    return r;
  }
  r = (*(Alloc *)(DAT_007d3ff8 + 0x108))(0x3000, 0x40180);
  DAT_007dc8bc = r;
  if (r == 0) {
    (*(Free *)(DAT_007d3ff8 + 0x10c))(DAT_007dc8b8);
    DAT_007dc8b8 = 0;
    return 0;
  }
  DAT_007dc8b4 = 0;
  DAT_007dc8b0 = 0;
  DAT_007dc8ac = 1;
  return 1;
}
