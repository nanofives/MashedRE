// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042f7b0: 100% by hand (raw 42.0%). match.py scores 98.0% only because Ghidra's size (281 B) stops
   before the loop's jl/ret at 0x0042f8c9..0x0042f8cf; match.py --bytes: whole obj symbol 288 B BYTE-IDENTICAL
   (relocations masked). Source: one char[32][0x4c] array at 0x007f1044 read at [i*4+k] and [i*4+k+16] (one array,
   not two: with two arrays the compiler uses an index register instead of the 0x007f1504 pointer), an int[12][3]
   at 0x0067e850, outer loop i < 3 with the k = 0..3 body written out (the compiler does not unroll a k loop). */
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
typedef struct { char b[0x4c]; } Keys0042f7b0;
typedef struct { int v[3]; } Opt0042f7b0;
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBALA(Opt0042f7b0, DAT_0067e850, 12, 0x0067e850);
MGLOBALA(Keys0042f7b0, DAT_007f1044, 32, 0x007f1044);
// MATCH: 0x0042f7b0 FUN_0042f7b0
void FUN_0042f7b0(void)
{
  int i;
  if (DAT_0067eab0 == 0) {
    for (i = 0; i < 3; i++) {
      if (DAT_007f1044[i * 4 + 0].b[0] && !DAT_007f1044[i * 4 + 0 + 16].b[0] && DAT_0067e850[i * 4 + 0].v[0] != 0) {
        DAT_0067e850[i * 4 + 0].v[0]--;
      }
      if (DAT_007f1044[i * 4 + 0].b[1] && !DAT_007f1044[i * 4 + 0 + 16].b[1] && DAT_0067e850[i * 4 + 0].v[0] < 3) {
        DAT_0067e850[i * 4 + 0].v[0]++;
      }
      if (DAT_007f1044[i * 4 + 1].b[0] && !DAT_007f1044[i * 4 + 1 + 16].b[0] && DAT_0067e850[i * 4 + 1].v[0] != 0) {
        DAT_0067e850[i * 4 + 1].v[0]--;
      }
      if (DAT_007f1044[i * 4 + 1].b[1] && !DAT_007f1044[i * 4 + 1 + 16].b[1] && DAT_0067e850[i * 4 + 1].v[0] < 3) {
        DAT_0067e850[i * 4 + 1].v[0]++;
      }
      if (DAT_007f1044[i * 4 + 2].b[0] && !DAT_007f1044[i * 4 + 2 + 16].b[0] && DAT_0067e850[i * 4 + 2].v[0] != 0) {
        DAT_0067e850[i * 4 + 2].v[0]--;
      }
      if (DAT_007f1044[i * 4 + 2].b[1] && !DAT_007f1044[i * 4 + 2 + 16].b[1] && DAT_0067e850[i * 4 + 2].v[0] < 3) {
        DAT_0067e850[i * 4 + 2].v[0]++;
      }
      if (DAT_007f1044[i * 4 + 3].b[0] && !DAT_007f1044[i * 4 + 3 + 16].b[0] && DAT_0067e850[i * 4 + 3].v[0] != 0) {
        DAT_0067e850[i * 4 + 3].v[0]--;
      }
      if (DAT_007f1044[i * 4 + 3].b[1] && !DAT_007f1044[i * 4 + 3 + 16].b[1] && DAT_0067e850[i * 4 + 3].v[0] < 3) {
        DAT_0067e850[i * 4 + 3].v[0]++;
      }
    }
  }
}
