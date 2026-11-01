// FLAGS: /O2 /GS /Ob1
/* 0x00423270 (164 B). Best 100.0% exact (regs 100, ops 100).
   Raw g2s 14.0%. A switch gives a jump table (50%); the compare chain order 1,4,2,3,5,6 is a plain
   if / else-if chain in that order (cases 4 and 6 tail-merge, case 3 reuses edx). Then the two
   wrap-around steps of DAT_007f1a64 as ++/-- with the bound test. Residual: none. */
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
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(char, DAT_007f1071, 0x007f1071);
MGLOBAL(char, DAT_007f1531, 0x007f1531);
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
MGLOBAL(char, DAT_007f1070, 0x007f1070);
MGLOBAL(char, DAT_007f1530, 0x007f1530);

// MATCH: 0x00423270 FUN_00423270
void FUN_00423270(void)
{
  int lo = 0;
  int hi = 8;
  if (DAT_007f1a54 == 1) { hi = 3; lo = 2; }
  else if (DAT_007f1a54 == 4) { hi = 5; lo = 2; }
  else if (DAT_007f1a54 == 2) { hi = 4; lo = 2; }
  else if (DAT_007f1a54 == 3) { hi = 3; lo = 2; }
  else if (DAT_007f1a54 == 5) { hi = 3; lo = 2; }
  else if (DAT_007f1a54 == 6) { hi = 5; lo = 2; }
  if (DAT_007f1071 && !DAT_007f1531) {
    if (++DAT_007f1a64 > hi) DAT_007f1a64 = lo;
  }
  if (DAT_007f1070 && !DAT_007f1530) {
    if (--DAT_007f1a64 < lo) DAT_007f1a64 = hi;
  }
}
