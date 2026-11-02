// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c510 (h7): 84.2% (regs 84.2%, ops 99.5%), 215/215 ins, control flow identical. Raw g2s 38.4%.
   Hand: the 0x0067ebc8 block is one struct (a, b, int[20], c) so the three scalar stores stay ahead of the
   rep stosd; DAT_0067ea08++ after the clears puts xor eax,eax first; globals read directly (the compiler reloads
   0x0067e9fc / 0x0067ea08 after the calls). Residual: the original loads `total` (edi) from the spilled count slot
   `mov edi,[esp+0xc]` after the first wrap; `total = count` makes the compiler coalesce count into edi (80.5%,
   inc edx moves and the last wrap tail-merges), so this file writes `total = 1`. Same behaviour: total is only
   tested != 0 and count is 2 or 14. Tried: count as if/else, ternary both ways, int[1], unsigned count/total,
   declaration orders, init before/after the wrap, for(total = count;;), do-while nesting, permuter x2 (no gain).
   Class (b): a source form for the count/total pair not found. */
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
MGLOBAL(int, DAT_0067ea08, 0x0067ea08);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
typedef struct S_0067ebc8 {
  int a;
  int b;
  int arr[20];
  int c;
} S_0067ebc8;
MGLOBAL(S_0067ebc8, DAT_0067ebc8, 0x0067ebc8);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(int, DAT_005f76cc, 7, 0x005f76cc);
MGLOBALA(int, DAT_005f76e8, 7, 0x005f76e8);
MGLOBALA(int, DAT_005f7704, 7, 0x005f7704);
MGLOBALA(int, DAT_005f7790, 7, 0x005f7790);
MGLOBALA(int, DAT_005f77ac, 7, 0x005f77ac);
MGLOBALA(int, DAT_005f77c8, 7, 0x005f77c8);
MGLOBALA(int, DAT_005f77e4, 7, 0x005f77e4);
MGLOBALA(int, DAT_005f7800, 7, 0x005f7800);
MGLOBALA(int, DAT_005f781c, 7, 0x005f781c);
MGLOBALA(int, DAT_005f7838, 7, 0x005f7838);
MGLOBALA(int *, PTR_DAT_005f7854, 14, 0x005f7854);
MGLOBALA(int *, PTR_DAT_005f78ac, 2, 0x005f78ac);
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
// MATCH: 0x0042c510 FUN_0042c510
void FUN_0042c510(void)
{
  int count;
  int skip;
  int *p;
  int total;
  int i;

  DAT_0067ebc8.a = 0;
  DAT_0067ebc8.c = 0;
  DAT_0067ebc8.b = 0;
  for (i = 0; i < 20; i++) {
    DAT_0067ebc8.arr[i] = 0;
  }
  DAT_0067ea08++;
  count = (DAT_0067e9fc == 2) ? 2 : 14;
  if (DAT_0067ea08 >= count) {
    DAT_0067ea08 = 0;
  }
  total = 1; /* original: total = count (2 or 14); see header */
  for (;;) {
    skip = 0;
    if (DAT_0067e9fc == 2) {
      p = PTR_DAT_005f78ac[DAT_0067ea08];
    } else {
      p = PTR_DAT_005f7854[DAT_0067ea08];
    }
    if (DAT_007f0fd0 == 0) {
      if (p == DAT_005f76e8 || p == DAT_005f77ac) {
        skip = 1;
      }
    } else if (DAT_007f0fd0 == 4) {
      if (p == DAT_005f76e8 || p == DAT_005f77ac) {
        skip = 1;
      }
      if (p == DAT_005f76cc || p == DAT_005f7790) {
        skip = 1;
      }
      if (p == DAT_005f7704 || p == DAT_005f77c8) {
        skip = 1;
      }
    }
    if (DAT_0067e9fc != 6 && DAT_0067e9fc != 2) {
      if (p == DAT_005f7800 || p == DAT_005f77ac) {
        skip = 1;
      }
      if (p == DAT_005f77c8 || p == DAT_005f77e4) {
        skip = 1;
      }
      if (p == DAT_005f7790 || p == DAT_005f781c) {
        skip = 1;
      }
      if (p == DAT_005f7838) {
        skip = 1;
      }
    }
    if (skip) {
      DAT_0067ea08++;
    }
    if (DAT_0067ea08 == count) {
      DAT_0067ea08 = 0;
    }
    if (!skip) {
      if (DAT_0067e9fc == 2) {
        return;
      }
      while (*p != -1 && *p != 3 && *p != 2) {
        p++;
      }
      if (*p == 3 || *p == 2) {
        p++;
        total = 0;
        for (i = 0; i < 4; i++) {
          if (*p == 0) total += FUN_00423b40(i);
          if (*p == 1) total += FUN_00423b60(i);
          if (*p == 4) total += FUN_00423cc0(i);
          if (*p == 6) total += FUN_00423dd0(i);
          if (*p == 2) total += FUN_00423bc0(i);
          if (*p == 3) total += FUN_00423c40(i);
          if (*p == 5) total += FUN_00423d50(i);
          if (*p == 7) total += FUN_00423b80(i);
          if (*p == 8) total += FUN_00423ba0(i);
          if (*p == 0xb) total += FUN_00423f60(i);
          if (*p == 0xd) total += FUN_00424070(i);
          if (*p == 9) total += FUN_00423e60(i);
          if (*p == 10) total += FUN_00423ee0(i);
          if (*p == 0xc) total += FUN_00423ff0(i);
        }
      }
      if (total != 0) {
        return;
      }
      DAT_0067ea08++;
      if (DAT_0067ea08 == count) {
        DAT_0067ea08 = 0;
      }
    }
  }
}
