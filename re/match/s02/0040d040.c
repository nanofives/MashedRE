// FLAGS: /O2 /GS /Ob1
/* 0x0040d040 FUN_0040d040 (201 B): 100.0% exact, regs 100.0%.
   Raw g2s did not compile (C2059). Hand form: param_1 = table of entry pointers (+4 field), n = f4 / 6 in each
   assignment, n *= 6 hoisted, i-indexed for over the 4 x 0x10 records at 0x007f1a1c (the compiler makes the
   pointer loop with the signed jl), res = 0 before the calls, i = 0 in the for (i = 0 at the top swaps EBX/EBP).
   Residual: none. */
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
typedef struct Rec10 { int v; int pad[3]; } Rec10;
typedef struct TrackEnt { int f0; int f4; } TrackEnt;
MGLOBALA(Rec10, DAT_007f1a1c, 4, 0x007f1a1c);

int FUN_0041efc0(int p0);
int FUN_0041f320(int p0);
int FUN_0042f6a0(void);
int FUN_00431d70(void);

// MATCH: 0x0040d040 FUN_0040d040
int FUN_0040d040(TrackEnt **param_1, int param_2)
{
  int i;
  int want;
  int n;
  int res;

  res = 0;
  n = param_1[param_2]->f4 / 6;
  if (FUN_0042f6a0() == 10) {
    n = param_1[FUN_00431d70()]->f4 / 6;
  }
  if (FUN_0042f6a0() == 6) {
    n = param_1[FUN_00431d70()]->f4 / 6;
  }
  n = n * 6;
  for (i = 0; i < 4; i++) {
    want = DAT_007f1a1c[i].v + n;
    if (FUN_0042f6a0() == 0xb) {
      want = n + i;
    }
    if (FUN_0041f320(i) == 0 || FUN_0041efc0(i) != want) {
      res = 1;
    }
  }
  return res;
}
