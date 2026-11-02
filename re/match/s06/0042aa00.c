// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042aa00 (160 B): best 76.8% exact (regs 87.5, ops 89.3).
   Raw converter 29.6%. The 0x40-byte records are indexed as DAT_0067ed78[DAT_0067e9f8 - 1] (array base 0x0067ed78
   = 0x0067ed38 + 0x40): that is what gives the original's lea ebx, [idx*16 - 0x10] with the 0x0067ed84 flag
   displacement (base 0x0067ed38 with [DAT_0067e9f8] gave the same score but the wrong split). Field names are by
   offset, record count not established. Tried: do/while, for(;;) with break and a tail store, while (!found),
   ++n forms, found/n declaration order, a local copy of step, a local record pointer (39%), a separate index
   variable (33%), mixed bases (40-77%), permuter (95 compiles, no gain).
   Residual (b) register allocation: the original loads step into EBP in the prologue (before the count test)
   and keeps idx*16 - 16 in EBX; ours loads step into EBX after the early return and puts the bias in EBP. No
   statement order, loop form or declaration change moved it. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
/* 0x40-byte records at 0x0067ed38 indexed by DAT_0067e9f8: +0x08 = 0x0067ed40, +0x0c.. = 0x0067ed44 (int per entry),
   +0x3c = 0x0067ed74. Fields named by offset; the record count is not established (N = 1 here). */
typedef struct {
  int *f00;
  int f04;
  int f08;
  int f0c[12];
  int f3c;
} Rec67ed38;
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(Rec67ed38, DAT_0067ed78, 1, 0x0067ed78);
// MATCH: 0x0042aa00 FUN_0042aa00
void FUN_0042aa00(int step)
{
  int n = 0;
  int found = 0;
  if (DAT_0067ed78[DAT_0067e9f8 - 1].f3c == 0) {
    DAT_0067ed78[DAT_0067e9f8 - 1].f08 = -1;
    return;
  }
  do {
    DAT_0067ed78[DAT_0067e9f8 - 1].f08 += step;
    if (DAT_0067ed78[DAT_0067e9f8 - 1].f08 == DAT_0067ed78[DAT_0067e9f8 - 1].f3c)
      DAT_0067ed78[DAT_0067e9f8 - 1].f08 = 0;
    else if (DAT_0067ed78[DAT_0067e9f8 - 1].f08 < 0)
      DAT_0067ed78[DAT_0067e9f8 - 1].f08 = DAT_0067ed78[DAT_0067e9f8 - 1].f3c - 1;
    if (DAT_0067ed78[DAT_0067e9f8 - 1].f0c[DAT_0067ed78[DAT_0067e9f8 - 1].f08] == 1)
      found = 1;
    n++;
    if (n > DAT_0067ed78[DAT_0067e9f8 - 1].f3c) {
      DAT_0067ed78[DAT_0067e9f8 - 1].f08 = -1;
      return;
    }
  } while (!found);
}
