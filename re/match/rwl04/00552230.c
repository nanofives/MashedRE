// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00552230 (h2): 100.0% (regs 100). Raw fpatan -> intrinsic atan2 (#pragma intrinsic(atan2)), double product (float)((double)*param_1 * (double)DAT) gives fld [esi]; fmul [g]; vectors as float arrays; callee 4c4d20 float arg 3. */
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
typedef long double float10;
MGLOBAL(undefined1, DAT_005e488c, 0x005e488c);
MGLOBAL(undefined1, DAT_005e4898, 0x005e4898);
MGLOBAL(float, DAT_005e48a8, 0x005e48a8);
undefined4 FUN_004c4d20(float * p0, float * p1, float p2, int p3);
void FUN_004c51a0(float * p0, undefined1 * p1, int p2);
double atan2(double, double);
#pragma intrinsic(atan2)
// MATCH: 0x00552230 FUN_00552230
float * FUN_00552230(float *param_1,float *param_2)
{
  int iVar1;
  float *pfVar2;
  float10 fVar3;
  float local_4c[3];
  float local_40 [16];

  pfVar2 = local_40;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pfVar2 = *param_2;
    param_2 = param_2 + 1;
    pfVar2 = pfVar2 + 1;
  }
  fVar3 = atan2(-(double)local_40[4], (double)local_40[0]);
  local_4c[0] = 0; local_4c[1] = 0; local_4c[2] = 1.0f;
  *param_1 = (float)fVar3;
  FUN_004c51a0(local_40,&DAT_005e488c,2);
  FUN_004c4d20(local_40,local_4c,(float)((double)*param_1 * (double)DAT_005e48a8),2);
  FUN_004c51a0(local_40,&DAT_005e4898,2);
  param_1[1] = local_40[0];
  param_1[3] = local_40[1];
  param_1[2] = local_40[5];
  param_1[4] = local_40[12];
  param_1[5] = local_40[13];
  return param_1;
}
