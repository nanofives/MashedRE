// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00578d90: hand-matched (h2). 78.3% regs 78.3% ops 95.7%. Rewritten in float (0x005667c0 returns float; 0x005ceabc and 0x005cc558 are float constants, not a pointer / int as Ghidra typed them), (float)fabs() compares, && form with early return gives the orig and eax,0x4100/jne branches. Residual: in the first dot product the original loads terms [1],[0] then the one using the delta kept on the x87 stack (fmul st(2)); ours puts the delta term first, and the result spill (fstp+fstp st0+fld) differs. Tried 6 spellings of the negation, term order. Class b. */
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
MGLOBAL(float, DAT_005ceabc, 0x005ceabc);
MGLOBAL(float, DAT_005cc558, 0x005cc558);
float FUN_005667c0(float * p0, float * p1);
void FUN_00566830(float * p0, float * p1);
double fabs(double x);
#pragma intrinsic(fabs)
// MATCH: 0x00578d90 FUN_00578d90
void FUN_00578d90(float *param_1,float *param_2,float *param_3,float *param_4)
{
  float fVar1;
  float fVar2;
  float fVar3;

  *param_4 = *param_1 - *param_2;
  param_4[1] = param_1[1] - param_2[1];
  fVar2 = param_1[2] - param_2[2];
  param_4[2] = fVar2;
  fVar1 = -(param_3[1] * param_4[1] + *param_3 * *param_4 + param_3[2] * fVar2);
  *param_4 = fVar1 * *param_3 + *param_4;
  param_4[1] = param_3[1] * fVar1 + param_4[1];
  param_4[2] = param_3[2] * fVar1 + param_4[2];
  fVar3 = FUN_005667c0(param_4,param_4);
  if ((float)fabs(fVar1) > DAT_005ceabc && fVar3 / (float)fabs(fVar1) > DAT_005cc558) {
    return;
  }
  FUN_00566830(param_4,param_3);
  FUN_005667c0(param_4,param_4);
}
