// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004950b0 (h1): 100% (regs 100%), by hand. Residual: none.
   QueryPerformanceCounter into a zeroed 64-bit union local, t = t * 3000000 / freq (__allmul / __alldiv), the
   result written back into the union (that is the mov [esp+4],edx), low dword returned. Freq at 0x00771e70
   as __int64. */
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
typedef union { struct { uint LowPart; int HighPart; } s; __int64 QuadPart; } LargeInt;
MGLOBAL(__int64, DAT_00771e70, 0x00771e70);
__declspec(dllimport) int __stdcall QueryPerformanceCounter(LargeInt *lpPerformanceCount);
// MATCH: 0x004950b0 FUN_004950b0
uint FUN_004950b0(void)
{
  LargeInt t;
  t.s.LowPart = 0;
  t.s.HighPart = 0;
  QueryPerformanceCounter(&t);
  t.QuadPart = (t.QuadPart * 3000000) / DAT_00771e70;
  return t.s.LowPart;
}
