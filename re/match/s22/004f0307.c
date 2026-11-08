// FLAGS: /O1
/* MATCH-S22 0x004f0307 (h1): 100% under /O1 (D3DX code: odd address, ebp frame, calls the d3dx9-psgp row
   0x004eede9). Under /O2 /Ob1 /G5 it was 55.1% (/Oy- best). Mechanically: v = 0x004eede9(param_2), x/y/z =
   v[0..2] * -2.0f, out = 4x4 of v[r] * {x,y,z} with + 1.0f on the diagonal, column 3 = 0, 0, 0, 1, returns param_1.
   0x005cc34c (-2.0f) and 0x005cc320 (1.0f) are compiler literals: written as literals the operand order matches
   (as MGLOBAL globals cl loads them first, 91.3%). */
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
void __stdcall FUN_004eede9(float *out, int arg);
// MATCH: 0x004f0307 FUN_004f0307
float * __stdcall FUN_004f0307(float *param_1,int param_2)
{
  float v[4];
  float x, y, z, one;

  FUN_004eede9(v,param_2);
  x = v[0] * -2.0f;
  y = v[1] * -2.0f;
  z = v[2] * -2.0f;
  one = 1.0f;
  param_1[0] = v[0] * x + one;
  param_1[1] = y * v[0];
  param_1[2] = z * v[0];
  param_1[3] = 0.0f;
  param_1[4] = v[1] * x;
  param_1[5] = v[1] * y + one;
  param_1[6] = z * v[1];
  param_1[7] = 0.0f;
  param_1[8] = v[2] * x;
  param_1[9] = v[2] * y;
  param_1[10] = v[2] * z + one;
  param_1[0xb] = 0.0f;
  param_1[0xc] = v[3] * x;
  param_1[0xd] = v[3] * y;
  param_1[0xe] = v[3] * z;
  param_1[0xf] = 1.0f;
  return param_1;
}
