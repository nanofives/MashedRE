// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc580 (h2): 100% (regs 100%). 12-byte local struct {a, b, packed} passed by address to 0x004cbe80;
   the packed word written as ((v+0x10000)<<14 & 0xffc00000) | (v&0x3f)<<16 | w&0xffff. Raw 20.8% (the compiler had
   placed the locals in the param slots). Residual: none. relocheck OK. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct { undefined4 a; undefined4 b; uint c; } Ent3;
undefined4 * FUN_004cbe80(undefined4 p0, Ent3 * p1, uint p2);
// MATCH: 0x004cc580 FUN_004cc580
void FUN_004cc580(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint param_5)
{
  Ent3 e;
  e.a = param_2;
  e.b = param_3;
  e.c = (((param_4 + 0x10000) << 14) & 0xffc00000) | ((param_4 & 0x3f) << 16) | (param_5 & 0xffff);
  FUN_004cbe80(param_1,&e,0xc);
}
