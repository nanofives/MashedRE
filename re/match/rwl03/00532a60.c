// FLAGS: /O2 /Ob1 /G5
// h3: 64.8% regs 64.8% ops 93.2%. Quaternion to matrix with named float temps; the original spills xx,yy,zz,zy,zx,yx,wx and keeps wy,wz on the FPU stack; class (b).
/* MATCH-RWL03 0x00532a60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00532a60 FUN_00532a60
void FUN_00532a60(float *param_1,float *param_2)
{
  float x, y, z, w;
  float xx, yy, zz, zy, zx, yx, wx, wy, wz;
  float t;

  w = param_2[5];
  x = param_2[2];
  y = param_2[3];
  xx = x * x;
  yy = y * y;
  z = param_2[4];
  zy = z * y;
  zz = z * z;
  zx = z * x;
  yx = y * x;
  wx = w * x;
  wy = w * y;
  wz = w * z;
  t = zz + yy;
  *param_1 = DAT_005cc320 - (t + t);
  t = yx + wz;
  param_1[1] = t + t;
  t = zx - wy;
  param_1[2] = t + t;
  t = yx - wz;
  param_1[4] = t + t;
  t = zz + xx;
  param_1[5] = DAT_005cc320 - (t + t);
  t = wx + zy;
  param_1[6] = t + t;
  t = wy + zx;
  param_1[8] = t + t;
  t = zy - wx;
  param_1[9] = t + t;
  t = yy + xx;
  param_1[10] = DAT_005cc320 - (t + t);
  *(int *)(param_1 + 3) = 3;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xc] = param_2[6];
  param_1[0xd] = param_2[7];
  param_1[0xe] = param_2[8];
}
