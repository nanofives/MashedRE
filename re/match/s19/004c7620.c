// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c7620 (h4): 100% (regs 100%), relocheck OK. RwGlobals +0xa8 call (0,p,0), clear bits 0x18 of byte +0x22, return p. Under the game flags without /G5: 78.6% (9176 emits and byte ptr instead of the mov al/and al/mov RMW). */
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
typedef void (*fp3)(int, void *, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c7620 FUN_004c7620
unsigned char * FUN_004c7620(unsigned char *param_1)
{
  (*(fp3 *)(DAT_007d3ff8 + 0xa8))(0,param_1,0);
  param_1[0x22] &= 0xe7;
  return param_1;
}
