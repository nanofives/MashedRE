// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c380 (130 B): 100.0% exact (regs 100, ops 100), by hand, from the pilot's solution
   (re/match/pilot/a7_0041c380.c). Raw converter: 47.4%.
   Record in ESI: `static`, defined before its only caller FUN_0041cb80 (call site 0x0041cba0).
   DAT_0063cde4 is a file-`static` global (pilot finding 2): `static int` under C, MGLOBAL under C++. Only then
   are both of its loads hoisted above the o->mask stores. Its 3 .text references (0x0041c3a6, 0x0041c3e5,
   0x0041cb87) all sit in these two functions.
   This file also holds the caller FUN_0041cb80 (100%). Slot table as in 0x0041b720 (0x007f1a10, field +0xc).
   Record (stride 0x114 at 0x0063ce20): +0x108 idx, +0x10c mask, +0x110 flag. Residual: none. */
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
typedef struct { int pad0; int a; int b; int c; } Slot_0041c380;   /* stride 0x10 at 0x007f1a10 */
typedef struct { char pad[0x108]; int idx; unsigned int mask; int flag; } Emit0041c380;   /* stride 0x114 */
MGLOBALA(Slot_0041c380, DAT_007f1a10, 4, 0x007f1a10);
#ifndef __cplusplus
static int DAT_0063cde4;
#else
MGLOBAL(int, DAT_0063cde4, 0x0063cde4);
#endif
MGLOBAL(int, DAT_0063ce18, 0x0063ce18);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(Emit0041c380, DAT_0063ce20, 4, 0x0063ce20);
int FUN_0042f500(void);
int FUN_0040e340(void);
int FUN_0040b890(void);

// MATCH: 0x0041c380 FUN_0041c380
static void FUN_0041c380(Emit0041c380 *o)
{
  o->mask = 0;
  o->flag = 0;
  o->mask = 1 << DAT_007f1a10[o->idx].c;
  if (DAT_0063cde4 > 3) {
    if (!FUN_0042f500() && DAT_007f0fd0 != 1 && DAT_007f0fd0 != 2)
      o->mask |= 0x800;
    else
      o->mask |= 0x400;
  } else
    o->mask |= 0x400;
  if (o->idx < DAT_0063cde4)
    o->flag = 1;
}

// MATCH: 0x0041cb80 FUN_0041cb80
void FUN_0041cb80(void)
{
  int i;
  DAT_0063cde4 = FUN_0040e340();
  DAT_0063ce18 = FUN_0040b890();
  for (i = 0; i < 4; i++)
    FUN_0041c380(&DAT_0063ce20[i]);
}
