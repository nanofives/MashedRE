// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045ca30 (h2): 100% (byte-identical whole symbol, 118 B). Ghidra's size (109 B) stops before the outer
   loop's cmp/jl/ret, so match.py reports 91.7% on the Ghidra range plus 3 trailing instructions; `--bytes` prints
   BYTE-IDENTICAL. Plain 6x6 float matrix walk with three banded if/else-if tests. */
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
typedef float Row6[6];
MGLOBAL(float, DAT_005cc34c, 0x005cc34c);
MGLOBAL(float, DAT_005cca00, 0x005cca00);
MGLOBALA(Row6, DAT_0088f5e0, 6, 0x0088f5e0);
// MATCH: 0x0045ca30 FUN_0045ca30
void FUN_0045ca30(void)
{
  int i;
  int j;

  for (i = 0; i < 6; i++) {
    for (j = 0; j < 6; j++) {
      if (DAT_0088f5e0[i][j] > DAT_005cc34c)
        DAT_0088f5e0[i][j] = 0.0f;
      else if (DAT_0088f5e0[i][j] <= DAT_005cc34c && DAT_0088f5e0[i][j] > DAT_005cca00)
        DAT_0088f5e0[i][j] = -1.0f;
      else if (DAT_0088f5e0[i][j] <= DAT_005cca00)
        DAT_0088f5e0[i][j] = -2.0f;
    }
  }
}
