// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00479100 (h5, by hand): 69.1% (regs 77.6%). Typed hit record (tri +0x0c, geom +0x34, t +0x38),
   pos/dir as V3 structs, transform written z-term first, weights summed w2*d2+w1*d1+w0*d0, clamp as a ternary
   (gives the fld/fstp store), cols==0 as an else branch (gives push ebp before push ebx and the shared tail).
   Residual (b): entry register choice (orig loads param_1 into eax, &hit into ecx), the three vertex pointers
   (orig computes v0,v1,v2 into edx,esi,eax before pushing; ours computes each at its push), and the colour loads
   scheduled into the d[2] sum in the original. Tried: param_1 as char pointer or void pointer, local order, geom as a struct,
   RpTriangle struct indexing, vertex temps (early/late), pos copy by field, early-return layout, permuter (its
   commute rewrites were semantically wrong, discarded).
   PROV-DIVERGED-18 (2026-11-11): d[0] was a VALUE difference: the DDK reassociated the one-expression form to
   (m0*x + m8*z) + m4*y + m12 where the original sums (m8*z + m4*y) + m0*x + m12 (after the call at 0x0047920a);
   d[1] and d[2] already compiled in the original order. Replay: DIVERGED@161 (block 0x0063dc00). Now one statement
   per term: 70.9% (regs 79.4%, ops 78.8%). */
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
typedef struct { float x, y, z; } V3_00479100;
typedef struct { V3_00479100 start, end; } Line_00479100;
typedef struct { unsigned char red, green, blue, alpha; } RGBA_00479100;
typedef struct { float red, green, blue, alpha; } RGBAReal_00479100;
typedef struct {
  int pad0[3];
  int tri;            /* +0x0c */
  int pad10[9];
  int geom;           /* +0x34 */
  float t;            /* +0x38 */
  int pad3c;
} Hit_00479100;
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
int FUN_004b4cd0(int param_1, Line_00479100 *param_2, Hit_00479100 *param_3);
int FUN_00559040(float *param_1, int *param_2, float *param_3, float *param_4, float *param_5);
// MATCH: 0x00479100 FUN_00479100
int FUN_00479100(int param_1, Line_00479100 *param_2, float *param_3, float *param_4)
{
  unsigned short *tri;
  int verts;
  RGBA_00479100 *cols;
  RGBA_00479100 c0, c1, c2;
  float w[3];
  int unk;
  V3_00479100 pos;
  V3_00479100 dir;
  float m[16];
  float f;
  float d[3];
  Hit_00479100 hit;

  if (FUN_004b4cd0(*(int *)(param_1 + 0x10090), param_2, &hit) == 0) {
    return 0;
  }
  if (param_4 != 0) {
    *param_4 = hit.t;
  }
  if (*(short *)(hit.geom + 0x84) == 0) {
    return 0;
  }
  if (param_3 != 0) {
    cols = *(RGBA_00479100 **)(hit.geom + 0x30);
    if (cols != 0) {
  verts = *(int *)(hit.geom + 8);
  tri = (unsigned short *)(*(int *)(hit.geom + 4) + hit.tri * 8);
  dir.x = param_2->end.x - param_2->start.x;
  dir.y = param_2->end.y - param_2->start.y;
  dir.z = param_2->end.z - param_2->start.z;
  pos = param_2->start;
  pos.x = dir.x * hit.t + pos.x;
  pos.y = dir.y * hit.t + pos.y;
  pos.z = dir.z * hit.t + pos.z;
  FUN_00559040(m, &unk, (float *)(verts + tri[0] * 0xc), (float *)(verts + tri[1] * 0xc),
               (float *)(verts + tri[2] * 0xc));
  d[0] = m[8] * pos.z; d[0] = d[0] + m[4] * pos.y; d[0] = d[0] + m[0] * pos.x; d[0] = d[0] + m[12];
  d[1] = m[9] * pos.z + m[5] * pos.y + m[1] * pos.x + m[13];
  d[2] = m[10] * pos.z + m[6] * pos.y + m[2] * pos.x + m[14];
  c0 = cols[tri[0]];
  c1 = cols[tri[1]];
  c2 = cols[tri[2]];
  w[0] = (float)c0.red * DAT_005ceb90;
  w[1] = (float)c1.red * DAT_005ceb90;
  w[2] = (float)c2.red * DAT_005ceb90;
  f = w[2] * d[2] + w[1] * d[1] + w[0] * d[0];
  *param_3 = (f <= DAT_005cc320) ? f : DAT_005cc320;
    } else {
    *param_3 = 1.0f;
    }
  }
  return 1;
}
