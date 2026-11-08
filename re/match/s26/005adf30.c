// FLAGS: /O2 /GS /Ob1
/* MATCH-S26 0x005adf30: 100% by hand: inlined 16-byte memcmp via #pragma intrinsic(memcmp) (repe cmpsb + sbb/sbb), with the pointer-equality early return. FLAGS /O2 /GS /Ob1 (also 100% under /O2 /Ob1 /G5). cxx pending. */
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
int memcmp(const void *, const void *, unsigned int);
#pragma intrinsic(memcmp)
// MATCH: 0x005adf30 FUN_005adf30
int FUN_005adf30(const void *param_1,const void *param_2)
{
  if (param_1 == param_2) {
    return 0;
  }
  return memcmp(param_1,param_2,0x10);
}
