// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042aff0 (h6, by hand): 100% (regs 100%), whole obj symbol byte-identical. Raw 33.9%. Rewritten: found-flag over an 8-entry loop (the compiler unrolls it, Ghidra showed 7 ORs) or the 4-entry 16-byte table, timer add/compare on float 0x0067f1b0, range test written as >= 6 && <= 7 (> 5 && < 8 gave cmp 5/8), then the same two-loop tail as 0x0042aeb0. Residual: none. */
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
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(char, DAT_007f1046, 0x4c0, 0x007f1046);
MGLOBALA(char, DAT_007f1506, 0x4c0, 0x007f1506);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(float, DAT_0067f1b0, 0x0067f1b0);
MGLOBAL(float, DAT_005ccac0, 0x005ccac0);
MGLOBALA(int, DAT_0067ed3c, 0x100, 0x0067ed3c);
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e470(int p0);
// MATCH: 0x0042aff0 FUN_0042aff0
int FUN_0042aff0(void)
{
  int found;
  int i;
  found = 0;
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1046[i * 0x4c] != 0) {
        found = 1;
      }
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1 && FUN_0040e470(i) != 2 &&
          DAT_007f1046[DAT_007f1a14[i].idx * 0x4c] != 0) {
        found = 1;
      }
    }
  }
  if (found) {
    if (DAT_0067ed3c[DAT_0067e9f8 * 0x10] >= 6 && DAT_0067ed3c[DAT_0067e9f8 * 0x10] <= 7) {
      DAT_0067f1b0 = DAT_007f1004 + DAT_0067f1b0;
      if (DAT_0067f1b0 > DAT_005ccac0) {
        DAT_0067f1b0 = 0.2f;
        return 1;
      }
    }
  }
  else {
    DAT_0067f1b0 = 0.0f;
  }
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1046[i * 0x4c] != 0 && DAT_007f1046[i * 0x4c + 0x4c0] == 0) {
        return 1;
      }
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1 && FUN_0040e470(i) != 2 &&
          DAT_007f1046[DAT_007f1a14[i].idx * 0x4c] != 0 && DAT_007f1506[DAT_007f1a14[i].idx * 0x4c] == 0) {
        return 1;
      }
    }
  }
  return 0;
}
