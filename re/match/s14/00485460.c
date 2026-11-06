// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00485460 (h2): 100% (raw 50.0%). Moved: Vec3 v[2] local array with two struct copies
   (v[0] = m[1], v[1] = m[0]) instead of six scalar locals. */
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
typedef struct Vec3 { float x, y, z; } Vec3;
typedef struct Obj485460 {
  int f00;
  char *frame;
} Obj485460;
void FUN_00451730(Vec3 *m, Obj485460 *obj);
void FUN_004c3d90(Vec3 *out, Vec3 *in, int n, void *matrix);
void FUN_004c1800(Vec3 *m, Vec3 *v);
void FUN_004c1840(Vec3 *m, Vec3 *v);
// MATCH: 0x00485460 FUN_00485460
void FUN_00485460(Obj485460 *obj, Vec3 *m)
{
  Vec3 v[2];
  FUN_00451730(m, obj);
  v[0] = m[1];
  v[1] = m[0];
  FUN_004c3d90(v, v, 2, obj->frame + 0x10);
  FUN_004c1800(m, &v[0]);
  FUN_004c1840(m, &v[1]);
}
