// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041cdb0 (79 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 23.7%.
   Twin of 0x0041b720 (same source, record stride 0x160 at 0x0063d298, mask at +0x15c). `static`, defined before
   its only caller FUN_0041d730 (call site 0x0041d800), so the record arrives in EAX.
   This file also holds the caller FUN_0041d730 (226 B, 100%), twin of 0x0041bf20. Residual: none. */
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
typedef struct { int pad0; int a; int b; int c; } Slot_0041cdb0;   /* stride 0x10 at 0x007f1a10 */
typedef struct { int ids[3]; int count; char pad[0x15c - 0x10]; unsigned int mask; } Emit0041cdb0;   /* stride 0x160 */
MGLOBALA(Emit0041cdb0, DAT_0063d298, 2, 0x0063d298);
MGLOBALA(Slot_0041cdb0, DAT_007f1a10, 4, 0x007f1a10);
void FUN_004b6520(void *p, int n);

// MATCH: 0x0041cdb0 FUN_0041cdb0
MATCH_STATIC void FUN_0041cdb0(Emit0041cdb0 *e)
{
  int i;
  int n = e->count;
  e->mask = 0x800000 << (n - 1);
  for (i = 0; i < n; i++)
    e->mask |= 1 << (DAT_007f1a10[e->ids[i]].c + i * 6);
}

// MATCH: 0x0041d730 FUN_0041d730
void FUN_0041d730(void)
{
  int i;
  Emit0041cdb0 *e;
  for (i = 0; i < 2; i++)
    FUN_004b6520(&DAT_0063d298[i], 0x10);
  if (DAT_007f1a10[0].a != -1 && DAT_007f1a10[0].b != -1) {
    e = &DAT_0063d298[DAT_007f1a10[0].b];
    e->ids[e->count] = 0;
    e->count++;
  }
  if (DAT_007f1a10[1].a != -1 && DAT_007f1a10[1].b != -1) {
    e = &DAT_0063d298[DAT_007f1a10[1].b];
    e->ids[e->count] = 1;
    e->count++;
  }
  if (DAT_007f1a10[2].a != -1 && DAT_007f1a10[2].b != -1) {
    e = &DAT_0063d298[DAT_007f1a10[2].b];
    e->ids[e->count] = 2;
    e->count++;
  }
  if (DAT_007f1a10[3].a != -1 && DAT_007f1a10[3].b != -1) {
    e = &DAT_0063d298[DAT_007f1a10[3].b];
    e->ids[e->count] = 3;
    e->count++;
  }
  for (i = 0; i < 2; i++)
    FUN_0041cdb0(&DAT_0063d298[i]);
}
