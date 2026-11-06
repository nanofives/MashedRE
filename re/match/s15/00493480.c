// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493480 (h7, by hand): 100.0% (regs 100.0%). Raw 13.0%. What moved it: the snapped tick count
   is a local (no store back to DAT_007f1000 until the end), the accumulator a local a = DAT_007719d4 + t stored
   once, n = (unsigned)a / 50 then a -= n * 50 (not %, which gives div), float literal 0.00033333333f (pooled at
   0x005cc948, 1/3000). Residual: none. */
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
MGLOBAL(int, DAT_007f1000, 0x007f1000);
MGLOBAL(int, DAT_007719d4, 0x007719d4);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
void FUN_00493390(void);
// MATCH: 0x00493480 FUN_00493480
undefined4 FUN_00493480(void)
{
  int t;
  int n;
  int a;

  FUN_00493390();
  t = DAT_007f1000;
  if (t >= 47 && t <= 53) {
    t = 50;
  } else if (t >= 97 && t <= 103) {
    t = 100;
  } else if (t >= 147 && t <= 153) {
    t = 150;
  } else if (t >= 197 && t <= 203) {
    t = 200;
  }
  a = DAT_007719d4 + t;
  n = 0;
  if (a >= 50) {
    n = (unsigned int)a / 50;
    a -= n * 50;
  }
  DAT_007719d4 = a;
  if (a > 47) {
    n++;
    DAT_007719d4 = 0;
  } else if (a < 3) {
    DAT_007719d4 = 0;
  }
  DAT_007f1000 = n * 50;
  DAT_007f1004 = (float)DAT_007f1000 * 0.00033333333f;
  return 1;
}
