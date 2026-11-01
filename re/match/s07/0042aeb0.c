// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042aeb0 (h6, by hand): 91.1% on Ghidra's 151 B size, whole obj symbol BYTE-IDENTICAL (157 B, relocations masked): Ghidra's size stops before the loop-2 tail (jl/pop/xor/pop/ret). Raw 32.7%. Fixes: if/else around the two loops (puts push esi/edi before the branch and merges the return-1 block), loop 1 indexed as one array DAT_007f1043[i*0x4c] / [i*0x4c+0x4c0] (compiler folds the base to 0x7f1503), loop 2 over a 16-byte table. Residual: none. */
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
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_00898ab0, 0x00898ab0);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(char, DAT_007f1043, 0x4c0, 0x007f1043);
MGLOBALA(char, DAT_007f1503, 0x4c0, 0x007f1503);
MGLOBALA(S10, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e470(int p0);
// MATCH: 0x0042aeb0 FUN_0042aeb0
int FUN_0042aeb0(void)
{
  int i;
  if ((DAT_0067eab0 != 2) && (DAT_00898ab0 != 0)) {
    return 0;
  }
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1043[i * 0x4c] != 0 && DAT_007f1043[i * 0x4c + 0x4c0] == 0) {
        return 1;
      }
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1 && FUN_0040e470(i) != 2 &&
          DAT_007f1043[DAT_007f1a14[i].idx * 0x4c] != 0 && DAT_007f1503[DAT_007f1a14[i].idx * 0x4c] == 0) {
        return 1;
      }
    }
  }
  return 0;
}
