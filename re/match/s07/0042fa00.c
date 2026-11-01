// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042fa00: 100% by hand (raw 19.6%). The four "pad == i" tests are one inner loop
   for (j < 4) over the 16-byte records at 0x007f1a14, which the compiler unrolls completely (and fuses the last
   test into the if (f)); writing the four tests out by hand makes it unroll the OUTER loop by 3 instead. Keys are
   one char[32][0x4c] array at 0x007f1044 read at [i] and [i + 16] (as in 0x0042f7b0). */
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
typedef struct { char b[0x4c]; } Keys0042fa00;
typedef struct { int v[3]; } Opt0042fa00;
typedef struct { int pad; int f4; int f8; int fc; } Pl0042fa00;
MGLOBAL(int, DAT_0067ea88, 0x0067ea88);
MGLOBALA(Opt0042fa00, DAT_0067e938, 12, 0x0067e938);
MGLOBALA(Keys0042fa00, DAT_007f1044, 32, 0x007f1044);
MGLOBALA(Pl0042fa00, DAT_007f1a14, 4, 0x007f1a14);
// MATCH: 0x0042fa00 FUN_0042fa00
void FUN_0042fa00(void)
{
  int i;
  int j;
  int f;
  if (DAT_0067ea88 == 2) {
    DAT_0067ea88 = 0;
  }
  for (i = 0; i < 12; i++) {
    f = 0;
    for (j = 0; j < 4; j++) {
      if (DAT_007f1a14[j].pad == i) {
        f = 1;
      }
    }
    if (f) {
      if (DAT_007f1044[i].b[0] && !DAT_007f1044[i + 16].b[0] && DAT_0067e938[i].v[0] != 0) {
        DAT_0067e938[i].v[0]--;
      }
      if (DAT_007f1044[i].b[1] && !DAT_007f1044[i + 16].b[1] && DAT_0067e938[i].v[0] < 2) {
        DAT_0067e938[i].v[0]++;
      }
    }
  }
}
