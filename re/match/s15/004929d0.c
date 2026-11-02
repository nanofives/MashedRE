// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004929d0 (h7, by hand): 100.0% (regs 100.0%). Raw converter 72.8%. What moved it: nested
   switch on FUN_0042b930() (dec/sub eax pattern), outer case order in source 1,2,3,7,5,4,6 (the original's code
   order), callees returning int (not bool / unsigned: test eax + jl), every case ends in break (the returns are
   tail-duplicated by the compiler), DAT_007f1a18 as an int array indexed [id*4] (16-byte stride), and the
   FUN_0042b940(3/0) choice written as two calls in if/else (the compiler merges them into push 3 / push esi).
   Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_00771968, 0x00771968);
MGLOBAL(int, DAT_0077196c, 0x0077196c);
MGLOBAL(int, DAT_00771974, 0x00771974);
MGLOBAL(int, DAT_00771970, 0x00771970);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(int, DAT_007f1a18, 64, 0x007f1a18);
void FUN_0040b430(int p0, int p1);
void FUN_0040b540(int * p0);
int FUN_0040b6d0(int p0);
void FUN_0040b6e0(int p0);
void FUN_0040b700(int p0, int p1);
void FUN_0040d440(void);
int FUN_0040e340(void);
int FUN_0040e350(void);
int FUN_0040e450(void);
void FUN_0040e460(undefined4 p0);
int FUN_0042b8d0(void);
int FUN_0042b8e0(void);
int FUN_0042b8f0(void);
int FUN_0042b910(void);
int FUN_0042b930(void);
void FUN_0042b940(undefined4 p0);
int FUN_0042c1c0(void);
void FUN_0042c1d0(void);
int FUN_0042c220(void);
void FUN_0042c280(void);
int FUN_0042c2d0(void);
int FUN_0042c2e0(void);
void FUN_0042c2f0(undefined4 p0);
int FUN_0042f500(void);
int FUN_0042f6a0(void);
int FUN_00432080(undefined4 p0);
void FUN_004331a0(undefined4 p0);
void FUN_00448700(undefined4 p0, undefined4 p1);
void FUN_004927c0(void);
int FUN_005c9d00(void);
// MATCH: 0x004929d0 FUN_004929d0
undefined4 FUN_004929d0(void)
{
  int i;
  int n;
  int run;
  int a;
  int ref;
  int ids[4];

  FUN_004927c0();
  switch (DAT_00771968) {
  case 1:
    if (FUN_0042c2e0() != 0) {
      FUN_0042b940(FUN_0042c2d0());
      FUN_0042c2f0(0);
    }
    switch (FUN_0042b930()) {
    case 1:
      if (FUN_0042b8d0()) {
        DAT_00771968 = 2;
        FUN_0042b940(0);
      }
      break;
    case 4:
      DAT_00771968 = 4;
      FUN_0042b940(0);
      break;
    }
    break;
  case 2:
    FUN_0040d440();
    DAT_00771968 = 3;
    break;
  case 3:
    FUN_0042c1d0();
    if (FUN_0042c220() != 0) {
      FUN_0042c280();
    }
    if (FUN_0042c1c0() != 0) {
      DAT_00771968 = 7;
    }
    if (FUN_0042f6a0() != 0xb) {
      a = FUN_0040e350();
      if (a != 0xb && a != 9) {
        if (FUN_00432080(FUN_005c9d00()) != 0) {
          DAT_00771968 = 5;
        }
      }
    }
    if (FUN_0040e450() != 0) {
      FUN_004331a0(FUN_0042b910());
      DAT_00771968 = 5;
      DAT_00771974 = 1;
      FUN_0040b6e0(1);
      FUN_0040b540(ids);
      n = FUN_0040e340() - 1;
      if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
        ref = DAT_007f1a18[ids[0] * 4];
        for (i = 0; i < 4; i++) {
          if (ref == DAT_007f1a18[ids[i] * 4]) {
            FUN_0040b430(ids[i], 1);
            FUN_0040b700(ids[i], 1);
          } else {
            FUN_0040b700(ids[i], 0);
          }
        }
      } else {
        run = 0;
        for (i = 0; i < FUN_0040e340(); i++) {
          FUN_0040b430(ids[i], n);
          FUN_0040b700(ids[i], n);
          a = FUN_0040b6d0(ids[i]);
          if (i < 3) {
            if (a > FUN_0040b6d0(ids[i + 1])) {
              n += -1 - run;
              run = 0;
            } else {
              run++;
            }
          }
        }
      }
      FUN_00448700(1, ids[0]);
      FUN_0040e460(0);
    }
    break;
  case 7:
    if (FUN_0042c1c0() == 0) {
      DAT_00771968 = 3;
    }
    break;
  case 5:
    if (FUN_0042b8e0()) {
      DAT_00771968 = 4;
    }
    break;
  case 4:
    if (DAT_0077196c != 0 || DAT_00771970 != 0 || DAT_00771974 != 0) {
      DAT_00771968 = 1;
      if (DAT_00771974 != 0) {
        FUN_0042b940(3);
      } else {
        FUN_0042b940(0);
      }
      DAT_0077196c = 0;
      DAT_00771970 = 0;
      DAT_00771974 = 0;
    }
    if (FUN_0042c2e0() != 0) {
      FUN_0042b940(FUN_0042c2d0());
      FUN_0042c2f0(0);
    }
    if (FUN_0042b8f0()) {
      DAT_00771968 = 6;
    }
    break;
  case 6:
    if (FUN_0042b8d0()) {
      DAT_00771968 = 3;
    }
    break;
  }
  return 1;
}
