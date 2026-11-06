// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462760 (h6): 100.0% (regs 100%). Raw 42.9%. int[4] table indexed by an int counter (a
   pointer loop compares unsigned, jb instead of jl), four early `return 1` tests. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBALA(int, DAT_0069045c, 4, 0x0069045c);
int FUN_005a89a0(int p0);
int FUN_005a89b0(int p0);
// MATCH: 0x00462760 FUN_00462760
int FUN_00462760(void)
{
  int i;

  for (i = 0; i < 4; i++) {
    if (DAT_0069045c[i] == 0) {
      return 1;
    }
    if (FUN_005a89b0(DAT_0069045c[i]) == 0) {
      return 1;
    }
    if (FUN_005a89b0(DAT_0069045c[i]) == 2) {
      return 1;
    }
    if (FUN_005a89a0(DAT_0069045c[i]) == 2) {
      return 1;
    }
  }
  return 0;
}
