// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a6dc0: by hand. 100%: returns eax (the null arg) on the early exit, 4th arg is &param_4 (callee FUN_005a6d60 is cdecl with 4 args, add esp,0x10). */
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
int FUN_005a6d60(int p0, undefined4 p1, undefined4 p2, undefined4 * p3);
// MATCH: 0x005a6dc0 FUN_005a6dc0
int FUN_005a6dc0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)
{
  if (param_1 == 0) {
    return param_1;
  }
  return FUN_005a6d60(param_1,param_2,param_3,&param_4);
}
