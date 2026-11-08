// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 30.4% (regs 33.9%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined1, DAT_00617ff8, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e6d50, 0x005e6d50);
MGLOBAL(undefined1, DAT_00634924, 0x00634924);
MGLOBAL(undefined1, FUN_005b7750, 0x005b7750);
MGLOBAL(undefined1, FUN_005b7a90, 0x005b7a90);
MGLOBAL(undefined1, FUN_005b7880, 0x005b7880);
MGLOBAL(undefined1, FUN_005b7980, 0x005b7980);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005aee20(uint p0);
// MATCH: 0x005b74c0 FUN_005b74c0
/* WARNING: Removing unreachable block (ram,0x005b7554) */
/* WARNING: Removing unreachable block (ram,0x005b7535) */
/* WARNING: Removing unreachable block (ram,0x005b755c) */
/* WARNING: Removing unreachable block (ram,0x005b75c6) */
/* WARNING: Removing unreachable block (ram,0x005b7608) */
/* WARNING: Removing unreachable block (ram,0x005b75dc) */
/* WARNING: Removing unreachable block (ram,0x005b7568) */
/* WARNING: Removing unreachable block (ram,0x005b75ac) */
/* WARNING: Removing unreachable block (ram,0x005b757e) */
/* [C2 2026-06-01] Constructor/installer for the full audio-stream object: base init + CPU-feature
   variant select + handler-pointer? */

void FUN_005b74c0(int param_1)

{
  byte bVar1;
  int iVar2;
  char cVar4;
  uint uVar3;
  
  FUN_005a9e10(param_1,(undefined4)&DAT_005e6d50,0);
  *(undefined4 *)(param_1 + 0x40) = 0x1e0;
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
  if (!(cVar4 == '\0')) {
    bVar1 = FUN_005aee20(0x28);
    if (!((short)(1 << (bVar1 & 0x1f)) == 0)) {
      bVar1 = FUN_005aee20(0x28);
      iVar2 = (&DAT_00617ff8)[(int)(1 << (bVar1 & 0x1f) & 0xffffU) >> 8] + 7;
    } else {
      iVar2 = DAT_00617ff8 + 7;
    }
  } else {
    bVar1 = FUN_005aee20(0x28);
    if (!((short)(1 << (bVar1 & 0x1f)) == 0)) {
      bVar1 = FUN_005aee20(0x28);
      uVar3 = 1 << (bVar1 & 0x1f) & 0xffff;
    } else {
      uVar3 = 1;
    }
    iVar2 = (&DAT_00617ff8)[uVar3] + -1;
  }
  *(code **)(param_1 + 0x28) = (code *)(FUN_005b7750);
  *(code **)(param_1 + 0x34) = (code *)(FUN_005b7880);
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x2c) = (code *)(FUN_005b7980);
  *(code **)(param_1 + 0x3c) = (code *)(FUN_005b7a90);
  *(undefined2 *)(param_1 + 0x44) = 0x58;
  *(undefined **)(param_1 + 0x54) = &DAT_00634924;
  return;
}
