// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bcd10: 100.0% (regs 100.0%). Hand: p[] int-indexed, slot pointer kept, p[1]++ then compare with p[2] (not a temp). */
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
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LONG *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
// MATCH: 0x005bcd10 FUN_005bcd10
int FUN_005bcd10(int *p)
{
  int v; int *slot;
  WaitForSingleObject((HANDLE)p[4], 0xffffffff);
  slot = (int *)p[3] + p[1];
  v = *slot;
  if (v == 0) { ReleaseSemaphore((HANDLE)p[4], 1, (LONG *)0); return 0; }
  *slot = 0;
  p[1]++;
  if (p[1] == p[2]) p[1] = 0;
  ReleaseSemaphore((HANDLE)p[4], 1, (LONG *)0);
  return v;
}
