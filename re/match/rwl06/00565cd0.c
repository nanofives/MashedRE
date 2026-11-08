// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 100% regs 100%, relocheck OK (was 68.3). Literal 0.5f for DAT_005cc32c, the two endpoints as pointer locals a = param_2, b = param_2 + 4 (fixes the fld/fadd operand order of the x and z sums), radius (float)sqrt(...) * 0.5f. Supersedes the header below. */
/* MATCH-RWR05 2026-11-08: float DAT_ constants written as float literals (values read from the unpatched exe; relocheck compares __real@ constants by value). The compiler orders x87 operands differently for a literal than for a global. Score in results.tsv. */
/* MATCH-RWL06 h1 0x00565cd0: 68.3% (regs 63.4%), class a. x87 add/mul operand order canonicalised by compiler. Tried: operand flips, permuter. cxx:ok. */
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
double sqrt(double);
#pragma intrinsic(sqrt)
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
// MATCH: 0x00565cd0 FUN_00565cd0
void FUN_00565cd0(float *param_1,float *param_2)
{
  float *a = param_2;
  float *b = param_2 + 4;

  param_1[0] = a[0] + b[0];
  param_1[1] = a[1] + b[1];
  param_1[2] = a[2] + b[2];
  param_1[0] = 0.5f * param_1[0];
  param_1[1] = 0.5f * param_1[1];
  param_1[2] = 0.5f * param_1[2];
  param_1[3] = (float)sqrt((b[2] - a[2]) * (b[2] - a[2]) + (b[1] - a[1]) * (b[1] - a[1]) + (b[0] - a[0]) * (b[0] - a[0])) * 0.5f;
}
