// FLAGS: /O2 /GS /Ob1
/* 0x0040ab40 (Ghidra 89 B / 116 B symbol): 100.0% exact, regs 100.0%, whole obj symbol BYTE-IDENTICAL
   (jump table included). Raw g2s 21.6%: Ghidra inlined four tail-jumped case handlers that have no Ghidra
   function entry. This file holds them as statics, each byte-identical including its own jump/index
   tables (checked with match.py's score_function/byte_compare; HIDDEN marker instead of MATCH because
   match.py needs a Ghidra size):
     0x0040a9a0 (case 3, on 0x8a9590), 0x0040aa30 (case 2, on 0x8a958c),
     0x0040aa60 (case 1, on 0x8a9588), 0x0040aad0 (case 4, on 0x8a9594).
   Inner switches: case bodies written in the original's emission order (0x0040a9a0 has `default` falling
   into `case 5`). Residual: none. Same TU shape as 0x0040ac80 (sibling dispatcher). */
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

void FUN_00409930(void);
void FUN_00409970(void);
void FUN_0042c1a0(void);
void FUN_0045b350(void);

// HIDDEN: 0x0040a9a0 _FUN_0040a9a0 99
static void FUN_0040a9a0(void)
{
  switch (DAT_008a9590) {
  case 9:
  case 10:
    DAT_008a9590 = 3;
    return;
  case 0xc:
    DAT_008a9590 = 2;
    return;
  case 8:
    DAT_008a9590 = 6;
    return;
  case 6:
  case 7:
    FUN_00409970();
    return;
  default:
    DAT_008a958c = 6;
    DAT_008a9598 = 8;
  case 5:
    DAT_008a9584 = 0;
    DAT_008a9590 = 0xb;
    return;
  }
}

// HIDDEN: 0x0040aa30 _FUN_0040aa30 42
static void FUN_0040aa30(void)
{
  switch (DAT_008a958c) {
  case 7:
    DAT_008a958c = 2;
    return;
  case 0:
  case 5:
    DAT_008a9584 = 0;
    return;
  }
}

// HIDDEN: 0x0040aa60 _FUN_0040aa60 62
static void FUN_0040aa60(void)
{
  switch (DAT_008a9588) {
  case 6:
    DAT_008a9588 = 7;
    return;
  case 7:
  case 8:
    FUN_00409970();
    return;
  case 0:
  case 0x11:
    DAT_008a9584 = 0;
    return;
  case 0xb:
    DAT_008a9588 = 0xc;
    break;
  }
}

// HIDDEN: 0x0040aad0 _FUN_0040aad0 66
static void FUN_0040aad0(void)
{
  switch (DAT_008a9594) {
  case 9:
  case 10:
    DAT_008a9594 = 1;
    FUN_0045b350();
    return;
  case 8:
    DAT_008a9594 = 6;
    return;
  case 6:
  case 7:
    FUN_00409970();
    return;
  case 0:
    DAT_008a9584 = 0;
    break;
  }
}

// MATCH: 0x0040ab40 FUN_0040ab40
void FUN_0040ab40(void)
{
  switch (DAT_008a9584) {
  case 1:
    FUN_0040aa60();
    return;
  case 2:
    FUN_0040aa30();
    return;
  case 3:
    FUN_0040a9a0();
    return;
  case 4:
    FUN_0040aad0();
    return;
  case 6:
    FUN_0042c1a0();
    FUN_00409930();
    return;
  default:
    DAT_008a9584 = 0;
    DAT_008a9598 = 8;
    DAT_008a9590 = 0xb;
    DAT_008a958c = 6;
    return;
  }
}
