// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 lead 2026-10-31: store order fixed after re/tools/match/relocheck.py showed the stores went to swapped arrays at 100% masked score; now 100% and relocheck OK. */
/* 0x0040b810 (125 B): 100% exact, regs 100%. Raw Ghidra was 40% (flattened 21 scalar stores). Two unrolled
   for-loops over 4-dword arrays (9550/9540, then 9560/9530/94f0) give the three zero registers. No residual. */
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
MGLOBALA(undefined4, DAT_008a94f0, 4, 0x008a94f0);
MGLOBALA(undefined4, DAT_008a9530, 4, 0x008a9530);
MGLOBALA(undefined4, DAT_008a9540, 4, 0x008a9540);
MGLOBALA(undefined4, DAT_008a9550, 4, 0x008a9550);
MGLOBALA(undefined4, DAT_008a9560, 4, 0x008a9560);
MGLOBAL(undefined4, DAT_0063b8ec, 0x0063b8ec);

// MATCH: 0x0040b810 FUN_0040b810
void FUN_0040b810(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    DAT_008a9540[i] = 0;
    DAT_008a9550[i] = 0;
  }
  for (i = 0; i < 4; i++) {
    DAT_008a94f0[i] = 0;
    DAT_008a9530[i] = 0;
    DAT_008a9560[i] = 0;
  }
  DAT_0063b8ec = 0;
}
