// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494a80 (h1): 100% (regs 100%), by hand. Residual: none.
   cdecl, 3 stack args (param_1 unused). Callee arities from their own code: FUN_004c77c0 4 args cdecl,
   FUN_004944c0 1 arg cdecl. */
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
MGLOBAL(undefined4, DAT_00771a18, 0x00771a18);
MGLOBAL(undefined4, DAT_00771a04, 0x00771a04);
MGLOBAL(undefined4, DAT_00771a08, 0x00771a08);
char *FUN_004944c0(int param_1);
int FUN_004c77c0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
// MATCH: 0x00494a80 FUN_00494a80
void FUN_00494a80(undefined4 param_1, undefined4 param_2, undefined4 param_3)
{
  DAT_00771a18 = FUN_004c77c0(0, 0, 0, 0x84);
  FUN_004944c0(param_2);
  DAT_00771a04 = 1;
  DAT_00771a08 = param_3;
}
