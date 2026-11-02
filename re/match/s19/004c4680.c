// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c4680: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float10 FUN_004c3b90(float p0);
// MATCH: 0x004c4680 FUN_004c4680

void FUN_004c4680(float *param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float *pfVar4;
  float *pfVar5;
  float *pfVar6;
  float10 fVar7;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_8;
  float local_4;
  
  local_3c = *param_2;
  local_38 = param_2[1];
  local_34 = param_2[2];
  local_30 = param_2[4];
  local_2c = param_2[5];
  local_28 = param_2[6];
  local_24 = param_2[8];
  fVar3 = param_2[0xc];
  local_20 = param_2[9];
  local_1c = param_2[10];
  local_8 = param_2[0xd];
  local_4 = param_2[0xe];
  fVar7 = (float10)FUN_004c3b90(local_34 * local_34 + local_3c * local_3c + local_38 * local_38);
  local_18 = (float)fVar7;
  local_3c = (float)(fVar7 * (float10)local_3c);
  local_38 = local_18 * local_38;
  local_34 = local_18 * local_34;
  fVar7 = (float10)FUN_004c3b90(local_28 * local_28 + local_30 * local_30 + local_2c * local_2c);
  local_14 = (float)fVar7;
  local_30 = (float)(fVar7 * (float10)local_30);
  local_2c = local_14 * local_2c;
  local_28 = local_14 * local_28;
  fVar7 = (float10)FUN_004c3b90(local_1c * local_1c + local_24 * local_24 + local_20 * local_20);
  local_24 = (float)((float10)local_24 * fVar7);
  local_20 = (float)((float10)local_20 * fVar7);
  local_1c = (float)((float10)local_1c * fVar7);
  if (local_18 <= DAT_005d757c) {
LAB_004c48ef:
    pfVar4 = &local_30;
    pfVar5 = &local_24;
    pfVar6 = &local_3c;
  }
  else {
    if (local_14 <= DAT_005d757c) {
      pfVar4 = &local_24;
      pfVar5 = &local_3c;
      pfVar6 = &local_30;
      goto LAB_004c48fb;
    }
    if ((float10)DAT_005d757c < fVar7) {
      local_18 = local_1c * local_28 + local_24 * local_30 + local_20 * local_2c;
      if (local_18 < DAT_005d757c) {
        local_18 = -local_18;
      }
      local_14 = local_1c * local_34 + local_24 * local_3c + local_20 * local_38;
      if (local_14 < DAT_005d757c) {
        local_14 = -local_14;
      }
      fVar1 = local_28 * local_34 + local_30 * local_3c + local_2c * local_38;
      if (fVar1 < DAT_005d757c) {
        fVar1 = -fVar1;
      }
      if (local_14 <= local_18) {
        if (local_14 < fVar1) {
          pfVar4 = &local_24;
          pfVar5 = &local_3c;
          pfVar6 = &local_30;
          goto LAB_004c48fb;
        }
      }
      else if (local_18 < fVar1) goto LAB_004c48ef;
    }
    pfVar4 = &local_3c;
    pfVar5 = &local_30;
    pfVar6 = &local_24;
  }
LAB_004c48fb:
  *pfVar6 = pfVar5[2] * pfVar4[1] - pfVar5[1] * pfVar4[2];
  pfVar6[1] = pfVar4[2] * *pfVar5 - pfVar5[2] * *pfVar4;
  fVar1 = pfVar5[1] * *pfVar4 - pfVar4[1] * *pfVar5;
  pfVar6[2] = fVar1;
  fVar7 = (float10)FUN_004c3b90(fVar1 * fVar1 + pfVar6[1] * pfVar6[1] + *pfVar6 * *pfVar6);
  *pfVar6 = (float)(fVar7 * (float10)*pfVar6);
  fVar1 = pfVar6[1];
  pfVar6[1] = (float)((float10)fVar1 * fVar7);
  fVar2 = pfVar6[2];
  pfVar6[2] = (float)((float10)fVar2 * fVar7);
  *pfVar5 = (float)((float10)pfVar4[2] * (float10)fVar1 * fVar7 -
                   (float10)pfVar4[1] * (float10)fVar2 * fVar7);
  pfVar5[1] = pfVar6[2] * *pfVar4 - pfVar4[2] * *pfVar6;
  fVar1 = pfVar4[1] * *pfVar6 - pfVar6[1] * *pfVar4;
  pfVar5[2] = fVar1;
  fVar7 = (float10)FUN_004c3b90(fVar1 * fVar1 + pfVar5[1] * pfVar5[1] + *pfVar5 * *pfVar5);
  *pfVar5 = (float)(fVar7 * (float10)*pfVar5);
  pfVar5[1] = (float)((float10)pfVar5[1] * fVar7);
  pfVar5[2] = (float)((float10)pfVar5[2] * fVar7);
  *param_1 = local_3c;
  param_1[1] = local_38;
  param_1[2] = local_34;
  param_1[4] = local_30;
  param_1[5] = local_2c;
  param_1[6] = local_28;
  param_1[8] = local_24;
  param_1[9] = local_20;
  param_1[10] = local_1c;
  param_1[0xc] = fVar3;
  param_1[0xd] = local_8;
  param_1[0xe] = local_4;
  param_1[3] = (float)((uint)param_1[3] & 0xfffdffff | 3);
  return;
}
