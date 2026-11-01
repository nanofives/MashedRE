// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c7c0 (h7): 100% by bytes. match.py shows 95.9% only because Ghidra size 400 stops at 0x0042c950,
   before the last epilogue; `match.py --bytes` reports the 409 B obj symbol BYTE-IDENTICAL (relocations masked).
   Raw g2s 19.1%. Hand: 4 x 16-byte records at 0x007f1a14 in a counted loop (the compiler unrolls it),
   -1 init order a, c, b, three copy segments with a running int length, first return writes the first length.
   What moved it: `d = FUN_00427780(..); dst = d; d++;` (dst taken from the walking pointer) put dst in ebp with
   the param_1 slot as its spill (58.9% -> 95.9%). Residual: none. */
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
typedef struct Rec_007f1a14 {
  int f0;
  int team;
  int name;
  int fc;
} Rec_007f1a14;
MGLOBALA(Rec_007f1a14, DAT_007f1a14, 4, 0x007f1a14);
short *FUN_00427780(int p0);
// MATCH: 0x0042c7c0 FUN_0042c7c0
void FUN_0042c7c0(int param_1)
{
  int n;
  int i;
  int a;
  int b;
  int c;
  short *dst;
  short *d;
  short *s;
  int k;
  int len;

  a = -1;
  c = -1;
  b = -1;
  n = 0;
  for (i = 0; i < 4; i++) {
    if (DAT_007f1a14[i].team == param_1) {
      if (n == 0) {
        a = DAT_007f1a14[i].name;
      } else if (n == 1) {
        b = DAT_007f1a14[i].name;
      } else if (n == 2) {
        c = DAT_007f1a14[i].name;
      }
      n++;
    }
  }
  d = FUN_00427780(param_1 + 0xa4);
  dst = d;
  d++;
  s = FUN_00427780(a + 0x79);
  n = *s++;
  len = n;
  for (k = n; k != 0; k--) {
    *d++ = *s++;
  }
  if (b == -1) {
    *dst = (short)n;
    return;
  }
  *d++ = 0x2f;
  s = FUN_00427780(b + 0x79);
  n = *s++;
  len = len + n + 1;
  for (; n != 0; n--) {
    *d++ = *s++;
  }
  if (c == -1) {
    *dst = (short)len;
    return;
  }
  *d++ = 0x2f;
  s = FUN_00427780(c + 0x79);
  n = *s++;
  len = len + n + 1;
  for (; n != 0; n--) {
    *d++ = *s++;
  }
  *dst = (short)len;
}
