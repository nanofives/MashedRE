// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand from decomp (raw did not compile): 12-byte local struct for dx/dy (frame 0xc), FUN_005c1cb7(double,double) cdecl, s=h*2 then K/s, (float) casts on the products.
/* MATCH-S13 0x004820e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
typedef struct { float x; float y; float z; } V3;
double FUN_005c1cb7(double p0, double p1);
// MATCH: 0x004820e0 FUN_004820e0
void FUN_004820e0(float *param_1, float *param_2, float *param_3)
{
  V3 d;
  double s;

  d.x = param_3[0] - param_2[0];
  d.y = param_3[1] - param_2[1];
  s = FUN_005c1cb7(d.x, d.y) * 2.0;
  s = DAT_005cc320 / s;
  param_1[0] = (float)(d.x * s) + DAT_005cc32c;
  param_1[1] = (float)(s * d.y) + DAT_005cc32c;
}
