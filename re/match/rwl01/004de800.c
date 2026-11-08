// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004de800: 48.2% regs 57.3%.  class (b) Switch that selects among 12+ code addresses (ebp = &FUN_xxxx) then calls them per pixel format; raw Ghidra form treats the function addresses as byte globals. Original builds the selected callback in ebp inside each case and loops once; not reached in budget.. */
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
#ifdef __cplusplus
#define PROTO0 (...)
typedef int code(...);
#else
#define PROTO0 ()
typedef int code();
#endif
int FUN_004ded10 PROTO0;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
int FUN_004ded30 PROTO0;
int FUN_004ded60 PROTO0;
int FUN_004ded90 PROTO0;
int FUN_004dedb0 PROTO0;
int FUN_004dedd0 PROTO0;
int FUN_004dee00 PROTO0;
int FUN_004dee20 PROTO0;
int FUN_004deb20 PROTO0;
int FUN_004deb50 PROTO0;
int FUN_004deb80 PROTO0;
int FUN_004debb0 PROTO0;
int FUN_004debf0 PROTO0;
int FUN_004dec40 PROTO0;
int FUN_004dec70 PROTO0;
int FUN_004deca0 PROTO0;
int FUN_004decd0 PROTO0;
int FUN_004decf0 PROTO0;
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004de800 FUN_004de800
void FUN_004de800(int param_1,int *param_2)

{
  int *piVar1;
  int *piVar2;
  code *pcVar3;
  undefined1 *puVar4;
  undefined2 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  int local_8;
  undefined4 local_4;
  
  piVar2 = param_2;
  pcVar3 = (code *)0x0;
  switch(*(undefined4 *)(*param_2 + 0x18 + DAT_00911ae4)) {
  case 0x15:
    pcVar3 = FUN_004ded30;
    break;
  case 0x16:
    pcVar3 = FUN_004decd0;
    break;
  case 0x17:
    pcVar3 = FUN_004deb50;
    break;
  case 0x18:
    pcVar3 = FUN_004deb20;
    break;
  case 0x19:
    pcVar3 = FUN_004debb0;
    break;
  case 0x1a:
    pcVar3 = FUN_004debf0;
    break;
  case 0x1b:
    pcVar3 = FUN_004dec70;
    break;
  case 0x1c:
    pcVar3 = FUN_004dedb0;
    break;
  case 0x1d:
    pcVar3 = FUN_004dec40;
    break;
  case 0x1e:
    pcVar3 = FUN_004deca0;
    break;
  case 0x20:
  case 0x3f:
    pcVar3 = FUN_004ded60;
    break;
  case 0x21:
  case 0x3e:
    pcVar3 = FUN_004decf0;
    break;
  case 0x32:
    pcVar3 = FUN_004ded90;
    break;
  case 0x33:
    pcVar3 = FUN_004dee00;
    break;
  case 0x34:
    pcVar3 = FUN_004dedd0;
    break;
  case 0x3c:
  case 0x75:
    pcVar3 = FUN_004ded10;
    break;
  case 0x3d:
    pcVar3 = FUN_004deb80;
    break;
  case 0x51:
    pcVar3 = FUN_004dee20;
  }
  switch(param_2[5]) {
  case 4:
  case 8:
    piVar1 = param_2 + 4;
    param_2 = (int *)0x0;
    if (0 < *piVar1) {
      do {
        puVar4 = (undefined1 *)(piVar2[6] * (int)param_2 + piVar2[1]);
        local_8 = piVar2[3];
        iVar7 = *(int *)(param_1 + 0x10) * (int)param_2 + *(int *)(param_1 + 0x14);
        do {
          (*pcVar3)(iVar7,*puVar4);
          puVar4 = puVar4 + 1;
          iVar7 = iVar7 + 4;
          local_8 = local_8 + -1;
        } while (local_8 != 0);
        param_2 = (int *)((int)param_2 + 1);
      } while ((int)param_2 < piVar2[4]);
      return;
    }
    break;
  default:
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x8000000c);
    FUN_004d8480((undefined4 *)&local_8);
    break;
  case 0x10:
    piVar1 = param_2 + 4;
    param_2 = (int *)0x0;
    if (0 < *piVar1) {
      do {
        puVar5 = (undefined2 *)(piVar2[6] * (int)param_2 + piVar2[1]);
        local_8 = piVar2[3];
        iVar7 = *(int *)(param_1 + 0x10) * (int)param_2 + *(int *)(param_1 + 0x14);
        do {
          (*pcVar3)(iVar7,*puVar5);
          puVar5 = puVar5 + 1;
          iVar7 = iVar7 + 4;
          local_8 = local_8 + -1;
        } while (local_8 != 0);
        param_2 = (int *)((int)param_2 + 1);
      } while ((int)param_2 < piVar2[4]);
      return;
    }
    break;
  case 0x20:
    piVar1 = param_2 + 4;
    param_2 = (int *)0x0;
    if (0 < *piVar1) {
      do {
        puVar6 = (undefined4 *)(piVar2[6] * (int)param_2 + piVar2[1]);
        iVar7 = *(int *)(param_1 + 0x10) * (int)param_2 + *(int *)(param_1 + 0x14);
        local_8 = piVar2[3];
        do {
          (*pcVar3)(iVar7,*puVar6);
          puVar6 = puVar6 + 1;
          iVar7 = iVar7 + 4;
          local_8 = local_8 + -1;
        } while (local_8 != 0);
        param_2 = (int *)((int)param_2 + 1);
      } while ((int)param_2 < piVar2[4]);
      return;
    }
  }
  return;
}
