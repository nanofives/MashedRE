// FLAGS: /O1
// 100% under /O1 (CRT flags). hand-written (raw converter had no address for _FILETIME)
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
typedef struct { unsigned long dwLowDateTime; unsigned long dwHighDateTime; } FILETIME;
MGLOBAL(unsigned __int64, DAT_007de100, 0x007de100);
__declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(FILETIME *lpSystemTimeAsFileTime);
// MATCH: 0x005c2a0e FUN_005c2a0e
int FUN_005c2a0e(void)
{
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  DAT_007de100 = (unsigned __int64)ft.dwHighDateTime * 0x100000000ui64 + ft.dwLowDateTime;
  return 0;
}
