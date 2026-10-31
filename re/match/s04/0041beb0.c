// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041beb0 (14 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 57.1%.
   The emitter pointer arrives in EAX: FUN_0041beb0 is `static`, defined before its only caller FUN_0041c0e0
   (call site 0x0041c0e8), pilot finding 1. FUN_004e6e00 (RpClumpDestroy) is cdecl, 1 arg (pop ecx).
   This file also holds the caller FUN_0041c0e0 (100%, needed for the register convention; index loop over the
   2-entry array at 0x0063cab8, stride 0x16c, gives the `jl` pointer compare). Record +0x15c = clump.
   Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { char pad[0x15c]; int clump; char pad2[0x16c - 0x160]; } Emit0041beb0;   /* stride 0x16c */
MGLOBALA(Emit0041beb0, DAT_0063cab8, 2, 0x0063cab8);
int FUN_004e6e00(int clump);

// MATCH: 0x0041beb0 FUN_0041beb0
static void FUN_0041beb0(Emit0041beb0 *e)
{
  FUN_004e6e00(e->clump);
}

// MATCH: 0x0041c0e0 FUN_0041c0e0
void FUN_0041c0e0(void)
{
  int i;
  for (i = 0; i < 2; i++)
    FUN_0041beb0(&DAT_0063cab8[i]);
}
