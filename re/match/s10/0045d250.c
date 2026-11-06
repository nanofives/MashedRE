// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d250 (h2): 100% (regs 100%). Raw 65.0%. `if (a != -1) {..} else if (..)` with the a != -1 arm
   first, then three independent `DAT_0088fbc0 != mode && ..` tests (the compiler threads the repeated compares to
   the exit and pushes EBX for the -1 it knows b holds). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_0088fbc0, 0x0088fbc0);
int FUN_0040e350(void);
int FUN_0040e4b0(void);
int FUN_0040e500(void);
void FUN_0045ca30(void);
int FUN_0045cab0(int param_1);
void FUN_0045cb20(int param_1, int param_2, int param_3);
void FUN_0045d080(void);
// MATCH: 0x0045d250 FUN_0045d250
void FUN_0045d250(void)
{
  int mode;
  int sub;
  int a;
  int b;
  int i;

  mode = FUN_0040e350();
  sub = DAT_007f0fd0;
  a = FUN_0040e4b0();
  b = FUN_0040e500();
  if (a != -1) {
    if (DAT_0088fbc0 != mode && mode == 7 && b == -1) {
      FUN_0045d080();
      FUN_0045cb20(1, a, -1);
    }
  } else if (b == -1 && mode != 0xb) {
    for (i = 0; i < 4; i++) {
      if (FUN_0045cab0(i) != 0)
        FUN_0045cb20(0x27, i, -1);
    }
  }
  if (DAT_0088fbc0 != mode && mode == 0xb && b != -1) {
    FUN_0045d080();
    FUN_0045cb20(0, b, -1);
    FUN_0045ca30();
  }
  if (DAT_0088fbc0 != mode && mode == 0xb && sub == 4) {
    FUN_0045d080();
    FUN_0045cb20(0, 0, -1);
    FUN_0045ca30();
  }
}
