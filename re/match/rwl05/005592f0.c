// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x005592f0: hand-matched (h1). 29.9% (regs 29.9%, ops 19.2%), 129/167 ins. Raw converter body, not worked (610 B x87 interpolation, original has 38 more instructions than any form here produced; time budget). Class (b). */
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
// MATCH: 0x005592f0 FUN_005592f0
void FUN_005592f0(int param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  
  fVar7 = *param_3 / (float)*(int *)(*(int *)(param_1 + 0x60) + 0xc);
  fVar11 = param_3[1] / (float)*(int *)(*(int *)(param_1 + 0x60) + 0x10);
  fVar1 = *(float *)(param_1 + 0x140);
  fVar2 = *(float *)(param_1 + 0x14c);
  fVar3 = *(float *)(param_1 + 0x144);
  fVar4 = *(float *)(param_1 + 0x150);
  fVar5 = *(float *)(param_1 + 0x14c);
  fVar6 = *(float *)(param_1 + 0x150);
  fVar12 = (*(float *)(param_1 + 0x130) - *(float *)(param_1 + 0x124)) * fVar7 +
           *(float *)(param_1 + 0x124);
  fVar10 = *(float *)(param_1 + 0x128) +
           (*(float *)(param_1 + 0x134) - *(float *)(param_1 + 0x128)) * fVar7;
  fVar8 = *(float *)(param_1 + 300) +
          (*(float *)(param_1 + 0x138) - *(float *)(param_1 + 300)) * fVar7;
  fVar9 = (*(float *)(param_1 + 0x148) +
          (*(float *)(param_1 + 0x13c) - *(float *)(param_1 + 0x148)) * fVar7) - fVar12;
  *param_2 = fVar9;
  fVar2 = (fVar5 + (fVar1 - fVar2) * fVar7) - fVar10;
  param_2[1] = fVar2;
  fVar1 = (fVar6 + (fVar3 - fVar4) * fVar7) - fVar8;
  param_2[2] = fVar1;
  fVar2 = fVar2 * fVar11;
  fVar1 = fVar1 * fVar11;
  param_2[1] = fVar2;
  param_2[2] = fVar1;
  *param_2 = fVar9 * fVar11 + fVar12;
  param_2[1] = fVar2 + fVar10;
  param_2[2] = fVar1 + fVar8;
  fVar1 = *(float *)(param_1 + 0x170);
  fVar2 = *(float *)(param_1 + 0x17c);
  fVar3 = *(float *)(param_1 + 0x174);
  fVar4 = *(float *)(param_1 + 0x180);
  fVar5 = *(float *)(param_1 + 0x17c);
  fVar6 = *(float *)(param_1 + 0x180);
  fVar12 = *(float *)(param_1 + 0x154) +
           (*(float *)(param_1 + 0x160) - *(float *)(param_1 + 0x154)) * fVar7;
  fVar9 = *(float *)(param_1 + 0x158) +
          (*(float *)(param_1 + 0x164) - *(float *)(param_1 + 0x158)) * fVar7;
  fVar8 = *(float *)(param_1 + 0x15c) +
          (*(float *)(param_1 + 0x168) - *(float *)(param_1 + 0x15c)) * fVar7;
  fVar10 = (*(float *)(param_1 + 0x178) +
           (*(float *)(param_1 + 0x16c) - *(float *)(param_1 + 0x178)) * fVar7) - fVar12;
  param_2[3] = fVar10;
  fVar2 = (fVar5 + (fVar1 - fVar2) * fVar7) - fVar9;
  param_2[4] = fVar2;
  fVar1 = (fVar6 + (fVar3 - fVar4) * fVar7) - fVar8;
  param_2[5] = fVar1;
  fVar10 = fVar10 * fVar11;
  param_2[3] = fVar10;
  fVar2 = fVar2 * fVar11;
  param_2[4] = fVar2;
  fVar1 = fVar1 * fVar11;
  param_2[5] = fVar1;
  param_2[3] = fVar10 + fVar12;
  param_2[4] = fVar2 + fVar9;
  param_2[5] = fVar1 + fVar8;
  return;
}
