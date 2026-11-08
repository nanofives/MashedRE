// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aeea0 (h3): 100.0% exact (regs 100.0%). written by hand; (h ? -1 : 0) & param_1 gives the neg/sbb tail (-(h!=0) gave setne). */
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
typedef unsigned int undefined4;
typedef void *HANDLE;
typedef long LONG;
__declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(void *p0, LONG p1, LONG p2, const char *p3);
// MATCH: 0x005aeea0 FUN_005aeea0
uint FUN_005aeea0(undefined4 *param_1,LONG param_2,LONG param_3)
{
  uint h;
  h = (uint)CreateSemaphoreA((void *)0,param_2,param_3,(const char *)0);
  *param_1 = h;
  return (h ? -1 : 0) & (uint)param_1;
}
