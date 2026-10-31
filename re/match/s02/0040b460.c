// FLAGS: /O2 /GS /Ob1
/* 0x0040b460 FUN_0040b460 (221 B): 100.0% exact, regs 100.0%.
   Raw g2s was 47.7%. Hand form: score[4] from 0x008a94f0 (-1 when the 0x10-stride record at 0x007f1a14 is
   negative), out[i] = i, bubble sort descending swapping score and out together. Indexing score[i] directly
   gave 96.2% (the compiler based out-minus-score on &score[1]); a block-local q = score + i gives the original
   base. Residual: none. */
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
typedef struct Rec10 { int v; int pad[3]; } Rec10;
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fcc, 0x007f0fcc);
MGLOBALA(Rec10, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(int, DAT_008a94f0, 4, 0x008a94f0);

int FUN_00417740(int p0);

// MATCH: 0x0040b460 FUN_0040b460
void FUN_0040b460(int *param_1)
{
  int i;
  int n;
  int t;
  int score[4];

  for (i = 0; i < 4; i++) {
    score[i] = DAT_008a94f0[i];
    if (DAT_007f1a14[i].v < 0) {
      score[i] = -1;
    }
    param_1[i] = i;
  }
  for (n = 3; n > 0; n--) {
    for (i = 0; i < n; i++) {
      int *q = score + i;
      if (q[0] < q[1]) {
        t = q[0];
        q[0] = q[1];
        q[1] = t;
        t = param_1[i];
        param_1[i] = param_1[i + 1];
        param_1[i + 1] = t;
      }
    }
  }
  if (DAT_007f0fd0 == 4 || DAT_007f0fd0 == 7) {
    for (i = 0; i < 4; i++) {
      param_1[i] = FUN_00417740(i);
    }
  }
  if (DAT_007f0fd0 == 9) {
    if (DAT_007f0fcc != 0) {
      param_1[0] = 0;
      param_1[1] = 1;
    }
    else {
      param_1[0] = 1;
      param_1[1] = 0;
    }
  }
}
