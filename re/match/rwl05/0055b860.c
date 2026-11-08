// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055b860 (h2): hand, 100%: permuter found statement order; local V3/V3/V4 copied into the struct. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { float x, y, z, w; } V4;
typedef struct {
  int a0;
  V3 p;
  float f4;
  V3 q;
  V4 r;
  float f12, f13, f14;
  int pad[3];
  float f18;
} S;
// MATCH: 0x0055b860 FUN_0055b860
void FUN_0055b860(S *param_1,int param_2)
{
  V3 a;
  V3 b;
  V4 c;

  a.x = 0.0f; a.y = 0.0f; a.z = 0.0f;
  b.x = 1.0f; b.y = 1.0f; b.z = 1.0f;
  c.x = 0.0f; c.y = 0.0f; c.z = 0.0f; c.w = 1.0f;
  param_1->a0 = param_2;
  param_1->p = a;
  param_1->f13 = 0.01f;
  param_1->f14 = 0.01f;
  param_1->f4 = 1.0f;
  param_1->f12 = 1.0f;
  param_1->q = b;
  param_1->r = c;
  param_1->f18 = 0.0f;
}
