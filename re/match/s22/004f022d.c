// FLAGS: /O1
/* MATCH-S22 0x004f022d (h1): 100% under /O1 (D3DX code: odd address, ebp frame, calls the d3dx9-psgp row
   0x004eede9; the MATCH-S21 D3DX rows are /O1 too). Under /O2 /Ob1 /G5 it was 27.9% (/Oy- best). Mechanically:
   v = 0x004eede9(param_3) (4 floats), d = v.param_2 (4-term dot, terms in order 0..3), n = -v, out[r*4+c] =
   n[r] * param_2[c] (+ d on the diagonal), returns param_1. What fixed it on top of /O1: the dot in index order,
   the negated copy as a separate float[4] (the extra frame slots), and returning param_1 (eax = out). */
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
void __stdcall FUN_004eede9(float *out, int arg);
// MATCH: 0x004f022d FUN_004f022d
float * __stdcall FUN_004f022d(float *param_1,float *param_2,int param_3)
{
  float d;
  float v[4];
  float n[4];

  FUN_004eede9(v,param_3);
  d = v[0] * param_2[0] + v[1] * param_2[1] + v[2] * param_2[2] + v[3] * param_2[3];
  n[0] = -v[0];
  n[1] = -v[1];
  n[2] = -v[2];
  n[3] = -v[3];
  param_1[0] = n[0] * param_2[0] + d;
  param_1[4] = n[1] * param_2[0];
  param_1[8] = n[2] * param_2[0];
  param_1[0xc] = n[3] * param_2[0];
  param_1[1] = n[0] * param_2[1];
  param_1[5] = n[1] * param_2[1] + d;
  param_1[9] = n[2] * param_2[1];
  param_1[0xd] = n[3] * param_2[1];
  param_1[2] = n[0] * param_2[2];
  param_1[6] = n[1] * param_2[2];
  param_1[10] = n[2] * param_2[2] + d;
  param_1[0xe] = n[3] * param_2[2];
  param_1[3] = n[0] * param_2[3];
  param_1[7] = n[1] * param_2[3];
  param_1[0xb] = n[2] * param_2[3];
  param_1[0xf] = n[3] * param_2[3] + d;
  return param_1;
}
