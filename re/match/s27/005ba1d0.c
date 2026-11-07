// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 0x005ba1d0: 33.8% (regs 41.6%), raw converter body kept. Residual (b): 402 vs 461 instr, flags line /O2 /GS /Ob1 from the g2s sweep; no structural pass (budget).  C++ fixes: KR (variadic under C++) prototypes, int-typed pointer params, FUN_005bbc10 given its 6 params, casts on the garbage stack0x args. */
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
#define KR ...
#else
#define KR
#endif
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef int code(KR);
typedef void *HWND;
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
MGLOBAL(int, UNK_00000000, 0x00000000);
undefined4 DirectSoundCreate8(KR);
undefined4 FUN_005ba720(int p0);
void FUN_005ba760(int p0);
undefined4 FUN_005bac00(int p0);
void FUN_005baf00(int p0, undefined4 p1);
void FUN_005baf60(int p0, undefined4 p1);
undefined4 FUN_005baf90(int p0);
void FUN_005bb000(int p0, undefined4 * p1);
undefined4 FUN_005bbc10(undefined4 p0, byte * p1, int * p2, byte * p3, undefined4 p4, int ** p5);
uint FUN_005bbd50(int p0, int p1);
undefined4 FUN_005bbdb0(int p0, int * p1, int p2, undefined4 p3, uint p4, undefined4 * p5);
void FUN_005bbf30(int p0);
undefined4 FUN_005bbfc0(int p0);
undefined4 FUN_005bc400(void * p0, undefined4 * p1);
undefined4 FUN_005c7990(void * p0, undefined4 p1, undefined4 p2, undefined4 p3);
__declspec(dllimport) HWND __stdcall GetForegroundWindow(void);
MGLOBAL(int, stack0xffffff5b, 0xffffff5b);
MGLOBAL(int, stack0xffffff60, 0xffffff60);
MGLOBAL(int, stack0xffffff5a, 0xffffff5a);
MGLOBAL(int, stack0xffffff5c, 0xffffff5c);
MGLOBAL(int, iRam00000000, 0x00000000);
// MATCH: 0x005ba1d0 FUN_005ba1d0
int FUN_005ba1d0(int param_1,undefined4 param_2,undefined4 *param_3)

{
  byte bVar1;
  int iVar2;
  HWND pHVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  undefined4 uVar7;
  ushort uVar8;
  undefined4 *puVar9;
  int *piVar10;
  int iStack_9c;
  undefined *puStack_98;
  undefined4 uStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined4 uStack_88;
  uint uStack_84;
  undefined4 uStack_80;
  undefined4 uStack_7c;
  undefined4 uStack_78;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined4 auStack_6c [14];
  int iStack_34;
  
  iVar2 = FUN_005ba720(param_1);
  if (iVar2 == 0) {
    return 0;
  }
  puVar9 = (undefined4 *)(param_1 + 0x7c);
  *(undefined2 *)(param_1 + 0x28) = *(undefined2 *)((int)param_3 + 0x1a);
  *(undefined4 *)(param_1 + 0x30) = *param_3;
  *(undefined2 *)(param_1 + 0x34) = 0;
  *(undefined4 *)(param_1 + 0x38) = 0x3f800000;
  *(undefined4 *)(param_1 + 0x40) = 0x3f800000;
  *puVar9 = 0;
  *(undefined2 *)(param_1 + 0x36) = 0;
  *(undefined4 *)(param_1 + 0x78) = 0;
  *(undefined4 *)(param_1 + 0x11c) = 0;
  *(undefined4 *)(param_1 + 0x3c) = 0;
  *(undefined4 *)(param_1 + 0x10c) = 0;
  if (((param_3[3] != 0) && (param_3[4] != 0)) && (param_3[2] == 0)) {
    param_3[2] = 1;
  }
  if (((param_3[3] != 0) && (param_3[4] == 0)) && (param_3[2] != 0)) {
    param_3[4] = 1;
  }
  if (param_3[8] == 0) {
LAB_005ba27a:
    if (param_3[2] != 0) goto LAB_005ba27f;
  }
  else {
    if (param_3[2] == 0) {
      param_3[2] = 1;
      param_3[4] = 1;
      goto LAB_005ba27a;
    }
LAB_005ba27f:
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x20;
  }
  if (param_3[4] != 0) {
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x40;
  }
  if (param_3[8] != 0) {
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x400;
  }
  *(undefined4 *)(param_1 + 0x9c) = 0;
  *(undefined4 *)(param_1 + 0xa0) = 0;
  *(undefined4 *)(param_1 + 0xa4) = 0;
  FUN_005bb000(param_1,0);
  iVar2 = DirectSoundCreate8(param_3[1],puVar9,0);
  if (iVar2 < 0) {
    FUN_005ba760(param_1);
    return 0;
  }
  pHVar3 = *(HWND *)(param_1 + 0x30);
  if (pHVar3 == (HWND)0x0) {
    pHVar3 = GetForegroundWindow();
  }
  (**(code **)(*(int *)*puVar9 + 0x18))
            ((int *)*puVar9,pHVar3,2 - (uint)((*(uint *)(param_1 + 0x78) & 0x400) != 0));
  if ((*(uint *)(param_1 + 0x78) & 0x400) == 0) {
    uVar7 = param_3[5];
    bVar1 = *(byte *)(param_3 + 6);
    iVar2 = FUN_005bbc10(*(undefined4 *)(param_1 + 0x7c),(byte *)&stack0xffffff5b,&stack0xffffff60,
                         (byte *)&stack0xffffff5a,param_3[4] == 0,(int **)&stack0xffffff5c);
    if (iVar2 == 0) goto LAB_005ba376;
    *(undefined4 *)(param_1 + 0x2c) = uVar7;
    *(ushort *)(param_1 + 0x2a) = (ushort)bVar1;
    (**(code **)(iRam00000000 + 8))(0);
  }
  else {
LAB_005ba376:
    *(undefined4 *)(param_1 + 0x2c) = param_3[5];
    *(undefined2 *)(param_1 + 0x2a) = *(undefined2 *)(param_3 + 6);
  }
  iStack_9c = *(int *)(param_1 + 0x2c);
  uVar8 = 0x80;
  uStack_84 = uStack_84 & 0xffff0000;
  uStack_8c = 0;
  uStack_88 = 0;
  puStack_98 = &DAT_005e6424;
  uStack_94 = 4;
  (*(ushort *)((char *)&uStack_90 + 0)) = CONCAT11(1,*(undefined1 *)(param_1 + 0x2a));
  if (param_3[2] == 0) {
    puVar9 = auStack_6c;
    for (iVar2 = 0x18; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    auStack_6c[0] = 0x60;
    iVar2 = (**(code **)(**(int **)(param_1 + 0x7c) + 0x10))(*(int **)(param_1 + 0x7c),auStack_6c);
    if (iVar2 < 0) goto LAB_005ba5d5;
    if (iStack_34 == 0) {
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x20;
    }
    else {
      uVar8 = 0x84;
    }
  }
  if ((*(byte *)(param_1 + 0x78) & 0x20) != 0) {
    uVar8 = uVar8 | 8;
  }
  if ((*(uint *)(param_1 + 0x78) & 0x400) != 0) {
    uVar8 = CONCAT11(0x80,(char)uVar8);
  }
  iVar2 = param_1 + 0x80;
  iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&iStack_9c,1,1,uVar8,(undefined4 *)iVar2);
  if (iVar4 == 0) {
    if ((uVar8 & 4) != 0) {
      uVar8 = uVar8 & 0xfffb | 8;
    }
    iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&iStack_9c,1,1,uVar8,(undefined4 *)iVar2);
    if (iVar4 == 0) {
      (**(code **)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
      FUN_005ba760(param_1);
      return 0;
    }
  }
  if (param_3[3] != 0) {
    uVar5 = FUN_005bac00(iVar2);
    *(uint *)(param_1 + 0x78) = uVar5 | *(uint *)(param_1 + 0x78);
    if ((*(byte *)(param_1 + 0x78) & 0x27) == 0) {
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x20;
    }
  }
  FUN_005bbf30(iVar2);
  iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&iStack_9c,
                       ~*(uint *)(param_1 + 0x78) >> 6 & 1,1,uVar8,(undefined4 *)iVar2);
  if (iVar4 == 0) {
    (**(code **)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
    FUN_005ba760(param_1);
    return 0;
  }
  if ((param_3[3] != 0) && ((*(byte *)(param_1 + 0x78) & 0x20) != 0)) {
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 8;
  }
  if ((*(byte *)(param_1 + 0x78) & 8) == 0) {
    *(undefined4 *)(param_1 + 0x11c) = 0;
  }
  else {
    uStack_94 = param_3[5];
    puStack_98 = (undefined *)(uint)*(ushort *)((int)param_3 + 0x1a);
    uStack_90 = (uint)param_3[7] >> 1;
    iStack_9c = param_1;
    iVar4 = FUN_005c7990(&iStack_9c,4,0,0);
    *(int *)(param_1 + 0x11c) = iVar4;
    if (iVar4 == 0) {
      FUN_005bbf30(iVar2);
      (**(code **)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
      FUN_005ba760(param_1);
      return 0;
    }
  }
  if ((*(byte *)(param_1 + 0x78) & 0x40) != 0) {
LAB_005ba62f:
    iVar2 = FUN_005bbfc0(param_1 + 0x80);
    if (iVar2 != 0) {
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x10;
    }
    FUN_005baf00(param_1,0x3f800000);
    if (param_3[9] == 1) {
      FUN_005baf60(param_1,1);
    }
    else {
      uVar7 = FUN_005baf90(param_1);
      *(undefined4 *)(param_1 + 0x74) = uVar7;
      *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xfffffeff;
    }
    if ((*(byte *)(param_1 + 0x78) & 0xf) != 0) {
      uVar5 = *(uint *)(param_1 + 0x78);
      iStack_9c = -10000;
      puStack_98 = (undefined *)0xffffd8f0;
      uStack_94 = 0;
      uStack_90 = 0x3fbeb852;
      uStack_8c = 0x3f547ae1;
      uStack_88 = 0xfffff5d6;
      uStack_84 = 0x3be56042;
      uStack_80 = 200;
      uStack_7c = 0x3c343958;
      uStack_78 = 0x42c80000;
      uStack_74 = 0x42c80000;
      uStack_70 = 0x459c4000;
      piVar6 = &iStack_9c;
      piVar10 = (int *)(param_1 + 0x44);
      for (iVar2 = 0xc; iVar2 != 0; iVar2 = iVar2 + -1) {
        *piVar10 = *piVar6;
        piVar6 = piVar6 + 1;
        piVar10 = piVar10 + 1;
      }
      *(uint *)(param_1 + 0x78) = uVar5 | 0x80;
    }
    iVar2 = param_1 + 0x94;
    *(int *)(param_1 + 0x98) = iVar2;
    *(int *)iVar2 = iVar2;
    return param_1;
  }
  piVar6 = (int *)FUN_005bbd50(*(undefined4 *)(param_1 + 0x7c),1);
  if (piVar6 != (int *)0x0) {
    puVar9 = (undefined4 *)(param_1 + 0x8c);
    iVar2 = FUN_005bc400(piVar6,puVar9);
    (**(code **)(*piVar6 + 8))(piVar6);
    if (iVar2 != 0) {
      (**(code **)(*(int *)*puVar9 + 0x30))((int *)*puVar9,0x3f800000,0);
      (**(code **)(*(int *)*puVar9 + 0x40))((int *)*puVar9,0,0,0,0);
      (**(code **)(*(int *)*puVar9 + 0x34))((int *)*puVar9,0,0,0x3f800000,0,0x3f800000,0,0);
      goto LAB_005ba62f;
    }
  }
  FUN_005bb000(param_1,0);
  FUN_005bbf30(param_1 + 0x80);
LAB_005ba5d5:
  (**(code **)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
  FUN_005ba760(param_1);
  return 0;
}
