// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x005405c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(undefined1, DAT_00623d38, 0x00623d38);
MGLOBAL(undefined1, DAT_00623d3c, 0x00623d3c);
MGLOBAL(undefined1, DAT_00623d48, 0x00623d48);
MGLOBAL(undefined1, DAT_00623d4c, 0x00623d4c);
MGLOBAL(undefined1, DAT_00623d58, 0x00623d58);
MGLOBAL(undefined1, DAT_00623d5c, 0x00623d5c);
MGLOBAL(int, DAT_007dc614, 0x007dc614);
MGLOBAL(uint, DAT_007dc624, 0x007dc624);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc61c, 0x007dc61c);
MGLOBAL(int, DAT_00623d30, 0x00623d30);
MGLOBAL(int, DAT_007dc618, 0x007dc618);
MGLOBAL(undefined1, DAT_00000004, 0x00000004);
bool FUN_004cb330(int p0, int * p1);
void FUN_004d3b00();
void FUN_004d53b0(void);
void FUN_004d5480();
void FUN_004d54d0(int p0, undefined4 * p1);
void FUN_004d54f0();
undefined4 FUN_004d6c40(int p0);
undefined4 FUN_004d6c90(int p0);
undefined4 FUN_004d6e70();
void FUN_004d7100(int p0);
undefined4 FUN_004d71f0(void);
void FUN_00540340();
MGLOBAL(int, _DAT_00623d38, 0x00623d38);
MGLOBAL(int, _DAT_00623d3c, 0x00623d3c);
MGLOBAL(int, _DAT_00623d48, 0x00623d48);
MGLOBAL(int, _DAT_00623d4c, 0x00623d4c);
MGLOBAL(int, _DAT_00623d58, 0x00623d58);
MGLOBAL(int, _DAT_00623d5c, 0x00623d5c);
MGLOBAL(int, stack0xffffffe0, 0xffffffe0);
MGLOBAL(int, stack0xffffffe8, 0xffffffe8);
// MATCH: 0x005405c0 FUN_005405c0
/* [C1 2026-05-20] Five-arg multi-pass material renderer (undefined4 *param_1, int param_2, byte
   param_3, int *param_4, int *param_5). */

void FUN_005405c0(undefined4 *param_1,int param_2,byte param_3,int *param_4,int *param_5)

{
  undefined4 *puVar1;
  bool bVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int unaff_EBP;
  undefined4 uVar8;
  undefined4 uVar9;
  int *piVar10;
  undefined4 uVar11;
  int *piStack_28;
  undefined4 *puStack_24;
  int *local_10;
  int iStack_c;
  int iStack_8;
  
  piVar4 = param_5;
  piVar3 = param_4;
  if ((param_5 == (int *)0x0) || (*param_5 == 0)) {
    puStack_24 = param_1;
    piStack_28 = (int *)0x5412c4;
    FUN_00540340();
    return;
  }
  param_5 = (int *)0x0;
  local_10 = *(int **)(DAT_007dc5dc + *(int *)(param_2 + 8));
  bVar2 = true;
  if (local_10[0xc] == 6) {
    param_5 = local_10;
    local_10 = local_10 + 6;
  }
  piVar5 = param_5;
  puStack_24 = (undefined4 *)0x0;
  if ((param_3 & 0x84) == 0) {
    piStack_28 = (int *)0x0;
    FUN_004d6e70();
    FUN_004d54f0(0,1,3);
    piVar10 = (int *)0x3;
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,3);
    FUN_004d54f0(0,6,0);
  }
  else {
    piStack_28 = param_4;
    FUN_004d6e70();
    FUN_004d54f0(0,1,4);
    piVar10 = (int *)0x2;
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,4);
    FUN_004d54f0(0,5,2);
    puStack_24 = (undefined4 *)0x0;
    piStack_28 = (int *)0x6;
    FUN_004d54f0(0);
  }
  if (((piVar5 != (int *)0x0) && (puVar1 = (undefined4 *)*param_5, puVar1 != (undefined4 *)0x0)) &&
     ((puVar1[3] & 0x20000) == 0)) {
    puStack_24 = (undefined4 *)0x2;
    piStack_28 = (int *)0x18;
    FUN_004d54f0(0);
    _DAT_00623d38 = *puVar1;
    _DAT_00623d3c = puVar1[1];
    _DAT_00623d48 = puVar1[4];
    _DAT_00623d4c = puVar1[5];
    _DAT_00623d58 = puVar1[0xc];
    _DAT_00623d5c = puVar1[0xd];
    FUN_004cb330(0x10,&DAT_00623d38);
  }
  if (((piVar3 != (int *)0x0) && ((*(byte *)(*piVar3 + 0x23) & 0x20) != 0)) &&
     ((puStack_24 = (undefined4 *)*piVar4, (*(byte *)((int)puStack_24 + 0x23) & 0x20) != 0 &&
      (piVar3 != piVar4)))) {
    piStack_28 = (int *)0x540728;
    FUN_004d3b00();
  }
  if (DAT_007dc614 == 0) goto LAB_00540cc3;
  iVar6 = local_10[1];
  if (((iVar6 == 9) && (local_10[2] == 1)) || ((iVar6 == 1 && (local_10[2] == 3)))) {
    if (piVar3 == (int *)0x0) {
      puStack_24 = (undefined4 *)0x0;
      piStack_28 = piVar4;
      FUN_004d6e70();
      FUN_004d54f0(0,1,4);
      piVar10 = (int *)0x2;
      FUN_004d54f0(0,2,2);
      FUN_004d54f0(0,3,0);
      FUN_004d54f0(0,4,4);
      FUN_004d54f0(0,5,2);
      puStack_24 = (undefined4 *)0x0;
      piStack_28 = (int *)0x6;
      FUN_004d54f0(0);
      bVar2 = false;
    }
    else {
      puStack_24 = (undefined4 *)0x1;
      piStack_28 = piVar4;
      FUN_004d6e70();
      if (DAT_007dc624 == 3) {
        puStack_24 = (undefined4 *)0x2;
        piStack_28 = (int *)0x1;
        FUN_004d54f0(0);
        FUN_004d54f0(0,2,2);
        FUN_004d54f0(0,4,2);
        FUN_004d54f0(0,5,2);
        FUN_004d54f0(1,1,4);
        FUN_004d54f0(1,2,2);
        puStack_24 = (undefined4 *)0x1;
        piStack_28 = (int *)0x3;
        FUN_004d54f0(1);
        FUN_004d54f0(1,4,3);
        FUN_004d54f0(1,6,1);
        FUN_004d54f0(2,1,4);
        FUN_004d54f0(2,2,0);
        FUN_004d54f0(2,3,1);
        puStack_24 = (undefined4 *)0x4;
        piStack_28 = (int *)&DAT_00000004;
        FUN_004d54f0(2);
        FUN_004d54f0(2,5,0);
        piVar10 = (int *)0x1;
        FUN_004d54f0(2,6,1);
        bVar2 = false;
      }
      else {
        puStack_24 = (undefined4 *)0x4;
        piStack_28 = (int *)0x1;
        FUN_004d54f0(1);
        FUN_004d54f0(1,2,2);
        piVar10 = (int *)0x1;
        FUN_004d54f0(1,3,1);
        FUN_004d54f0(1,4,3);
        FUN_004d54f0(1,6,1);
        bVar2 = false;
      }
    }
LAB_00540c48:
    if (-1 < (char)param_3) {
      puStack_24 = (undefined4 *)0x0;
      piStack_28 = (int *)0xb;
      FUN_004d54f0(1);
    }
    if (((param_5 != (int *)0x0) && (puVar1 = (undefined4 *)param_5[1], puVar1 != (undefined4 *)0x0)
        ) && ((puVar1[3] & 0x20000) == 0)) {
      puStack_24 = (undefined4 *)0x2;
      piStack_28 = (int *)0x18;
      FUN_004d54f0(1);
      _DAT_00623d38 = *puVar1;
      _DAT_00623d3c = puVar1[1];
      _DAT_00623d48 = puVar1[4];
      _DAT_00623d4c = puVar1[5];
      _DAT_00623d58 = puVar1[0xc];
      _DAT_00623d5c = puVar1[0xd];
      FUN_004cb330(0x11,&DAT_00623d38);
    }
  }
  else {
    if ((iVar6 == 5) && (local_10[2] == 6)) {
      if (piVar3 == (int *)0x0) {
        puStack_24 = (undefined4 *)0xffffffff;
        piStack_28 = (int *)0x3c;
        FUN_004d5480();
        FUN_004d6e70(piVar4,0);
        FUN_004d54f0(0,1,0xd);
        FUN_004d54f0(0,2,2);
        FUN_004d54f0(0,3,3);
        FUN_004d54f0(0,4,3);
        puStack_24 = (undefined4 *)0x2;
        piStack_28 = (int *)0x5;
        FUN_004d54f0(0);
        FUN_004d54f0(0,6,3);
        FUN_004d54f0(1,1,4);
        FUN_004d54f0(1,2,0);
        FUN_004d54f0(1,3,1);
        FUN_004d54f0(1,4,4);
        puStack_24 = (undefined4 *)0x0;
        piStack_28 = (int *)0x5;
LAB_00540e0f:
        FUN_004d54f0(1);
        piVar10 = (int *)0x540e21;
        FUN_004d54f0(1,6,1);
        bVar2 = false;
      }
      else {
        if (DAT_007dc624 < 3) goto LAB_00540cc3;
        puStack_24 = (undefined4 *)0x1;
        piStack_28 = piVar4;
        FUN_004d6e70();
        FUN_004d54f0(0,1,2);
        FUN_004d54f0(0,2,2);
        FUN_004d54f0(0,4,2);
        FUN_004d54f0(0,5,2);
        FUN_004d54f0(1,1,0xd);
        puStack_24 = (undefined4 *)0x2;
        piStack_28 = (int *)0x2;
        FUN_004d54f0(1);
        FUN_004d54f0(1,3,1);
        FUN_004d54f0(1,4,0xd);
        FUN_004d54f0(1,5,2);
        FUN_004d54f0(1,6,1);
        FUN_004d54f0(2,1,4);
        puStack_24 = (undefined4 *)0x0;
        piStack_28 = (int *)0x2;
        FUN_004d54f0(2);
        FUN_004d54f0(2,3,1);
        piVar10 = (int *)&DAT_00000004;
        FUN_004d54f0(2,4,4);
        FUN_004d54f0(2,5,0);
        FUN_004d54f0(2,6,1);
        bVar2 = false;
      }
      goto LAB_00540c48;
    }
    if ((iVar6 == 9) && (local_10[2] == 3)) {
      if (DAT_007dc61c != 0) {
        if (piVar3 == (int *)0x0) {
          puStack_24 = (undefined4 *)0x0;
          piStack_28 = piVar4;
          FUN_004d6e70();
          FUN_004d54f0(0,1,5);
          piVar10 = (int *)&DAT_00000004;
          FUN_004d54f0(0,4,3);
          FUN_004d54f0(0,6,0);
        }
        else {
          puStack_24 = (undefined4 *)0x1;
          piStack_28 = piVar4;
          FUN_004d6e70();
          FUN_004d54f0(1,1,5);
          FUN_004d54f0(1,2,2);
          FUN_004d54f0(1,3,1);
          if (DAT_007dc624 == 3) {
            puStack_24 = (undefined4 *)0x2;
            piStack_28 = (int *)0x1;
            FUN_004d54f0(0);
            FUN_004d54f0(0,2,2);
            FUN_004d54f0(0,4,2);
            FUN_004d54f0(0,5,2);
            FUN_004d54f0(1,4,3);
            FUN_004d54f0(1,6,1);
            puStack_24 = (undefined4 *)0x4;
            piStack_28 = (int *)0x1;
            FUN_004d54f0(2);
            FUN_004d54f0(2,2,0);
            piVar10 = (int *)0x1;
            FUN_004d54f0(2,3,1);
            FUN_004d54f0(2,4,4);
            FUN_004d54f0(2,5,0);
            FUN_004d54f0(2,6,1);
          }
          else {
            puStack_24 = (undefined4 *)0x3;
            piStack_28 = (int *)&DAT_00000004;
            FUN_004d54f0(1);
            piVar10 = (int *)0x540b4f;
            FUN_004d54f0(1,6,1);
          }
        }
        if (DAT_00623d30 == 0) {
LAB_00540f9b:
          bVar2 = false;
        }
        else {
          param_4 = (int *)0x0;
          puStack_24 = (undefined4 *)0x540b98;
          FUN_004d53b0();
          puStack_24 = &param_4;
          piStack_28 = DAT_007d4110;
          iVar6 = (**(code **)(*DAT_007d4110 + 0x118))();
          if ((iVar6 < 0) || (param_4 != (int *)0x1)) {
            puStack_24 = (undefined4 *)0x0;
            piStack_28 = piVar3;
            DAT_007dc61c = 0;
            FUN_004d6e70();
            FUN_004d54f0(0,1,4);
            FUN_004d54f0(0,2,2);
            FUN_004d54f0(0,3,0);
            FUN_004d54f0(0,4,4);
            FUN_004d54f0(0,5,2);
            puStack_24 = (undefined4 *)0x0;
            piStack_28 = (int *)0x6;
            FUN_004d54f0(0);
            FUN_004d54f0(1,1,1);
            piVar10 = (int *)0x1;
            FUN_004d54f0(1,4,1);
            FUN_004d54f0(2,1,1);
            FUN_004d54f0(2,4,1);
            FUN_004d6e70(0,1);
          }
          else {
            bVar2 = false;
          }
          DAT_00623d30 = 0;
          if (bVar2) goto LAB_00540cc3;
        }
        goto LAB_00540c48;
      }
    }
    else if ((iVar6 == 2) && (local_10[2] == 2)) {
      if (piVar3 == (int *)0x0) {
        puStack_24 = (undefined4 *)0x0;
        piStack_28 = piVar4;
        FUN_004d6e70();
        FUN_004d54f0(0,4,3);
        FUN_004d54f0(0,6,0);
        FUN_004d54f0(1,1,7);
        FUN_004d54f0(1,2,0);
        FUN_004d54f0(1,3,1);
        puStack_24 = (undefined4 *)0x3;
        piStack_28 = (int *)&DAT_00000004;
        goto LAB_00540e0f;
      }
      if (DAT_007dc618 != 0) {
        puStack_24 = (undefined4 *)0x1;
        piStack_28 = piVar4;
        FUN_004d6e70();
        FUN_004d54f0(1,1,0x19);
        piVar10 = (int *)0x1a;
        FUN_004d54f0(1,0x1a,1);
        FUN_004d54f0(1,2,2);
        FUN_004d54f0(1,3,0);
        FUN_004d54f0(1,4,3);
        puStack_24 = (undefined4 *)0x1;
        piStack_28 = (int *)0x6;
        FUN_004d54f0(1);
        bVar2 = false;
        goto LAB_00540c48;
      }
    }
    else if ((iVar6 == 1) && (local_10[2] == 5)) {
      if (piVar3 == (int *)0x0) {
        puStack_24 = (undefined4 *)0x0;
        piStack_28 = piVar4;
        FUN_004d6e70();
        FUN_004d54f0(0,1,4);
        piVar10 = (int *)0x2;
        FUN_004d54f0(0,2,0x22);
        FUN_004d54f0(0,3,0);
        FUN_004d54f0(0,4,3);
        FUN_004d54f0(0,6,0);
        goto LAB_00540f9b;
      }
      puStack_24 = (undefined4 *)0x1;
      piStack_28 = piVar4;
      FUN_004d6e70();
      if (DAT_007dc624 == 3) {
        puStack_24 = (undefined4 *)0x2;
        piStack_28 = (int *)0x1;
        FUN_004d54f0(0);
        FUN_004d54f0(0,2,2);
        FUN_004d54f0(0,4,2);
        FUN_004d54f0(0,5,2);
        FUN_004d54f0(1,1,4);
        FUN_004d54f0(1,2,0x22);
        puStack_24 = (undefined4 *)0x1;
        piStack_28 = (int *)0x3;
        FUN_004d54f0(1);
        FUN_004d54f0(1,4,3);
        FUN_004d54f0(1,6,1);
        FUN_004d54f0(2,1,4);
        FUN_004d54f0(2,2,0);
        FUN_004d54f0(2,3,1);
        puStack_24 = (undefined4 *)0x4;
        piStack_28 = (int *)&DAT_00000004;
        FUN_004d54f0(2);
        FUN_004d54f0(2,5,0);
        piVar10 = (int *)0x1;
        FUN_004d54f0(2,6,1);
        bVar2 = false;
      }
      else {
        puStack_24 = (undefined4 *)0x4;
        piStack_28 = (int *)0x1;
        FUN_004d54f0(1);
        FUN_004d54f0(1,2,0x22);
        piVar10 = (int *)0x1;
        FUN_004d54f0(1,3,1);
        FUN_004d54f0(1,4,3);
        FUN_004d54f0(1,6,1);
        bVar2 = false;
      }
      goto LAB_00540c48;
    }
  }
LAB_00540cc3:
  if (DAT_006181d4 != 0) {
    DAT_006181d4 = 0;
    puStack_24 = (undefined4 *)0x0;
    piStack_28 = DAT_007d4110;
    (**(code **)(*DAT_007d4110 + 0x1ac))();
  }
  if (DAT_006181d0 != *(int *)(param_2 + 0x10)) {
    puStack_24 = *(undefined4 **)(param_2 + 0x10);
    piStack_28 = DAT_007d4110;
    DAT_006181d0 = *(int *)(param_2 + 0x10);
    (**(code **)(*DAT_007d4110 + 0x170))();
  }
  if (param_1[2] == 0) {
    puStack_24 = (undefined4 *)0x540fa7;
    FUN_004d53b0();
    puStack_24 = *(undefined4 **)(param_2 + 0x20);
    piStack_28 = *(int **)(param_2 + 0x14);
    iVar6 = param_1[3];
    uVar11 = 0x540fc5;
    (**(code **)(*DAT_007d4110 + 0x144))(DAT_007d4110);
  }
  else {
    puStack_24 = (undefined4 *)0x540d22;
    FUN_004d53b0();
    puStack_24 = *(undefined4 **)(param_2 + 0x20);
    piStack_28 = *(int **)(param_2 + 0x1c);
    iVar6 = *(int *)(param_2 + 0x18);
    uVar11 = *(undefined4 *)(param_2 + 0x14);
    piVar10 = DAT_007d4110;
    (**(code **)(*DAT_007d4110 + 0x148))(DAT_007d4110,param_1[3],uVar11,0);
  }
  if (!bVar2) {
    FUN_004d6e70(0,1);
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,4);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,0);
    FUN_004d54f0(1,1,1);
    FUN_004d54f0(1,4,1);
    FUN_004d54f0(2,1,1);
    FUN_004d54f0(2,4,1);
    if (-1 < (char)local_10) {
      FUN_004d54f0(1,0xb,1);
    }
    goto LAB_00541282;
  }
  FUN_004d6e70(piVar4,0);
  if (((iStack_8 != 0) && (puVar1 = *(undefined4 **)(iStack_8 + 4), puVar1 != (undefined4 *)0x0)) &&
     ((puVar1[3] & 0x20000) == 0)) {
    FUN_004d54f0(0,0x18,2);
    _DAT_00623d38 = *puVar1;
    _DAT_00623d3c = puVar1[1];
    _DAT_00623d48 = puVar1[4];
    _DAT_00623d4c = puVar1[5];
    _DAT_00623d58 = puVar1[0xc];
    _DAT_00623d5c = puVar1[0xd];
    FUN_004cb330(0x10,&DAT_00623d38);
  }
  iVar7 = FUN_004d71f0();
  if (iVar7 == 0) {
    FUN_004d7100(1);
  }
  iVar7 = *(int *)(iVar6 + 4);
  if (((iVar7 == 9) && (*(int *)(iVar6 + 8) == 1)) ||
     (((iVar7 == 1 && (*(int *)(iVar6 + 8) == 3)) || ((iVar7 == 9 && (*(int *)(iVar6 + 8) == 3))))))
  {
    FUN_004d5480(0xf,0);
  }
  (**(code **)(DAT_007d3ff8 + 0x24))(10,&puStack_24);
  (**(code **)(DAT_007d3ff8 + 0x24))(0xb,&stack0xffffffe0);
  FUN_004d6c40(*(undefined4 *)(iVar6 + 4));
  FUN_004d6c90(*(undefined4 *)(iVar6 + 8));
  FUN_004d54d0(0xe,&stack0xffffffe8);
  FUN_004d5480(0xe,0);
  FUN_004d54d0(0x1c,&iStack_c);
  if (iStack_c != 0) {
    FUN_004d54d0(0x22,&piStack_28);
    if (*(int *)(iVar6 + 8) == 2) {
      uVar9 = 0;
    }
    else {
      if ((*(int *)(iVar6 + 4) != 9) && (*(int *)(iVar6 + 8) != 3)) goto LAB_00541115;
      uVar9 = 0xffffffff;
    }
    FUN_004d5480(0x22,uVar9);
  }
LAB_00541115:
  if (((uint)local_10 & 0x80) != 0) {
    FUN_004d54f0(0,0xb,1);
  }
  if (param_1[2] == 0) {
    FUN_004d53b0();
    uVar9 = *(undefined4 *)(unaff_EBP + 0x20);
    uVar8 = *(undefined4 *)(unaff_EBP + 0x14);
    (**(code **)(*DAT_007d4110 + 0x144))(DAT_007d4110,param_1[3],uVar8,uVar9);
  }
  else {
    FUN_004d53b0();
    uVar9 = *(undefined4 *)(unaff_EBP + 0x20);
    uVar8 = *(undefined4 *)(unaff_EBP + 0x1c);
    (**(code **)(*DAT_007d4110 + 0x148))
              (DAT_007d4110,param_1[3],*(undefined4 *)(unaff_EBP + 0x14),0,
               *(undefined4 *)(unaff_EBP + 0x18),uVar8,uVar9);
  }
  FUN_004d5480(0xe,uVar11);
  if (piStack_28 != (int *)0x0) {
    FUN_004d5480(0x22,uVar8);
  }
  if (((uint)local_10 & 0x80) != 0) {
    FUN_004d54f0(0,0xb,0);
  }
  FUN_004d7100(0);
  (**(code **)(DAT_007d3ff8 + 0x20))(10,uVar9);
  (**(code **)(DAT_007d3ff8 + 0x20))(0xb,piVar10);
LAB_00541282:
  if (iStack_8 == 0) {
    return;
  }
  FUN_004d54f0(0,0x18,0);
  FUN_004d54f0(1,0x18,0);
  return;
}
