// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00452af0: 100% by hand. Ghidra's body runs past the real end (jmp 0x00452620, a sibling
   tail call passing the caller's stack arg through). FUN_0047d100 takes 2 stack args (Ghidra's callee proto
   says 1). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
void FUN_00451cc0(int param_1);
void FUN_0047d100(int param_1, int param_2);
void FUN_00452620(int param_1);
MGLOBAL(undefined4, DAT_005f978c, 0x005f978c);
MGLOBAL(undefined4, DAT_005f9790, 0x005f9790);
MGLOBALA(undefined, DAT_00684d18, 1, 0x00684d18);
// MATCH: 0x00452af0 FUN_00452af0
void FUN_00452af0(int param_1)
{
  FUN_00451cc0((int)DAT_00684d18);
  FUN_0047d100(DAT_005f978c, 0);
  FUN_0047d100(DAT_005f9790, 0);
  FUN_00452620(param_1);
}
