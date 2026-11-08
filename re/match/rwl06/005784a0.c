// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005784a0: hand-matched (h2). 69.6% regs 70.3% ops 96.4%. 0 < uVar1 guard (jbe) fixed the branch, scalar dx/dy/dz and a temp for the contact x give the original fsubr for x. Residual: orig spills dz to a stack slot and keeps the dot product in [1],[0],[2] order with a 0xc frame; ours keeps dz in a register (136 vs 138 instructions); term order and temps tried (6 permutations of the dot). Class b. */
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
typedef unsigned int uint;
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
// MATCH: 0x005784a0 FUN_005784a0
undefined4 FUN_005784a0(uint *param_1,float *param_2,float *param_3,float *param_4,float param_5)
{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float dx;
  float dy;
  float dz;
  float cx;
  float cy;
  float cz;

  uVar1 = *param_1;
  if (0 < uVar1) {
    cx = param_2[uVar1 * 4 - 4];
    dx = *param_3 - cx;
    dy = param_3[1] - param_2[uVar1 * 4 - 3];
    dz = param_3[2] - param_2[uVar1 * 4 - 2];
    fVar3 = -(param_4[2] * dz + dx * *param_4 + param_4[1] * dy);
    fVar2 = fVar3 * *param_4 + dx;
    fVar4 = fVar3 * param_4[1] + dy;
    fVar3 = fVar3 * param_4[2] + dz;
    if (fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < DAT_005ce54c) {
      return 0;
    }
  }
  if (1 < uVar1) {
    cx = *param_2;
    dx = *param_3 - cx;
    dy = param_3[1] - param_2[1];
    dz = param_3[2] - param_2[2];
    fVar3 = -(param_4[2] * dz + dx * *param_4 + param_4[1] * dy);
    fVar2 = fVar3 * *param_4 + dx;
    fVar4 = fVar3 * param_4[1] + dy;
    fVar3 = fVar3 * param_4[2] + dz;
    if (fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < DAT_005ce54c) {
      return 1;
    }
  }
  param_5 = param_5 - (param_3[2] * param_4[2] + *param_3 * *param_4 + param_3[1] * param_4[1]);
  param_2[uVar1 * 4] = param_5 * *param_4 + *param_3;
  param_2[*param_1 * 4 + 1] = param_5 * param_4[1] + param_3[1];
  param_2[*param_1 * 4 + 2] = param_5 * param_4[2] + param_3[2];
  *param_1 = *param_1 + 1;
  return 0;
}
