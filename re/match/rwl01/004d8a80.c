// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d8a80: 100% (regs 100%). Callee FUN_004d7ff0 takes 2 args, two separate 2-word stack arrays, size is unsigned and tested as 0 < size with the non-zero branch first. */
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
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ad8, 0x00911ad8);
MGLOBAL(int, DAT_00911adc, 0x00911adc);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004e2fa0(uint * p0, int p1);
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
// MATCH: 0x004d8a80 FUN_004d8a80
undefined4 FUN_004d8a80(undefined4 param_1,int param_2)
{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  uint iVar4;
  int iVar5;
  undefined4 loc1[2];
  undefined4 loc2[2];
  DAT_00911ad8 = param_2;
  iVar4 = *(uint *)(DAT_007d3ff8 + 0x128);
  piVar3 = (int *)(DAT_007d3ff8 + param_2);
  if (0 < iVar4) {
    iVar5 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar4,0x4040b);
    piVar3[3] = iVar5;
    if (iVar5 == 0) {
      loc1[0] = 1;
      loc1[1] = FUN_004d7ff0(0x80000013,iVar4);
      FUN_004d8480(loc1);
      return 0;
    }
    iVar5 = FUN_004e2fa0((uint *)iVar5,iVar4);
    if (iVar5 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3[3]);
      loc2[0] = 1;
      loc2[1] = FUN_004d7ff0(0xc,0);
      FUN_004d8480(loc2);
      return 0;
    }
  }
  else {
    piVar3[3] = 0;
  }
  piVar1 = piVar3 + 4;
  piVar2 = piVar3 + 6;
  piVar3[5] = (int)piVar1;
  piVar3[7] = (int)piVar2;
  *piVar1 = (int)piVar1;
  *piVar2 = (int)piVar2;
  piVar3[9] = (int)piVar1;
  piVar3[8] = (int)piVar2;
  *piVar3 = iVar4;
  piVar3[1] = 0;
  piVar3[2] = 0;
  DAT_00911adc = DAT_00911adc + 1;
  return param_1;
}
