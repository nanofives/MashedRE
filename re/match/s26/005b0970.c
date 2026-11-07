// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0970 (77 B). Best 100.0% (regs 100.0%), exact. flags switched to /O2 /Ob1 /G5; callback FUN_005b0a70 as function pointer. */
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
typedef long LONG;
void FUN_005b0a70(void);
int * FUN_005aa560(int p0, int p1, unsigned int p2, int p3, int p4, int p5);
__declspec(dllimport) LONG __stdcall InterlockedIncrement(LONG * p0);
// MATCH: 0x005b0970 FUN_005b0970
void FUN_005b0970(int param_1,int param_2,int param_3,int param_4)
{
  *(int *)(param_4 + 0x1c) = param_3;
  *(int *)(param_4 + 0xc) = *(int *)(param_1 + 0x10);
  *(int *)(param_4 + 0x28) = param_1;
  *(void (**)(void))(param_4 + 0x24) = FUN_005b0a70;
  InterlockedIncrement((LONG *)(param_1 + 4));
  FUN_005aa560(*(int *)(param_2 + 4),*(int *)(param_1 + 0x48),4,param_4,
               *(int *)(param_1 + 0x50),*(int *)(param_1 + 0x58));
}
