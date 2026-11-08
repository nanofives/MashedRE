// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c77c0: 100% (regs 100%). The free call at +0x11c takes 2 args (handle, block) and the failure branch comes first, success (FUN_004d8000) last. */
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
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d40a8, 0x007d40a8);
MGLOBAL(undefined4, DAT_00618180, 0x00618180);
int FUN_004d8000(int p0, undefined4 p1);
// MATCH: 0x004c77c0 FUN_004c77c0
int FUN_004c77c0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = (**(code **)(DAT_007d3ff8 + 0x118))
                    (*(undefined4 *)(DAT_007d40a8 + 0x60 + DAT_007d3ff8),0x30407);
  if (iVar2 != 0) {
    pcVar1 = *(code **)(DAT_007d3ff8 + 0x58);
    *(undefined4 *)(iVar2 + 0xc) = param_1;
    *(undefined1 *)(iVar2 + 0x22) = 0;
    *(undefined4 *)(iVar2 + 0x10) = param_2;
    *(undefined4 *)(iVar2 + 0x14) = param_3;
    *(undefined1 *)(iVar2 + 0x21) = 0;
    *(undefined2 *)(iVar2 + 0x1c) = 0;
    *(undefined2 *)(iVar2 + 0x1e) = 0;
    *(int *)iVar2 = iVar2;
    *(undefined4 *)(iVar2 + 4) = 0;
    *(undefined4 *)(iVar2 + 8) = 0;
    iVar3 = (*pcVar1)(0,iVar2,param_4);
    if (iVar3 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d40a8 + 0x60 + DAT_007d3ff8),iVar2);
    }
    else {
      FUN_004d8000((int)&DAT_00618180,iVar2);
      return iVar2;
    }
  }
  return 0;
}
