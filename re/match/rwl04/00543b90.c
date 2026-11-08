// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543b90 h1: best 60% (regs 100%, ops 100%): sibling jmp, only add operand order differs (ours add ecx,eax; orig add eax,ecx). Tried a+g, g+a, casts, ptr arg, 1-arg, flags /O2 /Ob1 /GS /G6 /Ox /Ob2. Class (a). */
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
MGLOBAL(int, DAT_007dc634, 0x007dc634);
int FUN_00543bb0(int a, int b, int c, int d);
// MATCH: 0x00543b90 FUN_00543b90
int FUN_00543b90(int a, int b, int c, int d)
{
  a += DAT_007dc634;
  return FUN_00543bb0(a, b, c, d);
}
