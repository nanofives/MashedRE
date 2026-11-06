// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00492e90 (h7, by hand): 100.0% (regs 100.0%). Raw 49.9%. What moved it: real callee arities
   (FUN_004671a0 takes 1 arg, FUN_004c1bb0 3, FUN_004c1be0 3, FUN_0042f510 0), FUN_004950b0 unsigned (mul by
   1/3000), the 0.8f pair written inline in case 1 (same body as FUN_00492e60), switch cases in source order
   1,2,3/7,5/6,4, case 3/7 as if (DAT_007719c8 != 0) { if (state != 7) { dec; break; } } else {...} then the
   state == 7 tail, RwGlobals+0x20 as a cdecl (int,int) pointer, the 96-entry 0x44-byte array loop, and the
   tail statements in order 998, 9a4++, 98c++ (the order 998, 98c++, 9a4++ is also
   100% by instruction but swaps the two globals: relocheck BAD). Float literal 0.001f is pooled at 0x005cc558. Residual: none. */
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
typedef struct {
  int f[17];
} Stat44;
typedef struct {
  char pad[0x20];
  int (*renderStateSet)(int state, int value);
} RwGlobalsE90;
MGLOBAL(int, DAT_0077198c, 0x0077198c);
MGLOBAL(int, DAT_007719a0, 0x007719a0);
MGLOBAL(int, DAT_00771998, 0x00771998);
MGLOBAL(int, DAT_0077199c, 0x0077199c);
MGLOBAL(float, DAT_00771988, 0x00771988);
MGLOBAL(int, DAT_007719a4, 0x007719a4);
MGLOBAL(int, DAT_00771990, 0x00771990);
MGLOBAL(int, DAT_00771968, 0x00771968);
MGLOBAL(int, DAT_007719c8, 0x007719c8);
MGLOBAL(int, DAT_007719ac, 0x007719ac);
MGLOBAL(int, DAT_007719b0, 0x007719b0);
MGLOBAL(RwGlobalsE90 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(unsigned char, DAT_006147b4, 4, 0x006147b4);
MGLOBALA(Stat44, DAT_00868320, 96, 0x00868320);
void FUN_00403050(void);
void FUN_00404320(void);
void FUN_0040de30(int p0);
void FUN_0040df20(int p0);
void FUN_0040df60(void);
void FUN_0040dfc0(void);
void FUN_00410b30(void);
void FUN_00426030(void);
void FUN_00426670(int p0);
void FUN_004266b0(int p0);
int FUN_0042a9f0(void);
int FUN_0042b8d0(void);
int FUN_0042b8f0(void);
int FUN_0042b930(void);
int FUN_0042d390(void);
int FUN_0042f510(void);
int FUN_0042f530(void);
void FUN_00430b90(void);
void FUN_00433f40(void);
void FUN_0043c5b0(void);
int FUN_004671a0(int p0);
int FUN_004671c0(void);
void FUN_00492440(Stat44 *p0);
void FUN_00492e60(void);
unsigned int FUN_004950b0(void);
void FUN_00498bf0(void);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
int FUN_004c1bb0(int p0, unsigned char *p1, int p2);
int FUN_004c1be0(int p0, int p1, int p2);
int FUN_004c1c80(int p0, float *p1);
// MATCH: 0x00492e90 FUN_00492e90
int FUN_00492e90(void)
{
  unsigned char col[4];
  unsigned char *c;
  int t0;
  int t1;
  int r;
  int i;
  float v[2];

  col[0] = 0;
  col[1] = 0;
  col[2] = 0;
  col[3] = 0xff;
  c = DAT_006147b4;
  t0 = FUN_004950b0();
  DAT_007719a0 = FUN_004950b0() / 3000;
  if (DAT_0077198c == 0) {
    DAT_00771998 = DAT_007719a0;
  }
  DAT_00771988 = (float)(DAT_007719a0 - DAT_00771998) * 0.001f;
  if (DAT_007719a0 > DAT_0077199c + 1000) {
    DAT_00771990 = DAT_007719a4;
    DAT_007719a4 = 0;
    DAT_0077199c = DAT_007719a0;
  }
  if (DAT_00771968 == 6) {
    DAT_007719c8 = FUN_0042a9f0() >= 200;
  }
  if (DAT_00771968 == 2 || DAT_00771968 == 1 || DAT_007719c8 != 0) {
    c = col;
  }
  FUN_004c1bb0(FUN_004671a0(-1), c, 3);
  switch (DAT_00771968) {
  case 1:
    r = FUN_0042b930();
    if (r == 1 && (FUN_0042b8f0() || FUN_0042b8d0())) {
      if (FUN_004c1a00(FUN_004671a0(-1))) {
        FUN_004c19f0(FUN_004671a0(-1));
      }
    } else {
      v[0] = 0.8f;
      v[1] = 0.8f;
      FUN_004c1c80(FUN_004671a0(-1), v);
      FUN_00492e60();
      if (FUN_004c1a00(FUN_004671a0(-1))) {
        FUN_0043c5b0();
        if (r == 0x21) {
          FUN_00403050();
        }
        FUN_004c19f0(FUN_004671a0(-1));
      }
      if (r == 3 && FUN_0042f530() && FUN_0042d390() < 3) {
        FUN_00426030();
        FUN_004c1bb0(FUN_0042f510(), DAT_006147b4, 3);
        FUN_00426670(FUN_0042f510());
        if (FUN_004c1a00(FUN_0042f510())) {
          FUN_00410b30();
          FUN_004c19f0(FUN_0042f510());
        }
        FUN_004266b0(FUN_0042f510());
      }
      if (FUN_004c1a00(FUN_004671a0(-1))) {
        FUN_00433f40();
        FUN_004c19f0(FUN_004671a0(-1));
      }
    }
    break;
  case 2:
    DAT_007719c8 = 4;
    break;
  case 3:
  case 7:
    if (DAT_007719c8 != 0) {
      if (DAT_00771968 != 7) {
        DAT_007719c8--;
        break;
      }
    } else {
      FUN_00426030();
      FUN_00426670(FUN_004671a0(0));
      if (FUN_004c1a00(FUN_004671a0(0))) {
        FUN_00410b30();
        FUN_0040df60();
        FUN_00404320();
        FUN_004c19f0(FUN_004671a0(0));
      }
      FUN_0040de30(FUN_004671c0());
      if (FUN_004c1a00(FUN_004671c0())) {
        DAT_007d3ff8->renderStateSet(6, 0);
        DAT_007d3ff8->renderStateSet(8, 0);
        FUN_0040dfc0();
        DAT_007d3ff8->renderStateSet(6, 1);
        DAT_007d3ff8->renderStateSet(8, 1);
        FUN_004c19f0(FUN_004671c0());
      }
      FUN_0040df20(FUN_004671c0());
      FUN_004266b0(FUN_004671a0(0));
    }
    if (DAT_00771968 == 7) {
      if (FUN_004c1a00(FUN_004671a0(0))) {
        FUN_00433f40();
        FUN_004c19f0(FUN_004671a0(0));
      }
    }
    break;
  case 5:
  case 6:
    if (DAT_007719c8 == 0) {
      FUN_00426670(FUN_004671a0(0));
      if (FUN_004c1a00(FUN_004671a0(0))) {
        FUN_00410b30();
        FUN_004c19f0(FUN_004671a0(0));
      }
      FUN_0040de30(FUN_004671c0());
      if (FUN_004c1a00(FUN_004671c0())) {
        DAT_007d3ff8->renderStateSet(6, 0);
        DAT_007d3ff8->renderStateSet(8, 0);
        FUN_0040dfc0();
        DAT_007d3ff8->renderStateSet(6, 1);
        DAT_007d3ff8->renderStateSet(8, 1);
        FUN_004c19f0(FUN_004671c0());
      }
      FUN_0040df20(FUN_004671c0());
      if (FUN_004c1a00(FUN_004671a0(0))) {
        FUN_0043c5b0();
        FUN_004c19f0(FUN_004671a0(0));
      }
      FUN_004266b0(FUN_004671a0(0));
    }
    break;
  case 4:
    FUN_00492e60();
    if (FUN_004c1a00(FUN_004671a0(0))) {
      FUN_0043c5b0();
      if (FUN_0042b930() == 0x20) {
        FUN_00430b90();
      }
      FUN_00433f40();
      FUN_004c19f0(FUN_004671a0(0));
    }
    break;
  }
  DAT_007719ac = FUN_004950b0() - t0;
  t1 = FUN_004950b0();
  FUN_00498bf0();
  FUN_004c1be0(FUN_004671a0(0), 0, 1);
  DAT_007719b0 = FUN_004950b0() - t1;
  for (i = 0; i < 96; i++) {
    FUN_00492440(&DAT_00868320[i]);
  }
  DAT_00771998 = DAT_007719a0;
  DAT_007719a4++;
  DAT_0077198c++;
  return 1;
}
