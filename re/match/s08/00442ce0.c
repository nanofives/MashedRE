// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442ce0: 100% by hand (h7). Raw 39.5%. One float local set to 0 and overwritten per (a, b) pair,
   returned as k * param_3 + 1.0f; the compiler tail-duplicates the multiply-add into each branch.
   The mode test is three separate `if (m == N) return` statements: `a || b || c`, a switch, an inverted body and a
   single-exit local all place the early return block at the end (80-91%). An __inline helper also gives 100%.
   MATCH-S08-FU: the .rdata floats _DAT_005cc320 (1.0), DAT_005d757c (0.0), _DAT_005cc318 (0.6), _DAT_005cc32c (0.5)
   and _DAT_005cc564 (0.25) are written as literals, because the exe's copy of the original .rdata reads 0. */
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_008989c8, 0x008989c8);
MGLOBALA(float, DAT_008989b0, 4, 0x008989b0);
int FUN_0040e340(void);
int FUN_0046dbe0(int p0);
// MATCH: 0x00442ce0 FUN_00442ce0
float FUN_00442ce0(int param_1, int param_2, float param_3)
{
  float k;
  int a;
  int b;

  if (DAT_007f0fd0 == 4) return 1.0f;
  if (DAT_007f0fd0 == 9) return 1.0f;
  if (DAT_007f0fd0 == 8) return 1.0f;
  k = 0.0f;
  FUN_0040e340();
  if (DAT_008989b0[param_1] == 0.0f) {
    a = FUN_0046dbe0(param_1);
    b = FUN_0046dbe0(DAT_008989c8);
    if (a == 0) {
      if (b == 3) {
        k = 0.6f;
      } else if (b == 2) {
        k = 0.5f;
      } else if (b == 1) {
        k = 0.25f;
      }
    } else if (a == 1) {
      if (b == 3) {
        k = 0.5f;
      } else if (b == 2) {
        k = 0.25f;
      }
    } else if (a == 2 && b == 3) {
      k = 0.25f;
    }
  }
  return k * param_3 + 1.0f;
}
