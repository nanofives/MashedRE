// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042b310 (h6, by hand): 100% (regs 100%), whole obj symbol byte-identical. Raw 57.1%. Rewritten: found-flag over the 8-entry loop (unrolled by the compiler) or the 16-byte table; switch on the mode word with case 0x13 holding two ifs (b==0||b==1, then b==2; an inner switch gave jl/jle) and case 0x1e (plain if/else-if put the blocks in the wrong order); result-flag tail loops. Residual: none. */
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
typedef struct { int a; int b; int pad[14]; } S40;
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(char, DAT_007f1044, 0x4c0, 0x007f1044);
MGLOBALA(char, DAT_007f1504, 0x4c0, 0x007f1504);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067f1b8, 0x0067f1b8);
MGLOBALA(S40, DAT_0067ed3c, 0x10, 0x0067ed3c);
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e470(int p0);
// MATCH: 0x0042b310 FUN_0042b310
int FUN_0042b310(void)
{
  int found;
  int result;
  int i;
  result = 0;
  found = 0;
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1044[i * 0x4c] != 0) {
        found = 1;
      }
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1 && FUN_0040e470(i) != 2 &&
          DAT_007f1044[DAT_007f1a14[i].idx * 0x4c] != 0) {
        found = 1;
      }
    }
  }
  if (found) {
    switch (DAT_0067ed3c[DAT_0067e9f8].a) {
    case 0x13:
      if (DAT_0067ed3c[DAT_0067e9f8].b == 0 || DAT_0067ed3c[DAT_0067e9f8].b == 1) {
        return 1;
      }
      if (DAT_0067ed3c[DAT_0067e9f8].b == 2) {
        return 1;
      }
      break;
    case 0x1e:
      return 1;
    }
  }
  else {
    DAT_0067f1b8 = 0;
  }
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1044[i * 0x4c] != 0 && DAT_007f1044[i * 0x4c + 0x4c0] == 0) {
        result = 1;
      }
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1 && FUN_0040e470(i) != 2 &&
          DAT_007f1044[DAT_007f1a14[i].idx * 0x4c] != 0 && DAT_007f1504[DAT_007f1a14[i].idx * 0x4c] == 0) {
        result = 1;
      }
    }
  }
  return result;
}
