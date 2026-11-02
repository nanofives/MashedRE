// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428590 by hand (h7): 100%. Colour = 4-byte struct {0,0,0,0xff} passed by address;
   0x3fc90fdb stored to DAT_0067d974 is 1.5707964f. */
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
MGLOBAL(float, DAT_0067d974, 0x0067d974);
void FUN_00428450(int p0, int p1);
int FUN_004671a0(int p0);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
void FUN_004c1bb0(int p0, MatchRGBA *p1, int p2);
void FUN_004c1be0(int p0, int p1, int p2);
// MATCH: 0x00428590 FUN_00428590
int FUN_00428590(void)
{
  MatchRGBA col;

  col.r = 0;
  col.g = 0;
  col.b = 0;
  col.a = 0xff;
  FUN_004c1bb0(FUN_004671a0(0), &col, 3);
  if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
    DAT_0067d974 = 1.5707964f;
    FUN_00428450(0x20, -0x20);
    FUN_004c19f0(FUN_004671a0(0));
  }
  FUN_004c1be0(FUN_004671a0(0), 0, 1);
  return 1;
}
