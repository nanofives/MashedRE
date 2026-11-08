// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched.
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

#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef struct { int x; int y; int z; } V3;
typedef struct { uchar r; uchar g; uchar b; uchar a; } RGBA;
typedef struct { int f0; RGBA color; int f8; V3 v; short s18; } Obj;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7260, 0x007d7260);
MGLOBAL(V3, DAT_006186bc, 0x006186bc);
MGLOBAL(undefined4, DAT_006186a4, 0x006186a4);
int FUN_004d8000(undefined4 *p0, Obj *p1);
// MATCH: 0x004e7fa0 FUN_004e7fa0
Obj *FUN_004e7fa0(void)
{
  Obj *o;
  RGBA white;

  o = (Obj *)(**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d7260 + DAT_007d3ff8), 0x30007);
  if (o == 0) {
    return 0;
  }
  o->s18 = 1;
  o->f0 = 0;
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = 0xff;
  o->color = white;
  o->f8 = 0;
  o->v = DAT_006186bc;
  FUN_004d8000(&DAT_006186a4, o);
  return o;
}
