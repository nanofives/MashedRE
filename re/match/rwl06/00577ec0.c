// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00577ec0: hand pass (h2), body kept in raw form. 15.1% regs 18.8% ops 50.4% under /O2 /Ob1 /G5. Compiles after: float fP1 replaces Ghidra's param_1-as-float reuse, constants floats. Deeper branch/loop shape not iterated (budget). Class b. */
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
typedef unsigned int undefined4;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00577ec0 FUN_00577ec0
/* [C1 2026-05-20] Signature recovered: `int FUN_00577ec0(float *p1_start, float *p1_dir, float
   p1_min, float p1_max,
 */

undefined4
FUN_00577ec0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6
            ,float param_7,float param_8,float *param_9,float *param_10,float *param_11)

{
  bool bVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fP1;
  
  fVar2 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar3 = param_6[2] * param_6[2] + param_6[1] * param_6[1] + *param_6 * *param_6;
  fVar4 = *param_5 - *param_1;
  fVar5 = param_5[1] - param_1[1];
  fVar6 = param_5[2] - param_1[2];
  if (fVar2 == DAT_005d757c) {
    *param_9 = param_3;
    bVar1 = fVar3 == DAT_005d757c;
    *param_11 = 0.0;
    if (bVar1) {
      *param_10 = param_7;
      return 0;
    }
    fVar2 = -(param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5);
    if (fVar3 * param_7 < fVar2) {
      if (fVar3 * param_8 < fVar2 == (fVar3 * param_8 == fVar2)) {
        *param_10 = fVar2 / fVar3;
        return 0;
      }
      *param_10 = param_8;
      return 0;
    }
    *param_10 = param_7;
    return 0;
  }
  if (fVar3 != DAT_005d757c) {
    fVar7 = param_2[2] * param_6[2] + *param_6 * *param_2 + param_6[1] * param_2[1];
    fVar9 = fVar3 * fVar2;
    fVar10 = fVar9 - fVar7 * fVar7;
    fVar8 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
    fVar4 = param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5;
    if (fVar10 <= DAT_005cd03c * fVar9) {
      fVar9 = DAT_005cc320 / fVar9;
      fVar3 = fVar9 * fVar3;
      fVar5 = fVar7 * fVar3;
      fVar3 = fVar3 * fVar8;
      if (fVar5 <= DAT_005d757c) {
        fP1 = param_8;
        param_8 = param_7;
      }
      else {
        fP1 = param_7;
      }
      fVar6 = fP1 * fVar5 + fVar3;
      fVar3 = fVar5 * param_8 + fVar3;
      if (fVar3 < param_3 == (fVar3 == param_3)) {
        if (fVar6 < param_4) {
          if (fVar6 < param_3) {
            fVar6 = param_3;
          }
          *param_9 = fVar6;
          if (param_4 < fVar3) {
            fVar3 = param_4;
          }
          *param_11 = fVar3 - fVar6;
          *param_10 = fVar9 * fVar2 * (fVar7 * *param_9 - fVar4);
          return 0;
        }
        *param_9 = param_4;
        *param_11 = 0.0;
        *param_10 = fP1;
        return 0;
      }
      *param_9 = param_3;
      *param_11 = 0.0;
      *param_10 = param_8;
      return 0;
    }
    fVar5 = fVar8 * fVar3 - fVar4 * fVar7;
    fVar6 = param_3;
    if ((fVar10 * param_3 <= fVar5) && (fVar6 = param_4, fVar5 <= fVar10 * param_4)) {
      fVar3 = fVar8 * fVar7 - fVar4 * fVar2;
      if ((fVar3 < fVar10 * param_7) || (param_7 = param_8, fVar10 * param_8 < fVar3)) {
        *param_10 = param_7;
        fVar8 = fVar7 * *param_10 + fVar8;
        if (fVar8 < fVar2 * param_3) {
          *param_9 = param_3;
          *param_11 = 0.0;
          return 1;
        }
        param_10 = param_9;
        if (fVar8 <= fVar2 * param_4) {
          *param_9 = fVar8 / fVar2;
          *param_11 = 0.0;
          return 1;
        }
      }
      else {
        param_4 = DAT_005cc320 / fVar10;
        *param_9 = fVar5 * param_4;
        param_4 = param_4 * fVar3;
      }
      *param_10 = param_4;
      *param_11 = 0.0;
      return 1;
    }
    *param_9 = fVar6;
    fVar4 = fVar7 * *param_9 - fVar4;
    if ((fVar3 * param_7 <= fVar4) && (param_7 = param_8, fVar4 <= fVar3 * param_8)) {
      *param_10 = fVar4 / fVar3;
      *param_11 = 0.0;
      return 1;
    }
    *param_10 = param_7;
    fVar8 = fVar7 * *param_10 + fVar8;
    if (fVar8 < fVar2 * param_3) {
      *param_9 = param_3;
      *param_11 = 0.0;
      return 1;
    }
    if (fVar8 <= fVar2 * param_4) {
      *param_9 = fVar8 / fVar2;
      *param_11 = 0.0;
      return 1;
    }
    *param_9 = param_4;
    *param_11 = 0.0;
    return 1;
  }
  *param_10 = param_7;
  *param_11 = 0.0;
  fVar3 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
  if (fVar3 <= fVar2 * param_3) {
    *param_9 = param_3;
    return 0;
  }
  if (fVar2 * param_4 < fVar3 == (fVar2 * param_4 == fVar3)) {
    *param_9 = fVar3 / fVar2;
    return 0;
  }
  *param_9 = param_4;
  return 0;
}
