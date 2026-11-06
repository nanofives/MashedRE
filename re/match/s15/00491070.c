// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00491070 (h5, by hand): 100% by whole symbol: Ghidra size 69 stops before the loop tail; match.py --bytes reports the 78 B obj symbol byte-identical (relocations masked). Index form v[j + i * 0xe0] gives the +0x1d base the original uses; a walking pointer did not. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct RainVert { unsigned char pad[0x1c]; unsigned char c0, c1, c2, c3; } RainVert;
MGLOBAL(RainVert *, DAT_00771530, 0x00771530);
MGLOBAL(unsigned char, DAT_00616030, 0x00616030);
MGLOBAL(unsigned char, DAT_00616031, 0x00616031);
MGLOBAL(unsigned char, DAT_00616032, 0x00616032);
MGLOBAL(unsigned char, DAT_00616033, 0x00616033);
// MATCH: 0x00491070 FUN_00491070
void FUN_00491070(void)
{
  RainVert *v;
  int i, j;
  v = DAT_00771530;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 0xe0; j++) {
      v[j + i * 0xe0].c0 = DAT_00616030;
      v[j + i * 0xe0].c1 = DAT_00616032;
      v[j + i * 0xe0].c2 = DAT_00616031;
      v[j + i * 0xe0].c3 = DAT_00616033;
    }
  }
}
