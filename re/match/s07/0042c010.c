// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c010 (121 B): 100% by hand (raw 19.4%). Two things moved it:
   1. The callees FUN_0042b8b0 / FUN_0042b8c0 (7 B each, `mov ax, [g]; ret`) are defined in this file before the
      caller. The compiler then keeps the first product on the x87 stack across the second call, as the original
      does (with them extern it spills it). This file holds 0x0042b8b0 and 0x0042b8c0 (both 100% here too).
   2. FUN_00472b10 takes its two points as 8-byte structs by value (x, y): the original builds them in stack
      temps and pushes each dword through a register.
   0x005cd5a8 = 0.0015625f (1/640), 0x005cc560 = 0.0020833334f (1/480). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x; float y; } V2d;
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
// HELD: 0x0042b8b0 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}
#else
short FUN_0042b8b0(void);
#endif
// HELD: 0x0042b8c0 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}
#else
short FUN_0042b8c0(void);
#endif
void FUN_00472b10(void *p0, V2d p1, V2d p2, int p3);
// MATCH: 0x0042c010 FUN_0042c010
void FUN_0042c010(float param_1, float param_2, float param_3, float param_4, int param_5)
{
  float sx;
  float sy;
  V2d a;
  V2d b;

  sx = (float)FUN_0042b8b0() * 0.0015625f;
  sy = (float)FUN_0042b8c0() * 0.0020833334f;
  a.x = param_1 * sx;
  a.y = param_2 * sy;
  b.x = param_3 * sx;
  b.y = param_4 * sy;
  FUN_00472b10(0, a, b, param_5);
}
