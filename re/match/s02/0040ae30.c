// FLAGS: /O2 /GS /Ob1
/* 0x0040ae30 FUN_0040ae30 (189 B): 100.0% exact, regs 100.0%.
   Raw g2s was 19.6%. Hand form: V3/V2/matrix structs (up row +0x10, at row +0x20), out = at*0.0f then
   t = up*f, out = t + out, with constants from .rdata (0x005d757c = 0.0f, 0x005cc9a0 = 0.05f).
   87.5% with float f (fld st(0); fmul mem instead of fld mem; fmul st(1)); the permuter found nothing;
   declaring the scale f as double gives the x87 schedule of the original. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct V3 { float x, y, z; } V3;
typedef struct V2 { float x, y; } V2;
typedef struct Mat { V3 right; float rpad; V3 up; float upad; V3 at; float apad; V3 pos; float ppad; } Mat;

undefined4 FUN_0046d660(V3 *out, uint h);
undefined4 FUN_0046d4a0(Mat **out, uint h);
float FUN_004c3bf0(V2 *v);
undefined4 FUN_0046bf50(V3 *v, uint h);

// MATCH: 0x0040ae30 FUN_0040ae30
void FUN_0040ae30(uint param_1)
{
  V3 v;
  V2 p;
  float len;
  Mat *m;
  double f;
  V3 t;

  FUN_0046d660(&v, param_1);
  p.x = v.x;
  p.y = v.z;
  len = FUN_004c3bf0(&p);
  FUN_0046d4a0(&m, param_1);
  v.x = m->at.x * 0.0f;
  v.y = m->at.y * 0.0f;
  v.z = m->at.z * 0.0f;
  f = len * 0.05f;
  t.x = f * m->up.x;
  t.y = f * m->up.y;
  t.z = f * m->up.z;
  v.x = t.x + v.x;
  v.y = t.y + v.y;
  v.z = t.z + v.z;
  FUN_0046bf50(&v, param_1);
}
