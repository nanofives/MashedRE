// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x004b5560: calls 0x004b5260(p0, p1) and returns p0. The callback that kept
   0x004b5580 a stand-in in POWERUPINIT-A. */
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
void FUN_004b5260(void *p0, void *p1);
// MATCH: 0x004b5560 FUN_004b5560
void *FUN_004b5560(void *param_1, void *param_2)
{
  FUN_004b5260(param_1, param_2);
  return param_1;
}
