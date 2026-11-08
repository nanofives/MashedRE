// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ac900: by hand. 100%: the two locals are one struct whose address escapes (separate locals let the compiler fold the return to 0). */
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
typedef struct { undefined4 a; undefined4 b; } S25_005ac900;
undefined4 FUN_005ac930(undefined4 p0);
void FUN_005aa0c0(undefined4 p0, undefined4 p1, undefined4 (*p2)(undefined4), S25_005ac900 * p3, undefined4 p4);
// MATCH: 0x005ac900 FUN_005ac900
undefined4 FUN_005ac900(undefined4 param_1)
{
  S25_005ac900 s;
  s.a = param_1;
  s.b = 0;
  FUN_005aa0c0(0,0,FUN_005ac930,&s,1);
  return s.b;
}
