// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d4a0 (h5, by hand): 100.0% (regs 100.0%). float[2] scale + int[4] rect locals; short callees divided by 4 and 2; FUN_004671a0 takes 1 arg (the rect pointer pushed first is FUN_004c7760's third arg). Residual: none. */
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
MGLOBAL(int, DAT_0067f18c, 0x0067f18c);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
int FUN_004671a0(int p0);
int FUN_004c1c80(int p0, float * p1);
int FUN_004c7760(int p0, int p1, int * p2);
// MATCH: 0x0042d4a0 FUN_0042d4a0
void FUN_0042d4a0(void)
{
  float scale[2];
  int rect[4];

  rect[0] = FUN_0042b8b0() / 4;
  rect[1] = FUN_0042b8c0() / 4;
  rect[2] = FUN_0042b8b0() / 2;
  rect[3] = FUN_0042b8c0() / 2;
  scale[0] = 0.8f;
  scale[1] = 0.8f;
  FUN_004c1c80(DAT_0067f18c, scale);
  FUN_004c7760(*(int *)(DAT_0067f18c + 0x60), *(int *)(FUN_004671a0(-1) + 0x60), rect);
  FUN_004c7760(*(int *)(DAT_0067f18c + 100), *(int *)(FUN_004671a0(-1) + 100), rect);
}
