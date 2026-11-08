// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL05 0x0055b080: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc558, 0x005cc558);
MGLOBAL(undefined1, DAT_005cd0c8, 0x005cd0c8);
void FUN_004c51a0(float * p0, float * p1, int p2);
undefined4 FUN_00546b10(float * p0, float * p1);
int * FUN_0055b4a0(int * p0, float * p1);
MGLOBAL(int, _DAT_005cd0c8, 0x005cd0c8);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc558, 0x005cc558);
// MATCH: 0x0055b080 FUN_0055b080
/* [C1 2026-05-19] Stores world ptr `*param_1 = param_2` (cited 0x0055b083). */

int * FUN_0055b080(int *param_1,int param_2,int param_3,undefined4 *param_4)

{
  uint *puVar1;
  float fVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  
  *param_1 = param_2;
  if (param_3 == 0) {
    fVar2 = (float)_DAT_005cd0c8;
    *(undefined4 *)(*(int *)(*(int *)(param_2 + 0x10) + 0x18) + param_1[1] * 4) = 0;
    *(undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0xc + param_1[1] * 0x30) =
         0x3f800000;
    fVar2 = _DAT_005cc320 / SQRT(fVar2);
    *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0xc + param_1[1] * 0x30) = fVar2;
    *(undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0x2c + param_1[1] * 0x30) =
         0x3f800000;
    *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0x2c + param_1[1] * 0x30) = fVar2;
    param_1[5] = 0x3c23d70a;
    param_1[6] = 0x3c23d70a;
    puVar4 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x50) + param_1[1] * 0x10);
    *puVar4 = 0;
    puVar4[1] = 0;
    puVar4[2] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1[4] = 0;
  }
  else {
    *(undefined4 *)(*(int *)(*(int *)(param_2 + 0x10) + 0x18) + param_1[1] * 4) =
         *(undefined4 *)(param_3 + 0x48);
    fVar2 = *(float *)(param_3 + 0x10);
    *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0xc + param_1[1] * 0x30) = fVar2;
    *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0xc + param_1[1] * 0x30) =
         _DAT_005cc320 / SQRT(fVar2);
    if ((*(byte *)(param_3 + 0x48) & 2) == 0) {
      fVar2 = *(float *)(param_3 + 0x30);
      *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x20) + 0x2c + param_1[1] * 0x30) = fVar2;
      *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x28) + 0x2c + param_1[1] * 0x30) =
           _DAT_005cc320 / SQRT(fVar2);
    }
    else {
      FUN_0055b4a0(param_1,param_3 + 0x14);
    }
    param_1[5] = *(int *)(param_3 + 0x34);
    param_1[6] = *(int *)(param_3 + 0x38);
    puVar4 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x50) + param_1[1] * 0x10);
    *puVar4 = *(undefined4 *)(param_3 + 0x3c);
    puVar4[1] = *(undefined4 *)(param_3 + 0x40);
    puVar4[2] = *(undefined4 *)(param_3 + 0x44);
    param_1[2] = *(int *)(param_3 + 4);
    param_1[3] = *(int *)(param_3 + 8);
    param_1[4] = *(int *)(param_3 + 0xc);
  }
  if (param_4 == (undefined4 *)0x0) {
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x28 + param_1[1] * 0x40) = 0x3f800000;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x14 + param_1[1] * 0x40) = 0x3f800000;
    *(undefined4 *)(param_1[1] * 0x40 + **(int **)(*param_1 + 0x10)) = 0x3f800000;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x10 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 8 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 4 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x24 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x20 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x18 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x38 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x34 + param_1[1] * 0x40) = 0;
    *(undefined4 *)(**(int **)(*param_1 + 0x10) + 0x30 + param_1[1] * 0x40) = 0;
    puVar1 = (uint *)(**(int **)(*param_1 + 0x10) + 0xc + param_1[1] * 0x40);
    *puVar1 = *puVar1 | 0x20003;
    *(undefined4 *)(param_1[1] * 0x10 + *(int *)(*(int *)(*param_1 + 0x10) + 0x10)) = 0;
    *(undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x10) + 4 + param_1[1] * 0x10) = 0;
    *(undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x10) + 8 + param_1[1] * 0x10) = 0;
    *(undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x10) + 0xc + param_1[1] * 0x10) =
         0x3f800000;
  }
  else {
    puVar4 = (undefined4 *)(**(int **)(*param_1 + 0x10) + param_1[1] * 0x40);
    for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar4 = *param_4;
      param_4 = param_4 + 1;
      puVar4 = puVar4 + 1;
    }
    FUN_00546b10(param_1[1] * 0x10 + (*(int **)(*param_1 + 0x10))[4],
                 param_1[1] * 0x40 + **(int **)(*param_1 + 0x10));
  }
  FUN_004c51a0(param_1[1] * 0x40 + **(int **)(*param_1 + 0x10),param_1 + 2,1);
  param_1[7] = (int)(_DAT_005cc558 /
                    *(float *)(param_1[1] * 0x30 + 0xc + *(int *)(*(int *)(*param_1 + 0x10) + 0x20))
                    );
  puVar4 = (undefined4 *)(param_1[1] * 0x20 + *(int *)(*(int *)(*param_1 + 0x10) + 0x30));
  *puVar4 = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  iVar3 = *(int *)(*(int *)(*param_1 + 0x10) + 0x30);
  iVar5 = iVar3 + 0x10 + param_1[1] * 0x20;
  *(undefined4 *)(iVar3 + 0x10 + param_1[1] * 0x20) = 0;
  *(undefined4 *)(iVar5 + 4) = 0;
  *(undefined4 *)(iVar5 + 8) = 0;
  puVar4 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + param_1[1] * 0x20);
  *puVar4 = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4 = (undefined4 *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 0x10 + param_1[1] * 0x20);
  *puVar4 = 0;
  puVar4[1] = 0;
  puVar4[2] = 0;
  return param_1;
}
