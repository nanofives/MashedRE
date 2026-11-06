// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495120 (h1): 100% (regs 100%), by hand. Residual: none.
   QueryPerformanceFrequency into a local, low / high dwords stored to 0x00771e70 / 0x00771e74, returns 1. */
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
MGLOBAL(uint, DAT_00771e70, 0x00771e70);
MGLOBAL(int, DAT_00771e74, 0x00771e74);
#ifdef __cplusplus
/* port: windows.h already declares QueryPerformanceFrequency(LARGE_INTEGER *); same layout as LargeInt */
#define QueryPerformanceFrequency(p) (QueryPerformanceFrequency)((LARGE_INTEGER *)(p))
#else
__declspec(dllimport) int __stdcall QueryPerformanceFrequency(LargeInt *lpFrequency);
#endif
// MATCH: 0x00495120 FUN_00495120
undefined4 FUN_00495120(void)
{
  LargeInt f;
  QueryPerformanceFrequency(&f);
  DAT_00771e70 = f.s.LowPart;
  DAT_00771e74 = f.s.HighPart;
  return 1;
}
