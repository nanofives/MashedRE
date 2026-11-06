// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004728f0 (h7): 100% (whole obj symbol byte-identical with --bytes; Ghidra size 178 stops 4 instructions
   before the epilogue, so the instruction score reads 92.5%). Arc-length sum over t in [0.001, 1] step 0.001
   (0x5cc558, 0x5cc320). The step had to be a float literal: through the MGLOBAL the add loaded the constant first.
   Residual: none. */
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
typedef struct { float x, y, z; } V3_004728f0;
float FUN_004c3ac0(V3_004728f0 *v);
V3_004728f0 *FUN_005459c0(void *curve, int n, float t, V3_004728f0 *pos, V3_004728f0 *tan);
// MATCH: 0x004728f0 FUN_004728f0
float FUN_004728f0(void *param_1)
{
  float t;
  float len;
  V3_004728f0 a, b, d, tan;
  len = 0.0f;
  t = 0.001f;
  do {
    FUN_005459c0(param_1, 10, t - 0.001f, &a, &tan);
    FUN_005459c0(param_1, 10, t, &b, &tan);
    d.x = b.x - a.x;
    d.y = b.y - a.y;
    d.z = b.z - a.z;
    len = FUN_004c3ac0(&d) + len;
    t = t + 0.001f;
  } while (t <= 1.0f);
  return len;
}
