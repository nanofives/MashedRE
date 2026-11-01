// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428450 by hand (h7): best 84.6% (regs 84.6%). The .rdata floats DAT_005cd5a8/005cc560/005cd274/
   005cd65c/005cd658/005cc730/005d757c/005cc56c are compiler literals (1/640, 1/480, 40, 512, 72, 80, 0, 0.1).
   Colour = 4-byte struct passed by value (mov al,0xff x4; push eax). x/y/w/h are one local struct (as in
   0x00428610, where the struct gives 100%): scalars land in the dead param slots (62.8%). Tried: args inline in
   the call (50%), float r[4], Rect by value (55%), scalar x + struct, x*=w split forms, declaration orders,
   permuter (r.y computed before h: 69.2 -> 84.6, frame now 0x2c).
   Residual (b) source shape: the original puts h's int->float temp in r.x's slot [esp+0x10] and computes
   x_inner, r.y, r.w, r.h, then r.x = w*x_inner, r.y = h*r.y; ours puts the temp in param_2's dead slot. No form
   found that packs a temp into an unwritten struct field. */
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
typedef struct { float x, y, w, h; } MatchRect;
MGLOBAL(float, DAT_0067d974, 0x0067d974);
MGLOBAL(int *, DAT_00771960, 0x00771960);
#ifdef __cplusplus
extern "C" {
#endif
double __cdecl sin(double);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(sin)
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
void FUN_00450b10(int p0, float p1, float p2, float p3, float p4, MatchRGBA p5, float *p6);
// MATCH: 0x00428450 FUN_00428450
void FUN_00428450(int param_1, int param_2)
{
  MatchRGBA col;
  float s;
  float w;
  float h;
  MatchRect r;
  float uv[4];
  int tex;

  s = (float)sin(DAT_0067d974);
  col.r = 0xff;
  col.g = 0xff;
  col.b = 0xff;
  col.a = 0xff;
  uv[0] = 0.0f;
  uv[1] = 0.0f;
  uv[2] = 0.5f;
  uv[3] = 1.0f;
  tex = 0;
  w = (float)FUN_0042b8b0() * 0.0015625f;
  r.y = (float)param_2 + 72.0f;
  h = (float)FUN_0042b8c0() * (1.0f / 480.0f);
  r.x = (512.0f - s * 40.0f) + (float)param_1;
  r.w = s * 80.0f * w;
  r.h = h * 80.0f;
  r.x = w * r.x;
  r.y = h * r.y;
  if (s < 0.0f) {
    uv[0] = 1.0f;
    uv[2] = 0.5f;
  }
  if (DAT_00771960 != 0) {
    tex = *DAT_00771960;
  }
  FUN_00450b10(tex, r.x, r.y, r.w, r.h, col, uv);
  DAT_0067d974 = DAT_0067d974 + 0.1f;
}
