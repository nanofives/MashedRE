// FLAGS: /O2 /GS /Ob1
/* 0x0040ac80 (Ghidra 228 B decomp / 80 B symbol): 100% of the code, whole obj symbol BYTE-IDENTICAL
   (match.py --bytes, relocations masked). The instruction score reads 54.5% only because Ghidra's size
   runs into the 6-entry jump table after the ret.
   The case handlers are four functions Ghidra has no entry for (it inlined them into the decomp because
   the dispatcher tail-jumps to them). This file holds them as statics, each byte-identical (checked with
   match.py's score_function/byte_compare at the given size; they carry a HIDDEN marker instead of MATCH
   because match.py needs a Ghidra size):
     0x0040abc0 (case 1), 0x0040abe0 (case 4), 0x0040ac00 (case 3), 0x0040ac40 (case 2).
   What moved it: cases written in the order 2,3,4,1,6 (the original's case body order), the >=/<= forms of
   the range tests, and in 0x0040ac40 the 0x8a958c test before the stores with both branches written out.
   Raw g2s was 15.9%. Residual: none. */
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
MGLOBAL(int, DAT_008a9584, 0x008a9584);
MGLOBAL(int, DAT_008a9588, 0x008a9588);
MGLOBAL(int, DAT_008a958c, 0x008a958c);
MGLOBAL(int, DAT_008a9590, 0x008a9590);
MGLOBAL(int, DAT_008a9594, 0x008a9594);
MGLOBAL(int, DAT_008a9598, 0x008a9598);

void FUN_0042c1a0(void);

// HIDDEN: 0x0040abc0 _FUN_0040abc0 31
static void FUN_0040abc0(void)
{
  if (DAT_008a9588 >= 6 && (DAT_008a9588 <= 8 || DAT_008a9588 == 0xb)) {
    DAT_008a9584 = 0;
  }
}

// HIDDEN: 0x0040abe0 _FUN_0040abe0 31
static void FUN_0040abe0(void)
{
  if (DAT_008a9594 >= 6 && DAT_008a9594 <= 10) {
    FUN_0042c1a0();
    DAT_008a9584 = 0;
  }
}

// HIDDEN: 0x0040ac00 _FUN_0040ac00 62
static void FUN_0040ac00(void)
{
  if (DAT_008a9590 >= 9 && DAT_008a9590 > 10 && DAT_008a9590 == 0xc) {
    DAT_008a958c = 0;
    return;
  }
  DAT_008a9584 = 0;
  DAT_008a9598 = 8;
  DAT_008a9590 = 0xb;
}

// HIDDEN: 0x0040ac40 _FUN_0040ac40 50
static void FUN_0040ac40(void)
{
  if (DAT_008a958c == 7) {
    DAT_008a958c = 6;
    DAT_008a9584 = 0;
  } else {
    DAT_008a958c = 6;
    DAT_008a9584 = 0;
    DAT_008a9598 = 8;
    DAT_008a9590 = 0xb;
  }
}

// MATCH: 0x0040ac80 FUN_0040ac80
void FUN_0040ac80(void)
{
  switch (DAT_008a9584) {
  case 2:
    FUN_0040ac40();
    return;
  case 3:
    FUN_0040ac00();
    return;
  case 4:
    FUN_0040abe0();
    return;
  case 1:
    FUN_0040abc0();
    return;
  case 6:
    FUN_0042c1a0();
    DAT_008a9584 = 0;
    return;
  }
}
