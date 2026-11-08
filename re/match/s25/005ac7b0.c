// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ac7b0: by hand. 100%: ternary (iVar1 != 0) ? param_1 : 0 gives neg/sbb/and (the Ghidra -(uint)(..) & form gave 91%). */
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
int FUN_005adfe0(uint p0, undefined4 p1);
int FUN_005ae010(uint p0, undefined4 p1);
// MATCH: 0x005ac7b0 FUN_005ac7b0
uint FUN_005ac7b0(uint param_1,undefined4 param_2,undefined4 param_3)
{
  int iVar1;
  iVar1 = FUN_005ae010(param_1,param_2);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_005adfe0(param_1,param_3);
  return (iVar1 != 0) ? param_1 : 0;
}
