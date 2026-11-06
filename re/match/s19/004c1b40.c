// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1b40 (h2 hand form, h6): 92.7%, regs 95.1%, cxx ok. RwCameraFrustumTestSphere shape: typed camera/plane/sphere, dot as y,x,z accumulate (the RwV3dDotProduct macro form computes x first: 85.4). /G5 (h6) took it from 76.2 to 92.7 (fixes and eax,0x4100 and the loop pad). Residual (b): original loads cam into eax and does lea ecx,[eax+0x94] (ours mov ecx/add ecx), and dec esi is scheduled before the x product. Tried: do-while/for counters, unsigned counter, &planes[0], RW typed frustum plane, operand orders, permuter 166 compiles. */
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
typedef struct { float x, y, z; } RwV3d_t;
typedef struct { RwV3d_t normal; float distance; int flags; } Plane_t;
typedef struct { RwV3d_t center; float radius; } Sphere_t;
typedef struct { char pad[0x94]; Plane_t planes[6]; } Cam_t;
// MATCH: 0x004c1b40 FUN_004c1b40
int FUN_004c1b40(Cam_t *cam, Sphere_t *sphere)
{
  int result = 2;
  Plane_t *p = cam->planes;
  int n = 6;
  while (n--) {
    float d;
    d = p->normal.y * sphere->center.y;
    d += p->normal.x * sphere->center.x;
    d += p->normal.z * sphere->center.z;
    d -= p->distance;
    if (d > sphere->radius) {
      return 0;
    }
    if (d > -sphere->radius) {
      result = 1;
    }
    p++;
  }
  return result;
}
