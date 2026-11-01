// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428610 by hand (h7): 100%. What moved it from 18.8% (raw): x/y/w/h as ONE local struct (scalars
   go into the dead param slots, 69.8% after the permuter); the uv 0.0/1.0 values read once from DAT_005d757c /
   DAT_005cc320 into float locals lo/hi (kept on the x87 stack across both switches; literals give mov-imm
   stores); w*param_4 / h*param_5 computed into the rect before the switch and reused in case 2; case 0 and
   case 1 written as separate cases (je/dec/je/dec/jne chain); switch(param_7) case 3 falls into case 2.
   Literals: 0.0015625f (1/640, 0x005cd5a8), 1/480 (0x005cc560), 0.5f (0x005cc32c). */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
void FUN_00450b10(int p0, float p1, float p2, float p3, float p4, MatchRGBA p5, float *p6);
// MATCH: 0x00428610 FUN_00428610
void FUN_00428610(int *param_1, float param_2, float param_3, float param_4, float param_5, int param_6,
                  int param_7)
{
  float w;
  float h;
  MatchRGBA col;
  MatchRect r;
  float uv[4];
  int tex;
  float lo;
  float hi;

  r.x = 0.0f;
  r.y = 0.0f;
  col.r = 0xff;
  col.g = 0xff;
  col.b = 0xff;
  col.a = 0xff;
  w = (float)FUN_0042b8b0() * 0.0015625f;
  h = (float)FUN_0042b8c0() * (1.0f / 480.0f);
  tex = 0;
  lo = DAT_005d757c;
  uv[0] = lo;
  uv[1] = lo;
  hi = DAT_005cc320;
  uv[2] = hi;
  uv[3] = hi;
  r.w = w * param_4;
  r.h = h * param_5;
  switch (param_6) {
  case 0:
    r.x = w * param_2;
    r.y = h * param_3;
    break;
  case 1:
    r.x = w * param_2;
    r.y = h * param_3;
    break;
  case 2:
    r.x = w * param_2 - r.w * 0.5f;
    r.y = h * param_3 - r.h * 0.5f;
    break;
  }
  switch (param_7) {
  case 1:
    uv[0] = hi;
    uv[2] = lo;
    break;
  case 3:
    uv[0] = hi;
    uv[2] = lo;
  case 2:
    uv[1] = hi;
    uv[3] = lo;
    break;
  }
  if (param_1 != 0) {
    tex = *param_1;
  }
  FUN_00450b10(tex, r.x, r.y, r.w, r.h, col, uv);
}
