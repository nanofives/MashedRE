// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5680: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
float10 FUN_004c3b90(float p0);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x004c5680 FUN_004c5680
/* [C1 2026-05-19] Signature `float* f(float* mat, float* axis_out)` (16-float RwMatrix */

float * FUN_004c5680(float *param_1,float *param_2)

{
  float10 fVar1;
  float local_c;
  float local_8;
  float local_4;
  
  if (*param_1 <= param_1[5]) {
    if (param_1[10] < param_1[5]) {
      local_8 = param_1[5] + _DAT_005cc320 + param_1[5] + _DAT_005cc320;
      local_4 = param_1[9] + param_1[6];
      local_c = param_1[4] + param_1[1];
      goto LAB_004c571f;
    }
  }
  else if (param_1[10] < *param_1) {
    local_c = *param_1 + _DAT_005cc320 + *param_1 + _DAT_005cc320;
    local_8 = param_1[4] + param_1[1];
    local_4 = param_1[8] + param_1[2];
    goto LAB_004c571f;
  }
  local_4 = param_1[10] + _DAT_005cc320 + param_1[10] + _DAT_005cc320;
  local_c = param_1[8] + param_1[2];
  local_8 = param_1[9] + param_1[6];
LAB_004c571f:
  fVar1 = (float10)FUN_004c3b90(local_4 * local_4 + local_c * local_c + local_8 * local_8);
  *param_2 = (float)((float10)local_c * fVar1);
  param_2[1] = (float)((float10)local_8 * fVar1);
  param_2[2] = (float)(fVar1 * (float10)local_4);
  return param_1;
}
