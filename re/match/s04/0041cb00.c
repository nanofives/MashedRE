// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041cb00 (14 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 57.1%.
   Same form as 0x0041beb0: `static`, defined before its only caller FUN_0041ccf0 (call site 0x0041ccf8), so the
   record arrives in EAX. This file also holds the caller FUN_0041ccf0 (100%): 4-entry array at 0x0063ce20,
   stride 0x114, then FUN_004c5930(DAT_0063cdd0) (cdecl, 1 arg). Record +0x100 = clump. Residual: none. */
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
typedef struct { char pad[0x100]; int clump; char pad2[0x114 - 0x104]; } Emit0041cb00;   /* stride 0x114 */
MGLOBALA(Emit0041cb00, DAT_0063ce20, 4, 0x0063ce20);
MGLOBAL(int, DAT_0063cdd0, 0x0063cdd0);
int FUN_004e6e00(int clump);
int FUN_004c5930(int a);

// MATCH: 0x0041cb00 FUN_0041cb00
static void FUN_0041cb00(Emit0041cb00 *e)
{
  FUN_004e6e00(e->clump);
}

// MATCH: 0x0041ccf0 FUN_0041ccf0
void FUN_0041ccf0(void)
{
  int i;
  for (i = 0; i < 4; i++)
    FUN_0041cb00(&DAT_0063ce20[i]);
  FUN_004c5930(DAT_0063cdd0);
}
