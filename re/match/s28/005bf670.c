// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bf670 : 40.0% (regs 40). Original: mov eax,[esp+4]; add eax,4; mov [esp+4],eax; jmp [InterlockedIncrement import]. This is the C++ multiple-inheritance this-adjustor thunk shape (arg rewritten in place, tail jump); the DDK C compiler emits push/call/ret for every C spelling tried (param reassign, cast, temp). Residual class (c): tail call with modified arg is not reachable from C. */

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

__declspec(dllimport) long __stdcall InterlockedIncrement(long *p);
// MATCH: 0x005bf670 FUN_005bf670
void FUN_005bf670(long *param_1)
{
  param_1 = (long *)((char *)param_1 + 4);
  InterlockedIncrement(param_1);
}
