// FLAGS: /O2 /Ob1 /G5
/* best 38.0% (regs 21.8% renamed), 344 vs 313 insns, frame 0x30 vs 0x28: written from the Ghidra decomp with float globals typed as float and
   the reused argument slots (param_1/param_3) kept as address-taken stack slots read back via *(float *)&param_N. Residual: x87 operand
   order in the 4 dot-product clamps and the slot-reuse frame layout; not tuned past the permuter (budget). Class (a)/(b). */
/* MATCH-S25 0x005a9280: hand-matched (h1). */
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

MGLOBAL(float, DAT_005cc9cc, 0x005cc9cc);
MGLOBAL(float, DAT_005e618c, 0x005e618c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005e6190, 0x005e6190);
float FUN_004c39b0(float *p0, float *p1);
void FUN_005aed20(float *p0, float *p1);
// MATCH: 0x005a9280 FUN_005a9280
void FUN_005a9280(int *param_1,undefined4 param_2,float *param_3,float *param_4,int param_5)
{
  float *pfVar1;
  int bVar6;
  int iVar2;
  float fVar8;
  float fVar5;
  float fVar7;
  float fVar3;
  float fVar4;
  float *pfVar9;
  float *pfVar10;
  float local_24;
  float local_1c;
  float local_20;
  float local_4;
  float local_14;
  float local_18;
  float local_10;
  pfVar9 = param_3;
  iVar2 = *param_1;
  local_4 = *(float *)(iVar2 + 0x30) - *(float *)(iVar2 + 0x3c);
  fVar5 = *(float *)(iVar2 + 0x68);
  local_20 = param_3[1] - param_3[4];
  fVar3 = *(float *)(iVar2 + 0x6c);
  pfVar1 = (float *)(iVar2 + 0x4c);
  local_1c = param_3[2] - param_3[5];
  fVar4 = *(float *)(iVar2 + 100);
  local_18 = (*param_3 - param_3[3]) - (*(float *)(iVar2 + 0x28) - *(float *)(iVar2 + 0x34));
  local_14 = local_20 - (*(float *)(iVar2 + 0x2c) - *(float *)(iVar2 + 0x38));
  local_10 = local_1c - local_4;
  FUN_005aed20((float *)&param_3,&local_18);
  local_24 = *pfVar9 - *(float *)(iVar2 + 0x28);
  pfVar10 = param_4;
  local_20 = pfVar9[1] - *(float *)(iVar2 + 0x2c);
  local_1c = pfVar9[2] - *(float *)(iVar2 + 0x30);
  FUN_005aed20((float *)&param_1,&local_24);
  fVar7 = *(float *)&param_1 - *(float *)&param_3;
  fVar8 = DAT_005cc9cc;
  if ((fVar7 <= DAT_005cc9cc) && (fVar8 = fVar7, fVar7 < DAT_005e6190)) {
    fVar8 = DAT_005e6190;
  }
  fVar3 = fVar3 * DAT_005e618c;
  param_4[1] = (fVar3 - fVar8 * fVar5) / fVar3;
  if (*(float *)&param_1 <= pfVar9[7]) {
    if (pfVar9[6] <= *(float *)&param_1) {
      *(float *)&param_1 = *(float *)&param_1 - pfVar9[6];
      *param_4 = ((pfVar9[7] - pfVar9[6]) - *(float *)&param_1) /
                 (*(float *)&param_1 * fVar4 + (pfVar9[7] - pfVar9[6]));
    }
    else {
      *param_4 = 1.0f;
    }
  }
  else {
    *param_4 = 0.0f;
  }
  if (((local_24 == DAT_005d757c) && (local_20 == DAT_005d757c)) && (local_1c == DAT_005d757c)) {
    local_24 = *pfVar1;
    local_20 = *(float *)(iVar2 + 0x50);
    local_1c = *(float *)(iVar2 + 0x54);
  }
  else {
    FUN_004c39b0(&local_24,&local_24);
  }
  if (((local_18 == DAT_005d757c) && (local_14 == DAT_005d757c)) && (local_10 == DAT_005d757c)) {
    local_18 = *pfVar1;
    local_14 = *(float *)(iVar2 + 0x50);
    local_10 = *(float *)(iVar2 + 0x54);
  }
  else {
    FUN_004c39b0(&local_18,&local_18);
  }
  fVar3 = DAT_005cc32c -
          (*(float *)(iVar2 + 0x60) * local_1c +
          local_24 * *(float *)(0x58 + iVar2) + *(float *)(iVar2 + 0x5c) * local_20) * DAT_005cc32c;
  bVar6 = DAT_005d757c <= fVar3;
  pfVar10[2] = fVar3;
  fVar4 = DAT_005d757c;
  if ((bVar6) && (fVar4 = fVar3, DAT_005cc320 < fVar3)) {
    fVar4 = DAT_005cc320;
  }
  pfVar10[2] = fVar4;
  fVar3 = DAT_005cc32c -
          (*(float *)(0x54 + iVar2) * local_1c +
          local_24 * *pfVar1 + *(float *)(0x50 + iVar2) * local_20) * DAT_005cc32c;
  pfVar10[3] = fVar3;
  fVar4 = DAT_005d757c;
  bVar6 = DAT_005d757c <= fVar3;
  if ((bVar6) && (fVar4 = fVar3, DAT_005cc320 < fVar3)) {
    fVar4 = DAT_005cc320;
  }
  pfVar10[3] = fVar4;
  fVar3 = DAT_005cc32c -
          (*(float *)(iVar2 + 0x60) * local_10 +
          local_18 * *(float *)(iVar2 + 0x58) + *(float *)(iVar2 + 0x5c) * local_14) * DAT_005cc32c;
  pfVar10[4] = fVar3;
  fVar4 = DAT_005d757c;
  bVar6 = DAT_005d757c <= fVar3;
  if ((bVar6) && (fVar4 = fVar3, DAT_005cc320 < fVar3)) {
    fVar4 = DAT_005cc320;
  }
  pfVar10[4] = fVar4;
  fVar3 = DAT_005cc32c -
          (*(float *)(iVar2 + 0x54) * local_10 +
          local_18 * *pfVar1 + *(float *)(iVar2 + 0x50) * local_14) * DAT_005cc32c;
  pfVar10[5] = fVar3;
  fVar4 = DAT_005d757c;
  bVar6 = DAT_005d757c <= fVar3;
  if ((bVar6) && (fVar4 = fVar3, DAT_005cc320 < fVar3)) {
    fVar4 = DAT_005cc320;
  }
  pfVar10[5] = fVar4;
  if (param_5 == 2) {
    pfVar10[2] = pfVar10[2] - DAT_005cc32c;
    pfVar10[4] = pfVar10[4] - DAT_005cc32c;
    pfVar10[2] = pfVar10[2] * DAT_005d757c;
    fVar3 = pfVar10[4] * DAT_005d757c;
  }
  else {
    if (param_5 != 3) {
      return;
    }
    pfVar10[2] = pfVar10[2] - DAT_005cc32c;
    pfVar10[4] = pfVar10[4] - DAT_005cc32c;
    pfVar10[2] = DAT_005cc32c * pfVar10[2];
    fVar3 = pfVar10[4] * DAT_005cc32c;
  }
  pfVar10[4] = fVar3;
  pfVar10[2] = pfVar10[2] + DAT_005cc32c;
  pfVar10[4] = pfVar10[4] + DAT_005cc32c;
  return;
}
