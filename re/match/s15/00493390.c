// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493390 (h7, by hand): 100.0% (regs 100.0%). Raw 31.1%. What moved it: FUN_004950b0 returns
   unsigned (the 2^32 fix-up at 0x005cc94c is the compiler's), t = FUN_004950b0(); now = t; t += DAT_00771984
   (puts now in edx, t in eax), DAT_007f1000 = 50 written before the DAT_00771984 store, float literals
   0.016666668f (0x3c888889) and 3.3333333e-07f (pooled at 0x005ccd68). Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_00771984, 0x00771984);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(int, DAT_007f0ffc, 0x007f0ffc);
MGLOBAL(int, DAT_007f1000, 0x007f1000);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(int, DAT_007f0ff0, 0x007f0ff0);
MGLOBAL(unsigned int, DAT_007f102c, 0x007f102c);
MGLOBAL(float, DAT_007f1010, 0x007f1010);
MGLOBAL(int, DAT_007f1024, 0x007f1024);
MGLOBAL(int, DAT_007f1028, 0x007f1028);
unsigned int FUN_004950b0(void);
// MATCH: 0x00493390 FUN_00493390
void FUN_00493390(void)
{
  unsigned int now;
  unsigned int t;

  t = FUN_004950b0();
  now = t;
  t += DAT_00771984;
  DAT_007f0ffc = t - DAT_007f1030;
  if (DAT_007f0ffc > 200000) {
    DAT_007f0ffc = 200000;
  } else if (DAT_007f0ffc < 0) {
    DAT_007f0ffc = -DAT_007f0ffc;
  }
  DAT_007f1000 = 50;
  DAT_00771984 = DAT_007f0ffc - 50000;
  if (DAT_00771984 > -64 && DAT_00771984 < 64) {
    DAT_00771984 = 0;
  } else {
    if (DAT_00771984 < -2) {
      DAT_00771984 += 2;
    }
    if (DAT_00771984 > 2) {
      DAT_00771984 -= 2;
    }
  }
  DAT_007f1004 = 0.016666668f;
  DAT_007f1010 = (float)now * 3.3333333e-07f;
  DAT_007f0ff0 += 50;
  DAT_007f1030 = now;
  if (t > DAT_007f102c + 3000000) {
    DAT_007f1028 = DAT_007f1024;
    DAT_007f1024 = 0;
    DAT_007f102c = t;
  }
}
