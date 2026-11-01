// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004295a0 (h9, by hand): 100% (regs 100%).
   Raw converter 17.6%: FUN_0040dc80 takes no args (the 4 pushes before it are FUN_00427e00's args 3-6), and the
   colour arg is a 4-byte RGBA struct by value. Initialising red before black puts red.g/red.b stores first.
   Residual: none. */
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
typedef struct { unsigned char r, g, b, a; } RGBA;
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
float FUN_0040dc80(void);
void FUN_00427e00(int p0, float p1, float p2, RGBA p3, float p4, int p5);
// MATCH: 0x004295a0 FUN_004295a0
/* [C2 2026-05-12] Renders two HUD text elements using a name/label lookup + text renderer pattern. */
void FUN_004295a0(void)
{
  RGBA black;
  RGBA red;

  red.r = 0xff;
  red.g = 0;
  red.b = 0;
  red.a = 0xff;
  black.r = 0;
  black.g = 0;
  black.b = 0;
  black.a = 0xff;
  FUN_00427e00(0xff, FUN_0040dc80() + DAT_005cc31c, 63.0f, black, 1.0f, 2);
  FUN_00427e00(0xff, FUN_0040dc80(), 60.0f, red, 1.0f, 2);
}
