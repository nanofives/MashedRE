// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cbb70 (h2): 93.3% (regs 96.7%). Raw 83.3%. Frustum/sphere test over 6 planes at camera+0x94
   (20-byte plane: normal, distance, closest bytes); dot product written z + (x + y) to get the original y, x, z
   evaluation order (any left-assoc order gives x/y/z by address); -radius hoisted into a float local (stored in the
   dead param slot as in the original); n = 6 first (permuter). Residual (b): original loads the camera into eax and
   does lea ecx,[eax+0x94]; ours loads it straight into ecx and adds. Tried typed camera struct, raw +0x94 cast,
   &planes[0], index loop, while(1) form. */
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
typedef unsigned int undefined4;
typedef struct { float x, y, z; } V3d;
typedef struct { V3d normal; float distance; unsigned char cx, cy, cz, pad; } FrustumPlane;
typedef struct { V3d center; float radius; } Sphere;
typedef struct { char pad[0x94]; FrustumPlane planes[6]; } Camera;
// MATCH: 0x004cbb70 FUN_004cbb70
undefined4 FUN_004cbb70(Camera *param_1,Sphere *param_2)
{
  const FrustumPlane *pl;
  int n;
  float nr;
  n = 6;
  pl = param_1->planes;
  nr = -param_2->radius;
  do {
    if ((pl->normal.z * param_2->center.z + (pl->normal.x * param_2->center.x + pl->normal.y * param_2->center.y)) - pl->distance > nr) return 0;
    pl++;
  } while (--n);
  return 1;
}
