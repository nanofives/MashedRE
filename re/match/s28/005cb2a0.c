// FLAGS: /O1
/* MATCH-S28 h1 0x005cb2a0 : 15.6% (regs 17.5, ops 29) at O1: raw converter output kept. The original is hand-written x87 assembly (push ebp frame saving eax/ebx/ecx, operands kept on the FPU stack with fmul st(n), fxch) for a 4x3 matrix multiply (param_1 = param_2 * param_3); the C compiler orders and spills these products differently. Residual class (c), assembly source. */

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

typedef unsigned int undefined4;
// MATCH: 0x005cb2a0 FUN_005cb2a0

undefined4 FUN_005cb2a0(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  undefined4 in_EAX;
  
  fVar1 = param_2[2];
  fVar2 = param_2[1];
  fVar3 = *param_2;
  fVar4 = param_3[1];
  *param_1 = param_3[8] * fVar1 + param_3[4] * fVar2 + *param_3 * fVar3;
  fVar5 = param_3[2];
  param_1[1] = param_3[9] * fVar1 + param_3[5] * fVar2 + fVar4 * fVar3;
  fVar4 = param_2[6];
  fVar6 = param_2[5];
  fVar7 = param_2[4];
  param_1[2] = fVar5 * fVar3 + param_3[6] * fVar2 + param_3[10] * fVar1;
  fVar1 = param_3[1];
  param_1[4] = param_3[8] * fVar4 + param_3[4] * fVar6 + *param_3 * fVar7;
  fVar2 = param_3[2];
  param_1[5] = param_3[9] * fVar4 + param_3[5] * fVar6 + fVar1 * fVar7;
  fVar1 = param_2[10];
  fVar3 = param_2[9];
  fVar5 = param_2[8];
  param_1[6] = fVar2 * fVar7 + param_3[6] * fVar6 + param_3[10] * fVar4;
  fVar2 = param_3[1];
  param_1[8] = param_3[8] * fVar1 + param_3[4] * fVar3 + *param_3 * fVar5;
  fVar4 = param_3[2];
  param_1[9] = param_3[9] * fVar1 + param_3[5] * fVar3 + fVar2 * fVar5;
  fVar2 = param_2[0xe];
  fVar6 = param_2[0xd];
  fVar7 = param_2[0xc];
  param_1[10] = fVar4 * fVar5 + param_3[6] * fVar3 + param_3[10] * fVar1;
  fVar1 = param_3[1];
  param_1[0xc] = param_3[0xc] + param_3[8] * fVar2 + param_3[4] * fVar6 + *param_3 * fVar7;
  fVar3 = param_3[2];
  param_1[0xd] = param_3[0xd] + param_3[9] * fVar2 + param_3[5] * fVar6 + fVar1 * fVar7;
  param_1[0xe] = param_3[0xe] + fVar3 * fVar7 + param_3[6] * fVar6 + param_3[10] * fVar2;
  return in_EAX;
}

