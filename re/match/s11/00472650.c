// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472650 (h7): 100%. Not __thiscall (push ecx is the fild temp slot); cdecl float(lo, hi), unsigned rand
   (FUN_00534870, no args) scaled by 1/2^31 (0x5cd314), grouped (hi-lo)*(r*k)+lo. Residual: none. */
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
MGLOBAL(float, DAT_005cd314, 0x005cd314);
unsigned int FUN_00534870(void);
// MATCH: 0x00472650 FUN_00472650
float FUN_00472650(float param_2,float param_3)
{
  return (param_3 - param_2) * ((float)(FUN_00534870() & 0x7fffffff) * DAT_005cd314) + param_2;
}
