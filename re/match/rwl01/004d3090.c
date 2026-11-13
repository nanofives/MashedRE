// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d3090: 32.8% regs 35.1%.  class (b) Raw Ghidra form only (budget). Original frame sub esp,8 with args at +0x1c, i.e. different local/arg layout; not analysed further.. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
/* RWC-CORE-RUN run 38: `call [ecx+8]` at 0x004d31f7 is COM Release, __stdcall (callee pops the argument; `pop edi`
   follows with no `add esp, 4`). Typed through `code` (cdecl) the caller also popped it. */
typedef unsigned long __stdcall fpRelease(void *self);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(undefined4 *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(undefined4, DAT_007d46e4, 0x007d46e4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(uint, DAT_007d46d0, 0x007d46d0);
MGLOBAL(uint, DAT_007d46cc, 0x007d46cc);
MGLOBAL(int, DAT_007d46d8, 0x007d46d8);
MGLOBAL(undefined4, DAT_007d46dc, 0x007d46dc);
MGLOBAL(int *, DAT_007d4114, 0x007d4114);
undefined4 FUN_004d6ce0(int p0, int p1);
undefined4 FUN_004d6e60(int p0);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d3090 FUN_004d3090
undefined4 FUN_004d3090(undefined4 param_1,int *param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined4 local_8;
  undefined4 local_4;
  
  uVar6 = 0;
  puVar1 = (undefined4 *)(DAT_00911ae4 + (int)param_2);
  do {
    piVar3 = (int *)FUN_004d6e60(uVar6);
    if (piVar3 == param_2) {
      FUN_004d6ce0(0,uVar6);
    }
    uVar6 = uVar6 + 1;
  } while (uVar6 < 8);
  if ((int *)*param_2 != param_2) {
    if ((char)param_2[8] != '\x02') {
      return 1;
    }
    puVar1 = DAT_007d46e0;
    puVar4 = (undefined4 *)0x0;
    if (DAT_007d46e0 == (undefined4 *)0x0) {
      return 1;
    }
    while (puVar5 = puVar1, (int *)*puVar5 != param_2) {
      puVar1 = (undefined4 *)puVar5[1];
      puVar4 = puVar5;
      if ((undefined4 *)puVar5[1] == (undefined4 *)0x0) {
        return 1;
      }
    }
LAB_004d33ef:
    if (puVar4 == (undefined4 *)0x0) {
      if (DAT_007d46e0 == puVar5) {
        DAT_007d46e0 = (undefined4 *)puVar5[1];
      }
    }
    else {
      puVar4[1] = puVar5[1];
    }
    (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46e4,puVar5);
    return 1;
  }
  puVar4 = DAT_007d46e0;
  switch((char)param_2[8]) {
  case '\0':
    if (*(char *)((int)param_2 + 0x21) < '\0') {
      return 1;
    }
    if (puVar1[1] == 0) {
      if ((0x45 < (int)puVar1[6]) && ((int)puVar1[6] < 0x51)) {
        puVar5 = (undefined4 *)0x0;
        for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)puVar4[1]) {
          if ((int *)*puVar4 == param_2) goto LAB_004d31bb;
          puVar5 = puVar4;
        }
      }
    }
    else {
      puVar5 = (undefined4 *)0x0;
      for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)puVar4[1]) {
        if ((int *)*puVar4 == param_2) goto LAB_004d3112;
        puVar5 = puVar4;
      }
LAB_004d3141:
      uVar2 = *(undefined4 *)(puVar1[1] + 0x400);
      if (DAT_007d46d0 <= DAT_007d46cc) {
        DAT_007d46d0 = DAT_007d46d0 + 0x200;
        if (DAT_007d46d8 == 0) {
          DAT_007d46d8 = (**(code **)(DAT_007d3ff8 + 0x108))();
        }
        else {
          DAT_007d46d8 = (**(code **)(DAT_007d3ff8 + 0x110))
                                   (DAT_007d46d8,DAT_007d46d0 * 2,0x1030411);
        }
      }
      *(short *)(DAT_007d46d8 + DAT_007d46cc * 2) = (short)uVar2;
      DAT_007d46cc = DAT_007d46cc + 1;
      (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46dc,puVar1[1]);
      puVar1[1] = 0;
    }
    break;
  case '\x01':
    if (*(char *)((int)param_2 + 0x21) < '\0') {
      return 1;
    }
    puVar4 = (undefined4 *)0x0;
    for (puVar5 = DAT_007d46e0; puVar5 != (undefined4 *)0x0; puVar5 = (undefined4 *)puVar5[1]) {
      if ((int *)*puVar5 == param_2) {
        if (puVar4 == (undefined4 *)0x0) {
          if (DAT_007d46e0 == puVar5) {
            DAT_007d46e0 = (undefined4 *)puVar5[1];
          }
        }
        else {
          puVar4[1] = puVar5[1];
        }
        (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46e4,puVar5);
        break;
      }
      puVar4 = puVar5;
    }
    piVar3 = (int *)*puVar1;
    if (piVar3 == DAT_007d4114) {
      return 1;
    }
    goto LAB_004d31ec;
  case '\x02':
    puVar1 = DAT_007d46e0;
    puVar4 = (undefined4 *)0x0;
    if (DAT_007d46e0 == (undefined4 *)0x0) {
      return 1;
    }
    while (puVar5 = puVar1, (int *)*puVar5 != param_2) {
      puVar1 = (undefined4 *)puVar5[1];
      puVar4 = puVar5;
      if ((undefined4 *)puVar5[1] == (undefined4 *)0x0) {
        return 1;
      }
    }
    goto LAB_004d33ef;
  default:
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x8000000d);
    FUN_004d8480(&local_8);
    return 0;
  case '\x04':
    if (*(char *)((int)param_2 + 0x21) < '\0') {
      return 1;
    }
    if (puVar1[1] != 0) {
      puVar5 = (undefined4 *)0x0;
      for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)puVar4[1]) {
        if ((int *)*puVar4 == param_2) goto LAB_004d3112;
        puVar5 = puVar4;
      }
      goto LAB_004d3141;
    }
    if ((0x45 < (int)puVar1[6]) && ((int)puVar1[6] < 0x51)) {
      puVar5 = (undefined4 *)0x0;
      for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)puVar4[1]) {
        if ((int *)*puVar4 == param_2) goto LAB_004d31bb;
        puVar5 = puVar4;
      }
    }
    break;
  case '\x05':
    if (*(char *)((int)param_2 + 0x21) < '\0') {
      return 1;
    }
    puVar5 = (undefined4 *)0x0;
    for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)puVar4[1]) {
      if ((int *)*puVar4 == param_2) goto LAB_004d31bb;
      puVar5 = puVar4;
    }
  }
  goto LAB_004d31ea;
LAB_004d3112:
  if (puVar5 == (undefined4 *)0x0) {
    if (DAT_007d46e0 == puVar4) {
      DAT_007d46e0 = (undefined4 *)puVar4[1];
    }
  }
  else {
    puVar5[1] = puVar4[1];
  }
  (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46e4,puVar4);
  goto LAB_004d3141;
LAB_004d31bb:
  if (puVar5 == (undefined4 *)0x0) {
    if (DAT_007d46e0 == puVar4) {
      DAT_007d46e0 = (undefined4 *)puVar4[1];
    }
  }
  else {
    puVar5[1] = puVar4[1];
  }
  (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46e4,puVar4);
LAB_004d31ea:
  piVar3 = (int *)*puVar1;
LAB_004d31ec:
  if (piVar3 == (int *)0x0) {
    return 1;
  }
  (**(fpRelease **)(*piVar3 + 8))(piVar3);
  return 1;
}
