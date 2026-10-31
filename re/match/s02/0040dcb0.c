// FLAGS: /O2 /GS /Ob1
/* 0x0040dcb0 FUN_0040dcb0 (176 B): 100.0% exact, regs 100.0%.
   Raw g2s was 8.8% (globals typed as bytes, if-chain). Hand form: switch on DAT_0063b90c, float globals,
   literal constants read from .rdata (0x005ccd7c = 700.0f, 0x005ccd74 = 320.0f, 0x005ccd78 = -320.0f,
   0x005d757c = 0.0f), <= compares. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063b90c, 0x0063b90c);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_0063b910, 0x0063b910);
MGLOBAL(float, DAT_0063b918, 0x0063b918);

// MATCH: 0x0040dcb0 FUN_0040dcb0
void FUN_0040dcb0(void)
{
  switch (DAT_0063b90c) {
  case 1:
    DAT_0063b910 = DAT_0063b910 - DAT_007f100c * 700.0f;
    if (DAT_0063b910 <= 320.0f) {
      DAT_0063b910 = 320.0f;
      DAT_0063b918 = 5.0f;
      DAT_0063b90c = 2;
    }
    break;
  case 2:
    DAT_0063b918 = DAT_0063b918 - DAT_007f100c;
    if (DAT_0063b918 <= 0.0f) {
      DAT_0063b90c = 3;
    }
    break;
  case 3:
    DAT_0063b910 = DAT_0063b910 - DAT_007f100c * 700.0f;
    if (DAT_0063b910 <= -320.0f) {
      DAT_0063b90c = 0;
    }
    break;
  }
}
