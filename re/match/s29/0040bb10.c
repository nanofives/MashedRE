// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (MATCH-40BB10) 0x0040bb10: the RpClumpForAllAtomics callback 0x0040cda0 passes. Calls
   0x0040bae0(atomic, data) and returns the atomic. */
/* ---- MATCH-S29 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
void FUN_0040bae0(void *atomic, void *data);
// MATCH: 0x0040bb10 FUN_0040bb10
void *FUN_0040bb10(void *param_1, void *param_2)
{
  FUN_0040bae0(param_1, param_2);
  return param_1;
}
