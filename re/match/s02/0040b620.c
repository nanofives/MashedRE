// FLAGS: /O2 /GS /Ob1
/* 0x0040b620 (136 B): 100% exact, regs 100%. Raw Ghidra 49%. Natural form (4-entry score copy, -100 when the 16-byte
   record at 0x007f1a14 holds -1, then a bubble sort of param_1 by score) gave 94.1%: the induction base was
   &score[1] instead of &score[0]. A named k = j + 1 for the neighbour index fixed it (swap order, pointer form,
   split temps, loop shape and the permuter did not). No residual. */
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
MGLOBALA(int, DAT_008a9530, 4, 0x008a9530);
MGLOBALA(int, DAT_007f1a14, 16, 0x007f1a14);

// MATCH: 0x0040b620 FUN_0040b620
void FUN_0040b620(int *param_1)
{
  int score[4];
  int i;
  int j;
  int t;
  int k;

  for (i = 0; i < 4; i++) {
    score[i] = DAT_008a9530[i];
    if (DAT_007f1a14[i * 4] == -1) {
      score[i] = -100;
    }
    param_1[i] = i;
  }
  for (i = 3; i > 0; i--) {
    for (j = 0; j < i; j++) {
      k = j + 1;
      if (score[j] < score[k]) {
        t = score[j];
        score[j] = score[k];
        score[k] = t;
        t = param_1[j];
        param_1[j] = param_1[k];
        param_1[k] = t;
      }
    }
  }
}
