// FLAGS: /O2 /GS /Ob1
/* 0x00423320 (180 B). Best 100.0% exact (regs 100, ops 100).
   Raw g2s 10.9% (Ghidra folded the read-modify-write into select temporaries). By hand: DAT_007f1a90 as int,
   a local step = 4 (held in ecx; the `add [0x7f1a90], ecx` is `+= step`), each key block as
   modify-then-undo on the bound test. Residual: none. */
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
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
MGLOBAL(char, DAT_007f1072, 0x007f1072);
MGLOBAL(char, DAT_007f1532, 0x007f1532);
MGLOBAL(int, DAT_007f1a90, 0x007f1a90);
MGLOBAL(char, DAT_007f1073, 0x007f1073);
MGLOBAL(char, DAT_007f1533, 0x007f1533);
MGLOBAL(char, DAT_007f1074, 0x007f1074);
MGLOBAL(char, DAT_007f1534, 0x007f1534);
MGLOBAL(char, DAT_007f1075, 0x007f1075);
MGLOBAL(char, DAT_007f1535, 0x007f1535);

// MATCH: 0x00423320 FUN_00423320
void FUN_00423320(void)
{
  int step = 4;
  if (DAT_007f1a64 != 2) {
    if (DAT_007f1072 && !DAT_007f1532) {
      DAT_007f1a90 -= step;
      if (DAT_007f1a90 < 0) DAT_007f1a90 += step;
    }
    if (DAT_007f1073 && !DAT_007f1533) {
      DAT_007f1a90 += step;
      if (DAT_007f1a90 >= 0x80) DAT_007f1a90 -= step;
    }
  }
  if (DAT_007f1074 && !DAT_007f1534) {
    DAT_007f1a90--;
    if ((DAT_007f1a90 & 3) == 3) DAT_007f1a90 += step;
  }
  if (DAT_007f1075 && !DAT_007f1535) {
    DAT_007f1a90++;
    if ((DAT_007f1a90 & 3) == 0) DAT_007f1a90 -= step;
  }
}
