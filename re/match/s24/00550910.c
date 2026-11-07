// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550910 (h4): 100.0% (regs 100.0%). Interface pointer (param_1+0x38)+0x28 as a separate local
   (same form as 0x00550950), slot +4 called under the 0x009124e0 critical section. relocheck unchecked are the
   two CriticalSection imports. */
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
typedef struct CritSec { int w[6]; } CritSec;
typedef void (*FsFn)(int file);
typedef struct { int f0; FsFn fn4; } FsIf;
typedef struct { char pad[0x28]; FsIf fsif; } Fs;
__declspec(dllimport) void __stdcall EnterCriticalSection(CritSec *cs);
__declspec(dllimport) void __stdcall LeaveCriticalSection(CritSec *cs);
MGLOBAL(CritSec, DAT_009124e0, 0x009124e0);
// MATCH: 0x00550910 FUN_00550910
int FUN_00550910(int param_1)
{
  FsIf *i = &(*(Fs **)(param_1 + 0x38))->fsif;
  EnterCriticalSection(&DAT_009124e0);
  *(int *)(param_1 + 0x20) = 1;
  i->fn4(param_1);
  LeaveCriticalSection(&DAT_009124e0);
  return 0;
}
