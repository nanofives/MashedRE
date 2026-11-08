// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055b940: hand-matched (h3). 23.4% (nosize). float10 return, x87-heavy. Raw form with PTR_DAT_005ceabc retyped as float so it compiles. class (b), not worked. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, PTR_DAT_005ceabc, 0x005ceabc);
MGLOBAL(undefined1, DAT_005ceac0, 0x005ceac0);
float10 FUN_0055bbf0(float * p0, int p1, float * p2);
MGLOBAL(int, _DAT_005ceac0, 0x005ceac0);
// MATCH: 0x0055b940 FUN_0055b940
float10 FUN_0055b940(undefined4 *param_1,float param_2,int param_3)

{
  float puVar1;
  uint uVar2;
  float10 fVar3;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  float local_1c;
  float local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  fVar3 = (float10)FUN_0055bbf0((float *)*param_1,(int)&local_1c,(float *)&local_28);
  puVar1 = PTR_DAT_005ceabc;
  if (param_2 < DAT_005d757c) {
    param_2 = (float)-((float10)param_2 * fVar3);
  }
  param_1[4] = param_2;
  param_1[1] = local_28;
  param_1[2] = local_24;
  param_1[3] = local_20;
  if (param_3 == 0) {
    if (param_2 <= puVar1) goto LAB_0055ba67;
    if (local_18 <= local_1c) {
      if (local_1c < local_14) goto LAB_0055ba4e;
    }
    else {
      local_1c = local_18;
      if (local_18 <= local_14) {
LAB_0055ba4e:
        local_1c = local_14;
      }
    }
    uVar2 = param_1[0x12] & 0xfffffffd;
    param_1[0xc] = local_1c * param_2;
  }
  else {
    if (puVar1 < param_2) {
      local_1c = local_1c * param_2;
      local_18 = local_18 * param_2;
      local_14 = local_14 * param_2;
    }
    param_1[5] = local_1c;
    param_1[6] = local_18;
    param_1[7] = local_14;
    param_1[8] = local_10;
    param_1[9] = local_c;
    param_1[10] = local_8;
    uVar2 = param_1[0x12] | 2;
    param_1[0xb] = local_4;
  }
  param_1[0x12] = uVar2;
LAB_0055ba67:
  if ((float10)PTR_DAT_005ceabc <= fVar3) {
    return (float10)param_2 / fVar3;
  }
  return (float10)_DAT_005ceac0;
}
