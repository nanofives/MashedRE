// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x004324a0 (276 B): 100% (regs 100%, ops 100%), byte-identical. Raw g2s 54.2%.
   Hand rewrite of the control flow: early return on the eab0/eabc/eab4 guard, `if (DAT_007f1a0c != 0x1000)`
   first (the original lays the != branch out first), tail call to FUN_0042c280. FUN_00432450 is called with 6
   arguments here (0x00432536..0x00432548 push 6, add esp 0x18) although its body (re/match/s08/00432450.c,
   declared `(void)` there) reads none: declared here with 6 int params, cdecl. Residual class: none. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_0067eabc, 0x0067eabc);
MGLOBAL(int, DAT_0067eab4, 0x0067eab4);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
int FUN_0042b930(void);
void FUN_0042bf30(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_0042c1d0(void);
int FUN_0042c1f0(void);
int FUN_0042c220(void);
void FUN_0042c280(void);
void FUN_00432450(int p0, int p1, int p2, int p3, int p4, int p5);
// MATCH: 0x004324a0 FUN_004324a0
void FUN_004324a0(void)
{
  int r;

  if (DAT_0067eab0 != 0 && DAT_0067eabc == -0xba0000 &&
      (DAT_0067eab4 == 0x233 || DAT_0067eab4 == 0x231 || DAT_0067eab4 == 0x1c1 || DAT_0067eab4 == 0x232)) {
    return;
  }
  if (DAT_007f1a0c != 0x1000) {
    if (DAT_0067eab0 == 0) {
      FUN_0042c1d0();
      if (FUN_0042c220() != 0) {
        FUN_0042c280();
      }
    }
    else if (DAT_0067eabc != -0xdf0000 && DAT_0067eabc != -0xde0000) {
      FUN_0042c1d0();
      if (FUN_0042c220() != 0) {
        FUN_00432450(0x213, -0xdf0000, 0, 0, 0, 0);
      }
    }
  }
  else {
    r = FUN_0042b930();
    if (r != 0x21 && r != 1 && FUN_0042c1f0() != 0) {
      if (DAT_0067eab0 == 0) {
        FUN_0042bf30(0x213, -0xde0000, 0, 0, 0, 0);
      }
      else if (DAT_0067eabc != -0xde0000) {
        FUN_00432450(0x213, -0xde0000, 0, 0, 0, 0);
      }
    }
  }
}
