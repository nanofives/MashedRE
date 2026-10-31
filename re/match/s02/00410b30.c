// FLAGS: /O2 /GS /Ob1
/* 0x00410b30 FUN_00410b30 (468 B): 100.0% exact, regs 100.0%.
   Raw g2s was 94.1%: Ghidra arities were wrong (0x004671a0 takes 1 arg, 0x0047b9e0 takes 3 = (FUN_004671a0(0), 0xc0, 0),
   0x0045b350 is called with 1, 2 and 0 args so it is declared unprototyped / (...)), plus the 0xc0 store split out
   of the if. Residual: none. */
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
typedef int (*RwStateFn)(int state, int value);
MGLOBAL(undefined4 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4 **, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba78, 0x0063ba78);
MGLOBAL(int, DAT_0063ba98, 0x0063ba98);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0f3c, 0x007f0f3c);
MGLOBAL(int, DAT_0063b920, 0x0063b920);
MGLOBAL(int, DAT_007f0f30, 0x007f0f30);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);

void FUN_00403910(void);
void FUN_00404650(void);
void FUN_004072e0(void);
void FUN_0040bde0(void);
int FUN_0040e350(void);
void FUN_00411ce0(void);
void FUN_00413a00(undefined4 p0);
void FUN_00413a40(void);
void FUN_004189c0(void);
void FUN_0041ebb0(undefined4 p0);
void FUN_00420050(int p0, undefined4 p1);
void FUN_004219c0(void);
void FUN_00425e40(void);
void FUN_00426640(undefined4 p0);
int FUN_00426ba0(void);
void FUN_004270f0(undefined4 p0);
void FUN_00429e10(void);
void FUN_00448730(void);
/* called here with 1, 2 and 0 arguments (cdecl) */
#ifdef __cplusplus
void FUN_0045b350(...);
#else
void FUN_0045b350();
#endif
void FUN_0045b990(void);
undefined4 FUN_004671a0(int p0);
void FUN_004725c0(void);
void FUN_00475d30(void);
void FUN_00475e50(void);
void FUN_00477810(void);
void FUN_00477a10(void);
void FUN_0047b9e0(undefined4 p0, int p1, int p2);
void FUN_004891f0(void);
void FUN_0048fd70(void);
void FUN_00490490(void);

// MATCH: 0x00410b30 FUN_00410b30
void FUN_00410b30(void)
{
  int i;

  ((RwStateFn)DAT_007d3ff8[8])(0x14, 2);
  FUN_004270f0(*PTR_PTR_005f2770[DAT_0063ba78]);
  ((RwStateFn)DAT_007d3ff8[8])(0xe, 1);
  FUN_004725c0();
  FUN_0045b350(*DAT_007d3ff8);
  if (DAT_0063ba98 == 0) {
    for (i = 0; i < 4; i++) {
      FUN_00420050(i, *DAT_007d3ff8);
    }
  }
  ((RwStateFn)DAT_007d3ff8[8])(0xe, 0);
  FUN_00411ce0();
  FUN_0041ebb0(*DAT_007d3ff8);
  for (i = 0; i < 4; i++) {
    FUN_0045b350(i, *DAT_007d3ff8);
  }
  ((RwStateFn)DAT_007d3ff8[8])(6, 1);
  ((RwStateFn)DAT_007d3ff8[8])(8, 1);
  FUN_004219c0();
  if (DAT_007f0fd0 != 5) {
    FUN_00425e40();
  }
  FUN_00426640(*PTR_PTR_005f2770[DAT_0063ba78]);
  FUN_0040bde0();
  FUN_004891f0();
  FUN_00475e50();
  FUN_00475d30();
  FUN_00477810();
  FUN_00477a10();
  FUN_004189c0();
  FUN_0048fd70();
  FUN_0045b990();
  FUN_00413a00(FUN_004671a0(0));
  FUN_00413a40();
  FUN_004072e0();
  FUN_00404650();
  if (FUN_0040e350() != 7) {
    DAT_0063b920 = 0xc0;
    if (DAT_007f0f3c != 0) {
      FUN_0047b9e0(FUN_004671a0(0), 0xc0, 0);
    }
  }
  ((RwStateFn)DAT_007d3ff8[8])(0x14, 1);
  ((RwStateFn)DAT_007d3ff8[8])(10, 5);
  ((RwStateFn)DAT_007d3ff8[8])(0xb, 6);
  FUN_00448730();
  FUN_00490490();
  if (DAT_007f0f30 != 0 && DAT_0063ba8c != 9 && DAT_0063ba8c != 10) {
    FUN_00403910();
  }
  if (FUN_00426ba0() != 0) {
    FUN_00429e10();
  }
  FUN_0045b350();
}
