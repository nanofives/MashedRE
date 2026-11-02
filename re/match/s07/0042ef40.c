// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042ef40 (h7): 100% (byte-identical incl. the jump tables past Ghidra size 148). Raw g2s 70.0%
   (goto web). Source: one switch, cases paired 2/1000, 3/1001, 4/1002, 5/1005, 10/1011, each testing byte
   0/1/2/5/11 of the 12-byte rows at 0x007f0e50 == 1. Residual: none. */
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
MGLOBALA(char, DAT_007f0e50, 0x9c, 0x007f0e50);
// MATCH: 0x0042ef40 FUN_0042ef40
int FUN_0042ef40(int param_1, int param_2)
{
  int r;

  r = 0;
  switch (param_2) {
  case 2:
  case 1000:
    if (DAT_007f0e50[param_1 * 0xc] == 1) {
      r = 1;
    }
    break;
  case 3:
  case 0x3e9:
    if (DAT_007f0e50[param_1 * 0xc + 1] == 1) {
      r = 1;
    }
    break;
  case 4:
  case 0x3ea:
    if (DAT_007f0e50[param_1 * 0xc + 2] == 1) {
      r = 1;
    }
    break;
  case 5:
  case 0x3ed:
    if (DAT_007f0e50[param_1 * 0xc + 5] == 1) {
      r = 1;
    }
    break;
  case 10:
  case 0x3f3:
    if (DAT_007f0e50[param_1 * 0xc + 0xb] == 1) {
      r = 1;
    }
    break;
  }
  return r;
}
