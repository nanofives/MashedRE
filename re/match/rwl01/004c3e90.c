// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 44.3% (regs 45.0%), class b. Fast-sqrt LUT builder: float bits via union, sqrt intrinsic (fsqrt), fdivr of the double at 0x005cd0c8, real function-pointer stores, 2-dword record array. 13.2 -> 44.3. Residual: original loops are not optimised (value kept in [esp] and param_2's stack slot [esp+0x10] reused as the result temp, plain inc/cmp loop), ours strength-reduces and unrolls; tried param_2 as the temp, no change. */
/* MATCH-RWL01 0x004c3e90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef union FI { float f; int i; } FI;
double sqrt(double x);
#pragma intrinsic(sqrt)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);
MGLOBAL(int, DAT_007d4000, 0x007d4000);
MGLOBAL(double, DAT_005cd0c8, 0x005cd0c8);
void FUN_004c3670(void);
void FUN_004c3730(void);
void FUN_004c37d0(void);
void FUN_004c3880(void);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004c3e90 FUN_004c3e90
undefined4 FUN_004c3e90(undefined4 param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  FI v;
  undefined4 uStack_8[2];

  DAT_007d3ffc = param_2;
  *(int *)(DAT_007d3ff8 + 8 + param_2) = (int)FUN_004c3730;
  *(int *)(DAT_007d3ff8 + 0xc + DAT_007d3ffc) = (int)FUN_004c3670;
  *(int *)(DAT_007d3ff8 + 0x10 + DAT_007d3ffc) = (int)FUN_004c3880;
  *(int *)(DAT_007d3ff8 + 0x14 + DAT_007d3ffc) = (int)FUN_004c37d0;
  iVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(0x4000,0x40401);
  if (iVar1 == 0) {
    uStack_8[0] = 1;
    uStack_8[1] = FUN_004d7ff0(0x80000013,0x4000);
    FUN_004d8480(uStack_8);
    return 0;
  }
  v.i = 0x3f800000;
  for (uVar2 = 0; uVar2 < 0x800; uVar2++) {
    *(float *)&param_2 = (float)sqrt(v.f);
    param_2 = param_2 - 0x1fc00000;
    *(int *)(iVar1 + 0x2000 + uVar2 * 4) = param_2;
    v.i = v.i + 0x1000;
  }
  for (uVar2 = 0; uVar2 < 0x800; uVar2++) {
    *(float *)&param_2 = (float)sqrt(v.f);
    param_2 = param_2 - 0x20000000;
    *(int *)(iVar1 + uVar2 * 4) = param_2;
    v.i = v.i + 0x1000;
  }
  *(int *)(DAT_007d3ff8 + DAT_007d3ffc) = iVar1;
  iVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(0x4000,0x40401);
  if (iVar1 == 0) {
    uStack_8[0] = 1;
    uStack_8[1] = FUN_004d7ff0(0x80000013,0x4000);
    FUN_004d8480(uStack_8);
    return 0;
  }
  v.i = 0x3f800000;
  for (uVar2 = 0; uVar2 < 0x800; uVar2++) {
    *(float *)&param_2 = (float)(DAT_005cd0c8 / sqrt(v.f));
    param_2 = param_2 - 0x20000000;
    *(int *)(iVar1 + 0x2000 + uVar2 * 4) = param_2;
    v.i = v.i + 0x1000;
  }
  for (uVar2 = 0; uVar2 < 0x800; uVar2++) {
    *(float *)&param_2 = (float)(DAT_005cd0c8 / sqrt(v.f));
    param_2 = param_2 - 0x1fc00000;
    *(int *)(iVar1 + uVar2 * 4) = param_2;
    v.i = v.i + 0x1000;
  }
  *(int *)(DAT_007d3ff8 + 4 + DAT_007d3ffc) = iVar1;
  DAT_007d4000 = DAT_007d4000 + 1;
  return param_1;
}
