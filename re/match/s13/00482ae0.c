// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00482ae0 (h9, by hand): 99.0% (regs 99.0%). Uniform Catmull-Rom on 3 floats; constants are
   literals (5.0 @0x005cc358, 4.0 @0x005cc35c, 3.0 @0x005cc31c, 0.5 @0x005cc32c). Residual (a): the
   original loads p0 (mov eax,[esp+0xc]) before the first fld t; ours one slot after. Tried: permuter
   (6 rounds), V3 struct pointer params, local order, t3 = t2*t, inline t*t, an extra p0 alias. None moved it. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
// MATCH: 0x00482ae0 FUN_00482ae0
void FUN_00482ae0(float t, float *out, float *p0, float *p1, float *p2, float *p3)
{
  float t2;
  float t3;

  t2 = t * t;
  t3 = t * t2;
  out[0] = (((p0[0] + p0[0]) - p1[0] * 5.0f + p2[0] * 4.0f - p3[0]) * t2
            + ((p1[0] * 3.0f - p0[0]) - p2[0] * 3.0f + p3[0]) * t3
            + (p2[0] - p0[0]) * t + (p1[0] + p1[0])) * 0.5f;
  out[1] = (((p0[1] + p0[1]) - p1[1] * 5.0f + p2[1] * 4.0f - p3[1]) * t2
            + ((p1[1] * 3.0f - p0[1]) - p2[1] * 3.0f + p3[1]) * t3
            + (p2[1] - p0[1]) * t + (p1[1] + p1[1])) * 0.5f;
  out[2] = (((p0[2] + p0[2]) - p1[2] * 5.0f + p2[2] * 4.0f - p3[2]) * t2
            + ((p1[2] * 3.0f - p0[2]) - p2[2] * 3.0f + p3[2]) * t3
            + (p2[2] - p0[2]) * t + (p1[2] + p1[2])) * 0.5f;
}
