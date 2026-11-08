// FLAGS: /O1
// MATCH-S27 0x005bad30: 33.8% (regs 21.8%) with /O1, raw Ghidra shape (C++ fixes only). Body uses unaff_EBP and stack0xffffff70 pseudo-variables (register and stack state live across a missing prologue), so the Ghidra boundary or calling convention is wrong. Class: c.
/* MATCH-S27 0x005bad30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef void *HWND;
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
undefined4 DirectSoundCreate8(int p0, int **p1, int p2);
undefined4 FUN_005bac00(int p0);
undefined4 FUN_005bbdb0(int * p0, int * p1, int p2, undefined4 p3, uint p4, undefined4 * p5);
void FUN_005bbf30(int * p0);
__declspec(dllimport) HWND __stdcall GetForegroundWindow(void);
MGLOBAL(int, stack0xffffff70, 0xffffff70);
// MATCH: 0x005bad30 FUN_005bad30
undefined4 FUN_005bad30(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  HWND pHVar2;
  uint uVar3;
  int *unaff_EBP;
  uint *puVar4;
  undefined4 uVar5;
  int *piStack_8c;
  undefined4 uStack_88;
  undefined4 uStack_84;
  undefined *puStack_80;
  undefined4 uStack_7c;
  undefined1 uStack_78;
  undefined1 uStack_77;
  undefined4 uStack_74;
  undefined4 uStack_70;
  undefined1 uStack_6c;
  undefined1 uStack_6b;
  uint auStack_60 [6];
  int iStack_48;
  uint uStack_44;
  uint uStack_2c;
  undefined4 uStack_8;
  
  *param_2 = 0;
  param_2[1] = 0;
  param_2[5] = 48000;
  *(undefined2 *)(param_2 + 6) = 0x10;
  param_2[2] = 0;
  param_2[3] = 1;
  param_2[4] = 0;
  param_2[8] = 0;
  param_2[9] = 1;
  param_2[7] = 0xa0;
  iVar1 = DirectSoundCreate8(0,&piStack_8c,0);
  if (-1 < iVar1) {
    puVar4 = auStack_60;
    for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    auStack_60[0] = 0x60;
    iVar1 = (**(code **)(*piStack_8c + 0x10))(piStack_8c,auStack_60);
    if (-1 < iVar1) {
      if (iStack_48 != 0) {
        uVar3 = auStack_60[0];
        if (((uint)param_2[5] < auStack_60[0]) ||
           (uVar3 = auStack_60[1], auStack_60[1] < (uint)param_2[5])) {
          param_2[5] = uVar3;
        }
      }
      if (uStack_2c == 0) {
        if (uStack_44 == 0) {
          param_2[2] = 1;
          *(undefined2 *)((int)param_2 + 0x1a) = 0x20;
        }
        else if ((uStack_44 < 0x10000) && ((ushort)uStack_44 < 0x81)) {
          *(ushort *)((int)param_2 + 0x1a) = (ushort)uStack_44;
        }
        else {
          *(undefined2 *)((int)param_2 + 0x1a) = 0x80;
        }
        param_2[4] = 1;
      }
      else {
        param_2[4] = 0;
        if ((uStack_2c < 0x10000) && ((ushort)uStack_2c < 0x81)) {
          *(ushort *)((int)param_2 + 0x1a) = (ushort)uStack_2c;
        }
        else {
          *(undefined2 *)((int)param_2 + 0x1a) = 0x80;
        }
      }
      uVar5 = 2;
      iVar1 = *unaff_EBP;
      pHVar2 = GetForegroundWindow();
      (**(code **)(iVar1 + 0x18))(unaff_EBP,pHVar2,uVar5);
      uStack_78 = *(undefined1 *)(param_2 + 6);
      uStack_84 = param_2[5];
      uStack_6c = 0;
      uStack_6b = 0;
      uStack_74 = 0;
      uStack_70 = 0;
      puStack_80 = &DAT_005e6424;
      uStack_7c = 4;
      uStack_77 = 1;
      piStack_8c = (int *)0x0;
      uStack_88 = 0;
      iVar1 = FUN_005bbdb0(unaff_EBP,(int *)&uStack_84,0,1,0x200b2,(undefined4 *)&stack0xffffff70);
      if (iVar1 != 0) {
        iVar1 = FUN_005bac00((int)&stack0xffffff70);
        if (iVar1 != 0) {
          param_2[3] = 1;
        }
        FUN_005bbf30(&stack0xffffff70);
      }
    }
    (**(code **)(*unaff_EBP + 8))(unaff_EBP);
    return uStack_8;
  }
  *(undefined2 *)((int)param_2 + 0x1a) = 0;
  return param_1;
}
