// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dfb60 (h4): 28.1% (regs 78.1%, ops 81.2%). Signed int form (raw unsigned form 15.6%: original divides with the signed magic 0x291ee171 / sar 0xc). Residual: constant-multiply expansion. Ours builds 59 as 3*5*4-1 and 30 as 3*5*2 with lea; the original builds 59 as ((8a-a)*4+a)*2+a and 30 with imul 0x1e. Same in every term order, with named locals, and under /O2 /Ob1, /G6, /Op, /Ox, /O1. Class (a). */
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
typedef unsigned int uint;
typedef unsigned char byte;
// MATCH: 0x004dfb60 FUN_004dfb60
int FUN_004dfb60(byte *param_1)

{
  int r = param_1[0], g = param_1[1], b = param_1[2], a = param_1[3];
  return ((r * 30 + g * 59 + b * 11) * a) / 25500 * 255;
}
