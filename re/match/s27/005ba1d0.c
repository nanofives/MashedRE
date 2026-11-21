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
int __stdcall FUN_005c1920(int p0, int **p1, int p2);   /* 0x005ba2c5: import thunk jmp [0x5cc01c] (DSOUND.dll ordinal 11) */
/* COM methods are __stdcall (callee pops: no add esp after the calls at 0x005ba308 / 0x005ba371 / 0x005ba3e5 / 0x005ba46b / 0x005ba5b3 / 0x005ba600 / 0x005ba610 / 0x005ba62c) */
typedef int (__stdcall *PFN1_005ba1d0)(int *);
typedef int (__stdcall *PFN2_005ba1d0)(int *, void *);
typedef int (__stdcall *PFN3_005ba1d0)(int *, int, int);
typedef int (__stdcall *PFN5_005ba1d0)(int *, int, int, int, int);
typedef int (__stdcall *PFN8_005ba1d0)(int *, int, int, int, int, int, int, int);
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
// MATCH: 0x005ba1d0 FUN_005ba1d0
int FUN_005ba1d0(int param_1,undefined4 param_2,undefined4 *param_3)

{

  int iVar2;
  HWND pHVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  undefined4 uVar7;
  uint uVar8;
  undefined4 *puVar9;
  int *piVar10;
  struct { int a; undefined *b; undefined4 c; undefined4 d; undefined4 e; undefined4 f; uint g;
           undefined4 h; undefined4 i; undefined4 j; undefined4 k; undefined4 l; } blk;   /* [esp+0x1c], 12 dwords */
  undefined4 auStack_6c [0x18];   /* [esp+0x4c], 0x60 bytes: 0x005ba3c5 clears 0x18 dwords */
  int iStack_rate;
  int *piStack_q;
  byte bStack_bits;
  byte bStack_2;
  
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
  iVar2 = FUN_005c1920(param_3[1],(int **)puVar9,0);
  if (iVar2 < 0) {
    FUN_005ba760(param_1);
    return 0;
  }
  pHVar3 = *(HWND *)(param_1 + 0x30);
  if (pHVar3 == (HWND)0x0) {
    pHVar3 = GetForegroundWindow();
  }
  (*(PFN3_005ba1d0 *)(*(int *)*puVar9 + 0x18))
            ((int *)*puVar9,(int)pHVar3,((*(uint *)(param_1 + 0x78) & 0x400) ? 1 : 2));
  if ((*(uint *)(param_1 + 0x78) & 0x400) == 0) {
    /* 0x005ba313..0x005ba374 (listing): locals [esp+0x18] = param_3[5], byte [esp+0x12] = byte param_3[6],
       byte [esp+0x13] = 2, [esp+0x14] = 0; their addresses are the five pointer args. */
    iStack_rate = param_3[5];
    bStack_bits = *(byte *)(param_3 + 6);
    bStack_2 = 2;
    piStack_q = (int *)0x0;
    iVar2 = FUN_005bbc10(*(undefined4 *)(param_1 + 0x7c),&bStack_2,&iStack_rate,
                         &bStack_bits,param_3[4] == 0,&piStack_q);
    if (iVar2 == 0) goto LAB_005ba376;
    *(int *)(param_1 + 0x2c) = iStack_rate;
    *(ushort *)(param_1 + 0x2a) = (ushort)bStack_bits;
    (*(PFN1_005ba1d0 *)(*piStack_q + 8))(piStack_q);
  }
  else {
LAB_005ba376:
    *(undefined4 *)(param_1 + 0x2c) = param_3[5];
    *(undefined2 *)(param_1 + 0x2a) = *(undefined2 *)(param_3 + 6);
  }
  /* 0x005ba384..0x005ba3bf: byte stores at [esp+0x34] / [esp+0x35] / [esp+0x28] / [esp+0x29] */
  uVar8 = 0x80;
  ((byte *)&blk.g)[0] = 0;
  ((byte *)&blk.g)[1] = 0;
  blk.e = 0;
  blk.f = 0;
  blk.b = &DAT_005e6424;
  ((byte *)&blk.d)[0] = *(byte *)(param_1 + 0x2a);
  blk.c = 4;
  ((byte *)&blk.d)[1] = 1;
  blk.a = *(int *)(param_1 + 0x2c);
  if (param_3[2] == 0) {
    puVar9 = auStack_6c;
    for (iVar2 = 0x18; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    auStack_6c[0] = 0x60;
    iVar2 = (*(PFN2_005ba1d0 *)(**(int **)(param_1 + 0x7c) + 0x10))(*(int **)(param_1 + 0x7c),auStack_6c);
    if (iVar2 < 0) goto LAB_005ba5d5;
    if (auStack_6c[14] == 0) {
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
    uVar8 = uVar8 | 0x8000;
  }
  iVar2 = param_1 + 0x80;
  iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&blk.a,1,1,uVar8,(undefined4 *)iVar2);
  if (iVar4 == 0) {
    if ((uVar8 & 4) != 0) {
      uVar8 = uVar8 & 0xfffffffb | 8;
    }
    iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&blk.a,1,1,uVar8,(undefined4 *)iVar2);
    if (iVar4 == 0) {
      (*(PFN1_005ba1d0 *)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
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
  iVar4 = FUN_005bbdb0(*(undefined4 *)(param_1 + 0x7c),&blk.a,
                       ~*(uint *)(param_1 + 0x78) >> 6 & 1,1,uVar8,(undefined4 *)iVar2);
  if (iVar4 == 0) {
    (*(PFN1_005ba1d0 *)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
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
    blk.c = param_3[5];
    blk.b = (undefined *)(uint)*(ushort *)((int)param_3 + 0x1a);
    blk.d = (uint)param_3[7] >> 1;
    blk.a = param_1;
    iVar4 = FUN_005c7990(&blk.a,4,0,0);
    *(int *)(param_1 + 0x11c) = iVar4;
    if (iVar4 == 0) {
      FUN_005bbf30(iVar2);
      (*(PFN1_005ba1d0 *)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
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
      blk.a = -10000;
      blk.b = (undefined *)0xffffd8f0;
      blk.c = 0;
      blk.d = 0x3fbeb852;
      blk.e = 0x3f547ae1;
      blk.f = 0xfffff5d6;
      blk.g = 0x3be56042;
      blk.h = 200;
      blk.i = 0x3c343958;
      blk.j = 0x42c80000;
      blk.k = 0x42c80000;
      blk.l = 0x459c4000;
      piVar6 = &blk.a;
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
    (*(PFN1_005ba1d0 *)(*piVar6 + 8))(piVar6);
    if (iVar2 != 0) {
      (*(PFN3_005ba1d0 *)(*(int *)*puVar9 + 0x30))((int *)*puVar9,0x3f800000,0);
      (*(PFN5_005ba1d0 *)(*(int *)*puVar9 + 0x40))((int *)*puVar9,0,0,0,0);
      (*(PFN8_005ba1d0 *)(*(int *)*puVar9 + 0x34))((int *)*puVar9,0,0,0x3f800000,0,0x3f800000,0,0);
      goto LAB_005ba62f;
    }
  }
  FUN_005bb000(param_1,0);
  FUN_005bbf30(param_1 + 0x80);
LAB_005ba5d5:
  (*(PFN1_005ba1d0 *)(**(int **)(param_1 + 0x7c) + 8))(*(int **)(param_1 + 0x7c));
  FUN_005ba760(param_1);
  return 0;
}
