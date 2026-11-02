// FLAGS: /O2 /GS /Ob1
/* 0x0048aa50: 85.4% (regs 85.4%, ops 90.2%). Raw converter 19.5%. By hand: typed 8-param prototype, 2-arg
   FUN_00472650(lo, hi), 7-arg FUN_0048a850, intrinsic sin (fsin) on param_5 * 0.017453292f (0x005cd7a8),
   the three rand results stored then scaled then offset by param_4 (matches the original x87 order).
   Residual, class (b): only the scheduling of the out = v * param_3 stores against the pushes for the
   FUN_0048a850 call (original computes out[0] and loads param_1 before the first push). Tried: local order,
   param_2 as counter (76.8%), __inline V3Scale helper, a local copy of param_3, V3 struct out; permuter
   183 compiles no gain. */
/* MATCH-S14 0x0048aa50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef struct { uchar r, g, b, a; } Rgba;
double sin(double x);
#pragma intrinsic(sin)
float FUN_00472650(float lo, float hi);
void FUN_0048a850(float *pos, int count, float *vel, Rgba *color, float *accel, float size, float p7);
float FUN_004c39b0(float *out, float *in);
// MATCH: 0x0048aa50 FUN_0048aa50
void FUN_0048aa50(float *param_1, int param_2, float param_3, float *param_4, float param_5,
                  Rgba *param_6, float *param_7, float param_8)
{
  float s;
  int n;
  float v[3];
  float out[3];

  if (param_2 != 0) {
    s = (float)sin(param_5 * 0.017453292f);
    n = param_2;
    do {
      v[0] = FUN_00472650(-1.0f, 1.0f);
      v[1] = FUN_00472650(-1.0f, 1.0f);
      v[2] = FUN_00472650(-1.0f, 1.0f);
      v[0] = v[0] * s;
      v[1] = v[1] * s;
      v[2] = v[2] * s;
      v[0] = v[0] + param_4[0];
      v[1] = v[1] + param_4[1];
      v[2] = v[2] + param_4[2];
      FUN_004c39b0(v, v);
      out[0] = v[0] * param_3;
      out[1] = v[1] * param_3;
      out[2] = v[2] * param_3;
      FUN_0048a850(param_1, 1, out, param_6, param_7, param_8, 1.0f);
    } while (--n != 0);
  }
}
