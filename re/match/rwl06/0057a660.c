// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0057a660: hand pass (h2), body kept in raw form. 26.6% regs 30.7% ops 63.9% under /O2 /Ob1 /G5. Compiles after: 0x0055bd70 prototype has 4 args, the 24-byte byte array + six scalar locals are float box/local arrays, float fBest replaces Ghidra's param_2-as-float reuse. The original copies *pfVar5 with dword moves (a 12-byte vector struct copy), ours loads floats: source shape not found. Class b. */
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
typedef unsigned char byte;
typedef long double float10;
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
void FUN_0055bd70(int p0, int p1, int p2, float * p3);
float FUN_005667c0(float * p0, float * p1);
float FUN_00579d50(float * p0, float * p1, int p2, int * p3, float * p4);
int FUN_0057ae20(int p0);
// MATCH: 0x0057a660 FUN_0057a660
/* [C1 2026-05-20] Signature recovered: `float FUN_0057a660(int *body_pair, float *out_axis)` —
   initial-axis selection
 */

float FUN_0057a660(int *param_1,float *param_2)

{
  int iVar1;
  int iVar2;
  float *pfVar3;
  float fVar4;
  float *pfVar5;
  float fVar6;
  float fVar7;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38[6];
  float box[7];
  float fBest;
  
  pfVar3 = param_2;
  iVar1 = *(int *)(*param_1 + 8);
  if ((**(short **)(iVar1 + 0x5c) == 4) && ((*(byte *)(iVar1 + 0x48) & 1) != 0)) {
    pfVar5 = (float *)FUN_0057ae20(iVar1);
    local_50 = *pfVar5;
    local_4c = pfVar5[1];
    local_44 = local_50 * DAT_005cc33c;
    local_48 = pfVar5[2];
    local_40 = local_4c * DAT_005cc33c;
    *param_2 = local_44;
    local_3c = local_48 * DAT_005cc33c;
    param_2[1] = local_40;
    param_2[2] = local_3c;
  }
  else {
    iVar2 = *(int *)(param_1[1] + 8);
    if ((**(short **)(iVar2 + 0x5c) == 4) && ((*(byte *)(iVar2 + 0x48) & 1) != 0)) {
      pfVar5 = (float *)FUN_0057ae20(iVar2);
      local_44 = *pfVar5;
      local_40 = pfVar5[1];
      local_3c = pfVar5[2];
      *param_2 = local_44;
      param_2[1] = local_40;
      param_2[2] = local_3c;
    }
    else {
      FUN_0055bd70(iVar1,*(int *)(*param_1 + 0x10),0,box);
      local_44 = (box[0] - box[4]) * DAT_005cc32c + box[4];
      local_40 = (box[1] - box[5]) * DAT_005cc32c + box[5];
      local_3c = (box[2] - box[6]) * DAT_005cc32c + box[6];
      FUN_0055bd70(*(int *)(param_1[1] + 8),*(int *)(param_1[1] + 0x10),0,box);
      local_50 = (box[0] - box[4]) * DAT_005cc32c + box[4];
      local_4c = (box[1] - box[5]) * DAT_005cc32c + box[5];
      local_48 = (box[2] - box[6]) * DAT_005cc32c + box[6];
      local_44 = local_44 - local_50;
      local_40 = local_40 - local_4c;
      local_3c = local_3c - local_48;
      FUN_005667c0(param_2,&local_44);
    }
  }
  fVar6 = FUN_00579d50(local_38,param_2,0xffffffff,param_1,0);
  local_50 = -1.0;
  if (DAT_005d757c <= local_44) {
    local_50 = 1.0;
  }
  local_4c = 0.0;
  local_48 = 0.0;
  fVar7 = FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  fVar4 = -fVar6;
  if (-fVar7 < -fVar6) {
    *param_2 = local_50;
    param_2[1] = local_4c;
    param_2[2] = local_48;
    fVar4 = -fVar7;
  }
  fBest = fVar4;
  local_50 = 0.0;
  local_4c = -1.0;
  if (DAT_005d757c <= local_40) {
    local_4c = 1.0;
  }
  local_48 = 0.0;
  fVar6 = FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  if (-fVar6 < fBest) {
    fBest = -fVar6;
    *pfVar3 = local_50;
    pfVar3[1] = local_4c;
    pfVar3[2] = local_48;
  }
  local_50 = 0.0;
  local_4c = 0.0;
  local_48 = -1.0;
  if (DAT_005d757c <= local_3c) {
    local_48 = 1.0;
  }
  fVar6 = FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  if (fBest <= -fVar6) {
    return fBest;
  }
  *pfVar3 = local_50;
  pfVar3[1] = local_4c;
  pfVar3[2] = local_48;
  return -fVar6;
}
