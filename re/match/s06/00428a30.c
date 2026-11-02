// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428a30 by hand (h7): 100%, whole symbol byte-identical. The colour's r/g/b = 0 stores are
   written in BOTH branches (the compiler hoists them above the branch, in b,g,r order): that gives the EBX
   zero register (one shared store before the if: 70.8%, no zero register). Colours are 4-byte structs, the
   white one passed by value. "Jun 14 2004" / "11:39:38" are the literal __DATE__/__TIME__ strings of the
   original build (0x005cd67c / 0x005cd688). /GS cookie and __security_check_cookie tail come from /GS (char[128]). */
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
typedef void (*MatchFn_20)(int, int);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067d978, 0x0067d978);
MGLOBAL(unsigned int, DAT_0067d858, 0x0067d858);
MGLOBAL(int, DAT_0067d960, 0x0067d960);
MGLOBALA(char, s_Toast_Toast_Toast_Toast_Toast_To_005f6560, 79, 0x005f6560);
int FUN_00427c90(void);
void FUN_00427e00(int p0, float p1, float p2, MatchRGBA p3, float p4, int p5);
float FUN_00428320(char *p0, float p1);
void FUN_00428450(int p0, int p1);
void FUN_004288a0(void);
int FUN_004671a0(int p0);
unsigned int FUN_004950b0(void);
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
// MATCH: 0x00428a30 FUN_00428a30
void FUN_00428a30(void)
{
  MatchRGBA col;
  MatchRGBA white;
  char buf[128];

  if (FUN_00427c90() == 0) {
    col.r = 0;
    col.g = 0;
    col.b = 0;
    col.a = 0xff;
    FUN_004c1bb0(FUN_004671a0(0), &col, 3);
    if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
      (*(MatchFn_20 *)(DAT_007d3ff8 + 0x20))(0x14, 1);
      FUN_00428450(0x20, -0x20);
      FUN_004c19f0(FUN_004671a0(0));
    }
  }
  else {
    white.r = 0xff;
    white.g = 0xff;
    white.b = 0xff;
    white.a = 0xff;
    col.r = 0;
    col.g = 0;
    col.b = 0;
    col.a = 0xff;
    if (DAT_0067d978 == 0) {
      DAT_0067d978 = 1;
      DAT_0067d858 = FUN_004950b0();
    }
    if (FUN_004950b0() - DAT_0067d858 > 24000000) {
      DAT_0067d960 = 1;
    }
    FUN_004c1bb0(FUN_004671a0(0), &col, 3);
    FUN_004a2b60(buf, "Build date : %s, %s\n", "Jun 14 2004", "11:39:38");
    if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
      FUN_00428320(strrchr(s_Toast_Toast_Toast_Toast_Toast_To_005f6560, 'T'), 1.0f);
      (*(MatchFn_20 *)(DAT_007d3ff8 + 0x20))(0x14, 1);
      FUN_004288a0();
      FUN_00428450(0x20, -0x20);
      FUN_00427e00(0x222, 580.0f, 140.0f, white, 0.9f, 1);
      FUN_004c19f0(FUN_004671a0(0));
    }
  }
  FUN_004c1be0(FUN_004671a0(0), 0, 1);
}
