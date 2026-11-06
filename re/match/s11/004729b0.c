// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004729b0 (h7): 100% (whole symbol byte-identical). Draws a curve as Im3D line segments for t in
   [0.005, 1] step 0.005 (0x5cd0ec, 0x5cc320): two FUN_005459c0 evaluations, two 0x24-byte vertices with
   (a<<24)|(r<<16)|(g<<8)|b colours from the two byte quads, FUN_004cd070(v, 2, 0, 2), FUN_004cd430(0, 1),
   FUN_004cd140(). Step/limit as float literals (same as 0x004728f0). Residual: none. */
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
typedef struct { float x, y, z; } V3_004729b0;
typedef struct {
    float x, y, z;          /* 0x00 */
    float nx, ny, nz;       /* 0x0c */
    unsigned int color;     /* 0x18 */
    float u, v;             /* 0x1c */
} Im3DVertex_004729b0;      /* 0x24 */
V3_004729b0 *FUN_005459c0(void *curve, int n, float t, V3_004729b0 *pos, V3_004729b0 *tan);
void *FUN_004cd070(void *verts, int n, void *matrix, unsigned int flags);
int FUN_004cd430(int i0, int i1);
int FUN_004cd140(void);
// MATCH: 0x004729b0 FUN_004729b0
void FUN_004729b0(void *param_1,unsigned char *param_2,unsigned char *param_3)
{
  float t;
  V3_004729b0 a, b, tan;
  Im3DVertex_004729b0 v[2];
  t = 0.005f;
  do {
    FUN_005459c0(param_1, 10, t - 0.005f, &a, &tan);
    FUN_005459c0(param_1, 10, t, &b, &tan);
    v[0].color = ((unsigned int)param_2[3] << 24) | ((unsigned int)param_2[0] << 16) | ((unsigned int)param_2[1] << 8) | (unsigned int)param_2[2];
    v[0].x = b.x;
    v[0].y = b.y;
    v[0].z = b.z;
    v[1].color = ((unsigned int)param_3[3] << 24) | ((unsigned int)param_3[0] << 16) | ((unsigned int)param_3[1] << 8) | (unsigned int)param_3[2];
    v[1].x = a.x;
    v[1].y = a.y;
    v[1].z = a.z;
    if (FUN_004cd070(v, 2, 0, 2)) {
      FUN_004cd430(0, 1);
      FUN_004cd140();
    }
    t = t + 0.005f;
  } while (t <= 1.0f);
}
