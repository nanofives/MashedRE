// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00565c20: hand-matched (h2). 100.0% regs 100.0%. The raw decomp used float10; the bytes are all float (0x004c3ac0 returns float in st0, _DAT_005cc32c is a float at 0x005cc32c). Divisor written inline (fVar1 + fVar1) three times (a fVar2 temp makes the compiler hoist a reciprocal), and the three delta locals as float v[3] (separate locals land in reversed stack order). */
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
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
float FUN_004c3ac0(float * p0);
// MATCH: 0x00565c20 FUN_00565c20
void FUN_00565c20(float *param_1,float *param_2,float *param_3)
{
  float fVar1;
  float fVar2;
  float v[3];

  v[0] = *param_3 - *param_2;
  v[1] = param_3[1] - param_2[1];
  v[2] = param_3[2] - param_2[2];
  fVar1 = FUN_004c3ac0(v);
  if (param_2[3] < fVar1) {
        *param_1 = (fVar1 - param_2[3]) / (fVar1 + fVar1) * v[0] + *param_2;
    param_1[1] = (fVar1 - param_2[3]) / (fVar1 + fVar1) * v[1] + param_2[1];
    param_1[2] = (fVar1 - param_2[3]) / (fVar1 + fVar1) * v[2] + param_2[2];
    param_1[3] = (fVar1 + param_2[3]) * DAT_005cc32c;
    return;
  }
  if (param_1 != param_2) {
    *param_1 = *param_2;
    param_1[1] = param_2[1];
    param_1[2] = param_2[2];
    param_1[3] = param_2[3];
  }
}
