// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042b9e0 (h6, by hand): 96.3% on Ghidra's 364 B size, whole obj symbol BYTE-IDENTICAL (377 B, relocations masked): Ghidra's size stops before the shared exit (store 2, return 0x1000). Raw 30.0%. Rewritten: counting loop over the 12-entry 12-byte table (compiler unrolls by 6), pairwise duplicate loop, then per-port level search. The duplicate loop's outer bound is i < 11 (relocheck caught i < 12: same bytes once relocations are masked). The register split came from placing level = 1 right before the port loop (the compiler schedules it into the count loop, which is where Ghidra showed it; level = 1 written there gave 48.5%). Residual: none. */
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
typedef struct { int idx; int b; int c; int d; } S10;
typedef struct { int a; int b; int c; } S0C;
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(S0C, DAT_0067eaf0, 12, 0x0067eaf0);
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
void FUN_0040e480(int p0, int p1);
// MATCH: 0x0042b9e0 FUN_0042b9e0
int FUN_0042b9e0(void)
{
  int state;
  int found;
  int count;
  int level;
  int port;
  int i;
  int j;
  state = 0;
  for (i = 0; i < 4; i++) {
    FUN_0040e480(i, 0);
  }
  count = 12;
  for (i = 0; i < 12; i++) {
    if (DAT_0067eaf0[i].a < 1) {
      count--;
    }
  }
  if (count < 2) {
    return 1;
  }
  found = 0;
  for (i = 0; i < 11; i++) {
    for (j = i + 1; j < 12; j++) {
      if (DAT_0067eaf0[i].a == DAT_0067eaf0[j].a && DAT_0067eaf0[i].a > 0) {
        found = 1;
      }
    }
  }
  if (found) {
    return 0;
  }
  for (i = 0; i < 4; i++) {
    DAT_007f1a14[i].idx = -1;
  }
  level = 1;
  for (port = 0; port < 4; port++) {
    if (state == 2) {
      break;
    }
    state = 0;
    do {
      for (j = 0; j < 12; j++) {
        if (DAT_0067eaf0[j].a == level) {
          DAT_007f1a14[port].idx = j;
          DAT_007f1a14[port].c = level - 1;
          FUN_0040e480(port, 1);
          state = 1;
        }
      }
      level++;
      if (level == 7) {
        state = 2;
        break;
      }
    } while (state == 0);
  }
  DAT_007f1a0c = 2;
  return 0x1000;
}
