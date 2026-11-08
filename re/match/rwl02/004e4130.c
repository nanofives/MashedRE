// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR01 2026-11-08: 100% (regs 100%), relocheck OK, was 67.6%. match_permute.py swap: cur = nx moves before the free callback (o is already computed, same semantics). Supersedes the header below. */
// h2 hand-matched. 67.6% (regs 83.8%, ops 91.9%). RW link-list removal of two links per node. Residual: original keeps cur in esi, loads prev/next/o before the first store and reloads next from esi; ours reloads differently and has an extra mov. Tried 5 orderings of the removal statements. Class (b).
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

#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7164, 0x007d7164);
typedef struct L { struct L *next; struct L *prev; } L;
typedef struct O { L a; int pad; L b; } O;
typedef struct { char pad[0x2c]; L list; } C;
// MATCH: 0x004e4130 FUN_004e4130
C *FUN_004e4130(C *clump)
{
  L *cur = clump->list.next;
  L *nx;
  while (cur != &clump->list) {
    O *o = (O *)((char *)cur - 0xc);
    nx = cur->next;
    o->b.prev->next = o->b.next;
    o->b.next->prev = o->b.prev;
    o->a.prev->next = o->a.next;
    o->a.next->prev = o->a.prev;
    cur = nx;
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7164 + 4 + DAT_007d3ff8), o);
  }
  return clump;
}
