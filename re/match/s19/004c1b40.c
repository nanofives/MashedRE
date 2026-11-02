// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c1b40 (h2): 76.2%, regs 78.6%. Hand: typed camera/plane/sphere, dot as y,x,z accumulate. Residual (a): original uses and eax,0x4100 where 9176 emits test ah,0x41 for the same fcom (tried >, !(<=), swapped operands), no loop-align pad in the original, lea ecx,[eax+0x94] vs add. */
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
