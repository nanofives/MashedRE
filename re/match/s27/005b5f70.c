// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 0x005b5f70: 14.6% (regs 27.3%), raw converter body kept. Residual (b): 260 vs 215 instr, flags line /O2 /GS /Ob1 from the g2s sweep; no structural pass (budget).  ABS/ROUND defined in the file (fabs intrinsic, (int) cast) so it compiles as C and C++; this moved 14.6% -> 18.3%. */
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
double fabs(double x);
#pragma intrinsic(fabs)
#define ABS(x) ((float)fabs(x))
#define ROUND(x) ((int)(x))
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc9b8, 0x005cc9b8);
MGLOBAL(undefined4, DAT_006343e8, 0x006343e8);
MGLOBAL(undefined4, DAT_006343ec, 0x006343ec);
MGLOBAL(int, _DAT_005cc9b8, 0x005cc9b8);
// MATCH: 0x005b5f70 FUN_005b5f70
/* [C2 2026-06-01] Inner 28-sample ADPCM block encoder: picks the best of 5 predictor coefficient
   pairs, computes a per-block shift, and? */

void FUN_005b5f70(int param_1,byte *param_2,int *param_3,int *param_4,float *param_5,float *param_6)

{
  float fVar1;
  short sVar2;
  float fVar3;
  int iVar4;
  uint uVar5;
  byte bVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  float *pfVar11;
  byte *pbVar12;
  int iVar13;
  int local_2a0;
  int local_290;
  float local_28c;
  float local_27c [5];
  short local_268 [28];
  float local_230 [140];
  
  iVar4 = 0;
  local_28c = 1e+10;
  local_290 = 0;
  do {
    sVar2 = *(short *)(param_1 + iVar4 * 2);
    if (sVar2 < 0x7800) {
      if (sVar2 < -0x7800) {
        local_268[iVar4] = -0x7800;
      }
      else {
        local_268[iVar4] = sVar2;
      }
    }
    else {
      local_268[iVar4] = 0x77ff;
    }
    iVar4 = iVar4 + 1;
  } while (iVar4 < 0x1c);
  iVar7 = 0;
  iVar4 = *param_3;
  iVar8 = *param_4;
  do {
    iVar9 = 0;
    local_27c[iVar7] = 0.0;
    pfVar11 = local_230 + iVar7 * 0x1c;
    iVar10 = iVar4;
    local_2a0 = iVar8;
    fVar1 = DAT_005d757c;
    do {
      iVar13 = iVar10;
      iVar10 = (int)local_268[iVar9];
      fVar3 = (float)iVar10 +
              (float)local_2a0 * (float)(&DAT_006343ec)[iVar7 * 2] +
              (float)iVar13 * (float)(&DAT_006343e8)[iVar7 * 2];
      *pfVar11 = fVar3;
      fVar3 = ABS(fVar3);
      if (fVar1 < fVar3) {
        fVar1 = fVar3;
      }
      iVar9 = iVar9 + 1;
      pfVar11 = pfVar11 + 1;
      local_2a0 = iVar13;
    } while (iVar9 < 0x1c);
    local_27c[iVar7] = fVar1;
    if (fVar1 < local_28c) {
      local_290 = iVar7;
      local_28c = fVar1;
    }
    if ((iVar7 == 0) && (local_27c[0] < _DAT_005cc9b8 != (local_27c[0] == _DAT_005cc9b8))) {
      local_290 = 0;
      break;
    }
    iVar7 = iVar7 + 1;
  } while (iVar7 < 5);
  fVar1 = local_27c[local_290];
  *param_3 = iVar10;
  *param_4 = iVar13;
  iVar4 = (int)ROUND(fVar1);
  if (iVar4 < 0x8000) {
    if (iVar4 < -0x8000) {
      iVar4 = 0;
    }
  }
  else {
    iVar4 = 0x7fff;
  }
  uVar5 = 0x4000;
  iVar8 = 0;
  do {
    if ((uVar5 & ((int)uVar5 >> 3) + iVar4) != 0) break;
    iVar8 = iVar8 + 1;
    uVar5 = (int)uVar5 >> 1;
  } while (iVar8 < 0xc);
  bVar6 = (byte)iVar8;
  param_2[1] = 0;
  pbVar12 = param_2 + 2;
  *param_2 = bVar6 & 0xf ^ (char)local_290 << 4;
  local_28c = 0.0;
  pfVar11 = local_230 + local_290 * 0x1c;
  do {
    fVar1 = *param_5 * (float)(&DAT_006343e8)[local_290 * 2] +
            (float)(&DAT_006343ec)[local_290 * 2] * *param_6 + *pfVar11;
    uVar5 = (int)ROUND((float)(1 << (bVar6 & 0x1f)) * fVar1) + 0x800U & 0xfffff000;
    if ((int)uVar5 < 0x8000) {
      if ((int)uVar5 < -0x8000) {
        uVar5 = 0xffff8000;
      }
    }
    else {
      uVar5 = 0x7fff;
    }
    if (((uint)local_28c & 1) == 0) {
      *pbVar12 = (byte)((int)uVar5 >> 0xc) & 0xf;
    }
    else {
      *pbVar12 = *pbVar12 | (byte)(uVar5 >> 8) & 0xf0;
      pbVar12 = pbVar12 + 1;
    }
    pfVar11 = pfVar11 + 1;
    *param_6 = *param_5;
    local_28c = (float)((int)local_28c + 1);
    *param_5 = (float)((int)uVar5 >> (bVar6 & 0x1f)) - fVar1;
  } while ((int)local_28c < 0x1c);
  return;
}
