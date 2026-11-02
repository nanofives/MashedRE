// FLAGS: /O2 /GS /Ob1
/* MATCH-GFFU 0x00417740 (16 B): mov eax,[esp+4]; fld dword [eax*4+0x0089a870]; jmp 0x004a2c48 (__ftol, tail call).
   Ghidra decomp lost the float load (it shows a bare call of 0x004a2c48); the source is the listing's form. */
/* ---- MATCH-GFFU preamble (same text in every re/match/gffu source; not part of any function body) ----
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

MGLOBALA(float, DAT_0089a870, 4, 0x0089a870);
// MATCH: 0x00417740 FUN_00417740
int FUN_00417740(int param_1)
{
  return (int)DAT_0089a870[param_1];
}
