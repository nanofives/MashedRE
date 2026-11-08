// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543da0 (h2): 60.0% (regs 100, ops 100). Raw had wrong arity: callee 0x00543bb0 takes 1 int (tail jmp with [esp+4] rewritten). Only residue: add operand order/register (add ecx,eax vs add eax,ecx); tried param+g, g+param, temp local, char* arithmetic. class (b). */
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
MGLOBAL(int, DAT_007dc63c, 0x007dc63c);
int FUN_00543bb0(int a, int b, int c, int d);
// MATCH: 0x00543da0 FUN_00543da0
int FUN_00543da0(int param_1, int b, int c, int d)
{
  param_1 += DAT_007dc63c;
  return FUN_00543bb0(param_1, b, c, d);
}
