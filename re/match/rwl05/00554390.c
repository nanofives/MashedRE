// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00554390: hand-matched (h1). 33.5% (regs 45.8%, ops 38.7%), 372/439 ins. Raw converter body (1110 B model loader/reader), not worked; C++ pointer/int overloads added under __cplusplus. Class (b). */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00912a20, 0x00912a20);
MGLOBAL(undefined1, FUN_00554940, 0x00554940);
MGLOBAL(undefined1, FUN_005547f0, 0x005547f0);
void FUN_004c5c80(undefined4 p0);
undefined4 FUN_004c5ca0(void);
int FUN_004c5cb0(undefined4 p0, int p1);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
int * FUN_00554200(undefined4 p0);
undefined4 * FUN_005551d0(void);
undefined4 FUN_00555830(int p0);
int FUN_005c4c60(int * p0, int p1, undefined4 p2);
undefined4 FUN_005c4d30(undefined4 * p0);
#ifdef __cplusplus
/* C++ only: pointer/int argument overloads (the DDK C build passes both as one dword) */
static inline undefined4 FUN_004cc790(undefined4 a, void *b, undefined4 c) { return FUN_004cc790(a, (undefined4)(unsigned int)b, c); }
static inline undefined4 FUN_00555830(int *a) { return FUN_00555830((int)a); }
static inline int FUN_005c4c60(int a, int b, undefined4 c) { return FUN_005c4c60((int *)a, b, c); }
static inline undefined4 FUN_005c4d30(int a) { return FUN_005c4d30((undefined4 *)a); }
static inline uint FUN_004cbd30(int a, int b, uint c) { return FUN_004cbd30((undefined4 *)a, (undefined4 *)b, c); }
static inline int FUN_004c5cb0(undefined1 *a, int b) { return FUN_004c5cb0((undefined4)(unsigned int)a, b); }
#endif
// MATCH: 0x00554390 FUN_00554390
int * FUN_00554390(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  int *piVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  int iVar11;
  int iStack_30;
  uint local_2c;
  undefined4 uStack_28;
  undefined1 auStack_24 [36];
  
  piVar2 = (int *)FUN_005551d0();
  iVar8 = param_1;
  if (piVar2 == (int *)0x0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(param_1,&local_2c,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  if (local_2c >> 0x18 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 1,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 2,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 3,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 4,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 0x49,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 0x4c,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  piVar6 = piVar2 + 0x4a;
  iVar3 = FUN_004cc790(iVar8,piVar6,4);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  if (0 < *piVar6) {
    puVar4 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(*piVar6 * 2,0x30190);
    iVar3 = *piVar6;
    piVar2[0x4b] = (int)puVar4;
    for (uVar7 = (uint)(iVar3 * 2) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (uVar7 = iVar3 * 2 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined1 *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
    if (piVar2[0x4b] == 0) {
      FUN_00555830(piVar2);
      return (int *)0x0;
    }
    iVar3 = FUN_004cc790(iVar8,piVar2[0x4b],*piVar6 * 2);
    if (iVar3 == 0) {
      return (int *)0x0;
    }
  }
  iVar3 = FUN_004cc790(iVar8,piVar2 + 9,0x100);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  FUN_005c4c60(piVar2[0x4d],piVar2[0x4c],0x30190);
  puVar4 = (undefined4 *)FUN_005c4d30(piVar2[0x4d]);
  for (uVar7 = (uint)(piVar2[0x4c] << 5) >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  iVar9 = 0;
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar4 = 0;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
  }
  if (*piVar2 == 0) {
    uStack_28 = FUN_004c5ca0();
    FUN_004c5c80(DAT_00912a20);
    iVar3 = FUN_005c4d30(piVar2[0x4d]);
    if (0 < piVar2[0x4c]) {
      iVar11 = iVar3 + 0x1c;
      do {
        iVar5 = FUN_004cc790(iVar8,iVar11 + -0x14,0x10);
        if (((iVar5 == 0) || (iVar5 = FUN_004cc790(iVar8,iVar11 + -0x1c,4), iVar5 == 0)) ||
           (iVar5 = FUN_004cbd30(iVar8,iVar11,1), iVar5 != 1)) goto LAB_005547c4;
        *(undefined4 *)(iVar11 + -4) = 0;
        iVar9 = iVar9 + 1;
        iVar11 = iVar11 + 0x20;
      } while (iVar9 < piVar2[0x4c]);
    }
    iVar9 = FUN_004cc790(iVar8,&param_1,4);
    if (iVar9 == 0) {
LAB_005547c4:
      FUN_00555830(piVar2);
      piVar2 = (int *)0x0;
    }
    else {
      iVar9 = 0;
      if (0 < param_1) {
        piVar6 = piVar2 + 5;
        do {
          iVar11 = FUN_004cbd30(iVar8,(int)(unsigned int)auStack_24,0x20);
          if (iVar11 != 0x20) goto LAB_005547c4;
          iVar11 = FUN_004c5cb0(auStack_24,0);
          *piVar6 = iVar11;
          if (iVar11 == 0) goto LAB_005547c4;
          iVar9 = iVar9 + 1;
          piVar6 = piVar6 + 1;
        } while (iVar9 < param_1);
      }
      iVar8 = 0;
      if (0 < piVar2[0x4c]) {
        piVar6 = (int *)(iVar3 + 4);
        do {
          piVar1 = (int *)piVar2[(char)piVar6[6] + 5];
          *piVar6 = (int)piVar1;
          if (*piVar1 == 0) goto LAB_005547c4;
          iVar8 = iVar8 + 1;
          piVar6 = piVar6 + 8;
        } while (iVar8 < piVar2[0x4c]);
      }
      piVar2[0x4f] = 0;
      piVar2[0x50] = 0;
      piVar2[0x4e] = (int)FUN_00554940;
    }
    FUN_004c5c80(uStack_28);
    return piVar2;
  }
  iVar3 = FUN_005c4d30(piVar2[0x4d]);
  iStack_30 = 0;
  if (0 < piVar2[0x4c]) {
    do {
      uVar10 = 0;
      iVar9 = FUN_004cc790(iVar8,&param_1,4);
      if (((iVar9 == 0) || (iVar9 = FUN_004cc790(iVar8,iVar3,4), iVar9 == 0)) ||
         (iVar9 = FUN_004cc790(iVar8,&param_1,4), iVar9 == 0)) goto LAB_0055467e;
      if (param_1 == 5) {
        iVar9 = FUN_004cc5e0(iVar8,0x1a1,0,0);
        if (iVar9 == 0) goto LAB_0055467e;
        uVar10 = (undefined4)(unsigned int)FUN_00554200(iVar8);
        iVar9 = FUN_004cc790(iVar8,&param_1,4);
        if (iVar9 == 0) goto LAB_0055467e;
      }
      iVar9 = FUN_004cc790(iVar8,&param_1,4);
      if (iVar9 == 0) {
LAB_0055467e:
        FUN_00555830(piVar2);
        return (int *)0x0;
      }
      *(undefined4 *)(iVar3 + 0x18) = uVar10;
      iStack_30 = iStack_30 + 1;
      iVar3 = iVar3 + 0x20;
    } while (iStack_30 < piVar2[0x4c]);
  }
  piVar2[0x4e] = (int)FUN_005547f0;
  return piVar2;
}
