// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL06 0x00566ea0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned int uint;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc33c, 0x005cc33c);
// MATCH: 0x00566ea0 FUN_00566ea0
/* [C1 2026-05-19] Signature: `uint FUN_00566ea0(float *param_1, float *param_2, float *param_3,
   float *param_4, float *param_5, float p6
 */

uint FUN_00566ea0(float *param_1,float *param_2,float *param_3,float *param_4,float *param_5,
                 float param_6,int param_7,int param_8)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  uint uVar7;
  
  fVar1 = *param_4 - *param_2;
  fVar2 = param_4[1] - param_2[1];
  fVar3 = param_4[2] - param_2[2];
  fVar4 = param_5[2] * fVar3 + fVar1 * *param_5 + param_5[1] * fVar2;
  fVar5 = ((fVar3 * fVar3 + fVar1 * fVar1 + fVar2 * fVar2) - fVar4 * fVar4) - param_6 * param_6;
  uVar7 = (uint)(fVar5 < DAT_005d757c);
  if (param_1 != (float *)0x0) {
    if ((uVar7 != 0) && (param_8 == 0)) {
      *param_1 = 0.0;
      param_1[1] = 1.0;
      return 1;
    }
    fVar6 = param_5[2] * param_3[2] + *param_3 * *param_5 + param_5[1] * param_3[1];
    fVar1 = (param_3[2] * fVar3 + fVar1 * *param_3 + param_3[1] * fVar2) - fVar6 * fVar4;
    if ((param_7 == 0) && (fVar1 < DAT_005d757c != (fVar1 == DAT_005d757c))) {
      return 0xffffffff;
    }
    fVar2 = (param_3[2] * param_3[2] + param_3[1] * param_3[1] + *param_3 * *param_3) -
            fVar6 * fVar6;
    param_1[1] = fVar2;
    fVar3 = fVar1 * fVar1 - fVar2 * fVar5;
    if (fVar3 < DAT_005d757c) {
      return 0;
    }
    fVar2 = fVar1 - fVar2;
    if (param_7 == 0) {
      fVar4 = _DAT_005cc320;
      if ((DAT_005d757c <= fVar2) && (fVar3 <= fVar2 * fVar2)) {
        return 0;
      }
    }
    else {
      if (DAT_005d757c <= fVar2) {
        return 0;
      }
      fVar4 = _DAT_005cc33c;
      if (fVar2 * fVar2 < fVar3 != (fVar2 * fVar2 == fVar3)) {
        return 0;
      }
    }
    uVar7 = 1;
    *param_1 = fVar1 - SQRT(fVar3) * fVar4;
  }
  return uVar7;
}
