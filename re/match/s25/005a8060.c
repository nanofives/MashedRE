// FLAGS: /O2 /Ob1 /G5
/* 40.0% (regs 45.2%, ops 51.0%), 141 vs 210 insns: Ghidra pruned the dead branches of a macro that re-evaluates (1 << FUN_005aee20(0x28))
   in every sub-expression (ushort test, & 0xffff, high-byte test, sar 0x18/0x10 table index into DAT_00617ff8); the original keeps all
   of those calls. Reproducing needs the original multi-evaluation macro text. Function addresses stored via (code *) casts. Class (b). */
/* MATCH-S25 0x005a8060: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char byte;
typedef int (__cdecl code)(void);
typedef void *HANDLE;
typedef int BOOL;
typedef long LONG;
void FUN_005a8460(void);
MGLOBAL(undefined4, DAT_007dcad8, 0x007dcad8);
void FUN_005a84d0(void);
MGLOBAL(undefined1, DAT_007dcadc, 0x007dcadc);
void FUN_005a8680(void);
MGLOBAL(undefined4, DAT_0063307c, 0x0063307c);
MGLOBAL(undefined4, DAT_007dcb4c, 0x007dcb4c);
MGLOBAL(undefined4, DAT_007dcb50, 0x007dcb50);
MGLOBAL(undefined4, DAT_007dcae0, 0x007dcae0);
MGLOBAL(undefined1, DAT_00617ff8, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e616c, 0x005e616c);
MGLOBAL(undefined1, DAT_007dcb54, 0x007dcb54);
void FUN_005a8390(void);
void FUN_005a8560(void);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005aee20(uint p0);
uint FUN_005aeea0(undefined4 * p0, LONG p1, LONG p2);
undefined4 FUN_005aef00(undefined4 * p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_005aef30(undefined4 * p0, undefined4 p1);
MGLOBAL(int, _DAT_007dcadc, 0x007dcadc);
// MATCH: 0x005a8060 FUN_005a8060
/* WARNING: Removing unreachable block (ram,0x005a80f4) */
/* WARNING: Removing unreachable block (ram,0x005a80d5) */
/* WARNING: Removing unreachable block (ram,0x005a80fc) */
/* WARNING: Removing unreachable block (ram,0x005a8166) */
/* WARNING: Removing unreachable block (ram,0x005a81a8) */
/* WARNING: Removing unreachable block (ram,0x005a817c) */
/* WARNING: Removing unreachable block (ram,0x005a8108) */
/* WARNING: Removing unreachable block (ram,0x005a814c) */
/* WARNING: Removing unreachable block (ram,0x005a811e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C2 2026-06-01] `void FUN_005a8060(int param_1)` */

void FUN_005a8060(int param_1)

{
  byte bVar1;
  int iVar2;
  char cVar4;
  uint uVar3;
  
  FUN_005a9e10(param_1,(undefined4)&DAT_005e616c,0);
  *(undefined4 *)(param_1 + 0x40) = 0xd78;
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) != 0) {
    FUN_005aee20(0x28);
  }
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) == 0) {
    cVar4 = '\0';
  }
  else {
    bVar1 = FUN_005aee20(0x28);
    cVar4 = (char)((uint)(1 << (bVar1 & 0x1f)) >> 8);
  }
  if (cVar4 == '\0') {
    bVar1 = FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      uVar3 = 1;
    }
    else {
      bVar1 = FUN_005aee20(0x28);
      uVar3 = 1 << (bVar1 & 0x1f) & 0xffff;
    }
    iVar2 = (&DAT_00617ff8)[uVar3] + -1;
  }
  else {
    bVar1 = FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      iVar2 = DAT_00617ff8 + 7;
    }
    else {
      bVar1 = FUN_005aee20(0x28);
      iVar2 = (&DAT_00617ff8)[(int)(1 << (bVar1 & 0x1f) & 0xffffU) >> 8] + 7;
    }
  }
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x28) = (code *)FUN_005a8390;
  *(code **)(param_1 + 0x34) = (code *)FUN_005a8460;
  *(code **)(param_1 + 0x30) = (code *)FUN_005a8560;
  *(code **)(param_1 + 0x3c) = (code *)FUN_005a8680;
  *(undefined2 *)(param_1 + 0x44) = 0xc;
  DAT_007dcad8 = (undefined4)&DAT_007dcad8;
  _DAT_007dcadc = (int)&DAT_007dcad8;
  iVar2 = FUN_005aeea0((undefined4 *)&DAT_007dcae0,1,1);
  if (iVar2 != 0) {
    iVar2 = FUN_005aeea0((undefined4 *)&DAT_007dcb50,0,1);
    if (iVar2 != 0) {
      DAT_007dcb4c = 0;
      iVar2 = FUN_005aef00((undefined4 *)&DAT_007dcb54,(undefined4)FUN_005a84d0,DAT_0063307c,0);
      if ((iVar2 != 0) && (iVar2 = (int)FUN_005aef30((undefined4 *)&DAT_007dcb54,0), iVar2 != 0)) {
        return;
      }
      CloseHandle((HANDLE)DAT_007dcb50);
    }
    CloseHandle((HANDLE)DAT_007dcae0);
  }
  return;
}
