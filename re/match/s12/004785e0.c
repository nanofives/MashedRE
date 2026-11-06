// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004785e0 (h4, by hand): 97.8% (regs 97.8%). Register-arg callee (p in eax, out in ecx, norm on stack) written as a static function with a stand-in caller (pilot finding 1). Residual (a): one fld st(0) (dup of b[2]) is scheduled after mov eax,[esp+0x10]/test in ours, before them in the original. Tried scalar/array mixes for a and b, operand order of the cross products, char/int norm, norm!=0, caller passing a variable, permuter 131 compiles: no change. */
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
float FUN_004c39b0(float *out, float *in);

// MATCH: 0x004785e0 FUN_004785e0
static void FUN_004785e0(float *p, float *out, int norm)
{
  float ax, ay, az;
  float b[3];

  ax = p[3] - p[0];
  ay = p[4] - p[1];
  az = p[5] - p[2];
  b[0] = p[6] - p[0];
  b[1] = p[7] - p[1];
  b[2] = p[8] - p[2];
  out[0] = b[2] * ay - b[1] * az;
  out[1] = b[0] * az - b[2] * ax;
  out[2] = b[1] * ax - b[0] * ay;
  if (norm) {
    FUN_004c39b0(out, out);
  }
}

/* stand-in caller (not a MATCH target): makes the compiler pick the register convention */
void FUN_004785e0_caller(float *p, float *out)
{
  FUN_004785e0(p, out, 1);
}
