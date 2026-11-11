// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054b6d0 h1: best 70.3% (regs 100%, ops 100%): same 37 instructions, register choice differs (ours esi for running sum, orig ecx). Tried temps, operand order, statement split, permuter. Class (b). */
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
int FUN_004f3ce0(int p0);
// MATCH: 0x0054b6d0 FUN_0054b6d0
int FUN_0054b6d0(int param_1)
{
  unsigned int f = *(unsigned int *)(param_1 + 0x2c);
  int s;
  s = ((((f >> 1 & 1) + (f & 1)) * 3 + (f >> 0x10 & 0xff) * 2 + (f >> 2 & 1)) * *(int *)(param_1 + 0x30)
       + *(int *)(param_1 + 0x38) * 0x11 + *(int *)(param_1 + 0x34) * 0xb) * 4 + 0x14;
  return s + FUN_004f3ce0(param_1 + 0x48) + 0xc;
}
