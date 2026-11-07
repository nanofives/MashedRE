// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00558b40 (h4): 85.1% (regs 85.1%, ops 98.9%). Segment/sphere test: dir = end - start normalized by
   FUN_004c3910 (returns a float kept in a float local), r2 = radius squared, v = centre - start, t = dot(v, dir),
   d2 = dot(v, v); hit when d2 < r2 or (t > eps and r2 - (d2 - t*t) >= eps), *param_3 = t * len. From the raw
   form (52.9%): float local for the call result and clean RwV3d dir. Residual (a): the two dot products,
   original `fld st(i); fmul m` and the pop esi after them against our `fld m; fmul st(i)` (the S19 x87
   operand-order class). Tried all 36 term orders of both dot products, swapped product operands, v as an RwV3d:
   all 85.1%. */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_004c3910(RwV3d *out, const RwV3d *in);
// MATCH: 0x00558b40 FUN_00558b40
int FUN_00558b40(float *param_1, float *param_2, float *param_3)
{
  RwV3d dir;
  float len;
  float r2;
  float vx, vy, vz;
  float t;
  float d2;

  dir.x = param_1[3] - param_1[0];
  dir.y = param_1[4] - param_1[1];
  dir.z = param_1[5] - param_1[2];
  len = FUN_004c3910(&dir, &dir);
  r2 = param_2[3] * param_2[3];
  vx = param_2[0] - param_1[0];
  vy = param_2[1] - param_1[1];
  vz = param_2[2] - param_1[2];
  t = vx * dir.x + vy * dir.y + vz * dir.z;
  d2 = vx * vx + vy * vy + vz * vz;
  if (d2 < r2) {
    *param_3 = t * len;
    return 1;
  }
  if (t > DAT_005d757c && r2 - (d2 - t * t) >= DAT_005d757c) {
    *param_3 = t * len;
    return 1;
  }
  return 0;
}
