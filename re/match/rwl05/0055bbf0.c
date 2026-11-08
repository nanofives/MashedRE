// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055bbf0: hand-matched (h1). 15.9% (regs 25.8%, ops 38.6%), 132/126 ins. Raw converter body (quaternion-style rotate + transform through plugin vtable +0xc), same x87 class as 0x0055bae0; not worked. Class (a)/(b). code typedef variadic under C++. */
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
typedef unsigned char byte;
typedef long double float10;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
undefined4 FUN_00546b10(float * p0, float * p1);
// MATCH: 0x0055bbf0 FUN_0055bbf0
float10 FUN_0055bbf0(float *param_1,int param_2,float *param_3)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float10 fVar8;
  float local_2c;
  float fStack_28;
  float fStack_24;
  float fStack_20;
  float fStack_1c;
  float fStack_18;
  float fStack_14;
  
  fVar8 = (float10)(**(code **)((int)param_1[0x17] + 0xc))(param_1,param_2,&local_2c);
  if ((*(byte *)((int)param_1[0x17] + 0x40) & 2) == 0) {
    FUN_00546b10(&fStack_20,param_1);
    fVar2 = *(float *)(param_2 + 0x10);
    pfVar1 = (float *)(param_2 + 0xc);
    fVar3 = *pfVar1;
    fVar4 = *(float *)(param_2 + 0x14);
    fVar5 = *pfVar1;
    fVar6 = *(float *)(param_2 + 0x10);
    fVar7 = *pfVar1;
    *pfVar1 = *(float *)(param_2 + 0x18) * fStack_20 +
              *pfVar1 * fStack_14 +
              (*(float *)(param_2 + 0x14) * fStack_1c - *(float *)(param_2 + 0x10) * fStack_18);
    *(float *)(param_2 + 0x10) =
         *(float *)(param_2 + 0x18) * fStack_1c +
         *(float *)(param_2 + 0x10) * fStack_14 +
         (fVar5 * fStack_18 - *(float *)(param_2 + 0x14) * fStack_20);
    *(float *)(param_2 + 0x14) =
         *(float *)(param_2 + 0x18) * fStack_18 +
         *(float *)(param_2 + 0x14) * fStack_14 + (fVar6 * fStack_20 - fVar7 * fStack_1c);
    *(float *)(param_2 + 0x18) =
         fStack_14 * *(float *)(param_2 + 0x18) -
         (fVar4 * fStack_18 + fVar3 * fStack_20 + fVar2 * fStack_1c);
    fVar2 = param_1[9];
    fVar3 = param_1[5];
    fVar4 = param_1[1];
    fVar5 = param_1[10];
    fVar6 = param_1[6];
    fVar7 = param_1[2];
    *param_3 = param_1[0xc] + local_2c * *param_1 + fStack_28 * param_1[4] + fStack_24 * param_1[8];
    param_3[1] = param_1[0xd] + local_2c * fVar4 + fStack_28 * fVar3 + fStack_24 * fVar2;
    param_3[2] = param_1[0xe] + local_2c * fVar7 + fStack_28 * fVar6 + fStack_24 * fVar5;
  }
  return (float10)(float)fVar8;
}
