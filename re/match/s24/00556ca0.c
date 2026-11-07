// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00556ca0 (h4): 60.0% (regs 60.0%). Tail call through (param_1 + 0x138) with the caller's 3 args.
   The original re-stores arg 3 ([esp+0xc] via edx) though it is unchanged; 9176 elides it. Tried: int, float, char,
   short, void pointer, struct, __int64 arg types, type-pun casts, local copy, varargs callee, 4 args, void return, struct
   member fn pointer, flags /O2 /GS /Ob1, /O1, /O1 /Oy-, /Ox. None re-stores arg 3. Residual (b): source form of
   the third argument not found. */
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
typedef int (*fn_00556ca0)(int, int, int);
// MATCH: 0x00556ca0 FUN_00556ca0
int FUN_00556ca0(int param_1, int param_2, int param_3)
{
  return (*(fn_00556ca0 *)(param_1 + 0x138))(param_1, param_2, param_3);
}
