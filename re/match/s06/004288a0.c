// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004288a0 by hand (h7): 100%. .rdata floats DAT_005cd660 (28.0), DAT_005cc32c (0.5) and
   DAT_005ccd74 (320.0) are literals. Colour = 4-byte struct by value (copied once into esi). The second x
   (x + a) needs its own local: reusing x lands one slot off (97.0%). */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct { uchar r, g, b, a; } MatchRGBA;
MGLOBAL(int *, DAT_0067d964, 0x0067d964);
MGLOBAL(int *, DAT_0067d970, 0x0067d970);
void FUN_00427e00(int p0, float p1, float p2, MatchRGBA p3, float p4, int p5);
float FUN_004282a0(int p0, float p1);
void FUN_00428760(int *p0, float p1, float p2, float p3, float p4, int p5);
// MATCH: 0x004288a0 FUN_004288a0
void FUN_004288a0(void)
{
  MatchRGBA col;
  float a;
  float b;
  float x;
  float x2;

  col.r = 0xff;
  col.g = 0xff;
  col.b = 0xff;
  col.a = 0xff;
  FUN_00428760(DAT_0067d964, 320.0f, 100.0f, 256.0f, 128.0f, 2);
  FUN_00427e00(0x1e5, 320.0f, 180.0f, col, 0.6f, 2);
  a = FUN_004282a0(0x1e6, 0.6f);
  b = FUN_004282a0(0x1e7, 0.6f);
  x = 320.0f - (b + a + 28.0f) * 0.5f;
  FUN_00427e00(0x1e6, x, 200.0f, col, 0.6f, 0);
  x2 = x + a;
  FUN_00428760(DAT_0067d970, x2, 188.0f, 24.0f, 24.0f, 0);
  FUN_00427e00(0x1e7, x2 + 28.0f, 200.0f, col, 0.6f, 0);
  FUN_00427e00(0x1e8, 320.0f, 220.0f, col, 0.6f, 2);
  FUN_00427e00(0x1e9, 320.0f, 240.0f, col, 0.6f, 2);
  FUN_00427e00(0x1ea, 320.0f, 260.0f, col, 0.6f, 2);
  FUN_00427e00(0x1eb, 320.0f, 280.0f, col, 0.6f, 2);
}
