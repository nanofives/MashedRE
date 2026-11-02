// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428bf0 by hand (h7): 100%, whole symbol byte-identical. Same shape as 0x00428a30's second branch:
   4-byte colour structs (white by value), literal build-date strings, /GS buffer char[128]. */
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
MGLOBAL(int *, DAT_00771964, 0x00771964);
MGLOBALA(char, s_Toast_Toast_Toast_Toast_Toast_To_005f6560, 79, 0x005f6560);
int FUN_00427c90(void);
void FUN_00427e00(int p0, float p1, float p2, MatchRGBA p3, float p4, int p5);
float FUN_00428320(char *p0, float p1);
void FUN_00428450(int p0, int p1);
void FUN_00428760(int *p0, float p1, float p2, float p3, float p4, int p5);
int FUN_004671a0(int p0);
int FUN_004a2b60(char *buf, const char *fmt, ...);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
void FUN_004c1bb0(int p0, MatchRGBA *p1, int p2);
void FUN_004c1be0(int p0, int p1, int p2);
#ifdef __cplusplus
extern "C" {
#endif
char *strrchr(const char *, int);
#ifdef __cplusplus
}
#endif
// MATCH: 0x00428bf0 FUN_00428bf0
void FUN_00428bf0(void)
{
  MatchRGBA white;
  MatchRGBA col;
  char buf[128];

  if (FUN_00427c90() != 0) {
    white.r = 0xff;
    white.g = 0xff;
    white.b = 0xff;
    white.a = 0xff;
    col.a = 0xff;
    col.r = 0;
    col.g = 0;
    col.b = 0;
    FUN_004c1bb0(FUN_004671a0(0), &col, 3);
    FUN_004a2b60(buf, "Build date : %s, %s\n", "Jun 14 2004", "11:39:38");
    if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
      FUN_00428320(strrchr(s_Toast_Toast_Toast_Toast_Toast_To_005f6560, 'T'), 1.0f);
      FUN_00428760(DAT_00771964, 320.0f, 260.0f, 480.0f, 240.0f, 2);
      FUN_00428450(0x20, -0x20);
      FUN_00427e00(0x222, 580.0f, 140.0f, white, 0.75f, 1);
      FUN_004c19f0(FUN_004671a0(0));
    }
    FUN_004c1be0(FUN_004671a0(0), 0, 1);
  }
}
