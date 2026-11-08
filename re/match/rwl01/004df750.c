// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004df750: 100% (regs 100%). The returned handlers are declared as functions (not MGLOBAL bytes) and the flags are /G5 (not /GS). */
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
typedef int code();
int FUN_004dfb60();
int FUN_004df870();
int FUN_004df8a0();
int FUN_004df8d0();
int FUN_004df910();
int FUN_004df950();
int FUN_004df980();
int FUN_004df9b0();
int FUN_004df9e0();
int FUN_004dfa10();
int FUN_004dfa40();
int FUN_004dfa70();
int FUN_004dfaa0();
int FUN_004dfab0();
int FUN_004dfb10();
// MATCH: 0x004df750 FUN_004df750
code * FUN_004df750(undefined4 param_1)
{
  code *pcVar1;
  pcVar1 = (code *)0x0;
  switch(param_1) {
  case 0x18:
    return FUN_004df870;
  case 0x17:
    return FUN_004df8a0;
  case 0x19:
    return FUN_004df8d0;
  case 0x1a:
    return FUN_004df910;
  case 0x1d:
    return FUN_004df950;
  case 0x32:
    return FUN_004dfab0;
  case 0x16:
    return FUN_004df9e0;
  case 0x15:
    return FUN_004dfa40;
  case 0x1e:
    return FUN_004df980;
  case 0x33:
    return FUN_004dfb10;
  case 0x51:
    return FUN_004dfb60;
  case 0x3c:
  case 0x75:
    return FUN_004dfaa0;
  case 0x3d:
    return FUN_004df9b0;
  case 0x20:
  case 0x3f:
    return FUN_004dfa70;
  case 0x21:
  case 0x3e:
    pcVar1 = FUN_004dfa10;
    break;
  }
  return pcVar1;
}
