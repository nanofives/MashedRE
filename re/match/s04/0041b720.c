// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b720 (79 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 23.7%.
   The record arrives in EAX: `static`, defined before its only caller FUN_0041bf20 (call site 0x0041bff0). The
   caller keeps its loop pointer in EAX across the call, which the compiler allows only because it sees the
   static callee's body. Bit index = slot[ids[i]].c + i * 6 (slot table stride 0x10 at 0x007f1a10, field +0xc =
   0x007f1a1c); the `i * 6` is strength-reduced into esi.
   This file also holds the caller FUN_0041bf20 (226 B, 100%): FUN_004b6520(rec, 0x10) over the 2 records, four
   written-out blocks (slot[p].a != -1 && slot[p].b != -1 -> rec[slot[p].b].ids[count++] = p), then the loop.
   Record (stride 0x16c at 0x0063cab8): +0 ids[3], +0xc count, +0x168 mask. Residual: none. */
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { int pad0; int a; int b; int c; } Slot_0041b720;   /* stride 0x10 at 0x007f1a10 */
typedef struct { int ids[3]; int count; char pad[0x168 - 0x10]; unsigned int mask; } Emit0041b720;   /* stride 0x16c */
MGLOBALA(Emit0041b720, DAT_0063cab8, 2, 0x0063cab8);
MGLOBALA(Slot_0041b720, DAT_007f1a10, 4, 0x007f1a10);
void FUN_004b6520(void *p, int n);

// MATCH: 0x0041b720 FUN_0041b720
MATCH_STATIC void FUN_0041b720(Emit0041b720 *e)
{
  int i;
  int n = e->count;
  e->mask = 0x800000 << (n - 1);
  for (i = 0; i < n; i++)
    e->mask |= 1 << (DAT_007f1a10[e->ids[i]].c + i * 6);
}

// MATCH: 0x0041bf20 FUN_0041bf20
void FUN_0041bf20(void)
{
  int i;
  Emit0041b720 *e;
  for (i = 0; i < 2; i++)
    FUN_004b6520(&DAT_0063cab8[i], 0x10);
  if (DAT_007f1a10[0].a != -1 && DAT_007f1a10[0].b != -1) {
    e = &DAT_0063cab8[DAT_007f1a10[0].b];
    e->ids[e->count] = 0;
    e->count++;
  }
  if (DAT_007f1a10[1].a != -1 && DAT_007f1a10[1].b != -1) {
    e = &DAT_0063cab8[DAT_007f1a10[1].b];
    e->ids[e->count] = 1;
    e->count++;
  }
  if (DAT_007f1a10[2].a != -1 && DAT_007f1a10[2].b != -1) {
    e = &DAT_0063cab8[DAT_007f1a10[2].b];
    e->ids[e->count] = 2;
    e->count++;
  }
  if (DAT_007f1a10[3].a != -1 && DAT_007f1a10[3].b != -1) {
    e = &DAT_0063cab8[DAT_007f1a10[3].b];
    e->ids[e->count] = 3;
    e->count++;
  }
  for (i = 0; i < 2; i++)
    FUN_0041b720(&DAT_0063cab8[i]);
}
