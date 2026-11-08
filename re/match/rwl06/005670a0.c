// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005670a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(undefined1, DAT_005cc35c, 0x005cc35c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cca00, 0x005cca00);
int FUN_005665a0(float * p0, float p1, float p2, int p3, float * p4);
MGLOBAL(int, _DAT_005cc35c, 0x005cc35c);
MGLOBAL(int, _DAT_005cca00, 0x005cca00);
// MATCH: 0x005670a0 FUN_005670a0
/* [C1 2026-05-19] Signature: `bool FUN_005670a0(float *param_1, float *param_2, float *param_3,
   float *param_4, float *param_5, float p6
 */

bool FUN_005670a0(float *param_1,float *param_2,float *param_3,float *param_4,float *param_5,
                 float param_6,float param_7)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  bool bVar6;
  int iVar7;
  float local_60;
  float local_5c;
  float local_58;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  
  local_60 = *param_4 - *param_2;
  local_5c = param_4[1] - param_2[1];
  local_58 = param_4[2] - param_2[2];
  fVar1 = param_5[2] * local_58 + local_60 * *param_5 + param_5[1] * local_5c;
  fVar2 = local_58 * local_58 + local_60 * local_60 + local_5c * local_5c;
  param_6 = param_6 * param_6;
  fVar3 = param_6 + param_7 * param_7;
  fVar4 = param_6 - param_7 * param_7;
  local_50 = fVar4 * fVar4 +
             param_6 * fVar1 * fVar1 * _DAT_005cc35c +
             (fVar2 * fVar2 - (fVar3 * fVar2 + fVar3 * fVar2));
  bVar6 = local_50 < DAT_005d757c;
  if (param_1 != (float *)0x0) {
    if (bVar6) {
      *param_1 = 0.0;
      param_1[1] = 1.0;
      return true;
    }
    fVar4 = param_3[2] * local_58 + local_60 * *param_3 + param_3[1] * local_5c;
    fVar5 = param_3[2] * param_5[2] + *param_3 * *param_5 + param_3[1] * param_5[1];
    local_40 = param_3[2] * param_3[2] + param_3[1] * param_3[1] + *param_3 * *param_3;
    fVar1 = fVar5 * param_6 * fVar1;
    local_4c = (fVar1 + fVar1 + (fVar2 - fVar3) * fVar4) * _DAT_005cca00;
    fVar1 = local_40 * fVar2 + fVar4 * fVar4 + fVar4 * fVar4;
    local_48 = fVar5 * fVar5 * param_6 * _DAT_005cc35c +
               ((fVar1 + fVar1) - (local_40 * fVar3 + local_40 * fVar3));
    local_44 = local_40 * fVar4 * _DAT_005cca00;
    local_40 = local_40 * local_40;
    iVar7 = FUN_005665a0(&local_60,0,0x3f800000,4,&local_50);
    if (iVar7 == 0) {
      return false;
    }
    param_1[1] = 1.0;
    *param_1 = local_60;
    bVar6 = true;
  }
  return bVar6;
}
