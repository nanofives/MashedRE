// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6b90 (h4): 100%, regs 100%. Register args (EAX = dividend, ECX = divisor; Ghidra in_EAX): pilot finding 1, FUN_004b6b90 is a static function defined before a stand-in caller (MatchStandIn_004b6b90, not a matched function, the real callers are at 0x004b6e18/0x004b6e95). Rounds down to a multiple: a / b * b. */
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
// MATCH: 0x004b6b90 FUN_004b6b90
static int FUN_004b6b90(int param_1, int param_2)
{
  return param_1 / param_2 * param_2;
}

/* stand-in caller (not a matched function): makes 9176 pass param_1 in EAX and param_2 in ECX */
int MatchStandIn_004b6b90(int a, int b)
{
  return FUN_004b6b90(a, b) + FUN_004b6b90(b, a);
}
