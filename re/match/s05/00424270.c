// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00424270 (Ghidra 1537 B): matched. match.py 99.6% (regs 99.6%, ops 99.6%), 484/482 ins. The only
   difference is the trailing `add esp,0x10; ret` the original has past Ghidra's body (the switch jump table at
   0x00424874 follows). `match.py --bytes`: 1596 B whole obj symbol, 0 differing unmasked bytes, BYTE-IDENTICAL.
   relocheck OK. By hand (h8), raw converter 42.5%. What moved it: the 4-entry 0x10-byte record array at 0x007f1a14
   (+0x00 compared with -1, +0x04 team index), the team sums written as a 4-iteration loop (the compiler unrolls it,
   47.6% -> regs 93%), the score swap written `t = score[j + 1]; score[j + 1] = score[j]; score[j] = t;` (the
   other spelling moved the order-minus-score offset out of ESI, 47.6% -> 99.4%; found from a permuter variant that
   itself changed behaviour and was not kept), and sum1 zeroed before sum0. Residual: none in bytes; class (c) for the
   match.py line (function boundary before the jump table). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct Rec_00424270 {
  int f_00;
  int team;
  int f_08;
  int f_0c;
} Rec_00424270;
MGLOBALA(Rec_00424270, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e340(void);
int FUN_00423b40(int p0);
int FUN_00423b60(int p0);
int FUN_00423b80(int p0);
int FUN_00423ba0(int p0);
int FUN_00423bc0(int p0);
int FUN_00423c40(int p0);
int FUN_00423cc0(int p0);
int FUN_00423d50(int p0);
int FUN_00423dd0(int p0);
int FUN_00423e60(int p0);
int FUN_00423ee0(int p0);
int FUN_00423f60(int p0);
int FUN_00423ff0(int p0);
int FUN_00424070(int p0);
int FUN_0042f500(void);
// MATCH: 0x00424270 FUN_00424270
void FUN_00424270(int *order, int kind, int ascending)
{
  int score[4];
  int i;
  int j;
  int t;
  int sum0;
  int sum1;

  switch (kind) {
  case 0:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423b40(i);
      order[i] = i;
    }
    break;
  case 1:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423b60(i);
      order[i] = i;
    }
    break;
  case 2:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423bc0(i);
      order[i] = i;
    }
    break;
  case 3:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423c40(i);
      order[i] = i;
    }
    break;
  case 4:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423cc0(i);
      order[i] = i;
    }
    break;
  case 5:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = 100000; else score[i] = FUN_00423d50(i);
      order[i] = i;
    }
    break;
  case 6:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423dd0(i);
      order[i] = i;
    }
    break;
  case 7:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423b80(i);
      order[i] = i;
    }
    break;
  case 8:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423ba0(i);
      order[i] = i;
    }
    break;
  case 9:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423e60(i);
      order[i] = i;
    }
    break;
  case 10:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423ee0(i);
      order[i] = i;
    }
    break;
  case 11:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00423f60(i);
      order[i] = i;
    }
    break;
  case 12:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = 100000; else score[i] = FUN_00423ff0(i);
      order[i] = i;
    }
    break;
  case 13:
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].f_00 == -1) score[i] = -100; else score[i] = FUN_00424070(i);
      order[i] = i;
    }
    break;
  default:
    order[0] = 0;
    order[1] = 1;
    order[2] = 2;
    order[3] = 3;
    return;
  }
  if (FUN_0042f500() && kind != 0 && kind != 1 && kind != 7 && kind != 8) {
    sum1 = 0;
    sum0 = 0;
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].team == 0) sum0 += score[i];
      if (DAT_007f1a14[i].team == 1) sum1 += score[i];
    }
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].team == 0) score[i] = sum0;
      if (DAT_007f1a14[i].team == 1) score[i] = sum1;
    }
  }
  if (ascending) {
    for (i = 0; i < FUN_0040e340() - 1; i++) {
      for (j = 0; j < FUN_0040e340() - i - 1; j++) {
        if (score[j] > score[j + 1]) {
          t = score[j + 1];
          score[j + 1] = score[j];
          score[j] = t;
          t = order[j];
          order[j] = order[j + 1];
          order[j + 1] = t;
        }
      }
    }
  } else {
    for (i = 0; i < FUN_0040e340(); i++) {
      for (j = 0; j < FUN_0040e340() - i - 1; j++) {
        if (score[j] < score[j + 1]) {
          t = score[j + 1];
          score[j + 1] = score[j];
          score[j] = t;
          t = order[j];
          order[j] = order[j + 1];
          order[j + 1] = t;
        }
      }
    }
  }
  if (FUN_0042f500()) {
    if (DAT_007f1a14[order[0]].team == DAT_007f1a14[order[1]].team) {
      order[1] = order[2];
    }
    if (DAT_007f1a14[order[0]].team == DAT_007f1a14[order[1]].team &&
        DAT_007f1a14[order[1]].team == DAT_007f1a14[order[2]].team) {
      order[1] = order[3];
    }
  }
}
