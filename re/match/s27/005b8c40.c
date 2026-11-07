// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 30.0% (regs 31.0%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned __int64 ulonglong;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(undefined1, DAT_005ce268, 0x005ce268);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005ce990, 0x005ce990);
MGLOBAL(undefined1, DAT_005e7090, 0x005e7090);
MGLOBAL(undefined1, DAT_005e70d0, 0x005e70d0);
MGLOBAL(undefined1, DAT_005e70f0, 0x005e70f0);
MGLOBAL(undefined1, DAT_005e7100, 0x005e7100);
void FUN_005a9280(int * p0, undefined4 p1, float * p2, float * p3, int p4);
uint FUN_005aeb90();
uint FUN_005bc020();
undefined4 FUN_005c75b0();
double _pow(double p0, double p1);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005ce990, 0x005ce990);
MGLOBAL(int, _DAT_005ce268, 0x005ce268);
#ifdef __cplusplus
static int ROUND(double x) { return (int)(x + (x < 0 ? -0.5 : 0.5)); }
static int ABS(int x) { return x < 0 ? -x : x; }
#endif
// MATCH: 0x005b8c40 FUN_005b8c40
/* [C2 2026-06-02] `iVar7 = *(param_1+0x94)` (caps/device owner); `uVar10 = caps[+0x50] & 8`
   (hardware-mix select). - **Early out**: if? */

uint FUN_005b8c40(float param_1)

{
  uint *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  uint uVar4;
  int *piVar5;
  uint uVar6;
  int iVar7;
  double dVar11;
  int iVar8;
  uint uVar10;
  undefined2 in_FPUControlWord;
  float local_4c;
  undefined4 local_48;
  undefined4 uVar9;
  float local_44;
  float local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_20;
  undefined4 local_1c;
  float local_28;
  undefined4 local_18;
  undefined4 local_24;
  undefined4 uStack_10;
  undefined4 uStack_8;
  undefined4 uStack_c;
  undefined4 uStack_14;
  undefined4 uStack_4;
  
  uVar6 = (uint)param_1;
  uVar10 = *(uint *)(iVar7 + 0x50) & 8;
  iVar7 = *(int *)((int)param_1 + 0x94);
  if (*(int *)((int)param_1 + 0x84) == 0) {
    *(undefined4 *)((int)param_1 + 0x88) = 0;
    *(uint *)((int)param_1 + 0x28) = *(uint *)((int)param_1 + 0x28) & 0xfffffa03;
    return (uint)param_1;
  }
  if (((*(byte *)(*(int *)((int)param_1 + 0x84) + 0x54) & 2) == 0) || (*(int *)(iVar7 + 0x14) == 0))
  goto LAB_005b8e4d;
  if (uVar10 == 0) {
    local_18 = *(undefined4 *)((int)param_1 + 0xd4);
    local_34 = -*(float *)((int)param_1 + 0xa0);
    local_2c = *(undefined4 *)((int)param_1 + 0xa8);
    local_24 = *(undefined4 *)((int)param_1 + 0xb0);
    local_20 = *(undefined4 *)((int)param_1 + 0xb4);
    local_30 = *(undefined4 *)((int)param_1 + 0xa4);
    local_28 = -*(float *)((int)param_1 + 0xac);
    FUN_005a9280((int *)(iVar7 + 0x14),1,&local_34,&local_4c,*(undefined4 *)(iVar7 + 0x4c));
    local_1c = *(undefined4 *)((int)param_1 + 0x98);
    uVar4 = *(uint *)(uVar6 + 0x28);
    if ((uVar4 & 0x1000) == 0) {
      *(float *)(uVar6 + 0x38) =
           (local_4c - *(float *)(uVar6 + 0x38)) * _DAT_005cc32c + *(float *)(uVar6 + 0x38);
    }
    else {
      *(float *)(uVar6 + 0x38) = local_4c;
    }
    *(uint *)(uVar6 + 0x28) = uVar4 | 0x40;
    if ((*(byte *)(*(int *)(uVar6 + 0x94) + 0x50) & 0x40) != 0) {
      *(undefined4 *)(uVar6 + 0x3c) = local_48;
      *(uint *)(uVar6 + 0x28) = uVar4 | 0x60;
      param_1 = (float)(CONCAT22((*(ushort *)((char *)&param_1 + 2)),in_FPUControlWord) | 0xc00);
      (**(code **)(**(int **)(uVar6 + 0xdc) + 0x40))
                (*(int **)(uVar6 + 0xdc),
                 (int)ROUND((((local_44 + local_44) - _DAT_005cc320) * _DAT_005cc32c + _DAT_005cc320
                            ) * _DAT_005ce990 - _DAT_005ce990));
      goto LAB_005b8e4d;
    }
    if ((uVar4 & 4) == 0) goto LAB_005b8e4d;
    (**(code **)(**(int **)(uVar6 + 0xe0) + 0x30))(*(int **)(uVar6 + 0xe0),uVar6 + 0x9c,0);
  }
  else {
    if ((*(byte *)((int)param_1 + 0x28) & 4) == 0) goto LAB_005b8e4d;
    iVar7 = *(int *)((int)param_1 + 0x11c);
    *(undefined4 *)(iVar7 + 100) = *(undefined4 *)((int)param_1 + 0x98);
    *(undefined4 *)(iVar7 + 0x68) = *(undefined4 *)((int)param_1 + 0xd4);
    uVar9 = *(undefined4 *)((int)param_1 + 0xa4);
    uVar3 = *(undefined4 *)((int)param_1 + 0xa8);
    local_48 = *(undefined4 *)((int)param_1 + 0xb0);
    local_4c = -*(float *)((int)param_1 + 0xac);
    *(float *)(iVar7 + 0x4c) = -*(float *)((int)param_1 + 0xa0);
    *(undefined4 *)(iVar7 + 0x50) = uVar9;
    *(undefined4 *)(iVar7 + 0x54) = uVar3;
    local_44 = *(float *)((int)param_1 + 0xb4);
    *(float *)(iVar7 + 0x58) = local_4c;
    *(undefined4 *)(iVar7 + 0x5c) = local_48;
    *(float *)(iVar7 + 0x60) = local_44;
  }
  *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xfffffffb;
LAB_005b8e4d:
  if ((*(uint *)(uVar6 + 0x28) & 0x1000) != 0) {
    if ((*(uint *)(uVar6 + 0x28) & 0x20) == 0) {
      uVar4 = *(uint *)(*(int *)0x2c + (uVar6 + 0x84));
      *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) | 0x20;
      *(float *)(uVar6 + 0x2c) = (float)uVar4;
    }
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffefff;
  }
  if (((((*(byte *)(*(int *)(uVar6 + 0x94) + 0x50) & 7) != 0) &&
       ((*(uint *)(uVar6 + 0x28) & 8) != 0)) && ((*(uint *)(uVar6 + 0x28) & 1) != 0)) &&
     (piVar5 = *(int **)(uVar6 + 0xe4), piVar5 != (int *)0x0)) {
    (**(code **)(*piVar5 + 0x10))(piVar5,&DAT_005e7100,0,0,0,0xe8 + uVar6,0x34);
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xfffffff7;
  }
  if ((*(byte *)(uVar6 + 0x28) & 0x20) != 0) {
    if (!uVar10) {
      param_1 = (float)(CONCAT22((*(ushort *)((char *)&param_1 + 2)),in_FPUControlWord) | 0xc00);
      (**(code **)(**(int **)(uVar6 + 0xdc) + 0x44))
                (*(int **)(uVar6 + 0xdc),
                 (int)ROUND(*(float *)(uVar6 + 0x3c) * *(float *)(uVar6 + 0x2c)));
    }
    else {
      *(undefined4 *)(*(int *)(uVar6 + 0x11c) + 0x34) = *(undefined4 *)(uVar6 + 0x2c);
      puVar1 = (uint *)(*(int *)(uVar6 + 0x11c) + 0xcc);
      *puVar1 = *puVar1 | 4;
    }
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffffdf;
  }
  if ((*(byte *)(uVar6 + 0x28) & 0x40) != 0) {
    if (uVar10 == 0) {
      uVar9 = FUN_005aeb90();
      (**(code **)(**(int **)(uVar6 + 0xdc) + 0x3c))(*(int **)(uVar6 + 0xdc),uVar9);
    }
    else {
      *(undefined4 *)(*(int *)(uVar6 + 0x11c) + 0x3c) = *(undefined4 *)(uVar6 + 0x30);
      puVar1 = (uint *)(*(int *)(uVar6 + 0x11c) + 0xcc);
      *puVar1 = *puVar1 | 0x10;
    }
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffffbf;
  }
  if (((*(char *)(uVar6 + 0x28) < '\0') && (*(int *)(uVar6 + 0x84) != 0)) &&
     ((~*(uint *)(*(int *)(uVar6 + 0x84) + 0x54) >> 1 & 1) != 0)) {
    if (uVar10 == 0) {
      param_1 = (float)(CONCAT22((*(ushort *)((char *)&param_1 + 2)),in_FPUControlWord) | 0xc00);
      (**(code **)(**(int **)(uVar6 + 0xdc) + 0x40))
                (*(int **)(uVar6 + 0xdc),
                 (int)ROUND((*(float *)(uVar6 + 0x34) * _DAT_005cc32c + _DAT_005cc320) *
                            _DAT_005ce990 - _DAT_005ce990));
    }
    else {
      *(uint *)(*(int *)(uVar6 + 0x11c) + 0xcc) = *(uint *)(*(int *)(uVar6 + 0x11c) + 0xcc) | 2;
      *(undefined4 *)(*(int *)(uVar6 + 0x11c) + 0x48) = *(undefined4 *)(uVar6 + 0x34);
    }
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffff7f;
  }
  if (uVar10 != 0) {
    uVar9 = FUN_005c75b0();
    *(undefined4 *)(uVar6 + 0x88) = uVar9;
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xfffffaef;
    return uVar6;
  }
  puVar2 = (undefined4 *)(uVar6 + 0xdc);
  iVar7 = FUN_005bc020();
  if ((*(uint *)(uVar6 + 0x28) & 0x100) != 0) {
    if (iVar7 != 0) {
      (**(code **)(*(int *)*puVar2 + 0x48))();
      iVar7 = 0;
    }
    (**(code **)(*(int *)*puVar2 + 0x34))((int *)*puVar2,*(undefined4 *)(uVar6 + 0x90));
    *(undefined4 *)(uVar6 + 0x8c) = *(undefined4 *)(uVar6 + 0x90);
    uVar10 = *(uint *)(uVar6 + 0x28);
    *(uint *)(uVar6 + 0x28) = uVar10 & 0xfffffbff;
    if ((uVar10 & 0x200) != 0) {
      iVar8 = (**(code **)(*(int *)*puVar2 + 0x30))((int *)*puVar2,0,0,uVar10 >> 0xb & 1);
      if (-1 < iVar8) {
        iVar7 = 1;
      }
      *(undefined4 *)(uVar6 + 0x90) = 0;
    }
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xfffffcff;
  }
  if ((*(uint *)(uVar6 + 0x28) & 0x400) != 0) {
    (**(code **)(*(int *)*puVar2 + 0x34))((int *)*puVar2,*(undefined4 *)(uVar6 + 0x8c));
    *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xfffffbff;
  }
  puVar1 = (uint *)(uVar6 + 0x8c);
  if (iVar7 == 0) {
    *puVar1 = *(uint *)(uVar6 + 0x90);
  }
  else {
    (**(code **)(*(int *)*puVar2 + 0x10))((int *)*puVar2,puVar1,0);
  }
  *(int *)(uVar6 + 0x88) =
       (int)(((ulonglong)*puVar1 / (ulonglong)(*(byte *)(*(int *)0x38 + (uVar6 + 0x84)) >> 3)) /
            (ulonglong)*(byte *)(*(int *)(uVar6 + 0x84) + 0x39));
  if ((*(byte *)(uVar6 + 0x28) & 0x10) == 0) {
    return uVar6;
  }
  piVar5 = *(int **)(uVar6 + 0xe4);
  if (piVar5 != (int *)0x0) {
    iVar7 = *(int *)(uVar6 + 0x94);
    if ((*(byte *)(iVar7 + 0x50) & 1) != 0) {
      local_34 = *(float *)(uVar6 + 0x70);
      local_24 = *(undefined4 *)(uVar6 + 0x48);
      local_28 = *(float *)(uVar6 + 0x44);
      local_2c = *(undefined4 *)(uVar6 + 0x40);
      local_18 = *(undefined4 *)(uVar6 + 0x7c);
      local_1c = *(undefined4 *)(uVar6 + 0x78);
      uStack_14 = *(undefined4 *)(uVar6 + 0x80);
      local_30 = 0;
      (**(code **)(*piVar5 + 0x10))(piVar5,&DAT_005e70f0,0,0,0,&local_34,0x24);
      *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffffef;
      local_20 = *(undefined4 *)(uVar6 + 0x74);
      return uVar6;
    }
    if ((*(byte *)(iVar7 + 0x50) & 2) != 0) {
      param_1 = (float)dVar11;
      dVar11 = _pow(10.0,(double)((float)*(int *)(uVar6 + 0x40) * _DAT_005ce268));
      (**(code **)(**(int **)(uVar6 + 0xe4) + 0x10))
                (*(int **)(uVar6 + 0xe4),&DAT_005e7090,0,0,0,&param_1,4);
      *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffffef;
      return uVar6;
    }
    if ((*(byte *)(iVar7 + 0x50) & 4) != 0) {
      local_2c = *(undefined4 *)(uVar6 + 0x40);
      local_34 = *(float *)(uVar6 + 0x70);
      local_24 = *(undefined4 *)(uVar6 + 0x48);
      local_20 = *(undefined4 *)(uVar6 + 0x74);
      local_1c = *(undefined4 *)(uVar6 + 0x78);
      uStack_10 = 0x3f000000;
      uStack_14 = *(undefined4 *)(uVar6 + 0x80);
      local_28 = *(float *)(uVar6 + 0x44);
      uStack_8 = 0x3f800000;
      local_18 = *(undefined4 *)(uVar6 + 0x7c);
      local_30 = 0;
      uStack_c = local_28;
      uStack_4 = 1;
      (**(code **)(*piVar5 + 0x10))(piVar5,&DAT_005e70d0,1,0,0,&local_34,0x34);
    }
  }
  *(uint *)(uVar6 + 0x28) = *(uint *)(uVar6 + 0x28) & 0xffffffef;
  return uVar6;
}
