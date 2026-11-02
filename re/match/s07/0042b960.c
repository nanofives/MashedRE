// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042b960 (h6, by hand): 90.9% on Ghidra's 109 B size, whole obj symbol BYTE-IDENTICAL (116 B, relocations masked): Ghidra's size stops before the last call/add esp/ret. Raw 84.8%. Fix: counted for loop with break into a local, stored to DAT_007f1a14[0] after the loop (Ghidra's in-loop global increment stored 0 first). Residual: none. */
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
typedef struct { int idx; int pad[3]; } S10;
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(char, DAT_007f1042, 0x4c0, 0x007f1042);
void FUN_0040e480(int p0, int p1);
// MATCH: 0x0042b960 FUN_0042b960
void FUN_0042b960(void)
{
  int i;
  for (i = 0; i < 8; i++) {
    if (DAT_007f1042[i * 0x4c] != 0 && DAT_007f1042[i * 0x4c + 0x4c0] == 0) {
      break;
    }
  }
  DAT_007f1a14[0].idx = i;
  DAT_007f1a14[1].idx = -1;
  DAT_007f1a14[2].idx = -1;
  DAT_007f1a14[3].idx = -1;
  DAT_007f1a0c = 1;
  FUN_0040e480(0, 1);
  FUN_0040e480(1, 0);
  FUN_0040e480(2, 0);
  FUN_0040e480(3, 0);
}
