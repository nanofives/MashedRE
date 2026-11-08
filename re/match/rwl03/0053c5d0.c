// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053c5d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
int FUN_004c0ed0(int p0);
undefined4 FUN_004c3d90(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
float * FUN_004c4dc0(void * p0, undefined4 p1);
void FUN_0053a7a0(int p0, void * p1, undefined4 p2, undefined4 p3, undefined4 p4);
void FUN_0053c6e0(int p0, int p1, void * p2, undefined4 p3, undefined4 p4, undefined4 p5);
void FUN_0053ccc0(int p0, void * p1, undefined4 p2, undefined4 p3, undefined4 p4);
int RpAtomicGetWorldBoundingSphere(int p0);
typedef struct { undefined4 a, b, c, d; } Sph;
// MATCH: 0x0053c5d0 FUN_0053c5d0
int FUN_0053c5d0(int param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  struct { undefined4 p[2]; undefined1 m[64]; } L;
  Sph sph;

  switch (param_2[6]) {
  case 1:
    iVar1 = *(int *)(param_1 + 0x18);
    uVar3 = FUN_004c0ed0(*(undefined4 *)(param_1 + 4));
    FUN_004c4dc0(L.m,uVar3);
    FUN_004c3d90((undefined4)&sph,(undefined4)param_2,2,(undefined4)L.m);
    if (1 < *(int *)(iVar1 + 0x18)) {
      FUN_0053c6e0(iVar1,param_1 + 0x4c,&sph,(undefined4)param_2,param_3,param_4);
    } else {
      FUN_0053a7a0(iVar1,&sph,(undefined4)param_2,param_3,param_4);
    }
    return param_1;
  case 3:
    FUN_0053ccc0(param_1,param_2,(undefined4)param_2,param_3,param_4);
    return param_1;
  case 5:
    puVar2 = (undefined4 *)RpAtomicGetWorldBoundingSphere(*param_2);
    sph.a = puVar2[0];
    sph.b = puVar2[1];
    sph.c = puVar2[2];
    sph.d = puVar2[3];
    FUN_0053ccc0(param_1,&sph,(undefined4)param_2,param_3,param_4);
    return param_1;
  }
  return param_1;
}
