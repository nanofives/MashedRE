// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00474de0 (h9): by hand, 94.7% (regs 94.7%). out = a + (FUN_004671d0(0) - a) * t, written as the
   RwV3dSub/Scale/Add macro expansion (FUN_004671d0 called once per component, cdecl, pushes deferred to one
   add esp,0xc). Residual: 2 scheduling slots only (orig puts `fsub [esi+4]` before the 2nd `push 0`, and the
   `fld d` before `add esp,0xc`). Tried: float[3] / struct / scalar temps, separate e temp, `t*d` order,
   `d.x=..; d.x-=..`, int/uchar/float/pointer callee arg types, int vs V3* return, permuter (129 compiles, no
   gain). Class (a)/(b): no source change moved the push/add esp placement. */
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
typedef struct { float x, y, z; } V3;
int FUN_004671d0(int p0);
// MATCH: 0x00474de0 FUN_00474de0
void FUN_00474de0(V3 *a, V3 *out, float t)
{
  V3 d;
  d.x = (float)(((V3 *)FUN_004671d0(0))->x - (double)a->x);
  d.y = ((V3 *)FUN_004671d0(0))->y - a->y;
  d.z = ((V3 *)FUN_004671d0(0))->z - a->z;
  d.x *= t; d.y *= t; d.z *= t;
  out->x = d.x + a->x;
  out->y = d.y + a->y;
  out->z = d.z + a->z;
}
