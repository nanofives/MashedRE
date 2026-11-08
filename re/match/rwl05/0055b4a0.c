// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055b4a0 (h2): permuter + sqrt intrinsic (raw SQRT was an implicit int call), 50.8%, class (b). */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
void FUN_005655a0(float * p0, float * p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
double sqrt(double);
#pragma intrinsic(sqrt)
// MATCH: 0x0055b4a0 FUN_0055b4a0
/* [C1 2026-05-19] Validates `param_2[0..2] != DAT_005d757c` (cited 0x0055b4a6..0x0055b4ba) —
   guards against degenerate inertia. */

int * FUN_0055b4a0(int *param_1,float *param_2)

{
  undefined4 *puVar1;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_50;
  float local_4c;
  undefined4 local_3c;
  float local_54;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_48;
  undefined4 local_38;
  undefined4 local_28;
  undefined4 local_10;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_14;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_18;
  undefined4 local_4;
  
  if (((*param_2 != DAT_005d757c) && (param_2[1] != DAT_005d757c)) && (param_2[2] != DAT_005d757c))
  {
    local_64 = _DAT_005cc320 / sqrt(*param_2);
    local_4c = param_2[6];
    local_54 = param_2[4];
    local_50 = param_2[5];
    local_58 = param_2[3];
    local_60 = _DAT_005cc320 / sqrt(param_2[1]);
    *puVar1 = local_48;
    FUN_005655a0(&local_48,param_2);
    FUN_005655a0(&local_24,&local_64);
    local_5c = _DAT_005cc320 / sqrt(param_2[2]);
    puVar1 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0x30 * param_1[1]);
    puVar1[1] = local_44;
    puVar1 = (undefined4 *)(*(int *)(*(int *)0x20 + (*param_1 + 0x10)) + 0x10 + param_1[1] * 0x30);
    *puVar1 = local_3c;
    puVar1[1] = local_38;
    puVar1[2] = local_34;
    puVar1 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0x20 + param_1[1] * 0x30);
    puVar1[2] = local_40;
    *puVar1 = local_30;
    puVar1[1] = local_2c;
    puVar1[2] = local_28;
    puVar1 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + param_1[1] * 0x30);
    *puVar1 = local_24;
    puVar1[1] = local_20;
    puVar1 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0x10 + param_1[1] * 0x30);
    puVar1[2] = local_1c;
    *puVar1 = local_18;
    puVar1[1] = local_14;
    puVar1[2] = local_10;
    puVar1 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0x20 + param_1[1] * 0x30);
    *puVar1 = local_c;
    puVar1[1] = local_8;
    puVar1[2] = local_4;
    return param_1;
  }
  return (int *)0x0;
}
