// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00424b80 (815 B): best 88.7% (regs 85.7%, ops 100%), instruction count equal (230/230). By hand (h8),
   raw converter 43.5%. What moved it: the 0x138-byte record array at 0x008994c0 (4 entries; this file zeroes
   +0x08/+0x0c/+0x10/+0xac/+0x104/+0x108/+0x130/+0x134, memset intrinsic for the 16 bytes at +0x10c, increments +0x00,
   +0xf4, +0xf8); `!=` with the zero branch second (the original falls through to the non-zero branch); literal
   0.0f (the compiler's own 0.0 constant is the one at 0x005d757c); the first max value held in r0; separate
   temporaries for the second (min) block. Tried without effect: declaration order, dead initial stores, compare
   spellings (a < b, b > mx, mx < b, ternary), merged/split temporaries, two permuter runs (its 88.3% variant read an
   uninitialised value and was rejected). Residual (b): stack-slot assignment of the float temporaries and the mode
   local (ours one slot off), and the compiler compares the next value against the copied-from variable (fcomp st(2),
   fcomp [a2]) where the original compares against the max/min copy (fcomp st(1), fcomp [mn]). */
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
typedef struct S00424b80 {
  int f_000;
  int f_004;
  int f_008;
  int f_00c;
  int f_010;
  char pad_014[0x98];
  int f_0ac;
  char pad_0b0[0x44];
  int f_0f4;
  int f_0f8;
  char pad_0fc[0x8];
  int f_104;
  int f_108;
  int f_10c[4];
  char pad_11c[0x14];
  int f_130;
  int f_134;
} S00424b80;
MGLOBALA(S00424b80, DAT_008994c0, 4, 0x008994c0);
MGLOBAL(int, DAT_00899560, 0x00899560);
MGLOBAL(int, DAT_00899698, 0x00899698);
MGLOBAL(int, DAT_008997d0, 0x008997d0);
MGLOBAL(int, DAT_00899908, 0x00899908);
MGLOBAL(int, DAT_008995b0, 0x008995b0);
MGLOBAL(int, DAT_008996e8, 0x008996e8);
MGLOBAL(int, DAT_00899820, 0x00899820);
MGLOBAL(int, DAT_00899958, 0x00899958);
MGLOBAL(int, DAT_008995b4, 0x008995b4);
MGLOBAL(int, DAT_008996ec, 0x008996ec);
MGLOBAL(int, DAT_00899824, 0x00899824);
MGLOBAL(int, DAT_0089995c, 0x0089995c);
MGLOBAL(int, DAT_008995b8, 0x008995b8);
MGLOBAL(int, DAT_008996f0, 0x008996f0);
MGLOBAL(int, DAT_00899828, 0x00899828);
MGLOBAL(int, DAT_00899960, 0x00899960);
MGLOBAL(int, DAT_008995bc, 0x008995bc);
MGLOBAL(int, DAT_008996f4, 0x008996f4);
MGLOBAL(int, DAT_0089982c, 0x0089982c);
MGLOBAL(int, DAT_00899964, 0x00899964);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
int FUN_0040b6d0(int p0);
int FUN_0040e340(void);
int FUN_0042f500(void);
void FUN_004987b0(const char *fmt, ...);
// MATCH: 0x00424b80 FUN_00424b80
void FUN_00424b80(void)
{
  int mode;
  int i;
  int r;
  float a, b, c, d, mx, s;
  float r0, r1, r2, r3;
  float a2, b2, c2, d2, mn;

  mode = FUN_0040e340();
  for (i = 0; i < 4; i++) {
    DAT_008994c0[i].f_008 = 0;
    DAT_008994c0[i].f_00c = 0;
    DAT_008994c0[i].f_010 = 0;
    DAT_008994c0[i].f_0ac = 0;
    DAT_008994c0[i].f_104 = 0;
    DAT_008994c0[i].f_108 = 0;
    DAT_008994c0[i].f_000++;
    memset(DAT_008994c0[i].f_10c, 0, sizeof(DAT_008994c0[i].f_10c));
    DAT_008994c0[i].f_130 = 0;
    DAT_008994c0[i].f_134 = 0;
    r = FUN_0040b6d0(i);
    if (r == 0) {
      DAT_008994c0[i].f_0f8++;
    }
    if (mode == 4) {
      if (r == 10 || r == 11) {
        DAT_008994c0[i].f_0f4++;
      }
    } else if (mode == 3 || mode == 2) {
      if (r == 7) {
        DAT_008994c0[i].f_0f4++;
      }
    }
    if (FUN_0042f500() != 0 && mode == 4 && r == 7) {
      DAT_008994c0[i].f_0f4++;
    }
  }
  r0 = (float)DAT_00899560;
  mx = r0;
  r1 = (float)DAT_00899698;
  if (r1 > mx) mx = r1;
  r2 = (float)DAT_008997d0;
  if (r2 > mx) mx = r2;
  d = (float)DAT_00899908;
  if (d > mx) mx = d;
  if (mx != 0.0f) {
    s = DAT_005cc320 / mx;
    r0 = s * r0;
    r1 = r1 * s;
    r2 = r2 * s;
    r3 = s * d;
  } else {
    r2 = 0.0f;
    r1 = 0.0f;
    r0 = 0.0f;
    r3 = 0.0f;
  }
  a2 = (float)DAT_008995b0;
  mn = a2;
  b2 = (float)DAT_008996e8;
  if (b2 < mn && DAT_008996e8 != 0) mn = b2;
  c2 = (float)DAT_00899820;
  if (c2 < mn && DAT_00899820 != 0) mn = c2;
  d2 = (float)DAT_00899958;
  if (d2 < mn && DAT_00899958 != 0) mn = d2;
  if (mn != 0.0f) {
    if (DAT_008995b0 != 0) r0 = DAT_005cc320 / (a2 / mn);
    if (DAT_008996e8 != 0) r1 = DAT_005cc320 / (b2 / mn);
    if (DAT_00899820 != 0) r2 = DAT_005cc320 / (c2 / mn);
    if (DAT_00899958 != 0) r3 = DAT_005cc320 / (d2 / mn);
  } else {
    r3 = 0.0f;
    r2 = 0.0f;
    r1 = 0.0f;
    r0 = 0.0f;
  }
  FUN_004987b0("cleanest line      %.2f   %.2f   %.2f   %.2f\n", (double)r0, (double)r1, (double)r2, (double)r3);
  FUN_004987b0("number of pants    %d      %d      %d      %d\n", DAT_008995b8, DAT_008996f0, DAT_00899828,
               DAT_00899960);
  FUN_004987b0("number of crowns   %d      %d      %d      %d\n", DAT_008995b4, DAT_008996ec, DAT_00899824,
               DAT_0089995c);
  FUN_004987b0("turbo effective    %d      %d      %d      %d\n", DAT_008995bc, DAT_008996f4, DAT_0089982c,
               DAT_00899964);
}
