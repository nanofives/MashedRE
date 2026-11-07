// FLAGS: /O1
/* MATCH-S28 h1 0x005c1da2 : 0.0%. The 10 bytes are push [ebp+8]; call 0x004a5f6d; pop ecx; ret with no prologue: a funclet body (SEH __finally / unwind handler) that runs in its parent function's ebp frame, so it cannot be produced as a stand-alone C function. Kept as a plain call-through for the port. Residual class (c). */

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

void FUN_004a5f6d(int p0);
// MATCH: 0x005c1da2 FUN_005c1da2
void FUN_005c1da2(int param_1)
{
  FUN_004a5f6d(param_1);
}
