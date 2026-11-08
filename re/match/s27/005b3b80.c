// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b3b80: 43.4% (regs 53.0%). Struct copies (S7 for the 7-int copies), intrinsic memcpy for the two byte copies (31.8 -> 39.0%), permuter 300 s (-> 43.4%). Prototypes fixed for C++ (code typedef, uint* args). Residual: stack-aliased local_38[(int)param_1 - 4] indexing kept from Ghidra, 238 vs 249 ins, loop and slot layout. Class: b.
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
typedef uint code(void *, void *, int, void *, void *, void *);
typedef struct S7 { int a[7]; } S7;
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
void FUN_004522d0(undefined4 *p0);
undefined4 FUN_005ac5f0(uint *p0, uint *p1);
void FUN_005ac740(uint *p0);
void FUN_005ac980(undefined4 p0, uint *p1, int p2);
undefined4 *FUN_005aea50(uint p0, int p1, int p2);
undefined4 FUN_005b36f0(undefined4 *p0, int p1);
uint FUN_005b3760(int p0, int p1, int p2, int p3, int *p4, uint *p5, uint p6, uint p7, uint p8);
// MATCH: 0x005b3b80 FUN_005b3b80
uint FUN_005b3b80(uint *param_1,undefined4 *param_2,undefined4 param_3,uint *param_4,
                 undefined4 *param_5)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  uint *puVar11;
  undefined4 *puVar12;
  uint *puVar13;
  uint local_5c;
  uint uStack_50;
  undefined4 uStack_4c;
  int local_48 [4];
  uint local_38 [3];
  undefined1 local_2c;
  undefined1 local_2b;
  uint local_28;
  uint local_24;
  undefined1 local_20;
  undefined1 local_1f;
  uint local_1c [7];
  
  iVar2 = FUN_005ac5f0(param_1,param_4);
  if (iVar2 != 0) {
    memcpy(param_5,param_2,param_1[2]);
    return param_1[2];
  }
  uVar3 = FUN_005b36f0(param_1,1);
  puVar9 = (undefined4 *)0x0;
  uVar4 = FUN_005b36f0(param_4,0);
  uVar7 = FUN_005b3760(uVar3,*param_1,uVar4,*param_4,local_48,local_38,4,0,0xffffffff);
  uVar8 = param_1[2];
  *(S7 *)local_1c = *(S7 *)param_1;
  local_5c = 0;
  local_38[0] = 0;
  local_38[1] = 0;
  local_38[2] = 0;
  local_2c = 0;
  local_2b = 0;
  local_20 = 0;
  local_1f = 0;
  local_28 = 0;
  local_24 = 0;
  puVar5 = FUN_005aea50(uVar8,1,0x10806);
  if (puVar5 == (undefined4 *)0x0) {
    return 0;
  }
  uStack_50 = param_1[2];
  puVar10 = param_2;
  puVar12 = puVar5;
  for (uVar8 = uStack_50 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *puVar12 = *puVar10;
    puVar10 = puVar10 + 1;
    puVar12 = puVar12 + 1;
  }
  for (uVar8 = uStack_50 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined1 *)puVar12 = *(undefined1 *)puVar10;
    puVar10 = (undefined4 *)((int)puVar10 + 1);
    puVar12 = (undefined4 *)((int)puVar12 + 1);
  }
  puVar10 = puVar9;
  param_1 = (uint *)0x0;
  if (uVar7 != 0) {
    do {
      puVar9 = puVar5;
      FUN_005ac980(*(undefined4 *)(*(int *)local_38[(int)param_1 - 4] + 4),local_38,1);
      if (!((*(byte *)(*(int *)local_38[(int)param_1 - 4] + 8) & 1) == 0)) {
        local_38[0] = *param_4;
      } else {
        local_38[0] = local_1c[0];
      }
      local_38[2] = (*(code *)((int *)local_38[(int)param_1 - 4])[1])
                              (local_1c,param_2,param_3,local_38,0,&uStack_4c);
      puVar5 = puVar10;
      if (local_38[2] > local_5c) {
        if (puVar10 != (undefined4 *)0x0) {
          FUN_004522d0(puVar10);
        }
        puVar5 = FUN_005aea50(local_38[2],1,0x10806);
        if (puVar5 == (undefined4 *)0x0) goto LAB_005b3e3f;
        local_5c = local_38[2];
      }
      uVar1 = local_38[2];
      uVar6 = (**(code **)(local_38[(int)param_1 - 4] + 4))(local_1c,puVar9,0,local_38,puVar5,0);
      uVar8 = param_4[4];
      local_28 = uVar8;
      local_24 = param_4[5];
      if (uVar1 != uVar6) {
        if (puVar5 != (undefined4 *)0x0) {
          FUN_004522d0(puVar5);
        }
LAB_005b3e3f:
        if (puVar9 != (undefined4 *)0x0) {
          FUN_004522d0(puVar9);
        }
        return 0;
      }
      if (param_1 == (uint *)(uVar7 - 1)) {
        param_4[4] = local_28;
        param_4[5] = local_24;
      }
      else if (uVar8 == 0) {
        FUN_005ac740(local_38);
      }
      param_3 = uStack_4c;
      *(S7 *)local_1c = *(S7 *)local_38;
      uVar8 = uStack_50;
      uStack_50 = local_5c;
      puVar10 = puVar9;
      local_5c = uVar8;
      param_1 = (uint *)((int)param_1 + 1);
    } while ((uint)param_1 < uVar7);
  }
  memcpy(param_5,puVar5,local_38[2]);
  FUN_004522d0(puVar5);
  FUN_004522d0(puVar9);
  uVar8 = local_38[2];
  return uVar8;
}
