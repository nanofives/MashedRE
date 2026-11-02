// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421060 (26 B): 100.0% (regs 100, ops 100), by hand. Same file text as 00421690.c (0x00421690,
   22 B, also 100.0%). This file holds 0x00421060. DAT_0063e4b8 = 4 records of 0x24 bytes. 0x00421060 takes the record pointer in ESI (Ghidra unaff_ESI): pilot
   finding 1, `static`, defined before its caller 0x00421690, which loads ESI and saves it around the call.
   Residual: none. Relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct Rec0063e4b8 {
  float v[2];    /* +0x00 */
  int a[2];      /* +0x08 */
  int b[2];      /* +0x10 */
  int id;        /* +0x18 */
  int f7;        /* +0x1c */
  int f8;        /* +0x20 */
} Rec0063e4b8;
MGLOBALA(Rec0063e4b8, DAT_0063e4b8, 4, 0x0063e4b8);
void FUN_004b6520(void *p, int n);
// MATCH: 0x00421060 FUN_00421060
MATCH_STATIC void FUN_00421060(Rec0063e4b8 *r)
{
  FUN_004b6520(r, 8);
  r->a[0] = 0;
  r->a[1] = 0;
  r->b[0] = 0;
  r->b[1] = 0;
}
// MATCH: 0x00421690 FUN_00421690
void FUN_00421690(int i)
{
  FUN_00421060(&DAT_0063e4b8[i]);
}
