// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00547230 h1: best 28.4% (regs 30.9%, ops 84.6%); structure only (529 B ray-triangle test; Ghidra compare forms rewritten as plain <= / >). Orig keeps the six edge differences in six stack slots (frame 0x30) and computes them in x,y,z order via the FPU stack; ours keeps some in registers. Class (b). */
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
MGLOBAL(float, DAT_005e4574, 0x005e4574);
MGLOBAL(float, DAT_005cea1c, 0x005cea1c);
// MATCH: 0x00547230 FUN_00547230


int
FUN_00547230(float *param_1,float *param_2,float *param_3,float *param_4,float *param_5,
            float *param_6)

{
  float e1[3];
  float e2[3];
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float t1, t2, t3;
  
  e1[0] = *param_4 - *param_3;
  e1[1] = param_4[1] - param_3[1];
  e1[2] = param_4[2] - param_3[2];
  e2[0] = *param_5 - *param_3;
  e2[1] = param_5[1] - param_3[1];
  e2[2] = param_5[2] - param_3[2];
  fVar12 = param_2[1] * e2[2] - param_2[2] * e2[1];
  fVar14 = param_2[2] * e2[0] - e2[2] * *param_2;
  fVar11 = e2[1] * *param_2 - param_2[1] * e2[0];
  fVar13 = fVar11 * e1[2] + fVar12 * e1[0] + fVar14 * e1[1];
  if (DAT_005e4574 < fVar13) {
    fVar15 = fVar13 * DAT_005cea1c;
    fVar7 = fVar13 - fVar15;
    fVar8 = *param_1 - *param_3;
    fVar9 = param_1[1] - param_3[1];
    fVar10 = param_1[2] - param_3[2];
    fVar12 = fVar10 * fVar11 + fVar8 * fVar12 + fVar9 * fVar14;
    if ((fVar12 < fVar15) || (fVar12 > fVar7)) {
      return 0;
    }
    fVar11 = fVar9 * e1[2] - fVar10 * e1[1];
    t3 = fVar10 * e1[0] - fVar8 * e1[2];
    t1 = fVar8 * e1[1] - fVar9 * e1[0];
    t2 = param_2[2] * t1 + fVar11 * *param_2 + param_2[1] * t3;
    if ((fVar15 <= t2) && (fVar12 = fVar12 + t2, fVar12 <= fVar7)) {
      t1 = t1 * e2[2] + fVar11 * e2[0] + t3 * e2[1];
      *param_6 = t1;
      if ((fVar15 <= t1) && (t1 <= fVar7)) {
        *param_6 = t1 / fVar13;
        return 1;
      }
    }
  }
  return 0;
}
