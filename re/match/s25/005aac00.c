// FLAGS: /O2 /Ob1 /G5
/* 0x005aac00: 100.0%, 24 B BYTE-IDENTICAL, source from the listing (PORT-TOP-AUDIO-5AEF30-KNOB, 2026-11-16). Returns param_1 when the dword at param_1 - 4
   equals param_2 (mov edx,[eax-4] / cmp edx,esi at 0x005aac0b / 0x005aac0e), else 0 (setne / dec / and at 0x005aac11..0x005aac15). */
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
// MATCH: 0x005aac00 FUN_005aac00
uint *FUN_005aac00(uint *param_1, uint param_2)
{
  return (param_1[-1] == param_2) ? param_1 : 0;
}
