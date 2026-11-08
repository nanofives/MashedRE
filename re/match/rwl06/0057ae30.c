// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 100% regs 100%, relocheck OK (was 90.7). Three changes: 1/3 as the literal 0.33333334f (global gave the constant-first fmul), the first vertex sum through two pointer locals a = param_1, b = param_1 + 4 (direct param_1[k] indexing loads the lower offset first for x and z), and the sort index built in idx first, then 0x21312300 >> (idx * 4) (a single expression distributes the shift). Supersedes the header below. */
/* MATCH-RWR05 2026-11-08: float DAT_ constants written as float literals (values read from the unpatched exe; relocheck compares __real@ constants by value). The compiler orders x87 operands differently for a literal than for a global. Score in results.tsv. */
/* MATCH-RWL06 h3 0x0057ae30: best 90.7% (regs 90.7%, ops 98.4%), class (a). fadd/fmul operand order and shift distribution (shl 3 + shl 2 against add,add,shl 2) are compiler canonicalisation; flips, temp t and permuter tried. */
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
typedef unsigned char undefined1;
MGLOBAL(undefined1, DAT_005ccac8, 0x005ccac8);
MGLOBAL(float, _DAT_005ccac8, 0x005ccac8);
// MATCH: 0x0057ae30 FUN_0057ae30
/* [C1 2026-05-20] Signature recovered: `float *FUN_0057ae30(float *triangle, float
   *out_circumsphere_or_centroid)` —
 */

double sqrt(double);
#pragma intrinsic(sqrt)
float * FUN_0057ae30(float *param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float local_c [3];
  uint idx;
  
  {
    float *a = param_1;
    float *b = param_1 + 4;
    param_2[0] = a[0] + b[0];
    param_2[1] = a[1] + b[1];
    param_2[2] = a[2] + b[2];
  }
  *param_2 = param_1[8] + *param_2;
  param_2[1] = param_1[9] + param_2[1];
  param_2[2] = param_1[10] + param_2[2];
  fVar1 = *param_2 * 0.33333334f;
  *param_2 = fVar1;
  fVar2 = param_2[1] * 0.33333334f;
  param_2[1] = fVar2;
  fVar3 = param_2[2] * 0.33333334f;
  param_2[2] = fVar3;
  local_c[0] = -((fVar3 - param_1[2]) * (fVar3 - param_1[2]) +
                (fVar2 - param_1[1]) * (fVar2 - param_1[1]) +
                (fVar1 - *param_1) * (fVar1 - *param_1));
  local_c[1] = -((fVar3 - param_1[6]) * (fVar3 - param_1[6]) +
                (fVar2 - param_1[5]) * (fVar2 - param_1[5]) +
                (fVar1 - param_1[4]) * (fVar1 - param_1[4]));
  local_c[2] = -((fVar3 - param_1[10]) * (fVar3 - param_1[10]) +
                (fVar2 - param_1[9]) * (fVar2 - param_1[9]) +
                (fVar1 - param_1[8]) * (fVar1 - param_1[8]));
  idx = (((local_c[0] > local_c[1]) * 2 | (local_c[0] > local_c[2])) * 2) | (local_c[1] > local_c[2]);
  idx = 0x21312300 >> (idx * 4) & 3;
  param_2[3] = (float)sqrt(-local_c[idx]) + param_1[0x13];
  return param_2;
}
